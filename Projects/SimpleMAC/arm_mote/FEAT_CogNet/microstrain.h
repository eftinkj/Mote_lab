#ifndef _MICROSTRAIN_H_
#define _MICROSTRAIN_H_

#include "defs/esnl_pub.h"

void microstrain_power_up(void);

void microstrain_init(void);

float microstrain_get_yaw();

float microstrain_get_angular_velocity();

uint8_t is_microstrain_active();

#endif //#ifndef _MICROSTRAIN_H_