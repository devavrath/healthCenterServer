all: healthcenterserver doctor1 doctor2 patient1 patient2

healthcenterserver:healthcenterserver.c
	gcc -o healthcenterserver healthcenterserver.c -lsocket -lresolv -lnsl
	
doctor1:doctor1.c
	gcc -o doctor1 doctor1.c -lsocket -lresolv -lnsl
	
doctor2:doctor2.c
	gcc -o doctor2 doctor2.c -lsocket -lresolv -lnsl

patient1:patient1.c
	gcc -o patient1 patient1.c -lsocket -lresolv -lnsl

patient2:patient2.c
	gcc -o patient2 patient2.c -lsocket -lresolv -lnsl


