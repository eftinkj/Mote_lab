#include "math.h"
#include "stdlib.h"
#include "defs/esnl_pub.h"


typedef struct RSSI_Store
{	
        int8_t pre_rssi[3][10];
	int8_t rssi[3][10];
	boolean ready[2][3];
	int8_t current_node;
	float mean[3];
	float var[3];
	float radius;
//        uint32_t time;
}RSSI_Store;

extern RSSI_Store rssi_store;

float request_rssi(unsigned char *p);