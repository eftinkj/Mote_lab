
/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/

#include "pwm8_gen.h"
#ifdef _ENABLE_PWM8_MODULE_

#include "sys_func.h"

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/

pwm8_duty_t	pwm8_duty_idle[PWM8_MAX_PWMS];
uint8_t	pwm8_count_pwm;


/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

/**
* pwm8_init(*config) - setup of the PWM generation based on PCA hardware
*	Accepts pointer to configuration structure
*/
void pwm8_init(pwm8_config_t *config)
{
	uint8_t i;
	if (NULL == config)
	{
		for(i=0;i<PWM8_MAX_PWMS; i++)
		{
			pwm8_duty_idle[i] = PWM8_DUTY_OFF;
		}
		pwm8_count_pwm = PWM8_MAX_PWMS;
	}
	else
	{
		if (PWM8_CONFIG_VER_01 == config->ver)
		{
			pwm8_duty_t *temp = (pwm8_duty_t *) &(config->idle);
			pwm8_count_pwm = config->length;
			for(i=0; i < pwm8_count_pwm ; i++)
			{
				pwm8_duty_idle[i] = temp[i];
			}
		}
		else
		{
			sys_error(SYS_ERROR_PWM8_WRONG_CONFIG);
		}
	}
	pwm8_make_all_idle();
	pwm8_enable_all(true);
}

/**
* pwm8_set_duty(unit8_t pwm_no, pwm8_duty_t new_duty) - sets new duty cycle 
*		for a given PWM (pwm_no)
*	RETURN: returns the old value of duty cycle
*/
pwm8_duty_t pwm8_set_duty(uint8_t pwm_no, pwm8_duty_t new_duty)
{
	pwm8_duty_t	temp_duty = 0x00;
	if (pwm_no >= pwm8_count_pwm)
	{
		sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
		return 0x00;
	}
	HW_STOP_INTR();
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = PCA0_PAGE;
#endif // _SFRPAGE_EXIST_
	switch (pwm_no)
	{
		case 0:
			temp_duty = PCA0CPH0;
			PCA0CPH0  = PWM8_TRANSLATE_DUTY(new_duty);
			break;
		case 1:
			temp_duty = PCA0CPH1;
			PCA0CPH1  = PWM8_TRANSLATE_DUTY(new_duty);
			break;
		case 2:
			temp_duty = PCA0CPH2;
			PCA0CPH2  = PWM8_TRANSLATE_DUTY(new_duty);
			break;
		case 3:
			temp_duty = PCA0CPH3;
			PCA0CPH3  = PWM8_TRANSLATE_DUTY(new_duty);
			break;
		case 4:
			temp_duty = PCA0CPH4;
			PCA0CPH4  = PWM8_TRANSLATE_DUTY(new_duty);
			break;
#ifdef _F120_
		case 5:
			temp_duty = PCA0CPH5;
			PCA0CPH5  = PWM8_TRANSLATE_DUTY(new_duty);
			break;
#endif // _F120_
		default:
			sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
	}
	HW_START_INTR();
	return PWM8_TRANSLATE_DUTY(temp_duty);
}


/**
* pwm8_make_idle(unit8_t pwm_no) - sets the idle cycle, based on the config,
*		for a given PWM (pwm_no)
*	RETURN: returns the old value of duty cycle
*/
pwm8_duty_t pwm8_make_idle(uint8_t pwm_no)
{
	if (pwm_no >= pwm8_count_pwm)
	{
		sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
		return 0x00;
	}
	return pwm8_set_duty(pwm_no, pwm8_duty_idle[pwm_no]);
}

/**
* pwm8_make_all_idle() - sets the idle cycle, based on the config,
*		for a all PWMs
*/
void pwm8_make_all_idle()
{
	uint8_t i;
	for(i=0; i< pwm8_count_pwm; i++)
	{
		pwm8_make_idle(i);
	}
}

/*
Mod 0 -> 0xFC
Mod 1 -> 0xFE
Mod 2 -> 0xEA
Mod 3 -> 0xEC
Mod 4 -> 0xEE
Mod 5 -> 0xE2
*/
//sfr PWM_REG[6] = {0xFC, 0xFE, 0xEA, 0xEC, 0xEE, 0xE2};


