#ifndef _RTLS_BEACON_H_
#define _RTLS_BEACON_H_

#include "defs/esnl_pub.h"
enum
{
RTLS_BCAST_RQST = 0
,RTLS_BCAST
,RTLS_RSSI_RQST
,RTLS_RSSI_RESP
};

void rtls_beacon_power_up();

void rtls_beacon_init();


#endif //#ifndef _RTLS_BEACON_H_