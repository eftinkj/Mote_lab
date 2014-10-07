#ifndef _PWM8_GEN_
#define _PWM8_GEN_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

#define PWM8_MAX_DUTY	0xFF

#ifdef _ENABLE_ROBOTICS_04_
#define PWM8_DUTY_OFF 0xEA
// Translate between duty cycle (0-255) and PCA register value, AND reverse transformation!!!
#define PWM8_TRANSLATE_DUTY(duty)	(duty)

#endif  //_ENABLE_ROBOTICS_04_


#ifndef PWM8_DUTY_OFF
#define PWM8_DUTY_OFF 0x00
// Translate between duty cycle (0-255) and PCA register value, AND reverse transformation!!!
#define PWM8_TRANSLATE_DUTY(duty)	(PWM8_MAX_DUTY - duty)

#endif //PWM_DUTY_OFF





#define PWM8_MAX_PWMS	6

// Config structure version
#define PWM8_CONFIG_VER_01	1


typedef uint8_t pwm8_duty_t;

typedef struct
	{
		uint8_t	ver;		// Version of the config structure (MANDATORY)
		uint8_t length;		// Length of the remaining bytes in config (MANDATORY)
		pwm8_duty_t	idle;	// list of idle settings for PWMs	(OPTIONAL / VARIABLE)
	} pwm8_config_t	;


/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/
extern pwm8_duty_t	pwm8_duty_idle[PWM8_MAX_PWMS];
extern uint8_t		pwm8_count_pwm;


/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/

/**
* pwm8_init(*config) - setup of the PWM generation based on PCA hardware
*	Accepts pointer to configuration structure
*/
void pwm8_init(pwm8_config_t *config);


/**
* pwm8_set_duty(pwm_no, new_duty) - sets new duty cycle 
*		for a given PWM (pwm_no)
*	RETURN: returns the old value of duty cycle
*/
pwm8_duty_t pwm8_set_duty(uint8_t pwm_no, pwm8_duty_t new_duty);


/**
* pwm8_make_idle(unit8_t pwm_no) - sets the idle cycle, based on the config,
*		for a given PWM (pwm_no)
*	RETURN: returns the old value of duty cycle
*/
pwm8_duty_t pwm8_make_idle(uint8_t pwm_no);


/**
* pwm8_make_all_idle() - sets the idle cycle, based on the config,
*		for a all PWMs
*/
void pwm8_make_all_idle();


/**
* pwm8_turn_off(unit8_t pwm_no) - sets the duty cycle to "0" turning the PWM OFF
*		for a given PWM (pwm_no)
*	RETURN: returns the old value of duty cycle
*/
pwm8_duty_t pwm8_turn_off(uint8_t pwm_no);


/**
* pwm8_turn_off_all() - sets the duty cycle to "0" for all PWMs, thus turning them OFF
*/
void pwm8_turn_off_all();


/**
* pwm8_enable(pwm_no, enable) - enables or disables "pwm_no" PCA module (8-bit PWM mode)
*/
void pwm8_enable(uint8_t pwm_no, uint8_t enable);

/**
* pwm8_enable_all(enable) - enables or disables all PCA modules (8-bit PWM mode)
*/
void pwm8_enable_all(uint8_t enable);

/**
* pwm8_set_freq(timer_reload_value) - changes Timer 0 reload value to change 
*		basic frequency of the PWM waveform
*/
void pwm8_set_freq(uint8_t timer0_reload);


/**
* pwm8_get_PCA0CPHx(uint8_t pca_no) - returns the value of PCA0CPH(pca_no)
*/
uint8_t pwm8_get_PCA0CPHx(uint8_t pca_no);


/**
* pwm8_reset_PCA0CPHx(uint8_t pca_no) - resets the value of PCA0CPH(pca_no)
*/
void pwm8_reset_PCA0CPHx(uint8_t pca_no);


#endif // _PWM_GEN_
