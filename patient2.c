/*
onlineHospitalAppointmentSystem: patient2: client that connects to healthcenterserver for tcp connections 
and doctor for udp connections

usage:  healthcenterserver,doctor1,doctor2

Devavrath Subramanyam
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define PORT "21849" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define BUFFER_SIZE 20
#define INCREMENTAL_SIZE 10
#define BUFLEN 100
static char *patientData;
static int size;
static int counter;
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
/* Dynamically push patient credentials */
void pushData(char ch) {
    if(size == counter) {
        counter+=INCREMENTAL_SIZE;
        patientData=realloc(patientData, counter*sizeof(char));
    }
    *(patientData+size)=ch;
    size++;
}

int main(int argc, char *argv[]) {
    FILE *fp;
    char ch;
    char ch1[70]="authenticate ";
    int flag2,i;
    int sockfd;
    char buf[MAXDATASIZE],docInfoPatient[MAXDATASIZE],buf1[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p, *res;
    int rv,numbytes;
    char s[INET6_ADDRSTRLEN],*userPassword;
    char availabilityIndex[15];
    char aval2[15]="selection ";
    char *docName,*docPortNo;
    struct sockaddr_in myaddr, remaddr;
    int fd, slen=sizeof(remaddr);
    char bufUDP[MAXDATASIZE];	/* message buffer */
    int recvlen;		/* # bytes in acknowledgement message */
    char *server = "127.0.0.1";	/* change this to use a different server */
    int docPortInt;
    char *pos;
    FILE *fp4;
    srand(time(NULL));
    
    socklen_t lenMyaddr,lenNew;
    struct sockaddr_storage addr1;
    char ipstr[INET6_ADDRSTRLEN];
    int port;
    
    patientData = malloc(sizeof(char)*BUFFER_SIZE);
    size = 0;
    counter = BUFFER_SIZE;
    fp = fopen("patient2.txt","r");
    if(fp == NULL) {
        perror("Error opening file");
        return(-1);
    } else {
        while( ( ch = fgetc(fp) ) != EOF ) {
            pushData(ch);
        }
        fclose(fp);
    }
    strcat(ch1,patientData);
    patientData=strtok(patientData," ");
    userPassword=strtok(NULL," ");
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    // use my IP
    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    //Sample to get my IP
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
    void *addr = &(ipv4->sin_addr);
    inet_ntop(servinfo->ai_family, addr, s, sizeof s);
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        // To get my Patient's port number
        if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
            perror("getsockname"); else
                printf("Phase 1: Patient 2 has TCP port number %d and IP address %s.\n", (int) ntohs(sin.sin_port),s);
            break;
        }
        if (p == NULL) {
            fprintf(stderr, "server: failed to bind\n");
            return 2;
        }
        if (send(sockfd, ch1, strlen(ch1), 0) == -1)
            perror("send");
        printf("Phase 1: Authentication request from Patient 2 with username %s and password %s has been sent to the Health Center Server.\n",patientData,userPassword);
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("Phase 1: Patient 2 authentication result: %s.\n",buf);
        printf("Phase 1: End of Phase 1 for Patient 2.\n");
            
        // Start of Phase 2
        
        if (send(sockfd, "available", 9, 0) == -1)
            perror("send");
        memset((char *)&buf, 0, 100);
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        
        //Received the availability
        printf("Phase 2: The following appointments are available for Patient 2:\n%s\nPlease Enter the preferred appointment index and press enter:\n",buf);
            fgets(availabilityIndex, sizeof(availabilityIndex), stdin);
        
        for(i=0;i<sizeof(buf);i++){
            if(buf[i]==availabilityIndex[0]) {
                if(buf[i+1]==' ') { flag2=1;break;}
            }
        }
        if(flag2!=1) {
            printf("Incorrect Index!! Please Enter index again:\n"); scanf("%s",availabilityIndex);
            for(i=0;i<sizeof(buf);i++){
                if(buf[i]==availabilityIndex[0]) {
                    if(buf[i+1]==' ') { flag2=1;break;}
                }
            }
        }
        if(flag2!=1){
            printf("Phase 2: The requested appointment from Patient 2 is not available. Exiting...\n");
            close(sockfd);
            freeaddrinfo(servinfo);
            exit(0);
        }//End of If
        strcat(aval2,availabilityIndex);
        
        /* Sending Token no */
        if (send(sockfd,aval2,sizeof(aval2), 0) == -1)
            perror("send");
        
        /* Listening for Doctor schedule and port no */
        docInfoPatient[0]='\0';
        memset((char *)&docPortNo, 0, sizeof(docPortNo));
        memset((char *)&docInfoPatient, 0, sizeof(docInfoPatient));
        if ((numbytes = recv(sockfd, docInfoPatient,MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        if((strcmp(docInfoPatient,"not available")==0)){
            printf("Phase 2: The requested appointment from Patient 2 is not available. Exiting...\n");
            close(sockfd);
            exit(0);
        }
        docName=strtok_r(docInfoPatient," ",&docPortNo);
        printf("Phase 2: The requested appointment is available and reserved to Patient 2. The assigned doctor port number is %s.\n", docPortNo);
        
        close(sockfd);
        /* Start of Phase 3 */
        
        /* UDP Socket Creation */
        
        /* create a socket */
        
        if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
            printf("socket created\n");
        
        /* bind it to all local addresses and pick any port number */
        
        memset((char *)&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        //	myaddr.sin_addr.s_addr = htonl("localhost");
        myaddr.sin_port = htons(0);
        docPortInt=atoi(docPortNo);
        
        if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
            perror("bind failed");
            return 0;
        }
        
        /* now define remaddr, the address to whom we want to send messages */
        /* For convenience, the host address is expressed as a numeric IP address */
        /* that we will convert to a binary format via inet_aton */
        
        lenNew = sizeof addr1;
        if(getsockname(fd, (struct sockaddr*)&addr1, &lenNew)==-1)perror("getsockname");
        
        // deal with both IPv4 and IPv6:
        if (addr1.ss_family == AF_INET) {
            struct sockaddr_in *s1 = (struct sockaddr_in *)&addr1;
            port = ntohs(s1->sin_port);
            inet_ntop(AF_INET, &s1->sin_addr, ipstr, sizeof ipstr);
        } else { // AF_INET6
            struct sockaddr_in6 *s1 = (struct sockaddr_in6 *)&addr1;
            port = ntohs(s1->sin6_port);
            inet_ntop(AF_INET6, &s1->sin6_addr, ipstr, sizeof ipstr);
        }
        
        printf("Phase 3: Patient 2 has a dynamic UDP port number %d and IP address %s.\n",port,s);
        
        fp4=fopen("patient2insurance.txt" , "r");
        if(fp4 == NULL) {
            perror("Error opening file");
        }// end of if
        if(fgets (bufUDP, 600, fp4)==NULL){
            printf("Error in reading Insurance File!!!\n");
        }
        if ((pos=strchr(bufUDP, '\n')) != NULL)
            *pos = '\0';
        
        memset((char *) &remaddr, 0, sizeof(remaddr));
        remaddr.sin_family = AF_INET;
        remaddr.sin_port = htons(docPortInt);
        if (inet_aton(server, &remaddr.sin_addr)==0) {
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }
                
        /* now let's send the messages */
        printf("Phase 3: The cost estimation request from Patient 2 with insurance plan %s has been sent to the doctor with port number %d and IP address %s.\n",bufUDP,docPortInt,server);
        sprintf(buf, "%s", bufUDP);
        if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
            perror("sendto");
            exit(1);
        }
        /* now receive an acknowledgement from the server */
        recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
        if (recvlen >= 0) {
            buf[recvlen] = 0;	/* expect a printable string - terminate it */
            printf("Phase 3: Patient 2 receives %s$ estimation cost from doctor with port number %d and name %s\n.\n",buf,docPortInt,docInfoPatient);
            printf("End of Phase 3\n");
        }
        
        close(fd);
        freeaddrinfo(servinfo);
        
        // free the linked list
        exit(0);
    }
