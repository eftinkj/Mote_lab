/****************************************************************************
**
** 
** 
** Author:  Rana Basheer
** eMail:   mrbxcf@mst.edu
** Version: 1.0
**
****************************************************************************/
#include "defs/esnl_pub.h"

#if defined(_ENABLE_MICROSTRAIN_)

#include "common_constants.h"
#include "common.h"
#include "sys_func.h"
#include "FEAT_Networking/tsp_common_pub.h"

#include "hal/micro/cortexm3/uart.h"
#include "microstrain.h"
#include "FEAT_Scheduler/sch_basic_pub.h"
#include "FEAT_STM32W/STM32W_Radio.h"

#include <string.h>

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/
#define ORIENTMATRIXLEN 3
#define ORIENTMATRIXWID 3
//size of the orientation matrix

#define INVALID_YAW     900

typedef enum  {/* RAWACCELANGR = 0xC1,  ACCELANGR = 0xC2, */ 
              DELANGDELVEL = 0xC3, SETCONTMODE = 0xC4, /*ORIENTMATRIX = 0xC5,*/
              /*ATTUPDMATRIX = 0xC6, MAGVECTOR = 0xC7, ACCELANGRORINET = 0xC8,
			  WRACCELBIASCOR = 0xC9, WRGYROBIASCOR = 0xCA,
			  ACCELANGRMAGNET = 0xCB, ACCELANGRMAGORIENT = 0xCC, 
			  CAPGYROBIAS = 0xCD, */ EULERANG = 0xCE, EULERANGANGR = 0xCF,
			  /*TRNASQUNAN = 0xD0, TEMP = 0xD1, GYROSTABACCELANGRMAG = 0xD2,
			  DELANGDELVELMAG = 0xD3, WRWORD = 0xE4, REWORD = 0xE5, */
			  STOPCONTMODE = 0xFA, CONFBYTEONE = 0xC1,
			  CONFBYTETWO = 0x29} MicCommand;

/* Command   Definition
   0x02      Wireless Ping (wireless units only)
   0xC1      Raw Accelerometer and Angular Rate Sensor Outputs
   0xC2      Acceleration & Angular Rate
   0xC3      DeltaAngle & DeltaVelocity
   0xC4      Set Continuous Mode
   0xC5      Orientation Matrix
   0xC6      Attitude Update Matrix
   0xC7      Magnetometer Vector
   0xC8      Acceleration, Angular Rate & Orientation Matrix
   0xC9      Write Accelerometer Bias Correction
   0xCA      Write Gyro Bias Correction
   0xCB      Acceleration, Angular Rate & Magnetometer Vector
   0xCC      Accel, Ang Rate & Mag Vectors & Orientation Matrix
   0xCD      Capture Gyro Bias
   0xCE      Euler Angles
   0xCF      Euler Angles and Angular Rates
   0xD0      Transfer Quantity to Non-Volatile Memory
   0xD1      Temperatures
   0xD2      Gyro Stabilized Acceleration, Angular Rate &
             Magnetometer Vector
   0xD3      DeltaAngle & DeltaVelocity & Magnetometer Vectors
   0xE4      Write Word to EEPROM
   0xE5      Read Word from EEPROM
   0xFA      Stop Continuous Mode (no reply) */
#define EULERANGDATALEN     19
// Length of Microstrain response to an EulerAngle command
#define DELANGDELVELDATALEN 31
// Length of Microstrain response to a delta angle delta velocity command
#define ORIENTMATRIXDATALEN 43
// Length of Microstrain response to a orientation matrix command
#define EULERANGANGRDATALEN 31
// Length of Microstrain response to a Euler Angle/Angular Rate command
#define ACCELANGRDATALEN 31
// Length of Microstrain response to a Acceleration/Angular Rate command

#define MAXPACKETSIZE 90
//Maximum packet size that can be received
#define MAXBYTES 30
#define TWO_PI 6.2831853071
#define MICROPRECISION 100.0
#define MICRO_TIMER_ID      1
#define ANG_VEL_UPD_PERIOD  100
#define MICROSTRAIN_CHECK_PERIOD 100
#define MICROSTRAIN_WAIT_TIMEOUT 20
#define NUM_HEADINGS 2
#define GYRO_HEADING	0xA

