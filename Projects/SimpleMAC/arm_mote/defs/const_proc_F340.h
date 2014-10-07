#ifndef CONST_PROC_F310_H
#define CONST_PROC_F310_H


// ####################################################
// ### Interrupts vector
// ####################################################
#define	HWM_INT_EXT0  				0
#define	HWM_INT_TIMER0_OVFLW		1	
#define	HWM_INT_EXT1				2
#define	HWM_INT_TIMIER1_OVFLW		3
#define	HWM_INT_UART0				4
#define	HWM_INT_TIMER2				5
#define	HWM_INT_SPI					6
#define	HWM_INT_SMB					7
#define HWM_INT_USB0				8
#define	HWM_INT_ADC0_WNDW_COMP		9
#define	HWM_INT_ADC0_EOC			10
#define	HWM_INT_PCA					11
#define	HWM_INT_COMP0_FALL_EDGE		12
#define	HWM_INT_COMP0_RISE_EDGE		12
#define	HWM_INT_COMP1_FALL_EDGE		13
#define	HWM_INT_COMP1_RISE_EDGE		13
#define	HWM_INT_TIMER3				14
#define HWM_INT_VBUS				15
#define HWM_INT_UART1				16


// ####################################################
// ### 
// ####################################################
#define SPIBSY_MASK		0x80


#define TSK_disable_all_interrupts() {EA = 0; EA = 0;}
#define TSK_enable_all_interrupts() (EA = 1)

// ####################################################
// ### Features of F340:
// ####################################################
#undef _SFRPAGE_EXIST_


// ####################################################
// ### Hardware specific configuration
// ####################################################
//#define RTC_TMR_IFLAG	TF0
//#define RTC_TMR_INT		HWM_INT_TIMER0_OVFLW


#define F340_TF3H	0x80

//sbit YLED1 = P2^2;
//sbit YLED2 = P2^3;

#endif // CONST_PROC_F120_H
