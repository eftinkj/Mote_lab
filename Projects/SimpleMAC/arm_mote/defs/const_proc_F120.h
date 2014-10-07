#ifndef CONST_PROC_F120_H
#define CONST_PROC_F120_H

#define	HWM_INT_EXT0  				0
#define	HWM_INT_TIMER0_OVFLW		1	
#define	HWM_INT_EXT1				2
#define	HWM_INT_TIMIER1_OVFLW		3
#define	HWM_INT_UART0				4
#define	HWM_INT_TIMER2				5
#define	HWM_INT_SPI					6
#define	HWM_INT_SMB					7
#define	HWM_INT_ADC0_WNDW_COMP		8
#define	HWM_INT_PCA					9
#define	HWM_INT_COMP0_FALL_EDGE		10
#define	HWM_INT_COMP0_RISE_EDGE		11
#define	HWM_INT_COMP1_FALL_EDGE		12
#define	HWM_INT_COMP1_RISE_EDGE		13
#define	HWM_INT_TIMER3				14
#define	HWM_INT_ADC0_EOC			15
#define	HWM_INT_TIMER4				16
#define	HWM_INT_ADC2_WNDW_COMP		17
#define	HWM_INT_ADC2_EOC			18
#define	HWM_INT_RESERVED			19
#define	HWM_INT_UART1				20



#define TSK_disable_all_interrupts() {EA = 0; EA = 0;}
#define TSK_enable_all_interrupts() (EA = 1)



// ####################################################
// ### Features of F120
// ####################################################
// F120 has SFR pages
#define _SFRPAGE_EXIST_


// ####################################################
// ### Hardware specific configuration
// ####################################################
#define RTC_TMR_IFLAG	TF4
#define RTC_TMR_INT		HWM_INT_TIMER4


// SFR PAGES:
#define RTC_TMR_PAGE TMR4_PAGE


#endif // CONST_PROC_F120_H
