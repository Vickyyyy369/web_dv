/*****************************************************************
*	CS6543  Spring 2001 	Project 2
*
*	DVsim.h 	Distance Vector Routing Simulation Package
*			Include file for network shown below.
*
*
*         	             1            2
*		      [ 0 ]-------[ 1 ]--------[ 2 ]
*			| \	                 |
*			|   \                    |
*		     9	|     \ 5                |  1
*			|       \                |
*			|         \              |
*		      [ 3 ]-------[ 4 ]--------[ 5 ]
*		              2           1
*
*---------------------------------------------------------------*/
#include 	<stdio.h>
#include 	<stdlib.h>
#include	<unistd.h>

#define		INF		999
#define		NODES		5
#define 	LINKCHANGES 	0 


/*--------------------------------------------------------------
*	distance_table definition: 
*
*	costs[i][j]= path cost from this node to node i 
*			when first hop is node j
*--------------------------------------------------------------*/
struct 	distance_table 
	{ 
		int costs[NODES][NODES]; 
	};


#ifdef  DVSIM_H
/*------------------------------
*	Trace level 
*------------------------------*/
int	TRACE = 1;

/*--------------------------------------
*	distance tables
*	minimum path costs 
*	link costs 
*--------------------------------------*/
struct 	distance_table	dt[NODES]; 
int 	min_cost[NODES][NODES];
int 	linkcost[NODES][NODES] = { 	{   0,   4,  INF,   12,   INF},
		     		   				{   4,   0,   7,  INF,   5},
				   					{ 	INF,   7,   0,  1,	10},
		     		   				{   12,  INF,	1,   0,  2},
									{   INF,  5,	10,   2,  0}};
#else 
extern  int 	TRACE;
extern  struct	distance_table dt[NODES];
extern  int 	min_cost[NODES][NODES];
extern  int 	linkcost[NODES][NODES];
#endif


/*--------------------------------------------------------------
*   	rtpkt   	Routing packet 
*
*	sourceid	source router
*	destid   	dest router (must be immediate neighbor)
*	mincost[]	minimum cost to other nodes 
*--------------------------------------------------------------*/
struct rtpkt {
        int sourceid;
        int destid;
        int mincost[NODES];
};

/*------------------------------
*	function prototypes
*------------------------------*/
void 	rtinit0();
void 	rtinit1();
void 	rtinit2();
void 	rtinit3();
void 	rtinit4();
void 	rtinit5();

void 	rtupdate0 (struct rtpkt *);
void 	rtupdate1 (struct rtpkt *);
void 	rtupdate2 (struct rtpkt *);
void 	rtupdate3 (struct rtpkt *);
void 	rtupdate4 (struct rtpkt *);
void 	rtupdate5 (struct rtpkt *);

void 	printdt_0 (struct distance_table *);
void 	printdt_1 (struct distance_table *);
void 	printdt_2 (struct distance_table *);
void 	printdt_3 (struct distance_table *);
void 	printdt_4 (struct distance_table *);
void 	printdt_5 (struct distance_table *);

void 	linkhandler0 (int, int);
void 	linkhandler1 (int, int);
void 	linkhandler2 (int, int);
void 	linkhandler3 (int, int);
void 	linkhandler4 (int, int);


double	get_clock();
void	tolayer2 (struct rtpkt);


/*	END  DVsim.h ------------------------------------------*/
