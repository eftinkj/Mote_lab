
#include <stdio.h>
#include "defs\esnl_pub.h"


struct mpr_selector_table {
  uint16_t mssn;
  struct mpr_selector_hash *mpr_selector_hash;
};

struct mpr_selector_info {

    uint32_t mprselector_hash;
    uint32_t mpr_selector_addr;
	uint16_t mpr_selector_seq;
	struct timeval mpr_selector_timer;		        
};

struct mpr_selector_entry {
	struct mpr_selector_entry *mpr_selector_forw;
	struct mpr_selector_entry *mpr_selector_back;
    struct mpr_selector_info mpr_selector_infos;
};


#define mprselector_hash mpr_selector_infos.mprselector_hash

#define	mpr_selector_addr mpr_selector_infos.mpr_selector_addr    /* MultiPoint Relay selector address */
#define	mpr_selector_seq  mpr_selector_infos.mpr_selector_seq     /* MPRS sequence number with
									   which mprs_addr has selected
									   this node as its MPR */
#define	mpr_selector_timer mpr_selector_infos.mpr_selector_timer   /* for invalidation */

struct mpr_selector_hash {
  struct mpr_selector_entry *mpr_selector_forw;
  struct mpr_selector_entry *mpr_selector_back;
};



int mcr_delete_mpr_selector_table(struct mpr_selector_entry *mprs);

int mcr_insert_mpr_selector_table(struct mpr_selector_entry *mprs);

struct mpr_selector_entry *mcr_lookup_mpr_selector_table(uint32_t *dst);

int mcr_update_mpr_selector_table(struct hello_message *message);

int mcr_release_mpr_seletector_table();

int mcr_time_out_mpr_selector_table();

int mcr_print_mpr_selector_table();
