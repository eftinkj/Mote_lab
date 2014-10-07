/**
  ******************************************************************************
  * @file    bootloader_demo.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This file contains all the functions prototypes for the bootloader_demo 
  *          firmware library.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOTLOADER_DEMO_H
#define BOOTLOADER_DEMO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32w108xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* WDG activation function ****************************************************/
void setDestEui64Action(void);
void getDestEui64Action(void);
void loadImageAction(void);
void getCommandAction(void);
void getIdCommandAction(void);
void getVersionCommandAction(void);
void readCommandAction(void);
void writeCommandAction(void);
void eraseCommandAction(void);
void goAddressCommandAction(void);
void writeIncrementalCommandAction(void);
void broadcastGetCommandAction(void);
void broadcastGetIdCommandAction(void);
void broadcastGetVersionCommandAction(void);
void findNodesInBootloaderModeAction(void);
void helpAction(void);
#endif /* BOOTLOADER_DEMO_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
