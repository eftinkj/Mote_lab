/**
  ******************************************************************************
  * @file    mbxxx_i2c_lis302dl.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file provides a set of functions needed to manage the LIS302DL
  *          MEMS accelerometer available on MBxxx board.
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
#include "mbxxx_i2c_lis302dl.h"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup COMMON
  * @{
  */
  
/** @addtogroup MBXXX_I2C_LIS302DL
  * @{
  */


/** @defgroup MBXXX_I2C_LIS302DL_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup MBXXX_I2C_LIS302DL_Private_Defines
  * @{
  */
__IO uint8_t   LIS302DLAddress = 0; 
__IO uint32_t  LIS302DLTimeout = LIS302DL_FLAG_TIMEOUT;

/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x80)

/**
  * @}
  */

/** @defgroup MBXXX_I2C_LIS302DL_Private_Macros
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup MBXXX_I2C_LIS302DL_Private_Variables
  * @{
  */ 

/**
  * @}
  */

/** @defgroup MBXXX_I2C_LIS302DL_Private_FunctionPrototypes
  * @{
  */
static uint8_t LIS302DL_SendFrame(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
static uint8_t LIS302DL_ReceiveFrame(uint8_t* pBuffer, uint8_t ReadAddr, uint8_t NumByteToRead);

/**
  * @}
  */

/** @defgroup MBXXX_I2C_LIS302DL_Private_Functions
  * @{
  */


/**
  * @brief  Set LIS302DL Initialization.
  * @param  LIS302DL_InitStruct: pointer to a LIS302DL_InitTypeDef structure 
  *         that contains the configuration setting for the LIS302DL.
  * @retval None
  */
void LIS302DL_Init(LIS302DL_InitTypeDef *LIS302DL_InitStruct)
{
  uint8_t ctrl = 0x00;
  
  /* Configure the low level interface ---------------------------------------*/
  LIS302DL_LowLevel_Init();
  
  /*!< Select the MEMS address */
  LIS302DLAddress = LIS302DL_HW_ADDRESS; 
    
  /* Clear the MEMS STATUS regsister */
  LIS302DL_Write(&ctrl, LIS302DL_STATUS_REG_ADDR, 1);
  
  /* Clear the MEMS FF_WU_CFG1 regsister */
  LIS302DL_Write(&ctrl, LIS302DL_FF_WU_CFG1_REG_ADDR, 1);
  
  /* Clear the MEMS CLICK_CFG regsister */
  LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);
  
  /* Clear the MEMS CTRL_REG2 regsister */
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG2_ADDR, 1);
  
  /* Configure MEMS: data rate, power mode, full scale, self test and axes */
  ctrl = (uint8_t) (LIS302DL_InitStruct->Output_DataRate | LIS302DL_InitStruct->Power_Mode | \
                    LIS302DL_InitStruct->Full_Scale | LIS302DL_InitStruct->Self_Test | \
                    LIS302DL_InitStruct->Axes_Enable);
  
  /* Write value to MEMS CTRL_REG1 regsister */
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Set LIS302DL Internal High Pass Filter configuration.
  * @param  LIS302DL_FilterConfigStruct: pointer to a LIS302DL_FilterConfigTypeDef 
  *         structure that contains the configuration setting for the LIS302DL Filter.
  * @retval None
  */
void LIS302DL_FilterConfig(LIS302DL_FilterConfigTypeDef *LIS302DL_FilterConfigStruct)
{
  uint8_t ctrl = 0x00;
  
  /* Read CTRL_REG2 register */
  LIS302DL_Read(&ctrl, LIS302DL_CTRL_REG2_ADDR, 1);
  
  /* Clear high pass filter cut-off level, interrupt and data selection bits*/
  ctrl &= (uint8_t)~(LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER | \
                     LIS302DL_HIGHPASSFILTER_LEVEL_3 | \
                     LIS302DL_HIGHPASSFILTERINTERRUPT_1_2);
  /* Configure MEMS high pass filter cut-off level, interrupt and data selection bits */                     
  ctrl |= (uint8_t)(LIS302DL_FilterConfigStruct->HighPassFilter_Data_Selection | \
                    LIS302DL_FilterConfigStruct->HighPassFilter_CutOff_Frequency | \
                    LIS302DL_FilterConfigStruct->HighPassFilter_Interrupt);
  
  /* Write value to MEMS CTRL_REG2 register */
  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG2_ADDR, 1);
}

/**
  * @brief  Change the lowpower mode for LIS302DL
  * @param  LowPowerMode: new state for the lowpower mode.
  *   This parameter can be one of the following values:
  *     @arg LIS302DL_LOWPOWERMODE_POWERDOWN: Power down mode
  *     @arg LIS302DL_LOWPOWERMODE_ACTIVE: Active mode  
  * @retval None
  */
