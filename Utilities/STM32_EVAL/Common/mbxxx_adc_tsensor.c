/**
  ******************************************************************************
  * @file    mbxxx_adc_tsensor.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file provides a set of functions needed to manage an ADC STLM20 
  *          
  *          
  *          ===================================================================      
  *          Notes: 
  *           - This driver is intended for STM32W108 families devices only.     
  *          ===================================================================
  *          
  *    
  *              
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mbxxx_adc_tsensor.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup COMMON
  * @{
  */
  
/** @addtogroup MBXXX_ADC_TSENSOR
  * @brief      This file includes the ADC TSENSOR driver of MBXXX board.
  * @{
  */ 

/** @defgroup MBXXX_ADC_TSENSOR_Private_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MBXXX_ADC_TSENSOR_Private_Defines
  * @{
  */  
/**
  * @}
  */ 


/** @defgroup MBXXX_ADC_TSENSOR_Private_Macros
  * @{
  */
/**
  * @}
  */ 
  

/** @defgroup MBXXX_ADC_TSENSOR_Private_Variables
  * @{
  */
__IO int16_t ADCDATA;
__IO int16_t Voltage;
__IO int16_t Vout;
__IO int16_t Temperature;
int16_t adc_ChopCorr;
int16_t adcval;
uint16_t adc_gain_global;
int16_t adc_offset_global;
__IO int16_t adc_offset_hv_global;
int16_t i;
ADC_InitTypeDef ADC_InitStruct;

/**
  * @}
  */ 

/** @defgroup MBXXX_ADC_TSENSOR_Private_Function_Prototypes
  * @{
  */ 
static uint16_t gain_correction(void);
static int16_t offset_correction(void);
static int16_t offset_hv_correction(void);

/**
  * @}
  */ 

/** @defgroup MBXXX_ADC_TSENSOR_Private_Functions
  * @{
  */ 

/**
  * @brief  DeInitializes peripherals used by the ADC TSENSOR driver.
  * @param  None
  * @retval None
  */
void STLM20_DeInit(void)
{
  STLM20_LowLevel_DeInit(); 
}

/**
  * @brief  Initializes peripherals used by the ADC TSENSOR driver.
  * @param  None
  * @retval None
  */
void STLM20_Init(void)
{ 
  STLM20_LowLevel_Init();
  
  /* ADC configuartion */
  adc_gain_global = gain_correction();
  adc_offset_hv_global = offset_hv_correction();
  
  ADC_DMA_ChannelReset();   
  ADC_DMA_Config((uint32_t)&ADCDATA, 1);
  ADC_InitStruct.ADC_Gain = adc_gain_global;
  ADC_Init(&ADC_InitStruct);
  adc_offset_global = offset_correction();
  ADC_InitStruct.ADC_Offset = adc_offset_global;
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;  
  ADC_InitStruct.ADC_VoltageP = ADC_VoltageP_High;  
  ADC_InitStruct.ADC_DMAMode = ADC_DMAMode_Linear;
  ADC_InitStruct.ADC_Clock = ADC_Clock_1MHz;
  
/* Single-ended conversion of PA4 */
  ADC_InitStruct.ADC_Input =  ADC_SOURCE_ADC4_VREF2;
  ADC_Init(&ADC_InitStruct);
  
  ADC_Cmd(ENABLE);   
   
  /* Chopper */
  ADC_InitStruct.ADC_VoltageN = ADC_VoltageN_High;
  ADC_Init(&ADC_InitStruct);
}  

/**
  * @brief  Read Temperature from STLM20
  * @param  None
  * @retval STLM20 measured temperature value
  */
int16_t STLM20_ReadTemp(void)
{
    for(i=0; i<5; i++)
    {  
      /* Clear the ADC buffer full flag */
      ADC_ClearITPendingBit(ADC_IT_DMABF);
      /* Start DMA */
      ADC_DMA_ChannelLoadEnable();
      
      /* Wait until the ADC_IT_DMABF bit is set in ADC_ISR */
      while (!( ADC->ISR & ADC_IT_DMABF)); 
    }   
    /* read the result from ADCDATA */
    adcval = ADCDATA;
    adcval<<=1;
    adc_ChopCorr = adcval + adc_offset_hv_global;  
    
    /* converting adc result to volts */
    /* Voltage = adc_result(adc_ChopCorr) mltiplied by Vref*2/2^16-1 */
    /* Vref = 1.2*1000 = 1200 makes the result(Voltage) units of mVolts */
    
    Voltage = (int16_t)(adc_ChopCorr*1200*2/65535);
    Vout = (int16_t)(Voltage + 600)*4*204/100;
    
    Temperature = (1864100 - (int16_t)Vout*1000)/11710;
  
  return  (int16_t) Temperature;
}
/**
  * @brief  ADC gain correction.
  * @param  None
  * @retval the corrected value for the gain
  */
