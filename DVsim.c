/*****************************************************************
*	DVsim.c 	Distance Vector Routing Simulation Package
*
*	Event driven simulation of distributed, asynchronous, 
*	distance vector routing.  This file contains the network
*	simulation code which emulates layer 2 and below: 
*
*  		- simulates packet tranmissions between two 
*		  physically connected nodes
*
*		- assumes with no packet loss or corruption
*
*  		- calls initializations routines rtinit0, etc., before
*    		  beginning simulation
*
*
*	Original code from Kurose and Ross, Computer Networking
*
*	Modified by S. Dykes 	Apr. 2000
*	1. Changed RNG to use drand48() with seed = getpid()
*	2. Replaced 999 with INF 
*	3. created common include file DVsim.h
*	4. extended to 6 node network 
*
*---------------------------------------------------------------*/
#define		DVSIM_H
#include 	"DVsim.h"

#define 	FROM_LAYER2     2
#define 	LINK_CHANGE     10

/*----------------------------------------
* 	Event list and possible events
*---------------------------------------*/
struct event 
{
   double	evtime;         /* event time */
   int 		evtype;         /* event type code */
   int 		eventity;       /* entity where event occurs */
   struct rtpkt *rtpktptr; 	/* ptr to packet (if any) assoc w/ this event */
   struct event *prev;
   struct event *next;
};
struct event *evlist = NULL;   

/*--------------------------------
* 	Simulation clock
*-------------------------------*/
double	clocktime = 0.000;
double	get_clock() { return clocktime; }

/*--------------------------------
* 	Function prototypes
*-------------------------------*/
void 	init();
void 	insertevent (struct event *);
void 	printevlist();
void 	tolayer2 (struct rtpkt );

/*-------------------------------
*	MAIN
*-------------------------------*/

int receive_num[NODES];
int send_num[NODES];


main()
{
   struct event *eventptr;
   
   /*----------------------------------------
   * 	Loop until we run out of events
   *---------------------------------------*/
   init();
   while (1) 
   {
	/*----------------------------------------
	*	Get next event from top of list
	*       Done if event list is empty
	*---------------------------------------*/
        if (! (eventptr = evlist) ) break;            
        evlist = evlist->next;        
        if (evlist) evlist->prev=NULL;

	/*----------------------------------------
	*	Trace info
	*---------------------------------------*/
        if (TRACE>1) {
           printf("\nMAIN: rcv event, t=%.3f, at %d", 	
			eventptr->evtime, eventptr->eventity);
           if (eventptr->evtype == FROM_LAYER2 ) {
	      printf(" src:%2d,",  eventptr->rtpktptr->sourceid);
              printf(" dest:%2d,", eventptr->rtpktptr->destid);
              printf(" contents: ");
	      int i;
         for(i=0; i<NODES; i++) printf ("%3d ",eventptr->rtpktptr->mincost[i]);
              printf("\n");
         
	   }
        }
	/*----------------------------------------
	*	Update clock to next event time
	*---------------------------------------*/
        clocktime = eventptr->evtime;    

	/*----------------------------------------
	*	Call event handler 
	*---------------------------------------*/
        if (eventptr->evtype == FROM_LAYER2 ) {
            if      (eventptr->eventity == 0) rtupdate0(eventptr->rtpktptr);
	    else if (eventptr->eventity == 1) rtupdate1(eventptr->rtpktptr);
	    else if (eventptr->eventity == 2) rtupdate2(eventptr->rtpktptr);
	    else if (eventptr->eventity == 3) rtupdate3(eventptr->rtpktptr);
	    else if (eventptr->eventity == 4) rtupdate4(eventptr->rtpktptr);
	   //  else if (eventptr->eventity == 5) rtupdate5(eventptr->rtpktptr);
            else { printf("Panic: unknown event entity\n"); exit(0); }

      //统计发送和接收的数据包数
      // send_num[eventptr->rtpktptr->sourceid]++;
      // receive_num[eventptr->rtpktptr->destid]++;

	}
        else if (eventptr->evtype == LINK_CHANGE ) {
            if (clocktime < 10001.0) { linkhandler0(1,20); linkhandler1(0,20); }
	    else   		     { linkhandler0(1,1);  linkhandler1(0,1); }
	}
        else {   
	    printf("Panic: unknown event type\n");  
	    exit(0); 
        }
	/*--------------------------------------------------------
	*	Free memory for packet (if any) and event struct
	*-------------------------------------------------------*/
        if (eventptr->evtype == FROM_LAYER2 ) free(eventptr->rtpktptr);      
        free(eventptr);                 
   }
   /*-------------------------------
   *	SIMULATION TERMINATED 
   *-------------------------------*/
   printf("\nSimulator terminated at t=%f, no packets in medium\n", clocktime);

   /*打印发送和接收的消息总数*/
   int i,sum_send = 0,sum_receive = 0;
   printf("\n-------- The num of sent packets ----------\n");
   for(i = 0; i < NODES; i++){
      printf("Node %2d:%3d\n",i,send_num[i]);
      sum_send += send_num[i];
   }
   printf("Sum of send packets:%3d\n",sum_send);
   printf("\n-------- The num of received packets ----------\n");
   for(i = 0; i < NODES; i++){
      printf("Node %2d:%3d\n",i,receive_num[i]);
      sum_receive += receive_num[i];
   }
   printf("Sum of receive packets:%3d\n",sum_receive);
}