//Typedefs
typedef struct
{
   float roll;
   float pitch;
   float yaw; 
   float DelAngx;
   float DelAngy;
   float DelAngz;
   float DelVelx; 
   float DelVely;
   float DelVelz;
   float angRatex;
   float angRatey;
   float angRatez;
   float AngV; 
   float lastYaw[NUM_HEADINGS];
   uint32_t time;
   uint8_t initialized;
   uint8_t newYaw;

} micro_data_t; 

//Structure for holding the microstrain data


static uint8_t recPacket[MAXPACKETSIZE]; //Buffer for receiving packet
static uint8_t bytesRec;                 //Number of bytes currently received
static uint8_t packetSize;               //Expected packet size
static uint8_t microstrain_available;
static uint32_t next_update_time;
/*****************/

static micro_data_t gaugeData;           //Stores the received data 

static void microstrain_request_all_data
	(
	void
	);

static void microstrain_loop
	(
	 void
	);

static void microstrain_fsm
	(
	uint8_t ser_data
	);

static void microstrain_update_angular_velocity
	(
	void
	);

static void micro_send_request
	(
	uint8_t command
	);

static float microstrain_round_off
	(
	float number, 
	const int precision
	);

static void update_microstrain
	(
	uint8_t XDATA *context
	);

static void check_microstrain
	(
	uint8_t XDATA *context
	);

static void check_microstrain
	(
	uint8_t XDATA *context
	)
{
if(!microstrain_available)
	{
	printf("\r\nMicrostrain not connected\r\n");
	return;
	}
microstrain_request_all_data();
microstrain_update_angular_velocity();
microstrain_available-=1;
if(!gaugeData.initialized)
	{
	if(gaugeData.lastYaw[0] != INVALID_YAW)
		{
		gaugeData.initialized = 1;
		//sch_create_timeout( rtc_get_ticks()+ANG_VEL_UPD_PERIOD, update_microstrain, NULL); 
		next_update_time = rtc_get_ticks()+ANG_VEL_UPD_PERIOD;
		}
	else
		{
		sch_create_timeout( rtc_get_ticks()+MICROSTRAIN_CHECK_PERIOD, check_microstrain, NULL);   
		}
	}
}

static void microstrain_request_all_data
	(
	void
	)
{
micro_send_request(EULERANG);
micro_send_request(DELANGDELVEL);
}

static void micro_send_request
	(
	uint8_t command
	)
{
   //uint8_t sendbyte = command;

   //IOP_comm_writeasdas(comm_hndl, &sendbyte, 1);
   //IOP_comm_flush(comm_hndl);
   //return;
  putchar(command);
}

static void microstrain_loop
	(
	 void
	)
{
if(is_microstrain_active())
	{
	uint8_t ser_data;
	if ( serialReadByte( &ser_data ) )
		{
		microstrain_fsm(ser_data);
		}
	if(gaugeData.initialized)
		{
		if(next_update_time < rtc_get_ticks())
			{
			next_update_time = rtc_get_ticks()+ANG_VEL_UPD_PERIOD;
			update_microstrain(NULL);
			}
		}
	}
}	  

