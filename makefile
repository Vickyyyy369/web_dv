#-------------------------------------------------------------------
#
#	Makefile for DV Routing simulation package
#
#-------------------------------------------------------------------
INC    = DVsim.h
OBJ    = DVsim.o node0.o node1.o node2.o node3.o node4.o
CC     = gcc
CFLAGS = -O

DVsim:	$(OBJ) $(INC)
	$(CC) $(CFLAGS) $(OBJ) -o DVsim 

DVsim.o: DVsim.c $(INC)
	$(CC) $(CFLAGS) -c DVsim.c

node0.o: node0.c $(INC)
	$(CC) $(CFLAGS) -c node0.c

node1.o: node1.c $(INC)
	$(CC) $(CFLAGS) -c node1.c

node2.o: node2.c $(INC)
	$(CC) $(CFLAGS) -c node2.c

node3.o: node3.c $(INC)
	$(CC) $(CFLAGS) -c node3.c

node4.o: node4.c $(INC)
	$(CC) $(CFLAGS) -c node4.c

# node5.o: node5.c $(INC)
# 	$(CC) $(CFLAGS) -c node5.c

clean:	
	rm *.o


