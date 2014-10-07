includes Tree;
#define MAX_REQUESTS 10
#define ALLOWED_REQUESTS 5
#define MAX_CHILDREN 4
#define NUM_NODES 50
module TreeM {
  provides {
    interface Tree;
  }
  uses {
    interface Timer as StartTreeConstructionTimer;
    interface Random;
    interface Timer as ParentMessageTimer;
    interface Timer as AckTimer;
	interface Timer as BroadcastTimer;
	interface Timer as DepthTimer;
	interface Timer as DepthAckTimer;
	interface Timer as ThresholdTimer;
    interface ReceiveMsg as ReceiveRequest;
    interface SendMsg as SendRequest;
    interface ReceiveMsg as ReceiveParent;
    interface SendMsg as SendParent;
    interface ReceiveMsg as ReceiveReject;
    interface SendMsg as SendReject;
    interface ReceiveMsg as ReceiveAccept;
    interface SendMsg as SendAccept; 
	interface ReceiveMsg as ReceiveHelp;
    interface SendMsg as SendHelp; 
	interface ReceiveMsg as ReceiveAck;
	interface SendMsg as SendAck;
	interface ReceiveMsg as ReceiveDepth;
	interface SendMsg as SendDepth;
  }
} implementation {
  TOS_Msg tree_packet;
  TOS_Msg depth_packet;
  TOS_MsgPtr buffer;
  bool accept = FALSE;
  bool helped = FALSE;
  bool call_for_help = FALSE;
  bool help_acknowledged = FALSE;
  bool depth_ack_received = FALSE;
  bool accept_already_sent = FALSE;
  uint8_t number_of_accept_sent =0;
  uint8_t number_of_requests =0;
  uint8_t level=0;
  parent_child links; 
  uint16_t second_choice_parent=0xFFFF;
  uint16_t blacklist[5];
  uint16_t requests[NUM_NODES];
  uint8_t depth_acks[MAX_CHILDREN];
  uint8_t depth_retransmission_count = 0;
  uint16_t child_count = 0;
  uint16_t BL_count = 0;
  uint8_t depthMessageCount=0;
  int isAdded(uint16_t) ;
  void deleteChild(uint16_t node);
  //********
  //********
  //Additional functions written by me
  //********
  //********
  /* returns -1 if the specified node is not in the child list else returns the position */
  int isAdded(uint16_t node)
  {
   int i;
   for (i=0;i<child_count;i++)
   {
      if (links.children[i] == node)
      return 1;
   }
    return -1;
  } 
  int find(uint16_t node)
  {
	int i;
   for (i=0;i<child_count;i++)
   {
      if (depth_acks[i] == node)
      return 1;
   }
    return -1;
  }
  /* returns TRUE is the specified node is in the blacklist*/
  bool isBlacklisted(uint16_t node)
  {
   int i;
   for (i=0;i<BL_count;i++)
   {
      if (blacklist[i] == node)
      return TRUE;
   }
    return FALSE;
  } 
  /* deletes the specified child from the children array*/
  void deleteChild(uint16_t node)
  {
	int i,j;
	dbg(DBG_TEMP,"Removing child %d from node %d's child list\n",node,TOS_LOCAL_ADDRESS);
	for(i=0;i<child_count;i++)
	{
		if (links.children[i]==node)
			{
				for(j=i;j<child_count-1;j++)
					links.children[j] = links.children[j+1];
				return;
			}
	}
	
  }
  //********
  //********
  //Start,Stop and Init commands
  //********
  //********
  command void Tree.init() {
    child_count=0;
	links.parent = 0xFFFF;
	level=1;
	links.position = 1;
	links.finished = FALSE;
    if(TOS_LOCAL_ADDRESS == 0)
    {
     links.parent = -1;
		 helped = TRUE;
		 accept = TRUE;
		 
	}
  }
  /* Returns a structure of type parent_child containing the children list and the node's parent*/
  /* This function will be called to start the tree construction*/
  command parent_child * Tree.start() 
  {
	if(TOS_LOCAL_ADDRESS == 0)
		call StartTreeConstructionTimer.start(TIMER_ONE_SHOT, 100);
	call ThresholdTimer.start(TIMER_ONE_SHOT, 200000);
	return &links;
  }
  command void Tree.stop() {
    call StartTreeConstructionTimer.stop();
    call ParentMessageTimer.stop();
    call AckTimer.stop();
	call BroadcastTimer.stop();
	call DepthTimer.stop();
	call DepthAckTimer.stop();
	call ThresholdTimer.stop();
  }
  //********
  //********
  //Code for various Timers
  //********
  //********
  /*Broadcast a REQUEST message when this Timer is fired*/
  event result_t StartTreeConstructionTimer.fired() 
  {
	RequestMsg *a = (RequestMsg *) tree_packet.data;
    a->sourceaddr = TOS_LOCAL_ADDRESS;
    a->packet_type = 00;                                                            //tree packet of type HELLO message
    call SendRequest.send(TOS_BCAST_ADDR, sizeof(RequestMsg), &tree_packet);
    dbg(DBG_TEMP, "Broadcast sent from ROOT\n");
    return SUCCESS;
  }
  /*If the node has not yet been accepted by any other node as his child and The MAX_REQUESTS limit has not been reached send
  the parentmsg to the parent. If the limit has been reached and the parent has not responded send the parentmsg to the second 
  choice parent. If the second choice parent does not repond send a HELP broadcast*/
  event result_t ParentMessageTimer.fired() {
  int waitTime;
  HelpMsg *hm3;
  ParentMsg *p;
 
  
  if( (accept == FALSE) && (number_of_requests < MAX_REQUESTS))
    {
		p = (ParentMsg *) tree_packet.data;
		p->sourceaddr = TOS_LOCAL_ADDRESS;
		//p->packet_type =08;
        call SendParent.send(links.parent, sizeof(ParentMsg), &tree_packet);
        dbg(DBG_TEMP, "Sending a PARENT REQUEST to %d\n", links.parent);
        waitTime = (uint8_t) call Random.rand();
        call ParentMessageTimer.start(TIMER_ONE_SHOT, waitTime*10);
    	number_of_requests++;
		return;
    }
  if((accept==FALSE) && (number_of_requests == MAX_REQUESTS))
    {
		if(links.parent== second_choice_parent)
		{
				hm3 = (HelpMsg *) tree_packet.data;
				hm3->sourceaddr = TOS_LOCAL_ADDRESS;
				hm3->packet_type = 01;
				links.parent = 0xFFFF;
				call_for_help = TRUE;
				call SendHelp.send(TOS_BCAST_ADDR, sizeof(HelpMsg), &tree_packet);
				dbg(DBG_TEMP, "Broadcasting HELP \n");
			
		}
		if(links.parent!= second_choice_parent && call_for_help == FALSE)
		{
		atomic
			{
				number_of_requests=0;
				links.parent= second_choice_parent;
				p = (ParentMsg *) tree_packet.data;
				p->sourceaddr = TOS_LOCAL_ADDRESS;
			//	p->packet_type =08;
				call SendParent.send(links.parent, sizeof(ParentMsg), &tree_packet);
				dbg(DBG_TEMP, "Sending a PARENT REQUEST to second_choice_parent %d\n", links.parent);
				waitTime = (uint8_t) call Random.rand();
				call ParentMessageTimer.start(TIMER_ONE_SHOT, waitTime*10);
				number_of_requests++;
			}
		}
    }
  
  }
  event result_t BroadcastTimer.fired() 
  {
	RequestMsg *b;
	b = (RequestMsg *) tree_packet.data;
	b->sourceaddr = TOS_LOCAL_ADDRESS;
	b->packet_type= 00;
	call SendRequest.send(TOS_BCAST_ADDR, sizeof(RequestMsg), &tree_packet);
  }
    
  event result_t AckTimer.fired() 
  {
	HelpMsg *hmr;
	hmr = (HelpMsg *) tree_packet.data;                                          // packet to send
	hmr->packet_type = 03;                                                       // tree packet of type
	hmr->sourceaddr = TOS_LOCAL_ADDRESS;
	call SendAck.send(links.parent, sizeof(HelpMsg),&tree_packet);
  }
  
  /* When the ThresholdTimer is fired the leaf nodes send a Depth Msg to their parent*/
  event result_t ThresholdTimer.fired() 
  {
	int i;
	int waitTime;
	call ParentMessageTimer.stop();
	dbg(DBG_TEMP,"Threshold Timer fired, ----INFO LOG ---\n");
	dbg(DBG_TEMP,"parent: %d\n",links.parent);
	dbg(DBG_TEMP,"Number of children: %d\n",child_count);
	for(i = 0; i < child_count; i++)
	{
		dbg(DBG_TEMP, " %d \n", links.children[i]);
	}
	level =1;
	waitTime= (TOS_LOCAL_ADDRESS - links.parent)*(TOS_LOCAL_ADDRESS - links.parent)*50;
	if(TOS_LOCAL_ADDRESS!=0 && links.parent!= 0xFFFF)
	{
		if(child_count == 0)
		{
			call DepthTimer.start(TIMER_ONE_SHOT, waitTime);
			dbg(DBG_TEMP, " Sending depth message\n");
		}	
		
    } 
  }
  event result_t DepthTimer.fired() 
  {
    //TOS_Msg depth_packet;
		int waitTime = ((uint8_t) call Random.rand()) + 200*depth_retransmission_count;
		
	if (child_count==0)
	{
		DepthMsg *d = (DepthMsg *) depth_packet.data;
		/*Sending depth message from the leaves*/ 
		d->level =1;
		d->packet_type =06;
		d->sourceaddr=TOS_LOCAL_ADDRESS;
		depthMessageCount=1;
	}
	call SendDepth.send(links.parent, sizeof(DepthMsg), &depth_packet);
	call DepthAckTimer.start(TIMER_ONE_SHOT,waitTime );
	dbg(DBG_TEMP,"Depth Ack Timer started\n");
  }  
  event result_t DepthAckTimer.fired() 
  {
		if(!depth_ack_received)
		{
			if(depth_retransmission_count != ALLOWED_REQUESTS)
			{
				call DepthTimer.start(TIMER_ONE_SHOT, 1);
				dbg(DBG_TEMP,"Depth Ack Timer started\n");
				depth_retransmission_count++;
			}
		}else
		{
			links.finished = TRUE;
		}
  }  
  //********
  //********
  //Code for various SendDones
  //********
  //********
  
   event result_t SendRequest.sendDone(TOS_MsgPtr msg, bool success) {
    dbg(DBG_USR2,"MSG sent\n");
    return SUCCESS;
  }
  event result_t SendHelp.sendDone(TOS_MsgPtr msg, bool success) {
//(DBG_TEMP,"successful help message transmission");
	return SUCCESS;
  }
  event result_t SendDepth.sendDone(TOS_MsgPtr msg, bool success) {
	//TOS_Msg depth_packet;
   if(depthMessageCount==child_count)
   {
		DepthMsg * dm2;
		//uint8_t count=0;
		int waitTime;
		dm2 = (DepthMsg *)depth_packet.data;
		waitTime= (TOS_LOCAL_ADDRESS - links.parent)*(TOS_LOCAL_ADDRESS - links.parent)*50;
		dbg(DBG_TEMP,"Sending level information to my parent %d\n",links.parent);	
		dm2->sourceaddr  =TOS_LOCAL_ADDRESS;
		dm2->packet_type =06;
		dm2->level = level;		
		call DepthTimer.start(TIMER_ONE_SHOT, waitTime);		
		//call SendDepth.send(links.parent, sizeof(DepthMsg), &depth_packet);
		depthMessageCount=9999;
	}
   
    return SUCCESS;
  }
  event result_t SendAck.sendDone(TOS_MsgPtr msg, bool success) {
   
    return SUCCESS;
  }
  event result_t SendParent.sendDone(TOS_MsgPtr msg, bool success) {
   
    return SUCCESS;
  }
  event result_t SendReject.sendDone(TOS_MsgPtr msg, bool success) {
   
  }
  event result_t SendAccept.sendDone(TOS_MsgPtr msg, bool success) {
   
    return SUCCESS;
  }
  //********
  //********
  //Code for Tasks
  //********
  //********
  /*Task to handle PARENT REQUESTs sent to a node. If child count of the node is less than the allowed MAX_CHILDREN limit 
  and the number of requests from this node is less than ALLOWED_REQUESTS, send an AcceptMsg. If the number of requests 
  goes above ALLOWED REQUESTS, it is assumed there might be some problem with the comunication channel. The node is 
  deleted from the list and blacklisted, no further request from this node is entertained. */
  task void parentRequestHandler()
    {   
        int i = 0;
	    ParentMsg *p;
		AcceptMsg *a;
		RejectMsg *r;
		accept_already_sent = FALSE;
        p = (ParentMsg *) buffer->data;
		requests[p->sourceaddr]++;
		if(!isBlacklisted(p->sourceaddr))
		{
			if(!(requests[p->sourceaddr]==ALLOWED_REQUESTS))
			{
				if(child_count < MAX_CHILDREN)
				{
					a = (AcceptMsg *) tree_packet.data;
					a->packet_type =04;
					
					if(isAdded(p->sourceaddr) == -1)
						links.children[child_count++] = p->sourceaddr;	
					dbg(DBG_TEMP," Received a PARENT REQUEST from %d\n",p->sourceaddr);
					dbg(DBG_TEMP, "Node %d's children are: \n", TOS_LOCAL_ADDRESS);
					for(i = 0; i < child_count; i++)
					{
					   dbg(DBG_TEMP, " %d \n", links.children[i]);
					}
					a->sourceaddr = TOS_LOCAL_ADDRESS;
					call SendAccept.send(p->sourceaddr, sizeof(AcceptMsg), &tree_packet);
					dbg(DBG_TEMP,"Sending an ACCEPTED message  to %d\n",p->sourceaddr);
				}
				/* If number of requests are less than ALLOWED_REQUESTS and the node was already added to the children list send an AcceptMsg, If it was not already added send a reject message*/
				else
				{
					if(isAdded(p->sourceaddr) != -1)
					{
						a = (AcceptMsg *) tree_packet.data;
						a->packet_type =04;
						call SendAccept.send(p->sourceaddr, sizeof(AcceptMsg), &tree_packet);
						dbg(DBG_TEMP,"Sending an ACCEPTED message  to %d \n",p->sourceaddr);
					}
					else
					{
						r = (RejectMsg *) tree_packet.data;
						r->sourceaddr= TOS_LOCAL_ADDRESS;
						r->packet_type =05;
						call SendReject.send(p->sourceaddr, sizeof(RejectMsg), &tree_packet);
						dbg(DBG_TEMP,"Sending a REJECTED message  to %d\n",p->sourceaddr);
					}	
					
				}
			}
			else
			{
				if(isAdded(p->sourceaddr) != -1)
				{
					deleteChild(p->sourceaddr);
					for(i = 0; i < child_count; i++)
					{
					   dbg(DBG_TEMP, " %d \n", links.children[i]);
					}	
					blacklist[BL_count++]=p->sourceaddr;
					dbg(DBG_TEMP,"Blacklisting and removing node %d\n",p->sourceaddr);
					child_count--;
				}
			}
		}
		
    }
	/* Send an acknowledgement for the HELP RESPONSE. Wait for a randm amount of time and fire the AckTimer*/
   task void sendAck()
    {
		int waitTime;
		helped = TRUE;
		accept = TRUE;
		waitTime = (uint8_t) call Random.rand();
		call AckTimer.start(TIMER_ONE_SHOT, waitTime*20);
				
    }
	/* When a depth Message is received, check the level value and take action accordingly */
    task void depthMessageHandler()
    {
		//TOS_Msg depth_packet;
		DepthMsg *dm1;
		DepthMsg *dm2;
		dm2 = (DepthMsg *)depth_packet.data;
		dm1 = (DepthMsg *)buffer->data; 
		if(dm1->packet_type == 06)
		{
			if(isAdded(dm1->sourceaddr))
			{
				//dbg(DBG_TEMP,"Depth Packet received from %d\n",dm1->sourceaddr);
				if(depthMessageCount<child_count)
				{
					if(dm1->level >= level)
					{
						level = dm1->level + 1;
					}
					
					if (find(dm1->sourceaddr)!=1)
						depthMessageCount++;
					depth_acks[depthMessageCount]=dm1->sourceaddr;
					dbg(DBG_TEMP,"My child count is %d and number of depth packets received is %d\n",child_count, depthMessageCount);
					if(depthMessageCount==child_count)
					{
						//level++;
						dbg(DBG_TEMP,"My level is %d\n",level);		
						//links.position = level;
						if(TOS_LOCAL_ADDRESS == 0)
							links.finished = TRUE;
						
					}
				}
				dm2->sourceaddr  =TOS_LOCAL_ADDRESS;
				dm2->packet_type =07; 
				dbg(DBG_TEMP, "Level Echo---- %d\n", level);
				links.position = level;
				call SendDepth.send(dm1->sourceaddr, sizeof(DepthMsg), &depth_packet);
			}
		}
		if(dm1->packet_type == 07)
		{
			depth_ack_received = TRUE;
			dbg(DBG_TEMP,"Depth Ack received\n");
		}
		
    }
    
  //********
  //********
  //Code for Receives
  //********
  //********
  /* Code for receive event when a request message is received*/
  /*If the packet contains the correct packet type elect the sender as the parent, wait for certain period of time and call the ParentMessageTimer*/
  /*If the parent has already been elected, choose the sender as the second choice parent*/
  event TOS_MsgPtr ReceiveRequest.receive(TOS_MsgPtr recv_packet) {
  
  RequestMsg *a;
  int waitTime;
  a = (RequestMsg *) recv_packet->data;
  if(a->packet_type==00)
	{
		//dbg(DBG_TEMP, "Message received");
		if(TOS_LOCAL_ADDRESS != 0)
		{
			if(a->sourceaddr<TOS_LOCAL_ADDRESS)
			{
				if(links.parent == 0xFFFF)
				{
					links.parent = a->sourceaddr;
					dbg(DBG_TEMP, "Broadcast received at Node: %d from : %d\n", TOS_LOCAL_ADDRESS, a->sourceaddr);
					
					waitTime = (TOS_LOCAL_ADDRESS - a->sourceaddr)*(TOS_LOCAL_ADDRESS - a->sourceaddr)*100;
					call ParentMessageTimer.start(TIMER_ONE_SHOT, waitTime);
							
				}
				else
				{
					//if(links.parent!=0xFFFF && second_choice_parent==0xFFFF)
					if(links.parent!=0xFFFF )
					{
						if(a->sourceaddr < TOS_LOCAL_ADDRESS)
							second_choice_parent = a->sourceaddr;
					}
				}
			}
		}
	}
	return recv_packet;
  }
  
  event TOS_MsgPtr ReceiveParent.receive(TOS_MsgPtr recv_packet) {
    TOS_MsgPtr temp;
    temp =buffer;
    buffer = recv_packet;
    post parentRequestHandler();
    
    return temp;
  }
  /* If the packet received is an Accept packet, and if the local node hasn't yet called for help, modify the accept variable as 1, 
  indicating the node is accepted by its parent, and start the BroadcastTimer */
  event TOS_MsgPtr ReceiveAccept.receive(TOS_MsgPtr recv_packet) {
	uint8_t waitTime;
	AcceptMsg *am;
	am= (AcceptMsg *) recv_packet->data;
	if(am->packet_type ==04)
	{
		if(accept == FALSE && call_for_help == FALSE)
		{
		  links.parent = am->sourceaddr;
		  accept = TRUE;
		  
		  dbg (DBG_TEMP,"I am Accepted, ACCEPTED received from %d\n",am->sourceaddr);
		  if(TOS_LOCAL_ADDRESS != 0)
			{
				dbg(DBG_TEMP, "Now Broadcasting HELLO from Node: %d\n",TOS_LOCAL_ADDRESS);
				waitTime = (uint8_t) call Random.rand();
				call BroadcastTimer.start(TIMER_ONE_SHOT, waitTime*20);
				
			}
		}
	}
      return recv_packet;
  }
 event TOS_MsgPtr ReceiveReject.receive(TOS_MsgPtr recv_packet) {
	RejectMsg *r= (RejectMsg *) recv_packet->data;
	if(r->packet_type ==05)
	{
		links.parent = 0xFFFF;
		helped =FALSE;
		accept =FALSE;
		number_of_requests=MAX_REQUESTS;
		//call ParentMessageTimer.stop();
	}
	dbg(DBG_TEMP, "Received a REJECTED from Node: %d\n",r->sourceaddr);
    return recv_packet;
  }
  event TOS_MsgPtr ReceiveHelp.receive(TOS_MsgPtr recv_packet) {
	TOS_MsgPtr temp;
	HelpMsg *hm,*h;
	RejectMsg *rm;
	temp =buffer;
	buffer = recv_packet;
	hm = (HelpMsg *) buffer->data;         											//received packet
	//dbg(DBG_TEMP,"Packet received from %d\n",hm->sourceaddr);
	if((hm->packet_type == 01) && (accept ==TRUE)&& (links.parent !=0xFFFF) )			// If it is a HELP REQUEST and this node has been accepted by some other node as a child
	{
	
		if((isAdded(hm->sourceaddr) == -1) )          								// If the Requesting node is not in the children list do the following
		{
			if((child_count <MAX_CHILDREN)&& (TOS_LOCAL_ADDRESS < hm->sourceaddr))  // The condition serves the purpose of minimizing HLEP RESPONSES in the system.
			{
				h = (HelpMsg *) tree_packet.data;          							// packet to send
				h->sourceaddr = TOS_LOCAL_ADDRESS;
				h->packet_type = 02;                      							// The packet is a HELP RESPONSE packet
				call SendHelp.send(hm->sourceaddr, sizeof(HelpMsg), &tree_packet);
				dbg(DBG_TEMP,"Sending HELP RESPONSE to %d\n",hm->sourceaddr);
				//dbg(DBG_TEMP,"My parent is %d and my accept value is %d\n",links.parent,accept);
			}
		}
		/*If the requesting child was already a child delete it from the list and do nothing. ( Probably the channel is noisy)*/
		else
		{
			int i;
			deleteChild(hm->sourceaddr);                                            
			
         	for(i = 0; i < child_count; i++)
			{
			   dbg(DBG_TEMP, " %d \n", links.children[i]);
			}	
			child_count--;	
		}
		return temp;
	}
	if(hm->packet_type == 02 && (helped == FALSE)&& (accept==FALSE))                      // If it is a HELP RESPONSE packet and the node hasn't been HELPED yet. Change accept to 1 and send a HELP ACK
	{
		helped = TRUE;
		accept = TRUE;
		links.parent = hm->sourceaddr;
		dbg(DBG_TEMP,"Sending HELP ACK to %d\n",hm->sourceaddr);
		post sendAck();
       	return temp;
	}
	/* If a HELP ACK is received and the number of children is less than MAX_CHILDREN add the node to the children list else reject it and send a REJECT packet*/
	if(hm->packet_type == 03) 														
	{
		int i;
		if(child_count <MAX_CHILDREN)
		{
			if(isAdded(hm->sourceaddr) == -1)
				links.children[child_count++] = hm->sourceaddr;
			dbg(DBG_TEMP," Received a HELP ACK from %d\n",hm->sourceaddr);
			dbg(DBG_TEMP, "Node %d's children are: \n", TOS_LOCAL_ADDRESS);
			for(i = 0; i < child_count; i++)
				{
				   dbg(DBG_TEMP, " %d \n", links.children[i]);
				}
			number_of_accept_sent++;	
		}
		else
		{
			if(isAdded(hm->sourceaddr) != -1)
				deleteChild(isAdded(hm->sourceaddr));
			rm = (RejectMsg *) tree_packet.data;
			rm->sourceaddr= TOS_LOCAL_ADDRESS;
			rm->packet_type =05;
			call SendReject.send(hm->sourceaddr, sizeof(RejectMsg), &tree_packet);
			dbg(DBG_TEMP,"Help rejected message  to %d\n",hm->sourceaddr);
		}
			
		return temp;
	}
	return temp;
  }
 
   
  event TOS_MsgPtr ReceiveAck.receive(TOS_MsgPtr recv_packet) {
  
  return recv_packet;
  }
  
   event TOS_MsgPtr ReceiveDepth.receive(TOS_MsgPtr recv_packet) {
    
	TOS_MsgPtr temp;
	temp =buffer;
	buffer = recv_packet;
	post depthMessageHandler();
	return temp;
	}
  
}
