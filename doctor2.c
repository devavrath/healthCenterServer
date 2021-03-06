/*
onlineHospitalAppointmentSystem: doctor2: a simple udp server named doctor2
receive udp messages

usage:  patient1,patient2

Devavrath Subramanyam
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define SERVICE_PORT 42849

struct insuranceDetails {
    char ins[11];
    char price[5];
}insDetails[4];
#define BUFSIZE 2048

int main(int argc, char **argv)
{
    struct sockaddr_in myaddr;	/* our address */
    struct sockaddr_in remaddr;	/* remote address */
    socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
    int recvlen,recvlen1;			/* # bytes received */
    int fd;				/* our socket */
    int msgcnt = 0;			/* count # of messages we received */
    unsigned char buf[BUFSIZE],buf1[BUFSIZE];	/* receive buffer */
    char str[300];
    char *token;
    int j,i,fd2;
    char *pos;
    char *server="127.0.0.1";
    int patientUDPPort;
    struct sockaddr_in sin;
    
    FILE *fp;
    fp=fopen("doc2.txt" , "r");
    
    /* Reading file Contents and putting in DS */
    
    
    if(fp == NULL) {
        perror("Error opening file");
    }// end of if
    j=0;
    while(1)
    {
        if(fgets (str, 600, fp)==NULL){
            break;
        }
        token = strtok(str, " ");
        for (i=0; i<2;i++)
        {
            if (i==0){
                strcpy(insDetails[j].ins,token);
            }
            else if(i==1){
                strcpy(insDetails[j].price,token);
            }
            token = strtok(NULL, " ");
        }
        j++;
    } //end of current while
    
    /* create a UDP socket */
    
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket\n");
        return 0;
    }
    
    /* bind the socket to any valid IP address and a specific port */
    
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(SERVICE_PORT);
    if (inet_aton(server, &myaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }
    printf("Phase 3: Doctor 2 has a static UDP port %d and IP address %s.\n", SERVICE_PORT,inet_ntoa(myaddr.sin_addr));
    /* now loop, receiving data and printing what we received */
    for (;;) {
        
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
        if (recvlen > 0) {
            buf[recvlen] = 0;
            
            socklen_t len3=sizeof(remaddr);
            
            printf("Phase 3: Doctor 2 receives the request from the patient with port number %d and the insurance plan %s.\n",ntohs(remaddr.sin_port),buf);
            
        }
        else
            printf("uh oh - something went wrong!\n");
        
        for(i=0;i<3;i++) {
            if(strcmp(buf,insDetails[i].ins)==0){
                sprintf(buf, "%s", insDetails[i].price);
            }// End of If
        }//End of For
        
        if ((pos=strchr(buf, '\n')) != NULL)
            *pos = '\0';
        printf("Phase 3: Doctor 2 has sent estimated price %s$ to patient with port number %d.\n",buf,ntohs(remaddr.sin_port));
        if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
            perror("sendto");
    }
    /* never exits */
}
