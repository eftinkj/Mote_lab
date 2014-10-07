
#include "defs/esnl_pub.h"

#include "arm_adc.h"

#ifdef _ENABLE_ARM_ADC_

IAR_PACKED uint16_t *dma_buff_ PACKED;	// pointer to the ADC DMA destination buffer
uint32_t dma_size16_; // Size of DMA buffer (count of ADC conversions)
sch_cb_func_t *dma_callback_; // function to be called when conversion ended
bool aadc_busy_;	// indicates is ADC/DMA are busy
uint32_t aadc_cfg;	// current ADC configuration

#define BUF_SIZE	35
IAR_PACKED uint16_t tbuff[BUF_SIZE] PACKED;

void aadc_powerup ()
{
	int i;
	//init variables
	// init hardware config (e.g. GPIOs? ADCs?)
	//ADC0 input PB5
//	GPIO_PBCFGH[7:4] = 0;
//	GPIO_PBCFGH = GPIO_PBCFGH & 0x11111101;
	//ADC1 input PB6
//	GPIO_PBCFGH[11:8] = 0;
	//ADC2 input PB7
//	GPIO_PBCFGH[15:12] = 0;
//	GPIO_PBCFGH = GPIO_PBCFGH & 0xFFFF0FFF;
	//ADC3 input PC1
//	GPIO_PCCFGH[7:4] = 0;
	//ADC4 input PA4
//	GPIO_PACFGH[3:0] = 0;
	//ADC5 input PA5 -  PACFGH [7:4]
//	((gpia_pacfgh_t)GPIO_PACFGH).PA5 = 0;
	GPIO_PACFGH = GPIO_PACFGH & 0xFFFFFF0F;
	
	dma_buff_ = NULL;
	dma_size16_ = 0; //
	dma_callback_ = NULL;
	aadc_busy_ = FALSE;
	(*(uint32_t*)&aadc_cfg) = 0;
	
	for(i=0; i< BUF_SIZE; i++)
	{
		tbuff[i] = 0x11;
	}
}

void aadc_init ()
{
	// init and configure interfaces with system and other blocks
	// LOOP, timers, related components
	sch_add_loop(aadc_loop);
	aadc_request_datablock(BUF_SIZE, (uint16_t *)tbuff, NULL);
}

void aadc_request_datablock(uint32_t size, uint16_t *buff, void *callback)
{
	arm_adc_cfg_t *tcfg = (arm_adc_cfg_t *)&aadc_cfg;
	dma_buff_ = buff;
	dma_size16_ = size; //
	dma_callback_ = callback;
	aadc_busy_ = TRUE;
	tcfg->adc_period = ADC_CONVERSION_TIME_US_4096;
	tcfg->adc_hvselp = 1;
	tcfg->adc_hvseln = 1;
	tcfg->adc_muxp = ADC_MUX_ADC5;
	tcfg->adc_muxn = ADC_MUX_VREF2;
	tcfg->adc_1mhzclk = 1;
	tcfg->adc_enable = 1;
	
	
	ADC_DMABEG = (uint32_t)dma_buff_;
	ADC_DMASIZE = dma_size16_;
	
	uint32_t t1 = 0;
	amd_adc_dmacfg_t *temp_dmacfg = (amd_adc_dmacfg_t *)&t1;
	temp_dmacfg->adc_dmarst = 1; // reset DMA
	temp_dmacfg->adc_dmaautowrap = 1; // setup auto wrap of buffer
	temp_dmacfg->adc_dmaload = 1; // start DMA
	ADC_DMACFG = t1;
	
	ADC_CFG = aadc_cfg;
	INT_ADCFLAG = 0xFFFF;
	INT_CFGSET = INT_ADC;
}

void aadc_loop()
{
	int i;
	if (1 == (INT_ADCFLAG & INT_ADCULDFULL))
	{
		// read ADC data
		for (i=0; i < dma_size16_; i++)
		{
			ssn_recv_sample(0, dma_buff_[i]);
		}
		aadc_request_datablock(BUF_SIZE, (uint16_t *)tbuff, NULL);
	}
}

void aadc_ISR()
{
}

#endif // _ENABLE_ARM_ADC_