static void microstrain_fsm
	(
	uint8_t ser_data
	)
{	
uint8_t i;
float *num;
uint16_t actChecksum;      //Transmitted checksum
uint16_t checksum;         //Calculated checksum

recPacket[bytesRec++] = ser_data;

if(0 == packetSize)
	{
  	uint8_t currentByte = 0;
  	while(0 == packetSize && bytesRec > currentByte)
  	//Is there a valid command byte in the buffer?
  		{
     	switch(recPacket[currentByte])
     		{
	        case EULERANG:
	           packetSize = EULERANGDATALEN;
	           break;
	        case DELANGDELVEL:
	           packetSize = DELANGDELVELDATALEN;
	           break;
	        case EULERANGANGR:
	           packetSize = EULERANGANGRDATALEN;
	           break;
	/*	        case ORIENTMATRIX:
	           packetSize = ORIENTMATRIXDATALEN;
	           break;
			case ACCELANGR:
			   packetSize = ACCELANGRDATALEN;
			   break; */   
	        default:
			   currentByte++;
	           break;
	     	}
  		} //end of while
	
	if(0 == packetSize) //valid command byte not found, reset entire buffer
  		{
		bytesRec = 0;
  		}
	else if(currentByte > 0) //shift command byte into index 0
  		{
		for(i = currentByte; i < bytesRec; i++)
	 		{
	    	recPacket[i - currentByte] = recPacket[i];
	 		}
	 	bytesRec -= currentByte;
  		}
	} //if(0 == packetSize)
else if(bytesRec >= packetSize) //We potentially have a full packet
	{
	//Check checksum
	actChecksum = ((uint16_t)recPacket[packetSize - 2])<<8;
	actChecksum |= (uint16_t)(recPacket[packetSize - 1]);
	checksum = ((uint16_t)recPacket[0]);
	for(i = 1; i < packetSize - 2; i++)
		{
		checksum += ((uint16_t)recPacket[i]);	  
		}
	if(checksum == actChecksum)
		{
     //Load data
//#define swap_val(mem,pos1,pos2) {(mem)[pos1]^=(mem)[pos2]; mem[pos2]^=(mem)[pos1]; (mem)[pos1]^=(mem)[pos2];}
#define swap_val(mem,pos1,pos2) {*((mem)+pos1)^=*((mem)+pos2); *((mem)+pos2)^=*((mem)+pos1); *((mem)+pos1)^=*((mem)+pos2);}
#define set_float(num,mem)  {swap_val(mem,0,3); swap_val(mem,1,2);(num)=(float *)(mem);}	  
	switch(recPacket[0])
	 	{
        case EULERANGANGR:
//		   num = (float*)&recPacket[13];
		   set_float(num,recPacket+13);
		   gaugeData.DelAngx = *num;
		   //num = (float*)&recPacket[17];
		   set_float(num,recPacket+17);
		   gaugeData.DelAngy = *num;
		   //num = (float*)&recPacket[21];
		   set_float(num,recPacket+21);
		   gaugeData.DelAngz = *num;
		   //Lack of break intentional
		case EULERANG:
           //num = (float*)&recPacket[1]; 
		   set_float(num,recPacket+1);
           gaugeData.roll = *num;
		   //num = (float*)&recPacket[5];
		   set_float(num,recPacket+5);
		   gaugeData.pitch = *num;
		   //num = (float*)&recPacket[9];
		   set_float(num,recPacket+9);
		   gaugeData.yaw = *num;
		   gaugeData.yaw = -gaugeData.yaw;
		   gaugeData.newYaw = 1;
		   break;
        case DELANGDELVEL:
		   //num = (float*)&recPacket[1];
		   set_float(num,recPacket+1);
		   gaugeData.DelAngx = *num;
		   //num = (float*)&recPacket[5];
		   set_float(num,recPacket+5);
		   gaugeData.DelAngy = *num;
		   //num = (float*)&recPacket[9];
		   set_float(num,recPacket+9);
		   gaugeData.DelAngz = *num;
		   //num = (float*)&recPacket[13];
		   set_float(num,recPacket+13);
		   gaugeData.DelVelx = *num;
		   //num = (float*)&recPacket[17];
		   set_float(num,recPacket+17);
		   gaugeData.DelVely = *num;
		   //num = (float*)&recPacket[21];
		   set_float(num,recPacket+21);
		   gaugeData.DelVelz = *num;
		   break;
/*            case ORIENTMATRIX:
		   num = (float*)&recPacket[1];
		   for(i = 0; i < ORIENTMATRIXLEN*ORIENTMATRIXWID; i++)
		   {
		      gaugeData.Orient[i] = *num;
		      num++;
		   } 
		   break;
        case ACCELANGR:
		   //num = (float*)&recPacket[1];
           	set_float(num,recPacket+1);
			gaugeData.accelx = *num;
		   //num = (float*)&recPacket[5];
		   set_float(num,recPacket+5);
		   gaugeData.accely = *num;
		   //num = (float*)&recPacket[9];
		   set_float(num,recPacket+9);
		   gaugeData.accelz = *num;
		   //num = (float*)&recPacket[13];
		   set_float(num,recPacket+13);
		   gaugeData.angRatex = *num;
		   //num = (float*)&recPacket[17];
		   set_float(num,recPacket+17);
		   gaugeData.angRatey = *num;
		   //num = (float*)&recPacket[21];
		   set_float(num,recPacket+21);
		   gaugeData.angRatez = *num;
		   break;*/
        default:
		   //bad command byte, should not trigger
		   break;
	 	} //switch(recPacket[0])
	 
	 //purge the packet
    for(i = packetSize; i < bytesRec; i++)
     	{
        recPacket[i - packetSize] = recPacket[i];
     	}
    bytesRec -=packetSize;
	} //if(checksum == actChecksum)
else
	{
	recPacket[0] = 0; //set invalid command byte
	}
packetSize  = 0;
}

return;
}

