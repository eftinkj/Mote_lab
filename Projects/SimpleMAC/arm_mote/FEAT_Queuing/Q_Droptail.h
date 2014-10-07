/****************************************************************************
**
** Copyright (C) 2006-2005 Maciej Zawodniok, James W. Fonda. All rights reserved.
**
** This file is part of the documentation of the UMR Mote Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef _Q_DROPTAIL_H_
#define _Q_DROPTAIL_H_

#include "Queuing.h"


//Constants File for Queuing

//#define QUEUE_SPECIFIC_INIT() que_Droptail_init()

extern unsigned char XDATA Q_table[QSIZE];
extern unsigned char XDATA Q_head;
extern unsigned char XDATA Q_tail;


//Function Prototypes

void que_Droptail_init(void);

int que_Droptail_utilization(void);

uint8_t que_Droptail_enqueue ( unsigned char index );

unsigned char que_Droptail_dequeue(void);

//**********************



#endif

