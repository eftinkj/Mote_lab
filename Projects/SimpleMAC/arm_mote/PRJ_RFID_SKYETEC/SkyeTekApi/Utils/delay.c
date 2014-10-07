
#include "utils/delay.h"

void rfid_delay(int t)
{
	int i, j;
	for(i=0;i<t;i++)
	{
		for(j=0;j<t;j++)
		{
			j = j+i;
			j = j-i;
		}
	}
}