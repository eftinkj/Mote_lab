#include <stdlib.h>
#include <string.h>

#include "mpr_selector_table.h"
#include "packet.h"

struct mpr_selector_table mprstable;

int mcr_delete_mpr_selector_table(struct mpr_selector_entry *mprs)
{

  mcr_remque((struct mcr_qelem *)mprs);
  free((void *)mprs);
  
}

int mcr_insert_mpr_selector_table(struct mpr_selector_entry *mprs)
{

  uint32_t hash;
  struct mpr_selector_hash  *mprs_hash;
  
  mcr_hashing(&mprs->mpr_selector_addr,&hash);
  mprs->mprselector_hash=hash;
  mprs_hash=&mprstable.mpr_selector_hash[hash & HASHMASK];
  mcr_insque((struct mcr_qelem *)mprs, (struct mcr_qelem *)mprs_hash);
  
}


struct mpr_selector_entry *mcr_lookup_mpr_selector_table(uint32_t *dst)
{
  struct mpr_selector_entry *mprs;
  struct mpr_selector_hash  *mprs_hash;
  uint32_t hash;
  
   mcr_hashing(dst,&hash);
   mprs_hash=&mprstable.mpr_selector_hash[hash & HASHMASK];
  
  for(mprs=mprs_hash->mpr_selector_forw;mprs!=(struct mpr_selector_entry *)mprs_hash;mprs=mprs->mpr_selector_forw)
    {
      if(mprs->mprselector_hash!=hash)
	continue;

      if (0==memcmp(&mprs->mpr_selector_addr,dst,sizeof(uint32_t)))
	return(mprs);
    }
  return(NULL);
}

int mcr_update_mpr_selector_table(struct hello_packet_structure_MCR *message)
{
  /*
    If the mpr set is changed, then we must increment the mssn
  */

  struct mpr_selector_entry  *mprs;
    
  if(NULL==(mprs=mcr_lookup_mpr_selector_table(&message->src_id)))
    {

      mprs=(struct mpr_selector_entry *)malloc(sizeof(struct mpr_selector_entry));
      memcpy(&mprs->mpr_selector_addr,&message->src_id,sizeof(uint32_t));
      mcr_insert_mpr_selector_table(mprs);
      mprs->mpr_selector_seq=message->mpr_seq;
      timeradd(&now,&hold_time_neighbor,&mprs->mpr_selector_timer);
      /*increment the mssn number don't forget to increment this number on time out too*/
      mprstable.mssn++;
      /* Set the changes flag to UP ( This flag will be set to DOWN after sending a TC packet)*/
      changes=UP;
    }
  else
    {
      if(message->mpr_seq>=mprs->mpr_selector_seq)
	{	
	  mprs->mpr_selector_seq=message->mpr_seq;	  
	  timeradd(&now,&hold_time_neighbor,&mprs->mpr_selector_timer);
	}
    }
}


int mcr_release_mpr_seletector_table()
{
  struct mpr_selector_entry *mprs;
  struct mpr_selector_entry *mprs_tmp;
  struct mpr_selector_hash  *mprs_hash;
  uint8_t index;

  for(index=0;index<HASHSIZE;index++)
    {
      mprs_hash=&mprstable.mpr_selector_hash[index];

      mprs=mprs_hash->mpr_selector_forw;
      while(mprs!=(struct mpr_selector_entry *)mprs_hash)
	{
	  mprs_tmp=mprs;
	  mprs=mprs->mpr_selector_forw;
	  mcr_delete_mpr_selector_table(mprs_tmp);
	}
    }
  
}

int mcr_time_out_mpr_selector_table()
{
  struct mpr_selector_entry *mprs;
  struct mpr_selector_entry *mprs_tmp;
  struct mpr_selector_hash  *mprs_hash;
  uint8_t index;
  
  for(index=0;index<HASHSIZE;index++)
    {
      mprs_hash=&mprstable.mpr_selector_hash[index];
      mprs=mprs_hash->mpr_selector_forw;

      while(mprs!=(struct mpr_selector_entry *)mprs_hash)	
	{
	  if(mcr_timed_out(&mprs->mpr_selector_timer))
	    {
	      mprs_tmp=mprs;
	      mprs=mprs->mpr_selector_forw;
	      mcr_delete_mpr_selector_table(mprs_tmp);
	      
	      /*increment the mssn number don't forget to increment this number */
	      mprstable.mssn++;
	      /* The changes flag is set to UP*/
	      changes=UP;
	    }
	  else
	    mprs=mprs->mpr_selector_forw;
	}
    }
}

int mcr_print_mpr_selector_table()
{
  struct mpr_selector_entry *mprs;
  struct mpr_selector_hash  *mprs_hash;
  uint8_t index;

  printf("MPR SELECTOR TABLE\n");
  
  for(index=0;index<HASHSIZE;index++)
    {
      mprs_hash=&mprstable.mpr_selector_hash[index];
      
      for(mprs=mprs_hash->mpr_selector_forw;mprs!=(struct mpr_selector_entry *)mprs_hash;mprs=mprs->mpr_selector_forw)
	{	 	  
	  printf("%s            \n",convert_address_to_string(&mprs->mpr_selector_addr));	  
	}
    }
}

