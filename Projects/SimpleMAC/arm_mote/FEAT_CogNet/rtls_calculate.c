/****************************************************************************
** 
** By Guoda Liang <gl5xd@mst.edu>
** Version: 1.0
**
****************************************************************************/
#include "string.h"
#include "rtls_calculate.h"
#include "HW_LIB\RTC.h"

static RSSI_Store rssi_store;

void compute_position();

void res_store();

/**
* uint8_t request_rssi(unsigned char *p) - attain rssi value from the received packet
*		and calls the related functions to do the calculation
*/
float request_rssi(unsigned char *p)
{	
       char node=p[1];
	switch(node)
	{	case 'B':
		rssi_store.current_node=0;
		break;
		case 'C':
		rssi_store.current_node=1;
		break;
		case 'E':
		rssi_store.current_node=2;
		break;
                default:
                return 0.0;
	}
        int j=8;
	for(int i=0;i<10;i++)
	{
          rssi_store.rssi[rssi_store.current_node][i]=(p[j+1]-'0')*10+(p[j+2]-'0');                
          if(!(p[j]==' '))
            rssi_store.rssi[rssi_store.current_node][i]=(p[j]-'0')*100;
	  j+=4;
	}
//        if(!(rssi_store.ready[1][0]||rssi_store.ready[1][1]||rssi_store.ready[1][2]))
//        {
//          if(rtc_get_ticks()-time<60)
//          {
//            for(int i=0;i<10;i++)
//	    {
//              rssi_store.pre_rssi[rssi_store.current_node][i]=rssi_store.rssi[rssi_store.current_node][i];
//	    }
//          }
//          time=rtc_get_ticks();
//        }
//        else
//        {
	  rssi_store.ready[1][rssi_store.current_node]=1;
//        }
	if(rssi_store.ready[1][0]&&rssi_store.ready[1][1]&&rssi_store.ready[1][2])
	{
		if(!(rssi_store.ready[0][0]||rssi_store.ready[0][1]||rssi_store.ready[0][2]))
		{
                  res_store();
		}
		compute_position();
//		sprintf(str,"%20f",rssi_store.radius);
//		strcpy((char*)p, p1);
                return rssi_store.radius;
	}
        return 0.0;
}

/**
* void compute_position() - compute the relative position change
*		based on the rssi value
* no return
*/
void compute_position()
{	double x = 0,c = 0,a = 1.0/100.0;
	for(int j=0;j<3;j++)
	{
          rssi_store.mean[j]=0,rssi_store.var[j]=0;
		for(int i=0;i<10;i++)
		{
			rssi_store.mean[j]+=rssi_store.rssi[j][i];
		}
		rssi_store.mean[j]/=10.0;
		for(int i=0;i<10;i++)
		{
			float f=rssi_store.rssi[j][i]-rssi_store.mean[j];
			rssi_store.var[j]+=f*f;
		}
		rssi_store.var[j]/=10.0;
		for(int i=0;i<10;i++)
		{
			double f=rssi_store.rssi[j][i]-rssi_store.mean[j];
			x+=f*f/rssi_store.var[j];
			x = sqrt(x)*a + c;
		}
	}
	rssi_store.radius=x;
}

/**
* void res_store() - store rssi value as pre_rssi value
*		for the next calculation
* no return
*/
void res_store()
{	for(int i=0;i<3;i++)
	{
          for(int j=0;j<10;j++)
          {
		  rssi_store.pre_rssi[i][j]=rssi_store.rssi[i][j];
          }
		rssi_store.ready[1][i]= 0;
		rssi_store.ready[0][i]= 1;
	}
}