/**
* pwm8_turn_off(unit8_t pwm_no) - sets the duty cycle to "0" turning the PWM OFF
*		for a given PWM (pwm_no)
*	RETURN: returns the old value of duty cycle
*/
pwm8_duty_t pwm8_turn_off(uint8_t pwm_no)
{
	pwm8_duty_t	temp_duty = 0x00;
	if (pwm_no >= pwm8_count_pwm)
	{
		sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
		return 0x00;
	}
	return pwm8_set_duty(pwm_no, PWM8_DUTY_OFF);
}


/**
* pwm8_turn_off_all() - sets the duty cycle to "0" for all PWMs, thus turning them OFF
*/
void pwm8_turn_off_all()
{
	uint8_t i;
	for(i=0; i< pwm8_count_pwm; i++)
	{
		pwm8_turn_off(i);
	}
}


/**
* pwm8_enable(pwm_no, enable) - enables or disables "pwm_no" PCA module (8-bit PWM mode)
*/
void pwm8_enable(uint8_t pwm_no, uint8_t enable)
{
	uint8_t reg = 0x00;
	pwm8_duty_t	temp_duty = 0x00;
	if (pwm_no >= pwm8_count_pwm)
	{
		sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
		return;
	}
	if (0 != enable)
	{ 
		reg = 0x42;
	}
	HW_STOP_INTR();
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = PCA0_PAGE;
#endif // _SFRPAGE_EXIST_
	switch (pwm_no)
	{
		case 0:
		    PCA0CPM0  = reg;
			break;
		case 1:
		    PCA0CPM1  = reg;
			break;
		case 2:
		    PCA0CPM2  = reg;
			break;
		case 3:
		    PCA0CPM3  = reg;
			break;
		case 4:
		    PCA0CPM4  = reg;
			break;
#ifdef _F120_
		case 5:
		    PCA0CPM5  = reg;
			break;
#endif // #ifdef _F120_
		default:
			sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
	}
	HW_START_INTR();
	return;
}


/**
* pwm8_enable_all(enable) - enables or disables all PCA modules (8-bit PWM mode)
*/
void pwm8_enable_all(uint8_t enable)
{
	uint8_t i;
	for(i=0; i< pwm8_count_pwm; i++)
	{
		pwm8_enable(i, enable);
	}
}


/**
* pwm8_get_PCA0CPHx(uint8_t pca_no) - returns the value of PCA0CPH(pca_no)
*/
uint8_t pwm8_get_PCA0CPHx(uint8_t pca_no)
{
/*	if (pca_no >= pwm8_count_pwm)
	{
		sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
		return 0x00;
	}
*/
//	HW_STOP_INTR();
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = PCA0_PAGE;
#endif // _SFRPAGE_EXIST_
	switch (pca_no)
	{
		case 0:
			return PCA0CPH0;
			break;
		case 1:
			return PCA0CPH1;
			break;
		case 2:
			return PCA0CPH2;
			break;
		case 3:
			return PCA0CPH3;
			break;
		case 4:
			return PCA0CPH4;
			break;
#ifdef _F120_
		case 5:
			return PCA0CPH5;
			break;
#endif // #ifdef _F120_
		default:
			sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
			return 0x00;
	}
//	HW_START_INTR();
//	return PWM8_TRANSLATE_DUTY(temp_duty);
}



/**
* pwm8_reset_PCA0CPHx(uint8_t pca_no) - resets the value of PCA0CPH(pca_no)
*/
void pwm8_reset_PCA0CPHx(uint8_t pca_no)
{
/*	if (pca_no >= pwm8_count_pwm)
	{
		sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
		return ;
	}
*/
//	HW_STOP_INTR();
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = PCA0_PAGE;
#endif // _SFRPAGE_EXIST_
	switch (pca_no)
	{
		case 0:
			PCA0CPH0=0;
			break;
		case 1:
			PCA0CPH1=0;
			break;
		case 2:
			PCA0CPH2=0;
			break;
		case 3:
			PCA0CPH3=0;
			break;
		case 4:
			PCA0CPH4=0;
			break;
#ifdef _F120_
		case 5:
			PCA0CPH5=0;
			break;
#endif // #ifdef _F120_
		default:
			sys_error(SYS_ERROR_PWM8_WRONG_PWM_INDEX);
			break;
	}
}


/**
* pwm8_set_freq(timer_reload_value) - changes Timer 0 reload value to change 
*		basic frequency of the PWM waveform
*/
void pwm8_set_freq(uint8_t timer0_reload)
{
	HW_STOP_INTR();
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = TIMER01_PAGE;
#endif // _SFRPAGE_EXIST_
	TH0 = timer0_reload;
	HW_START_INTR();
}

#endif // _ENABLE_PWM8_MODULE_