/*-------------------------------
*	init
*-------------------------------*/
void 	init()     
{
   int i;
   double	sum, avg; 
   struct event *evptr;  
  
   /*---------------------
   * 	Input TRACE
   *--------------------*/
   printf ("Enter value for TRACE (no trace =0,  user= 1 or 2,  developer= 3): ");
   scanf ("%d", &TRACE);

   /*---------------------
   * 	Test RNG
   *--------------------*/
   srand48 ((long) getpid()); 
   for (i=0, sum=0.; i<1000; i++) sum+= drand48();
   avg = sum/1000.0;
   if (avg < 0.25 || avg > 0.75) {
   	printf("RNG PROBLEM\n");
	printf("Average of 1000 random numbers is %g   (expected 0.5).\n", avg);
   	printf("It is likely that random number generation on your machine\n"); 
   	printf("is different from what this emulator expects.  Please take\n");
   	printf("a look at the routine init() in the emulator code. \n");
   	exit(0);
   }
   /*-------------------------------------
   * 	Initialize clock and nodes
   *------------------------------------*/
   clocktime=0.0; 
   rtinit0();
   rtinit1();
   rtinit2();
   rtinit3();
   rtinit4();
   // rtinit5();

   /*-------------------------------------
   * 	Initialize future link changes	
   *------------------------------------*/
   if (LINKCHANGES==1)   
   {
   	evptr = (struct event *)malloc(sizeof(struct event));
   	evptr->evtime =  10000.0;
   	evptr->evtype =  LINK_CHANGE;
   	evptr->eventity =  -1;
   	evptr->rtpktptr =  NULL;
   	insertevent(evptr);
   	evptr = (struct event *)malloc(sizeof(struct event));
   	evptr->evtype =  LINK_CHANGE;
   	evptr->evtime =  20000.0;
   	evptr->rtpktptr =  NULL;
   	insertevent(evptr);    
   }
}

/********************* EVENT HANDLING ROUTINES *******
*
*  	These routines handle the event list 
*
*****************************************************/
/*------------------------------------------------------------
*	insertevent
*
*    	Inserts object in event list.  List is sorted by time.
*	p -> object to be inserted 
* 	q -> top of list 
*------------------------------------------------------------*/
void 	insertevent (struct event *p)
{
   struct event *q,*qold;

   if (TRACE>3) {
      printf("            INSERTEVENT: time is %lf\n",clocktime);
      printf("            INSERTEVENT: future time will be %lf\n",p->evtime); 
   }
   q = evlist;     	
   if (q==NULL) {			
 	/*--------------------------------
       	*   Empty list - insert at top
 	*-------------------------------*/
        evlist=p;
        p->next=NULL;
        p->prev=NULL;
   }
   else {
 	/*----------------------------------------
       	*   Find event time > p's time
 	*---------------------------------------*/
        for (qold=q; q !=NULL && p->evtime > q->evtime; q=q->next)  qold=q; 

        if (q==NULL) {   	
 	   /*---------------------
       	   *   End of list 
 	   *--------------------*/
           qold->next = p;
           p->prev = qold;
           p->next = NULL;
        }
        else if (q==evlist) { 		
 	   /*---------------------
       	   *   Top of list 
 	   *---------------------*/
           p->next=evlist;
           p->prev=NULL;
           p->next->prev=p;
           evlist = p;
        }
        else {     			
 	   /*---------------------
       	   *   Middle of list 
 	   *---------------------*/
           p->next=q;
           p->prev=q->prev;
           q->prev->next=p;
           q->prev=p;
        }  
    }
}


