/**
  ******************************************************************************
  * @file    mbxxx_i2c_ee.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file provides a set of functions needed to manage an I2C M24LR64 
  *          EEPROM memory.
  *          
  *          ===================================================================      
  *          Notes: 
  *           - This driver is intended for STM32W108 families devices only.     
  *          ===================================================================
  *              
  *          It implements a high level communication layer for read and write 
  *          from/to this memory. The needed STM32W108xx hardware resources (I2C and 
  *          GPIO) are defined in mb851.h and mb954 files, and the initialization is 
  *          performed in sEE_LowLevel_Init() function declared in mb851.c and mb954.c
  *          files.
  *          You can easily tailor this driver to any other development board, 
  *          by just adapting the defines for hardware resources and 
  *          sEE_LowLevel_Init() function. 
  *        
  *          @note In this driver, basic read and write functions (sEE_ReadBuffer() 
  *                and sEE_WritePage()) use Polling mode to perform the data transfer 
  *                to/from EEPROM memory.
  *            
  *     +-----------------------------------------------------------------+
  *     |               Pin assignment for M24M01 EEPROM                  |                 
  *     +---------------------------------------+-----------+-------------+
  *     |  STM32X108xx I2C Pins                 |   sEE     |   Pin       |
  *     +---------------------------------------+-----------+-------------+
  *     | .                                     |   NC      |    1        |
  *     | .                                     |   E1(VDD) |    2 (3.3V) |
  *     | .                                     |   E2(GND) |    3  (0V)  |
  *     | .                                     |   VSS     |    4  (0V)  |
  *     | sEE_I2C_SDA_PIN/ SDA                  |   SDA     |    5        |
  *     | sEE_I2C_SCL_PIN/ SCL                  |   SCL     |    6        |
  *     | .                                     |   /WC(VSS)|    7 (0V)   |
  *     | .                                     |   VDD     |    8 (3.3V) |
  *     +---------------------------------------+-----------+-------------+  
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
#include "mbxxx_i2c_ee.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup COMMON
  * @{
  */
  
/** @addtogroup MBXXX_I2C_EE
  * @brief      This file includes the I2C EEPROM driver of MBXXX board.
  * @{
  */ 

/** @defgroup MBXXX_I2C_EE_Private_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MBXXX_I2C_EE_Private_Defines
  * @{
  */  
/**
  * @}
  */ 


/** @defgroup MBXXX_I2C_EE_Private_Macros
  * @{
  */
/**
  * @}
  */ 
  

/** @defgroup MBXXX_I2C_EE_Private_Variables
  * @{
  */
__IO uint16_t  sEEAddress = 0;   
__IO uint32_t  sEETimeout = sEE_LONG_TIMEOUT;
__IO uint16_t  sEEDataNum;
/**
  * @}
  */ 


/** @defgroup MBXXX_I2C_EE_Private_Function_Prototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MBXXX_I2C_EE_Private_Functions
  * @{
  */ 

/**
  * @brief  DeInitializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_DeInit(void)
{
  sEE_LowLevel_DeInit(); 
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_Init(void)
{ 
  I2C_InitTypeDef  I2C_InitStructure;
  
  sEE_LowLevel_Init();
  
  /*!< I2C configuration */
  /* sEE_I2C configuration */
  I2C_InitStructure.I2C_ClockRate = 400000;
  
  /* Apply sEE_I2C configuration after enabling it */
  I2C_Init(sEE_I2C, &I2C_InitStructure);
   
  /* sEE_I2C Peripheral Enable */
  I2C_Cmd(sEE_I2C, ENABLE);
  
  /*!< Select the EEPROM address */
  sEEAddress = sEE_HW_ADDRESS;      
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  pBuffer : pointer to the buffer that receives the data read from 
  *         the EEPROM.
  * @param  ReadAddr : EEPROM's internal address to start reading from.
  * @param  NumByteToRead : pointer to the variable holding number of bytes to 
  *         be read from the EEPROM.
  *
  * @retval sEE_OK (0) if operation is correctly performed, else return value 
  *         different from sEE_OK (0) or the timeout user callback.
  */
