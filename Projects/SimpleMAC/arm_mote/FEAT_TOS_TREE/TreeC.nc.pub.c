#include "defs/esnl_pub.h"

///includes Tree;
#include "Tree.h"


///configuration TreeC {
///provides interface Tree;
///} implementation {
void TreeC_power_up()
{
///  components TreeM, RandomLFSR,TimerC, GenericComm as Comm;

  //Main.StdControl -> Comm;
  //Main.StdControl -> TimerC;
  //Main.StdControl -> TreeM;
  Tree = TreeM.Tree;
  TreeM.StartTreeConstructionTimer -> TimerC.Timer[unique("Timer")];
  TreeM.ParentMessageTimer -> TimerC.Timer[unique("Timer")];
  TreeM.BroadcastTimer -> TimerC.Timer[unique("Timer")];
  TreeM.DepthTimer -> TimerC.Timer[unique("Timer")];
  TreeM.AckTimer -> TimerC.Timer[unique("Timer")];
  TreeM.DepthAckTimer -> TimerC.Timer[unique("Timer")];
  TreeM.ThresholdTimer -> TimerC.Timer[unique("Timer")];
  TreeM.SendRequest -> Comm.SendMsg[AM_REQUESTMSG];
  TreeM.ReceiveRequest -> Comm.ReceiveMsg[AM_REQUESTMSG];
  TreeM.SendParent -> Comm.SendMsg[AM_PARENTMSG];
  TreeM.ReceiveParent -> Comm.ReceiveMsg[AM_PARENTMSG];
  TreeM.SendReject -> Comm.SendMsg[AM_REJECTMSG];
  TreeM.ReceiveReject -> Comm.ReceiveMsg[AM_REJECTMSG];
  TreeM.SendAccept -> Comm.SendMsg[AM_ACCEPTMSG];
  TreeM.ReceiveAccept -> Comm.ReceiveMsg[AM_ACCEPTMSG];
  TreeM.SendHelp -> Comm.SendMsg[AM_HELPMSG];
  TreeM.ReceiveHelp -> Comm.ReceiveMsg[AM_HELPMSG];
  TreeM.SendAck -> Comm.SendMsg[AM_ACKMSG];
  TreeM.ReceiveAck -> Comm.ReceiveMsg[AM_ACKMSG];
  TreeM.SendDepth -> Comm.SendMsg[AM_DEPTHMSG];
  TreeM.ReceiveDepth -> Comm.ReceiveMsg[AM_DEPTHMSG];
  TreeM.Random -> RandomLFSR;
}