uint16_t gain_correction(void)
{
  uint16_t Nvref; 
  uint16_t Ngnd;
  uint16_t adc_gain;

  ADC_DMA_ChannelReset(); 
  ADC_DMA_Config((uint32_t)&ADCDATA, 1);
  ADC_InitStruct.ADC_Offset = 0;
  ADC_InitStruct.ADC_Gain = 0x00008000;  
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_VoltageP = ADC_VoltageP_Low;
  ADC_InitStruct.ADC_VoltageN = ADC_VoltageN_Low;
  ADC_InitStruct.ADC_Input = ADC_SOURCE_VREF_VREF2;
  ADC_InitStruct.ADC_DMAMode = ADC_DMAMode_Linear;
  ADC_InitStruct.ADC_Clock = ADC_Clock_1MHz;
  ADC_Init(&ADC_InitStruct);
  
  ADC_Cmd(ENABLE);
 
 /* Clear the ADC buffer full flag */
  ADC_ClearITPendingBit(ADC_IT_DMABF);
  ADC_DMA_ChannelLoadEnable();
 
 /* Wait until the ADC_IT_DMABF bit is set in ADC_ISR */
  while (!( ADC->ISR & ADC_IT_DMABF));   

 /* VREF sampling */
  Nvref = ADCDATA;  
   
  ADC_Cmd(DISABLE);  
  ADC_InitStruct.ADC_Input = ADC_SOURCE_GND_VREF2;
  ADC_Init(&ADC_InitStruct);  
  ADC_Cmd(ENABLE);
  
  /* Clear the ADC buffer full flag */
  ADC_ClearITPendingBit(ADC_IT_DMABF);
  ADC_DMA_ChannelLoadEnable(); 
 /* Wait until the ADC_IT_DMABF bit is set in ADC_ISR */
  while (!( ADC->ISR & ADC_IT_DMABF));   

 /* GND sampling */
  Ngnd = (ADCDATA^0xFFFF);
  
  ADC_Cmd(DISABLE);
  adc_gain = (uint16_t)(32768*16384/(Nvref+Ngnd));
  
  return adc_gain;
}

/**
  * @brief  ADC offset correction.
  * @param  None
  * @retval the corrected value for the offset
  */
int16_t offset_correction(void)
{
  uint16_t Ngnd;
  int16_t adc_offset;
  
  ADC_DMA_ChannelReset(); 
  ADC_DMA_Config((uint32_t)&ADCDATA, 1);

  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_VoltageP = ADC_VoltageP_Low;
  ADC_InitStruct.ADC_VoltageN = ADC_VoltageN_Low;
  ADC_InitStruct.ADC_Input = ADC_SOURCE_GND_VREF2;
  ADC_InitStruct.ADC_DMAMode = ADC_DMAMode_Linear;
  ADC_InitStruct.ADC_Clock = ADC_Clock_1MHz;
  ADC_Init(&ADC_InitStruct);
  
  ADC_Cmd(ENABLE);
 
 /* Clear the ADC buffer full flag */
  ADC_ClearITPendingBit(ADC_IT_DMABF);
  ADC_DMA_ChannelLoadEnable();
 
 /* Wait until the ADC_IT_DMABF bit is set in ADC_ISR */
  while (!( ADC->ISR & ADC_IT_DMABF));
  
  Ngnd=ADCDATA;
  ADC_Cmd(DISABLE);  
  adc_offset = (int16_t)(2*(57344-Ngnd));
  
  return adc_offset;
}

/**
  * @brief  ADC high voltage offset correction.
  * @param  None
  * @retval the corrected value for the high voltage offset
  */
int16_t offset_hv_correction(void)
{

  int16_t adc_offset_hv;
  int32_t adc_data_p;
  int32_t adc_data_n;
  
  ADC_DMA_ChannelReset(); 
  ADC_DMA_Config((uint32_t)&ADCDATA, 1);

  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_VoltageP = ADC_VoltageP_High;
  ADC_InitStruct.ADC_VoltageN = ADC_VoltageN_High;
  ADC_InitStruct.ADC_Input = ADC_SOURCE_VREF2_VREF2;
  ADC_InitStruct.ADC_DMAMode = ADC_DMAMode_Linear;
  ADC_InitStruct.ADC_Clock = ADC_Clock_1MHz;
  ADC_Init(&ADC_InitStruct);

  /* meas ota_out_p/4 */
  ADC_Cmd(ENABLE);
 
 /* Clear the ADC buffer full flag */
  ADC_ClearITPendingBit(ADC_IT_DMABF);
  ADC_DMA_ChannelLoadEnable();
 
 /* Wait until the ADC_IT_DMABF bit is set in ADC_ISR */
  while (!( ADC->ISR & ADC_IT_DMABF));
  
  ADC_Cmd(DISABLE);
  
  adc_data_p=ADCDATA;
  
 /* meas ota_out_n/4 */
  ADC_InitStruct.ADC_VoltageN = ADC_VoltageN_Low;
  ADC_Init(&ADC_InitStruct);
  
  ADC_Cmd(ENABLE);

   /* Clear the ADC buffer full flag */
  ADC_ClearITPendingBit(ADC_IT_DMABF);
  ADC_DMA_ChannelLoadEnable();
 
 /* Wait until the ADC_IT_DMABF bit is set in ADC_ISR */
  while (!( ADC->ISR & ADC_IT_DMABF));
  
  ADC_Cmd(DISABLE);
  
  adc_data_n=ADCDATA;
  
  adc_offset_hv = (adc_data_n-adc_data_p);
  /* meas should be multiplied by 2,
     offset should be divided by 2,
     so do nothing */
  
  return adc_offset_hv;
  
}

/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */  


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
