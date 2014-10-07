#ifndef _HW_ADE_COMMON_H_
#define _HW_ADE_COMMON_H_

#ifndef QT4
    #include "defs/esnl_pub.h"
#else // QT4
    #include "esnl_pub.h"
#endif // else QT4


#define RESERVED	0x00
#define AWATTHR	0x01
#define BWATTHR	0x02
#define CWATTHR 	0x03
#define AVARHR 	0x04
#define BVARHR 	0x05
#define CVARHR 	0x06
#define AVAHR 	0x07
#define BVAHR 	0x08
#define CVAHR 	0x09
#define AIRMS 	0x0A
#define BIRMS 	0x0B
#define CIRMS 	0x0C
#define AVRMS 	0x0D
#define	BVRMS 	0x0E
#define	CVRMS 	0x0F
#define	FREQ 	0x10
#define	TEMP 	0x11
#define	WFORM 	0x12
#define	OPMODE 	0x13
#define	MMODE 	0x14
#define	WAVMODE 	0x15
#define	COMPMODE 	0x16
#define	LCYCMODE 	0x17
#define	MASK 	0x18
#define	STATUS 	0x19
#define	RSTATUS 	0x1A
#define	ZXTOUT 	0x1B
#define	LINECYC 	0x1C
#define	SAGCYC 	0x1D
#define	SAGLVL 	0x1E
#define	VPINTLVL 	0x1F
#define	IPINTLVL 	0x20
#define	VPEAK 	0x21
#define	IPEAK 	0x22
#define	GAIN 	0x23
#define	AVRMSGAIN 	0x24
#define	BVRMSGAIN 	0x25
#define	CVRMSGAIN 	0x26
#define	AIGAIN 	0x27
#define	BIGAIN 	0x28
#define	CIGAIN 	0x29
#define	AWG 	0x2A
#define	BWG 	0x2B
#define	CWG 	0x2C
#define	AVARG 	0x2D
#define	BVARG 	0x2E
#define	CVARG 	0x2F
#define	AVAG 	0x30
#define	BVAG 	0x31
#define	CVAG 	0x32
#define	AVRMSOS 	0x33
#define	BVRMSOS 	0x34
#define	CVRMSOS 	0x35
#define	AIRMSOS 	0x36
#define	BIRMSOS 	0x37
#define	CIRMSOS 	0x38
#define	AWATTOS 	0x39
#define	BWATTOS 	0x3A
#define	CWATTOS 	0x3B
#define	AVAROS 	0x3C
#define	BVAROS 	0x3D
#define	CVAROS 	0x3E
#define	APHCAL 	0x3F
#define	BPHCAL 	0x40
#define	CPHCAL 	0x41
#define	WDIV 	0x42
#define	VARDIV 	0x43
#define	VADIV 	0x44
#define	APCFNUM 	0x45
#define	APCFDEN 	0x46
#define	VARCFNUM 	0x47
#define	VARCFDEN 	0x48
#define	CHKSUM 	0x7E
#define	VERSION 	0x7F


#define ADE_WRITE_FLAG	0x80

#define ADE_REG_COUNT	0x80

#ifdef QT4
//	uint32_t ade_param_length[ADE_REG_COUNT];
//	uint32_t ade_param_value[ADE_REG_COUNT];

struct param_desc_record
{
	uint8_t addr;
	uint8_t size;
	QString label;
	QString tooltip;
	QString whatsThis;
	uint32_t	default_value;
	int64_t	min_value;
	int64_t	max_value;
	uint8_t	visible_in_config;
	uint8_t editable_in_config;
};

#define ENCODE_24(val32)		( SWAP32 ( (((uint32_t)val32) & 0x00FFFFFF) << 8)  )
#define ENCODE_16(val32)		( SWAP32 ( (((uint32_t)val32) & 0x0000FFFF) << 16)  )
#define ENCODE_12(val32)		( SWAP32 ( (((uint32_t)val32) & 0x00000FFF) << 16)  )
#define ENCODE_08(val32)		( SWAP32 ( (((uint32_t)val32) & 0x000000FF) << 24)  )
#define ENCODE_07(val32)		( SWAP32 ( (((uint32_t)val32) & 0x0000007F) << 24)  )

