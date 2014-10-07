#define MAX_CHILDREN 4
//#include "../interfaces/SClock.h"
struct RequestMsg {
  /* The 16-bit source node address. */
  uint16_t sourceaddr;
  uint8_t  packet_type;
} ;
typedef struct RequestMsg RequestMsg;

struct ParentMsg {
  /* The 16-bit source node address. */
  uint16_t sourceaddr;
  uint8_t  packet_type;
} __attribute__ ((packed));
typedef struct ParentMsg ParentMsg;

struct RejectMsg {
  /* The 16-bit source node address. */
  uint16_t sourceaddr;
  uint8_t  packet_type;
} __attribute__ ((packed));
typedef struct RejectMsg RejectMsg;
struct AcceptMsg {
  /* The 16-bit source node address. */
  uint16_t sourceaddr;
  uint8_t  packet_type;
} __attribute__ ((packed));
typedef struct AcceptMsg AcceptMsg;
struct HelpMsg {
  /* The 16-bit source node address. */
  uint16_t sourceaddr;
  uint8_t  packet_type;
} __attribute__ ((packed));
typedef struct HelpMsg HelpMsg;
struct AckMsg {
  /* The 16-bit source node address. */
  uint16_t sourceaddr;
  uint8_t  packet_type;

  } __attribute__ ((packed));
typedef struct AckMsg AckMsg;
struct DepthMsg {
  /* The 16-bit source node address. */
  uint16_t sourceaddr;
  uint8_t  packet_type;
  uint8_t  level;
} __attribute__ ((packed));
typedef struct DepthMsg DepthMsg;
struct parent_child
  {
	uint16_t children[MAX_CHILDREN];
	uint16_t parent;
	uint8_t position;
	boolean finished;
  }__attribute__ ((packed));
  typedef struct parent_child parent_child;
struct Header
{
  uint8_t identifier;
  //GTime timestamp;
}__attribute__ ((packed));
typedef struct Header Header;

enum {
  AM_REQUESTMSG = 64,
  AM_PARENTMSG = 65,
  AM_REJECTMSG = 66,
  AM_ACCEPTMSG = 67,
  AM_HELPMSG = 68,
  AM_ACKMSG = 69,
  AM_DEPTHMSG = 70,
  AM_DEPTH_ACKMSG = 71,

};

//packet_type 00   HELLO;
//packet_type 01   HELP REQUEST;
//packet_type 02   HELP RESPONSE;
//packet_type 03   HELP ACK;
//packet_type 04   ACCEPTED ;
//packet_type 05   REJECTED ;
//packet_type 06   DEPTH ;
//packet_type 07   DEPTH ACK;
//packet_type 08   PARENT MSG;

