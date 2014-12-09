
#include "lcd_char2x8.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/

/**
* lcd28_func_name( params ) - description of function in module (lcd28)
*	RETURN: 1 (SPI_R_OK) - if request has been fulfilled (SPI is freed by app)
*			0 (SPI_R_FAIL) - if unsucceflul (e.g. used by another app or free)
*/
void lcd28_clear(uint8_t count)
{
}


/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

/**
  * lcd28_power_up() - sets up lcd28 module
  */
void lcd28_power_up ( void )
{
	// Internal variables initializtion (nothing from outside is assumed)
}

/**
  * lcd28_init() - sets up lcd28 module
  */
void lcd28_init ( void )
{
	// External initialization (e.g. setup timers, connect to other components)
		GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure the GPIO_LED pin */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIOC->BSR = GPIO_Pin_2; // Set pin (1)
//	GPIOC->BRR = GPIO_Pin_2; // Reset pin (0)
  
  
	//GPIO_InitTypeDef  GPIO_InitStructure; 
	/* sEE_I2C Peripheral Disable */
	//I2C_Cmd(SC2_I2C, DISABLE);
 	/* _I2C DeInit */
	//I2C_DeInit(SC2_I2C);
	
	
	/*!< GPIO configuration */  
 	/*!< Configure _I2C pins: SCL */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Set pull-up on sEE_I2C SCL pin */
	GPIO_SetBits(GPIOA, GPIO_Pin_2);

	/*!< Configure _I2C pins: SDA */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Set pull-up on sEE_I2C SDA pin */
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
 
	TIM2->CCER &= 0xFFFFEEEE; 
  
	//SC_I2C_TypeDef* SCx_I2C;
	I2C_InitTypeDef I2C_InitStruct;	// Create structure for SC2_I2C)
	//uint8_t Address = LCD_I2C_ADDR;//0x3e; // LCD Address default?? //0x112;
	//uint8_t Data = 0xAA;
	uint8_t contrast = 35;
	
	I2C_StructInit(&I2C_InitStruct); // Initialize SC2_I2C structure with defaults
	I2C_InitStruct.I2C_ClockRate = 100000; // 100kHz
	I2C_Init(SC2_I2C, &I2C_InitStruct); // Initialize SC2 to work as I2C
	I2C_Cmd(SC2_I2C, ENABLE); // ENABLE or DISABLE SC2_I2C peripheral
	
	I2C_AcknowledgeConfig(SC2_I2C, ENABLE); /// Enable ACK mode
//	I2C_AcknowledgeConfig(SC2_I2C, DISABLE); /// Enable ACK mode

	
	halCommonDelayMilliseconds(10);

	lcd_cmd ( 0x38 );// 0b0011 1000 ) ; // function set
	lcd_cmd ( 0x39 );// 0b0011 1001 ) ; // function set
	lcd_cmd ( 0x04 );// 0b0000 0100 ) ; // EntryModeSet
	lcd_cmd ( 0x14 );// 0b0001 0100 ) ; // interval osc
	lcd_cmd ( 0x70 | ( contrast & 0xF ) );// 0b0111 0000 | ( contrast & 0xF ) ) ; // contrast Low
	lcd_cmd ( 0x5C | ( ( contrast >> 4 ) & 0x3 ) );// 0b0101 1100 | ( ( contrast >> 4 ) & 0x3 ) ) ; // contast High/icon/power
	lcd_cmd ( 0x6C );// 0b0110 1100 ) ; // follower control
	//delay ( 200 ) ;
	halCommonDelayMilliseconds(100);
	lcd_cmd ( 0x38 );// 0b0011 1000 ) ; // function set
	lcd_cmd ( 0x0C );// 0b0000 1100 ) ; // Display On
	lcd_cmd ( 0x01 );// 0b0000 0001 ) ; // Clear Display
	//delay ( 2 ) ; 
	halCommonDelayMilliseconds(2);	

	// Tryout displau
	lcd_setCursor ( 0 , 0 ) ;
	lcd_printStr ( "SWITCH" ) ; 
	
//	I2C_GenerateSTART(SC2_I2C);
//	I2C_Send7bitAddress(SC2_I2C, Address, I2C_Direction_Transmitter); // I2C_Direction_Transmitter or I2C_Direction_Receiver
//	I2C_SendData(SC2_I2C, Data);
//	Data=I2C_ReceiveData(SC2_I2C);
	
	
	
	/**
  * @brief  Checks whether the specified I2C flag is set or not.
  * @param  SCx_I2C: where x can be 1 or 2 to select the Serial controller peripheral.
  * @param  I2C_FLAG: specifies the SCx_I2C flag to check.
  *   This parameter can be one of the following values:
  *     @arg I2C_FLAG_NACK: Not acknowledge flag
  *     @arg I2C_FLAG_BTF: Byte transfer finished flag
  *     @arg I2C_FLAG_BRF: Byte receive finished flag
  *     @arg I2C_FLAG_CMDFIN: Command finished flag
  * @retval The new state of I2C_FLAG (SET or RESET).
  */
//FlagStatus I2C_GetFlagStatus(SC_I2C_TypeDef* SCx_I2C, uint32_t I2C_FLAG);

//	I2C_GenerateSTOP(SC2_I2C);
	
	
	sch_add_loop(lcd28_loop);
}

/**
  * lcd28_loop() - sets up lcd28 module
  */
void lcd28_loop ( void )
{
}




// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################