uint32_t sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{  
  uint16_t numberofbyteread = 0x00, numberofbytetoread = 0x00;
  
  numberofbytetoread = NumByteToRead;
  /* Generate start */
  I2C_GenerateSTART(sEE_I2C);
  
  /* Wait until CMDFIN flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /*!< Send EEPROM address for write */
  I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
  
  /* Wait until BTF flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  } 
  
  /*!< Send the EEPROM's internal address to read from: MSB of the address first */
  I2C_SendData(sEE_I2C, (uint8_t)((ReadAddr & 0xFF00) >> 8));    
  
  /* Wait until BTF flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /*!< Send the EEPROM's internal address to read from: LSB of the address */
  I2C_SendData(sEE_I2C, (uint8_t)(ReadAddr & 0x00FF));    
  
  /* Wait until BTF flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  } 
  
  /*!< Send STRAT condition a second time */  
  I2C_GenerateSTART(sEE_I2C);
  
  /* Wait until CMDFIN flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /*!< Send EEPROM address for read */
  I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Receiver);
  
  /* Wait until BTF flag is set */
  sEETimeout = sEE_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  } 
  
  /*!< While there is data to be read */
  while(numberofbyteread != numberofbytetoread)  
  {
    if (numberofbyteread == numberofbytetoread - 1)
    {
      /*!< Enable Acknowledgement: send ACK when data received */
      I2C_AcknowledgeConfig(sEE_I2C, DISABLE);
    }
    else
    {
      /*!< Disable Acknowledgement: don't send ACK for the last byte */
      I2C_AcknowledgeConfig(sEE_I2C, ENABLE);
    }
    
    /*!< Read a byte from the EEPROM */
    *pBuffer = I2C_ReceiveData(sEE_I2C);
    
    /* Wait until BRF flag is set */
    sEETimeout = sEE_FLAG_TIMEOUT;
    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BRF) != SET)
    {
      if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
    } 
    
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++; 
    
    /*!< Decrement the read bytes counter */
    numberofbyteread++;
  }
  
  /*!< Send STOP Condition */
  I2C_GenerateSTOP(sEE_I2C);
  
  /* Wait until CMDFIN flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /*!< Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(sEE_I2C, ENABLE);
  
  /* If all operations OK, return sEE_OK (0) */
  return sEE_OK;
}

/**
  * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle.
  *
  * @note   The number of bytes (combined to write start address) must not 
  *         cross the EEPROM page boundary. This function can only write into
  *         the boundaries of an EEPROM page.
  *         This function doesn't check on boundaries condition (in this driver 
  *         the function sEE_WriteBuffer() which calls sEE_WritePage() is 
  *         responsible of checking on Page boundaries).
  * 
  * @param  pBuffer : pointer to the buffer containing the data to be written to 
  *         the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : pointer to the variable holding number of bytes to 
  *         be written into the EEPROM.
  *
  * @retval sEE_OK (0) if operation is correctly performed, else return value 
  *         different from sEE_OK (0) or the timeout user callback.
  */
uint32_t sEE_WritePage(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t* NumByteToWrite)
{   
  uint32_t DataNum = 0;
  
  /* Generate start */
  I2C_GenerateSTART(sEE_I2C);
  
  /* Wait until CMDFIN flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /*!< Send EEPROM address for write */
  I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
  
  /* Wait until BTF flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  } 
  
  /*!< Send the EEPROM's internal address to read from: MSB of the address first */
  I2C_SendData(sEE_I2C, (uint8_t)((WriteAddr & 0xFF00) >> 8));    
  
  /* Wait until BTF flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  } 
  
  /*!< Send the EEPROM's internal address to read from: LSB of the address */
  I2C_SendData(sEE_I2C, (uint8_t)(WriteAddr & 0x00FF));    
  
  /* Wait until BTF flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  } 
  
  while (DataNum != (*NumByteToWrite))
  {      
    /* Write data to DR */
    I2C_SendData(sEE_I2C, (uint8_t)pBuffer[DataNum]);  
    
    /* Update number of transmitted data */
    DataNum++;
    
    /* Wait until BTF flag is set */
    sEETimeout = sEE_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
    {
      if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
    } 
  }
  
  /*!< Send STOP Condition */
  I2C_GenerateSTOP(sEE_I2C);
  
  /* Wait until STOPF flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }   
  
  /* If all operations OK, return sEE_OK (0) */
  return sEE_OK;
}

/**
  * @brief  Writes buffer of data to the I2C EEPROM.
  * @param  pBuffer : pointer to the buffer  containing the data to be written 
  *         to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the EEPROM.
  * @retval None
  */
void sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;
  
  Addr = WriteAddr % sEE_PAGESIZE;
  count = sEE_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
  NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
  
  /*!< If WriteAddr is sEE_PAGESIZE aligned  */
  if(Addr == 0) 
  {
    /*!< If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage == 0) 
    {
      /* Store the number of data to be written */
      sEEDataNum = NumOfSingle;
      /* Start writing data */
      sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum));
      sEE_WaitEepromStandbyState();
    }
    /*!< If NumByteToWrite > sEE_PAGESIZE */
    else  
    {
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;        
        sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum)); 
        sEE_WaitEepromStandbyState();
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;
      }
      
      if(NumOfSingle!=0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;          
        sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum));
        sEE_WaitEepromStandbyState();
      }
    }
  }
  /*!< If WriteAddr is not sEE_PAGESIZE aligned  */
  else 
  {
    /*!< If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage== 0) 
    {
      /*!< If the number of data to be written is more than the remaining space 
      in the current page: */
      if (NumByteToWrite > count)
      {
        /* Store the number of data to be written */
        sEEDataNum = count;        
        /*!< Write the data contained in same page */
        sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum));
        sEE_WaitEepromStandbyState();      
        
        /* Store the number of data to be written */
        sEEDataNum = (NumByteToWrite - count);          
        /*!< Write the remaining data in the following page */
        sEE_WritePage((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint8_t*)(&sEEDataNum));
        sEE_WaitEepromStandbyState();        
      }      
      else      
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;         
        sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum));
        sEE_WaitEepromStandbyState();        
      }     
    }
    /*!< If NumByteToWrite > sEE_PAGESIZE */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
      NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
      
      if(count != 0)
      {  
        /* Store the number of data to be written */
        sEEDataNum = count;         
        sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum));
        sEE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;          
        sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum));
        sEETimeout = sEE_LONG_TIMEOUT;
        sEE_WaitEepromStandbyState();
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;           
        sEE_WritePage(pBuffer, WriteAddr, (uint8_t*)(&sEEDataNum)); 
        sEE_WaitEepromStandbyState();
      }
    }
  }  
}

/**
  * @brief  Wait for EEPROM Standby state.
  * 
  * @note  This function allows to wait and check that EEPROM has finished the 
  *        last operation. It is mostly used after Write operation: after receiving
  *        the buffer to be written, the EEPROM may need additional time to actually
  *        perform the write operation. During this time, it doesn't answer to
  *        I2C packets addressed to it. Once the write operation is complete
  *        the EEPROM responds to its address.
  * 
  * @param  None
  *
  * @retval sEE_OK (0) if operation is correctly performed, else return value 
  *         different from sEE_OK (0) or the timeout user callback.
  */
uint32_t sEE_WaitEepromStandbyState(void)      
{
  __IO uint32_t sEETrials = 0;
  
  /* Keep looping till the slave acknowledge his address or maximum number 
  of trials is reached (this number is defined by sEE_MAX_TRIALS_NUMBER define
  in mbxxx_i2c_ee.h file) */
  do
  { 
    /* Initialize sEETimeout */
    sEETimeout = sEE_FLAG_TIMEOUT;
    
    if (sEETrials != 0)
    {
      /*!< Send STOP Condition */
      I2C_GenerateSTOP(sEE_I2C);
      
      /* Wait until CMDFIN flag is set */
      sEETimeout = sEE_LONG_TIMEOUT;
      while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
      {
        if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
      }
    }
    
    /* Generate start */
    I2C_GenerateSTART(sEE_I2C);
    
    /* Wait until CMDFIN flag is set */
    sEETimeout = sEE_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
    {
      if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
    }
    
    I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
    /* Wait until BTF flag is set */
    sEETimeout = sEE_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) != SET)
    {
      if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
    } 
    
    /* Wait until timeout elapsed */
    while (sEETimeout-- != 0); 
    
    /* Check if the maximum allowed numbe of trials has bee reached */
    if (sEETrials++ == sEE_MAX_TRIALS_NUMBER)
    {
      /* If the maximum number of trials has been reached, exit the function */
      return sEE_TIMEOUT_UserCallback();
    }
  }while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_NACK) == SET); 
  
  /*!< Send STOP Condition */
  I2C_GenerateSTOP(sEE_I2C);
  
  /* Wait until CMDFIN flag is set */
  sEETimeout = sEE_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_CMDFIN) != SET)
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /* Return sEE_OK if device is ready */
  return sEE_OK;
}

#ifdef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
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
