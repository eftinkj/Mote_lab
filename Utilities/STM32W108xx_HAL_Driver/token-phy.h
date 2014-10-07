/**
  ******************************************************************************
  * @file    token-phy.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Definitions for phy tokens.
  * @note    The file token-phy.h should not be included directly. 
  *          It is accessed by the other token files.   
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

/** @addtogroup HAL
  * @{
  */

/** @defgroup token-phy
  * @{
  */
  
#ifndef DEFINE_INDEXED_TOKEN
  #define DEFINE_INDEXED_TOKEN(name, type, arraysize, ...)  \
    TOKEN_DEF(name, CREATOR_##name, 0, 1, type, (arraysize),  __VA_ARGS__)
#endif


#if defined PHY_STM32W108XX 

#ifdef DEFINETYPES
typedef struct {
  uint8_t vcoAtLna; /* the VCO tune value at the time LNA value was calculated. */
  uint8_t modDac;   /* msb : cal needed , bit 0-5 : value */
  uint8_t filter;   /* msb : cal needed , bit 0-4 : value */
  uint8_t lna;      /* msb : cal needed , bit 0-5 : value */
} tokTypeStackCalData;
#endif 


#ifdef DEFINETOKENS

#define CREATOR_STACK_CAL_DATA         0xD243 /* msb+'R'+'C' (Radio Calibration) */
#define STACK_CAL_DATA_ARRAY_SIZE       16


/* This breaks the simulated eeprom unit test, so we ifdef it out
   of existence. */
#ifndef ST_TEST
DEFINE_INDEXED_TOKEN(STACK_CAL_DATA,
                     tokTypeStackCalData, STACK_CAL_DATA_ARRAY_SIZE,
                     { 
                       0xff, /* vcoAtLna default: invalid VCO value. */
                       0x80, /* modDac default: cal needed. */
                       0x80, /* filter default: cal needed. */
                       0x80  /* lna default: cal needed. */
                     })
#endif /* ST_TEST */

#endif /* DEFINETOKENS */

#endif /* defined PHY_STM32W108XX */ 

/**
  * @} 
  */
  
/**
  * @} 
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