void LIS302DL_LowpowerCmd(uint8_t LowPowerMode)
{
  uint8_t tmpreg;
  
  /* Read CTRL_REG1 register */
  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
  
  /* Set new low power mode configuration */
  tmpreg &= (uint8_t)~LIS302DL_LOWPOWERMODE_ACTIVE;
  tmpreg |= LowPowerMode;
  
  /* Write value to MEMS CTRL_REG1 regsister */
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Data Rate command 
  * @param  DataRateValue: Data rate value
  *   This parameter can be one of the following values:
  *     @arg LIS302DL_DATARATE_100: 100 Hz output data rate 
  *     @arg LIS302DL_DATARATE_400: 400 Hz output data rate    
  * @retval None
  */
void LIS302DL_DataRateCmd(uint8_t DataRateValue)
{
  uint8_t tmpreg;
  
  /* Read CTRL_REG1 register */
  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
  
  /* Set new Data rate configuration */
  tmpreg &= (uint8_t)~LIS302DL_DATARATE_400;
  tmpreg |= DataRateValue;
  
  /* Write value to MEMS CTRL_REG1 regsister */
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Change the Full Scale of LIS302DL
  * @param  FS_value: new full scale value. 
  *   This parameter can be one of the following values:
  *     @arg LIS302DL_FULLSCALE_2_3: +-2.3g
  *     @arg LIS302DL_FULLSCALE_9_2: +-9.2g   
  * @retval None
  */
void LIS302DL_FullScaleCmd(uint8_t FS_value)
{
  uint8_t tmpreg;
  
  /* Read CTRL_REG1 register */
  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
  
  /* Set new full scale configuration */
  tmpreg &= (uint8_t)~LIS302DL_FULLSCALE_9_2;
  tmpreg |= FS_value;
  
  /* Write value to MEMS CTRL_REG1 regsister */
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Reboot memory content of LIS302DL
  * @param  None
  * @retval None
  */
void LIS302DL_RebootCmd(void)
{
  uint8_t tmpreg;
  /* Read CTRL_REG2 register */
  LIS302DL_Read(&tmpreg, LIS302DL_CTRL_REG2_ADDR, 1);
  
  /* Enable or Disable the reboot memory */
  tmpreg |= LIS302DL_BOOT_REBOOTMEMORY;
  
  /* Write value to MEMS CTRL_REG2 regsister */
  LIS302DL_Write(&tmpreg, LIS302DL_CTRL_REG2_ADDR, 1);
}

/**
  * @brief  Writes one byte to the LIS302DL.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the LIS302DL.
  * @param  WriteAddr : LIS302DL's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval LIS302DL_OK (0) if operation is correctly performed, else return value 
  *         different from LIS302DL_OK (0) or the timeout user callback.
  */
uint8_t LIS302DL_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
  /* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
  }
  
  /* Send the Address of the indexed register and the the data that will be written
  into the device */
  return LIS302DL_SendFrame(pBuffer, WriteAddr, NumByteToWrite);
}

/**
  * @brief  Reads a block of data from the LIS302DL.
  * @param  pBuffer : pointer to the buffer that receives the data read from the LIS302DL.
  * @param  ReadAddr : LIS302DL's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the LIS302DL.
  * @retval LIS302DL_OK (0) if operation is correctly performed, else return value 
  *         different from LIS302DL_OK (0) or the timeout user callback.
  */
uint8_t LIS302DL_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint8_t NumByteToRead)
{  
  uint8_t numberofbytetoread = 0x00, readaddr = 0x00;
  
  numberofbytetoread = NumByteToRead;
  readaddr = ReadAddr; 
  
  if(numberofbytetoread > 0x01)
  {
    readaddr |= (uint8_t)(MULTIPLEBYTE_CMD);
  }
  
  /* Send the Address of the indexed register */
   return LIS302DL_ReceiveFrame(pBuffer, readaddr, numberofbytetoread);
}

/**
  * @brief  It reads 3 axes acceleration data from mems
  * @param  out: buffer to store data
  * @retval None
  */
void LIS302DL_ReadACC(LIS302DL_DataTypeDef *LIS302DL_Data)
{
  uint8_t buffer[8];
     
  LIS302DL_Read(buffer, LIS302DL_OUT_X_ADDR_L, 8);
  
  LIS302DL_Data->outx_h = buffer[0];
  LIS302DL_Data->outx_l = buffer[1];
  LIS302DL_Data->outy_h = buffer[2];
  LIS302DL_Data->outy_l = buffer[3];
  LIS302DL_Data->outz_h = buffer[4];
  LIS302DL_Data->outz_l = buffer[5];
}

/**
  * @brief  Reads a block of data from the MEMS accelerometre.
  * @param  pBuffer : pointer to the buffer that receives the data read from 
  *         the MEMS.
  * @param  ReadAddr : MEMS's internal address to start reading from.
  * @param  NumByteToRead : pointer to the variable holding number of bytes to 
  *         be read from the MEMS.
  *
  * @retval LIS302DL_OK (0) if operation is correctly performed, else return value 
  *         different from LIS302DL_OK (0) or the timeout user callback.
  */
static uint8_t LIS302DL_ReceiveFrame(uint8_t* pBuffer, uint8_t ReadAddr, uint8_t NumByteToRead)
{
  uint8_t numberofbyteread = 0x00, numberofbytetoread = 0x00;
  
  numberofbytetoread = NumByteToRead;
  
  /* Generate start */
  I2C_GenerateSTART(LIS302DL_I2C);
  
  /* Wait until CMDFIN flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }
  
  /*!< Send MEMS address for write */
  I2C_Send7bitAddress(LIS302DL_I2C, LIS302DLAddress, I2C_Direction_Transmitter);
  
  /* Wait until BTF flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_BTF) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  } 
  
  /*!< Send the MEMS's internal address to read */
  I2C_SendData(LIS302DL_I2C, ReadAddr);    
  
  /* Wait until BTF flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_BTF) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }
  
  /*!< Send STRAT condition a second time */  
  I2C_GenerateSTART(LIS302DL_I2C);
  
  /* Wait until CMDFIN flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }
  
  /*!< Send MEMS address for read */
  I2C_Send7bitAddress(LIS302DL_I2C, LIS302DLAddress, I2C_Direction_Receiver);
  
  /* Wait until BTF flag is set */
  LIS302DLTimeout = LIS302DL_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_BTF) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  } 
  
  /*!< While there is data to be read */
  while(numberofbyteread != numberofbytetoread)  
  {
    if (numberofbyteread == numberofbytetoread - 1)
    {
      /*!< Enable Acknowledgement: send ACK when data received */
      I2C_AcknowledgeConfig(LIS302DL_I2C, DISABLE);
    }
    else
    {
      /*!< Disable Acknowledgement: don't send ACK for the last byte */
      I2C_AcknowledgeConfig(LIS302DL_I2C, ENABLE);
    }
    
    /*!< Read a byte from the MEMS */
    *pBuffer = I2C_ReceiveData(LIS302DL_I2C);
    
    /* Wait until BRF flag is set */
    LIS302DLTimeout = LIS302DL_FLAG_TIMEOUT;
    while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_BRF) != SET)
    {
      if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
    } 
    
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++; 
    
    /*!< Decrement the read bytes counter */
    numberofbyteread++;
  }
  
  /*!< Send STOP Condition */
  I2C_GenerateSTOP(LIS302DL_I2C);
  
  /* Wait until CMDFIN flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }
  
  /*!< Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(LIS302DL_I2C, ENABLE);
  
  /* If all operations OK, return LIS302DL_OK (0) */
  return LIS302DL_OK;
}