static void update_microstrain
	(
	uint8_t XDATA *context
	)
{
microstrain_request_all_data();
microstrain_update_angular_velocity();
//sch_create_timeout( rtc_get_ticks()+ANG_VEL_UPD_PERIOD, update_microstrain, NULL); 
}
static void microstrain_update_angular_velocity
	(
	void
	)
{
   float sampT;

   if(gaugeData.newYaw)
   {
     sampT = rtc_get_ticks() - gaugeData.time;
     gaugeData.time = rtc_get_ticks();
     sampT /= 1000;
     gaugeData.lastYaw[0] = gaugeData.lastYaw[1];
     gaugeData.lastYaw[1] = gaugeData.yaw;

      if( (gaugeData.lastYaw[0] > TWO_PI/4.0) && (gaugeData.lastYaw[1] < -TWO_PI/4.0)) gaugeData.lastYaw[0] -= TWO_PI;
      else if( (gaugeData.lastYaw[0] < -TWO_PI/4.0) && (gaugeData.lastYaw[1] > TWO_PI/4.0)) gaugeData.lastYaw[0] += TWO_PI;

      gaugeData.AngV = (gaugeData.lastYaw[1] - gaugeData.lastYaw[0])/sampT;
      gaugeData.newYaw = 0;
   }
   return;
}

static float microstrain_round_off(float number, const int precision)
{
   uint8_t flip = 0;
   float retValue;
   if(number < 0)
   {
      number = -number;
	  flip = 1;
   }
   retValue = (number * precision + .5)/precision;
   if(flip) retValue = -retValue;
   return retValue;
} 

float microstrain_get_yaw()
{
   return microstrain_round_off(gaugeData.yaw, MICROPRECISION);
}

float microstrain_get_angular_velocity()
{
   return gaugeData.AngV;
}

void microstrain_power_up(void)
{
uint8_t i;
microstrain_available = MICROSTRAIN_WAIT_TIMEOUT;
bytesRec = 0;   
packetSize = 0;
gaugeData.yaw = INVALID_YAW;
for(i = 0; i < NUM_HEADINGS; i++)
	{	
  	gaugeData.lastYaw[i] = INVALID_YAW;
	}
gaugeData.AngV = INVALID_YAW;
gaugeData.initialized = 0;
gaugeData.newYaw = 0;
}


void microstrain_init
	(
	void
	)
{
sch_add_loop((sch_loop_func_t)microstrain_loop);
sch_create_timeout( rtc_get_ticks()+1, check_microstrain, NULL);
}

/*
This function returns 1 if the microstrain driver is using the uart port
*/
uint8_t is_microstrain_active()
{
  return 	microstrain_available?1:0;
}
#endif //_ENABLE_MICROSTRAIN_