/*
// Works only if 24bit registe read (3 bytes into 32bit uint) - bytes aligned to MSB!!!
#define EXTRACT_24(val32)		(( ((uint32_t)SWAP32(val32)) >> 8 ) & 0x00FFFFFF)
#define EXTRACT_16(val32)		(( ((uint32_t)SWAP32(val32)) >> 16) & 0x0000FFFF)
#define EXTRACT_12(val32)		(( ((uint32_t)SWAP32(val32)) >> 16) & 0x00000FFF)
#define EXTRACT_08(val32)		(( ((uint32_t)SWAP32(val32)) >> 24) & 0x000000FF)
#define EXTRACT_07(val32)		(( ((uint32_t)SWAP32(val32)) >> 24) & 0x0000007F)
*/

// Works only if exact number of bytes is read (e.g. 2 bytes for 12 and 16) - alligned to LSB
// NOTE: for 24 still need to shift bits
#define EXTRACT_24(val32)		(( ((uint32_t)SWAP32(val32)) >> 8 ) & 0x00FFFFFF)
#define EXTRACT_16(val32)		(( ((uint32_t)SWAP32(val32)) ) & 0x0000FFFF)
#define EXTRACT_12(val32)		(( ((uint32_t)SWAP32(val32)) ) & 0x00000FFF)
#define EXTRACT_08(val32)		(( ((uint32_t)SWAP32(val32)) ) & 0x000000FF)
#define EXTRACT_07(val32)		(( ((uint32_t)SWAP32(val32)) ) & 0x0000007F)

/*
#define EXTRACT_24(val32)		(( ((uint32_t)(val32)) >> 8 ) & 0x00FFFFFF)
#define EXTRACT_16(val32)		(( ((uint32_t)(val32)) >> 16) & 0x0000FFFF)
#define EXTRACT_12(val32)		(( ((uint32_t)(val32)) >> 20) & 0x00000FFF)
#define EXTRACT_08(val32)		(( ((uint32_t)(val32)) >> 24) & 0x000000FF)
#define EXTRACT_07(val32)		(( ((uint32_t)(val32)) >> 25) & 0x0000007F)
*/

#endif


struct mon_power_packet_mod
{
	uint8_t version;
	uint8_t length;
	uint8_t content;
} PACKED ;

typedef struct mon_power_packet_mod mon_power_cmd_packet_t;


struct power_report_1
{
	uint8_t module_type;
	uint8_t module_length;

	uint8_t report_type;

	uint16_t	aver_temp;
	uint16_t	aver_g_x;
	uint16_t	aver_g_y;
	uint16_t	aver_g_z;

	int16_t	acc_watt_a;
	int16_t	acc_watt_b;
	int16_t	acc_watt_c;

	int16_t	acc_var_a;
	int16_t	acc_var_b;
	int16_t	acc_var_c;

	uint16_t	acc_va_a;
	uint16_t	acc_va_b;
	uint16_t	acc_va_c;

} PACKED;

typedef struct power_report_1 mon_power_report_packet_v1_t;


struct power_report_2
{
	uint8_t module_type;
	uint8_t module_length;

	uint8_t report_type;

	uint16_t	aver_temp;
	uint16_t	aver_g_x;
	uint16_t	aver_g_y;
	uint16_t	aver_g_z;

	int16_t	acc_watt_a;
	int16_t	acc_watt_b;
	int16_t	acc_watt_c;

	int16_t	acc_var_a;
	int16_t	acc_var_b;
	int16_t	acc_var_c;

	uint16_t	acc_va_a;
	uint16_t	acc_va_b;
	uint16_t	acc_va_c;

	uint32_t	v_a;
	uint32_t	v_b;
	uint32_t	v_c;

	uint32_t	i_a;
	uint32_t	i_b;
	uint32_t	i_c;

