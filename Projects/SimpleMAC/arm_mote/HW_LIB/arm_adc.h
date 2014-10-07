#ifndef _ARM_ADC_H_
#define _ARM_ADC_H_

#include "FEAT_Scheduler/sch_basic_pub.h"



#ifdef _ENABLE_ARM_ADC_


typedef IAR_PACKED struct {
	uint32_t	reserved	:16; // MAC frame type: 0-beacon, 1-data, 2-ack, 3-MAC cmd, 4-reserved, 5-reserved
	uint32_t adc_period	:3;	// 1 - MAC-level cryptography: 1-enabled; 0-disabled
	uint32_t adc_hvselp	:1;	// Select voltage range for the P input channel. 0: Low voltage range (input buffer disabled) and 1: High voltage range (input buffer enabled).
	uint32_t adc_hvseln	:1;	// Select voltage range for the N input channel. 0: Low voltage range (input buffer disabled) and 1: High voltage range (input buffer enabled).
	uint32_t adc_muxp	:4;	// ADC_MUXP: Input selection for the P channel.
		//0x0: PB5 pin.		0x8: GND (0V) (not for high voltage range).
		//0x1: PB6 pin.		0x9: VREF/2 (0.6V).
		//0x2: PB7 pin.		0xA: VREF (1.2V).
		//0x3: PC1 pin.		0xB: VREG/2 (0.9V) (not for high voltage range).
		//0x4: PA4 pin.		0x6, 0x7, 0xC-0xF: Reserved.
		//0x5: PA5 pin.
	uint32_t adc_muxn	:4;	// ADC_MUXN: Input selection for the N channel.
	uint32_t adc_1mhzclk	:1; // ADC_1MHZCLK: Select ADC clock: 0: 6 MHz1: 1 MHz.
	uint32_t reserved2	:1;	// this bit must always be set to 0.
	uint32_t adc_enable	:1; // Enable the ADC: write 1 to enable continuous conversions, write 0 to stop.
}PACKED arm_adc_cfg_t;

typedef IAR_PACKED struct {
	uint32_t	reserved	:27; //
	uint32_t adc_dmarst	:1;	// Write 1 to reset the ADC DMA. This bit auto-clears.
	uint32_t reserved2	:2; //
	uint32_t adc_dmaautowrap	:1;	// Selects DMA mode.
		// 0: Linear mode, the DMA stops when the buffer is full.
		// 1: Auto-wrap mode, the DMA output wraps back to the start when the buffer is full.
	uint32_t adc_dmaload	:1; //  Loads the DMA buffer. Write 1 to start DMA (writing 0 has no effect). Cleared when DMA starts or is reset
}PACKED amd_adc_dmacfg_t;

typedef IAR_PACKED struct {
	uint32_t	reserved	:30; //
	uint32_t adc_dmaovf	:1;	// DMA overflow: occurs when an ADC result is ready and the DMA is not active. Cleared by DMA reset.
	uint32_t adc_dmaact	:1; // DMA status: reads 1 if DMA is active.
}PACKED amd_adc_dmastat_t;


typedef IAR_PACKED struct {
	uint32_t	reserved	:19; //
	uint32_t adc_dmabeg	:13;	// ADC buffer start address.
  		// Caution: This must be an even address - the least significant bit of this register is fixed at zero by hardware.
}PACKED amd_adc_dmabeg_t; // Base address for DMA destination


typedef IAR_PACKED struct {
	uint32_t	reserved	:20; //
	uint32_t adc_dmasize_field	:12;	// ADC buffer size. This is the number of 16-bit ADC conversion results
		// the buffer can hold, not its length in bytes. (The length in bytes is twice this value.)
}PACKED amd_adc_dmasize_t; // Number of conversions to be done (16-bit)
/*
typedef IAR_PACKED struct {
	uint32_t	reserved	:30; //
	uint32_t ADC_DMAOVF	:1;	// DMA overflow: occurs when an ADC result is ready and the DMA is not active. Cleared by DMA reset.
	uint32_t ADC_DMAACT	:1; // DMA status: reads 1 if DMA is active.
}PACKED amd_adc_dmastat_t;

typedef IAR_PACKED struct {
	uint32_t	reserved	:30; //
	uint32_t ADC_DMAOVF	:1;	// DMA overflow: occurs when an ADC result is ready and the DMA is not active. Cleared by DMA reset.
	uint32_t ADC_DMAACT	:1; // DMA status: reads 1 if DMA is active.
}PACKED amd_adc_dmastat_t;
*/



extern IAR_PACKED uint16_t *dma_buff_ PACKED;	// pointer to the ADC DMA destination buffer
extern uint32_t dma_size16_; // Size of DMA buffer (count of ADC conversions)
extern sch_cb_func_t *dma_callback_; // function to be called when conversion ended
extern bool aadc_busy_;	// indicates is ADC/DMA are busy
extern uint32_t aadc_cfg;	// current ADC configuration


/**
* aadc_powerup - initialize memory, variables, etc. (local)
*/
void aadc_powerup ();


/**
* aadc_init - initialize the module, connect to the rest of the system (timers, loop, etc)
*/
void aadc_init ();


/**
* aadc_request_datablock - requests single batch of ADC conversions (calback is used to signal end of conversion?? return partial data??)
*/
void aadc_request_datablock(uint32_t size, IAR_PACKED uint16_t *buff, void *callback);

/**
* aadc_loop - body of the main loop for ARM ADC module
*/
void aadc_loop();

#endif // _ENABLE_ARM_ADC_

#endif //_ARM_ADC_H_
