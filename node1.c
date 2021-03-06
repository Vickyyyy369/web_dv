/*************************************************************************
*   node0.c 	
*
*   Distance Vector Routing Simulation Package
*   Node event handlers
*
*   INITIALIZATION FUNCTION:	rtinit0
*	
*	1. initializes distance table for node 0
*	2. computes initial minimum path costs for node 0
*	3. send update packets to neighbors
*
* 
*   UPDATE FUNCTION: 		rtupdate0
*
*	Executes when the node receives an updates message:
*	1. receives update from neighbor
*	2. computes new values for distance table
*	3. if any minimum path cost changes, then sends update messages 
*	   to all neighbors.
*
*
*   TABLE PRINT FUNCTIONS: 	printdt_0
*
*	Prints distance table for node 0.
*
*
*   LINK HANDLER FUNCTIONS:	linkhandler_0, linkhandler_1
*
* 	These functions are called when the link cost from node 0 to 
*	node "linkid" changes from the current value to newcost. 
*	To use these, change constant LINKCHANGE in DVsim.c from 0 to 1.
* 	Leave these functions empty for the standard project. 
*
*-----------------------------------------------------------------------*/
#include 	"DVsim.h"
extern int send_num[];
extern int receive_num[];


/*------------------------
*	rtinit0
*------------------------*/
void rtinit1() 
{
   int id = 1;
   int i,j;
   for(i = 0; i < NODES; i++){
      min_cost[id][i] = linkcost[id][i];
      // min_cost[i][id] = linkcost[i][id];

   }

   // 初始化距离表 
   for(i = 0; i < NODES; i++){  //下一跳节点
      if(linkcost[id][i] != INF && i != id){
         for( j = 0; j < NODES; j++){ //目的节点
         if(j!=id){
            if(i == j) dt[id].costs[j][i] = linkcost[id][i];
            else  dt[id].costs[j][i] = INF;
         }
         
      } 
      }      
   }
   printf("Time t=%.2f        NODE 1:  Initial Distance Table\n",get_clock());
   printdt_1(&dt[id]);


   // 向每个邻居节点发送数据包 
   int k,t;
   for(k = 0; k < NODES; k++){
      if(k == id || linkcost[id][k] == INF) continue;
      struct rtpkt packet;
      packet.sourceid = id;
      packet.destid = k;
      for(t = 0; t < NODES; t++)  packet.mincost[t] = linkcost[id][t]; //初始时，距离向量为距离值
      tolayer2(packet);
      send_num[packet.sourceid]++;
      receive_num[packet.destid]++;
      printf("Time t=%.2f        NODE 1:  Sending cost update to Node %d:",get_clock(),k);
      printf("    %d   %d   %d   %d   %d\n",packet.mincost[0],packet.mincost[1],packet.mincost[2],packet.mincost[3],packet.mincost[4]);
      
   }
      printf("\n");

}

/*------------------------
*	rtupdate0
*------------------------*/
void rtupdate1 (struct rtpkt *rcvdpkt)
{
   int id = 1;
   int sourceid = rcvdpkt->sourceid;  //一定是从邻居节点收到数据包

   printf("Time t=%.2f        NODE 1:  Received cost update from Node %d:",get_clock(),sourceid);
   printf("    %d,   %d,   %d,   %d,   %d\n",rcvdpkt->mincost[0],rcvdpkt->mincost[1],rcvdpkt->mincost[2],rcvdpkt->mincost[3],rcvdpkt->mincost[4]);
   int i,j;
   int change = 0;
   for(i = 0; i < NODES; i++){  //目的节点,考虑下一跳节点为sourceid
      if(rcvdpkt->mincost[i] < INF && i != id ){
         int new_dis = rcvdpkt->mincost[i] + linkcost[id][sourceid];
         dt[id].costs[i][sourceid] = new_dis;
         if(new_dis < min_cost[id][i]){ //是否可回忆更新全局min_cost
               min_cost[id][i] = new_dis;
               change = 1;
            }    
         }
      } 
   printdt_1(&dt[id]);
   printf("\n");

   // 向每个邻居节点发送更新数据包
   if(change == 1){
      printf("Time t=%.2f        NODE 1:  New minimum costs.\n");
      int k,t;
      for(k = 0; k < NODES; k++){
         if(k == id || linkcost[id][k] == INF) continue;
         struct rtpkt packet;
         packet.sourceid = id;
         packet.destid = k;
         for (t=0; t<NODES; t++){
            if(t==id) packet.mincost[t] = 0;
            else packet.mincost[t] = min_cost[id][t];
      } 
      tolayer2(packet);
      send_num[packet.sourceid]++;
      receive_num[packet.destid]++;
      printf("Time t=%.2f        NODE 1:  Sending cost update to Node %d:",get_clock(),k);
      printf("    %d,   %d,   %d,   %d,   %d\n",packet.mincost[0],packet.mincost[1],packet.mincost[2],packet.mincost[3],packet.mincost[4]);
      
   }
   printf("\n");
   }
   else printf("Time t=%.2f        NODE 1:  No change in minimum costs.\n\n");
}

/*----------------------------------------------------
*	printdt_0 
*
*	Prints distance table at node 0.
*
*	(Format will be different for other nodes.)
*-----------------------------------------------------*/
void printdt_1 (struct distance_table *t)
{
   printf("\t\t  ----------------------\n");
   printf("\t\t      |         via     \n");
   printf("\t\t   D1 |    0     2     4\n");
   printf("\t\t  ----|-----------------\n");
   printf("\t\t     0|  %3d   %3d   %3d\n", t->costs[0][0], t->costs[0][2], t->costs[0][4]);
   printf("\t\tdest 2|  %3d   %3d   %3d\n", t->costs[2][0], t->costs[2][2], t->costs[2][4]);
   printf("\t\t     3|  %3d   %3d   %3d\n", t->costs[3][0], t->costs[3][2], t->costs[3][4]);
   printf("\t\t     4|  %3d   %3d   %3d\n", t->costs[4][0], t->costs[4][2], t->costs[4][4]);
   printf("\t\t  ----------------------\n");
}


/*--------------------------------------------------------------
*	linkhandler0
*---------------------------------------------------------------*/
void linkhandler1 (int linkid, int newcost) {}