	uint16_t	freq;
	int8_t		temp;
	uint32_t	wave;

} PACKED;

typedef struct power_report_2 mon_power_report_packet_v2_t;


struct mon_power_request
{
	uint8_t module_type;
	uint8_t module_length;

	uint8_t request_type;

	uint8_t	count_params;
	uint8_t	params_list;

} PACKED;

typedef struct mon_power_request mon_power_request_packet_v1_t;



struct mon_power_command
{
	uint8_t		module_type;
	uint8_t		module_length;

	uint8_t		command;
	uint8_t		command_param;
	uint16_t	command_wparam;

} PACKED;

typedef struct mon_power_command mon_power_command_packet_v1_t ;


// CONTENT field - values
#define MON_POWER_REPORT_V1		01
#define MON_POWER_REPORT_V2		02
#define MON_POWER_COMMAND_V1	10
#define MON_POWER_PARAM_REQUEST	20
#define MON_POWER_PARAM_CHANGE	21
#define MON_POWER_PARAM_REPORT	22

#define MON_POWER_CMD_STOP		0
#define MON_POWER_CMD_START		1
#define MON_POWER_CMD_INTERVAL	2

struct mon_power_param_request
{
	uint8_t	addr;
	uint8_t size;
} PACKED;
typedef struct mon_power_param_request mon_power_param_req_t ;

struct mon_power_param_report
{
	uint8_t	addr;
	uint8_t size;
	uint32_t	value;
} PACKED;

typedef struct mon_power_param_report mon_power_param_report_t;


#define MON_POWER_PARAM_REQ_LEN 2
// Limit number of requests per packet to 10 (max for responses)
#define MON_POWER_PARAM_REQ_MAX	10

#define MON_POWER_PARAM_REPORT_LEN 6
#define MON_POWER_PARAM_REPORT_MAX	10

// 15 * 5 + 4 overhead of module
#define MON_POWER_PARAM_REPORT_SIZE	65

// Fields in the module
#define MON_POWER_PKT_PARAM_REQ_MOD_LENGTH		(2 + MON_POWER_PARAM_REQ_LEN * MON_POWER_PARAM_REQ_MAX)
// Size with node IDs (2), module overhead (2) and the END MODULE byte (1)
#define MON_POWER_PKT_PARAM_REQ_LENGTH			(2 + 2 + MON_POWER_PKT_PARAM_REQ_MOD_LENGTH + 1)
// Total length of the packet to sent (with custom header)
#define MON_POWER_PKT_PARAM_REQ_LENGTH_TOTAL	(7 + MON_POWER_PKT_PARAM_REQ_LENGTH)


// Fields in the module
#define MON_POWER_PKT_PARAM_WRITE_MOD_LENGTH		(2 + MON_POWER_PARAM_REPORT_LEN * MON_POWER_PARAM_REPORT_MAX)
// Size with node IDs (2), module overhead (2) and the END MODULE byte (1)
#define MON_POWER_PKT_PARAM_WRITE_LENGTH			(2 + 2 + MON_POWER_PKT_PARAM_WRITE_MOD_LENGTH + 1)
// Total length of the packet to sent (with custom header)
#define MON_POWER_PKT_PARAM_WRITE_LENGTH_TOTAL		(7 + MON_POWER_PKT_PARAM_WRITE_LENGTH)

// Offset in bytes from begining of packet to start of the parameter list
#define ADE_PARAM_OFFSET_IN_PACKET 	13


// Fields in the module
#define MON_POWER_PKT_CMD_MOD_LENGTH		(2)
// Size with node IDs (2), module overhead (2) and the END MODULE byte (1)
#define MON_POWER_PKT_CMD_LENGTH			(2 + 2 + MON_POWER_PKT_CMD_MOD_LENGTH + 1)
// Total length of the packet to sent (with custom header)
#define MON_POWER_PKT_CMD_LENGTH_TOTAL		(7 + MON_POWER_PKT_CMD_LENGTH)


#endif // _HW_ADE_COMMON_H_