/*------------------------------------------------------------
*	printevlist - prints event list
*------------------------------------------------------------*/
void 	printevlist()
{
   struct event *q;
   printf("--------------\nEvent List Follows:\n");
   for (q = evlist; q!=NULL; q=q->next) 
      printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
   printf("--------------\n");
}


/*------------------------------------------------------------
*	tolayer2 
*------------------------------------------------------------*/
void 	tolayer2 (struct rtpkt packet)
{
   struct rtpkt *mypktptr;
   struct event *evptr, *q;
   double	lastime;
   int 		i;

  /*-------------------------------------------------------------------------
  *  Check for packet errors
  *  Check if source and destination id's are reasonable 
  *-------------------------------------------------------------------------*/
   if (packet.sourceid<0 || packet.sourceid >NODES) {
      printf("WARNING: illegal source id in your packet, ignoring packet!\n");
      return;
   }
   if (packet.destid<0 || packet.destid >NODES) {
      printf("WARNING: illegal dest id in your packet, ignoring packet!\n");
      return;
   }
   if (packet.sourceid == packet.destid)  {
      printf("WARNING: source and destination id's the same, ignoring packet!\n");
      return;
   }
   if (linkcost[packet.sourceid][packet.destid] == INF)  {
      printf("WARNING: source and destination not connected, ignoring packet!\n");
      return;
   }

  /*-------------------------------------------------------------------------
  *  Make a copy of the packet student just gave me since he/she may decide
  *  to do something with the packet after we return back to him/her. 
  *-------------------------------------------------------------------------*/
  mypktptr = (struct rtpkt *) malloc(sizeof(struct rtpkt));
  mypktptr->sourceid = packet.sourceid;
  mypktptr->destid   = packet.destid;
  for (i=0; i<NODES; i++)  mypktptr->mincost[i] = packet.mincost[i];

  /*---------------------
  *  Print trace info 
  *--------------------*/
  if (TRACE>2) 
  {
     printf("    TOLAYER2: source: %d, dest: %d\n              costs:", 
          	 mypktptr->sourceid, mypktptr->destid);
     for (i=0; i<4; i++)   printf("%d  ",mypktptr->mincost[i]);
     printf("\n");
  }

  /*-----------------------------------------------------------------
  *  Create future event for arrival of packet at the other side
  *-----------------------------------------------------------------*/
  evptr           = (struct event *)malloc(sizeof(struct event));
  evptr->evtype   =  FROM_LAYER2; 	/* packet will pop out from layer3 */
  evptr->eventity = packet.destid; 	/* event occurs at other entity */
  evptr->rtpktptr = mypktptr;       	/* save ptr to my copy of packet */

  /*-----------------------------------------------------------------
  *  Compute the arrival time of packet at the other end.
  *  Medium can not reorder, so make sure packet arrives between 1 and 10
  *     time units after the latest arrival time of packets
  *     currently in the medium on their way to the destination 
  *-----------------------------------------------------------------*/
  lastime = clocktime;
  for (q=evlist; q!=NULL ; q = q->next) 
  {
     if ( (q->evtype==FROM_LAYER2  && q->eventity==evptr->eventity) ) 
         lastime = q->evtime;
  }
  evptr->evtime =  lastime + 2.*drand48();

  /*------------------
  *  Trace info 
  *-----------------*/
  if (TRACE>2)  printf("    TOLAYER2: scheduling arrival on other side\n");

  /*---------------------------------------------------
  *  Insert message arrival into event list
  *--------------------------------------------------*/
  insertevent(evptr);
} 