/**
  * @brief  Writes buffer of data to the I2C MEMS accelerometre.
  * @param  pBuffer : pointer to the buffer  containing the data to be written 
  *         to the MEMS.
  * @param  WriteAddr : MEMS's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the MEMS.
  * @retval LIS302DL_OK (0) if operation is correctly performed, else return value 
  *         different from LIS302DL_OK (0) or the timeout user callback.
  */
static uint8_t LIS302DL_SendFrame(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
  uint32_t DataNum = 0;
  
  /* Generate start */
  I2C_GenerateSTART(LIS302DL_I2C);
  
  /* Wait until CMDFIN flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }
  
  /*!< Send MEMS address for write */
  I2C_Send7bitAddress(LIS302DL_I2C, LIS302DLAddress, I2C_Direction_Transmitter);
  
  /* Wait until BTF flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_BTF) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  } 
  
  /*!< Send the MEMS's internal address to read from: LSB of the address */
  I2C_SendData(LIS302DL_I2C, WriteAddr);    
  
  /* Wait until BTF flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_BTF) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  } 
  
  while (DataNum != NumByteToWrite)
  {      
    /* Write data to DR */
    I2C_SendData(LIS302DL_I2C, (uint8_t)pBuffer[DataNum]);  
    
    /* Update number of transmitted data */
    DataNum++;
    
    /* Wait until BTF flag is set */
    LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_BTF) != SET)
    {
      if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
    } 
  }
  
  /*!< Send STOP Condition */
  I2C_GenerateSTOP(LIS302DL_I2C);
  
  /* Wait until CMDFIN flag is set */
  LIS302DLTimeout = LIS302DL_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(LIS302DL_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((LIS302DLTimeout--) == 0) return LIS302DL_TIMEOUT_UserCallback();
  }   
  
  /* If all operations OK, return LIS302DL_OK (0) */
  return LIS302DL_OK;
}

/**
  * @brief  It reads status register from Mems
  * @param  None
  * @retval I2C frame
  */
uint8_t LIS302DL_GetStatus(void)
{
  uint8_t tmpreg;
  /* Read STATUS register */
  LIS302DL_Read(&tmpreg, LIS302DL_STATUS_REG_ADDR, 1);
  return tmpreg;
}

#ifdef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LIS302DL_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}
#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */

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
