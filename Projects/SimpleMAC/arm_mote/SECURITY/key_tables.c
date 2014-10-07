#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <REG52.H>                /* special function register declarations   */
/* for the intended 8051 derivative         */
//#include <stdio.h>                /* prototype declarations for I/O functions */
#ifdef MONITOR51                         /* Debugging with Monitor-51 needs   */
char code reserve [3] _at_ 0x23;         /* space for serial interrupt if     */
#endif                                   /* Stop Exection with Serial Intr.   */
/* is enabled                        */
/*------------------------------------------------
Setup the serial port for 1200 baud at 16MHz.
------------------------------------------------*/


xdata struct key_entry
{
	int node_ID;
	char key[16];
	struct key_entry * next;
};

typedef struct key_entry item;

item *curr, *head, *start, *last, *temp, *extra;

void create ( int node_num, char* key );
void append ( int node_num, char* key );
void del ( int node_num );
void print();
void update_key ( int node_num, char* key );

//char i;

void main()
{

	char temp_key[16];
	strcpy ( temp_key, "0123456789ABCDEF" );

#ifndef MONITOR51
	SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
	TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
	TH1   = 221;                /* TH1:  reload value for 1200 baud @ 16MHz   */
	TR1   = 1;                  /* TR1:  timer 1 run                          */
	TI    = 1;                  /* TI:   set TI to send first char of UART    */
#endif

	start = NULL; // always make start node NULL

	create ( 1, temp_key );
	print();
	append ( 2, temp_key );
	print();
	append ( 3, temp_key );
	print();
	del ( 1 );
	print();
	del ( 3 );
	print();
	del ( 2 );
	print();
	return;
}


// function to creat the list and initialize its first element.

void create ( int nodeid, char* key )
{
	if ( start == NULL )
	{
		start = malloc ( sizeof ( struct key_entry ) );  //alloc the struct key_entry
		start->node_ID = nodeid;
		strcpy ( start->key, key );
		start->next = NULL;
	}
	return;
}

void  append ( int nodeid, char* key )
{
	temp = start;
	if ( temp == NULL )   //Hmmm, let's see, does it exist??
	{create ( 1, "01234567879ABCDEF" );
	}
	else
	{
		last = malloc ( sizeof ( struct key_entry ) );
		if ( last == NULL ) {printf ( "\n\t\t\t\t not enough memory..." );return;}
		while ( temp->next != NULL ) temp = temp->next;
		last->node_ID = nodeid;
		strcpy ( last->key, key );
		last->next = NULL;
		temp->next = last;
	}
	return;
}

void  del ( int nodeid )
{
	temp = start;
	if ( start->node_ID == nodeid )
	{
		start = start->next;
		free ( temp );
	}

	else
	{
		while ( temp != NULL )                        //temp->next for next and temp for the given number
		{if ( temp->next->node_ID == nodeid )            //if you remove ->next next to next value is deleted
			{ extra = temp->next;                    //take the link to be deleted
				temp->next = temp->next->next;         // join the deleted
				free ( extra );                        // finally librate the link..
				break;
			}
			// printf("%d\n",cnt);
			temp = temp->next;

		}
	}
	return;
}

void print()
{
	unsigned char i;
	temp = start;
	printf ( "\n List ::=>" );
	while ( temp != NULL )
	{
		printf ( "\t%d\t", temp->node_ID );
		for ( i = 0;i < 16;i++ ) printf ( "%c", temp->key[i] );
		temp = temp->next;
	}
	return;
}


void update_key ( int nodeid, char* key )
{
	temp = start;
	if ( start->node_ID == nodeid )
	{
		extra = malloc ( sizeof ( struct key_entry ) );  //alloc the struct key_entry
		extra->node_ID = nodeid;
		strcpy ( extra->key, key );
		extra->next = start->next;
		start = extra;
		free ( extra );
		return;
	}
	while ( temp != NULL )
	{
		if ( temp->next->node_ID == nodeid )
		{
			extra = malloc ( sizeof ( struct key_entry ) );  //alloc the struct key_entry
			extra->node_ID = nodeid;
			strcpy ( extra->key, key );
			extra->next = temp->next->next;
			temp->next = extra;

		}
		temp = temp->next;
	}
	return;
}

