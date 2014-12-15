/*
onlineHospitalAppointmentSystem: healthcenterserver: a simple tcp server that receives 
tcp requests from patients

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
#define PORT "21849"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define BUFFER_SIZE 20
#define INCREMENTAL_SIZE 10
static char *userData;
static int size;
static int counter;
char str3[MAXDATASIZE];
int noOfApp=6;
struct loggedIn {
    char *user;
}
users[2];
struct availabilities {
    char index[2];
    char day[8];
    char time[10];
    char doc2[10];
    char portno[10];
    char flag;
}
avals[8];

char populateString(int fullForm) {
    int j;
    str3[0]='\0';
    str3[MAXDATASIZE]='\0';
    for(j=1;j<7;j++){
        if((avals[j].flag=='1')&&(fullForm==1)) {
            strcat(str3,avals[j].index);
            strcat(str3," ");
            strcat(str3,avals[j].day);
            strcat(str3," ");
            strcat(str3,avals[j].time);
            strcat(str3," ");
            strcat(str3,avals[j].doc2);
            strcat(str3," ");
            strcat(str3,avals[j].portno);
            strcat(str3,"\n");
        }//end of if
        else if((avals[j].flag=='1')&&(fullForm==0)) {
            strcat(str3,avals[j].index);
            strcat(str3," ");
            strcat(str3,avals[j].day);
            strcat(str3," ");
            strcat(str3,avals[j].time);
            strcat(str3,"\n");
        }
    }//end of for
}// end of function

int setElement(char setElement){
    int i,localFlag;
    localFlag=0;
    
    for(i=1;i<7;i++){
        if((avals[i].index[0]==setElement)&&(avals[i].flag=='1')){
            avals[i].flag='0';
            localFlag=i;
        }//end of if
    }//End of for
    return localFlag;
}

void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

/* get sockaddr, IPv4 or IPv6: */
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
        userData=realloc(userData, counter*sizeof(char));
    }
    *(userData+size)=ch;
    size++;
}

int main(void) {
    FILE *fp,*fp1;
    int sockfd, new_fd, numbytes,j;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    struct sockaddr_in cli_add;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int flag;
    struct availabilities tempStruct;
    int i;
    char *str1;
    char *temp;
    char str[300],str2[300],docInfo[20];
    char ch,*username,*token,*password,receivedUsername1[20];
    int localFlag;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    /* use my IP */
    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
    void *addr = &(ipv4->sin_addr);
    inet_ntop(servinfo->ai_family, addr, s, sizeof s);
    printf("Phase 1: The Health Center Server has port number %s and IP address %s.\n",PORT,s);
   /* loop through all the results and bind to the first we can */
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
        sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo);
    /* all done with this structure */
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    sa.sa_handler = sigchld_handler;
    /* reap all dead processes */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    fp=fopen("availabilities.txt" , "r");
    if(fp == NULL) {
        perror("Error opening file");
    }
    j=1;
    while(1)
    {
        if(fgets (str, 600, fp)==NULL){
            break;
            
        }
        token = strtok(str, " ");
        //walk through other tokens
        for (i=0; i<6;i++)
        {
            if (i==0){
                strcpy(avals[j].index,token);
            }
            else if(i==1){
                strcpy(avals[j].day,token);
            }
            else if(i==2){
                strcpy(avals[j].time,token);
            }
            else if(i==3){
                strcpy(avals[j].doc2,token);
            }
            else if(i==4){
                strcpy(avals[j].portno,token);
            }
            
            avals[j].flag='1';
            
            token = strtok(NULL, " ");
        }
        j++;
        
    } //end of current while
    
    // To get the usernames and passwords from text file
    
    userData = malloc(sizeof(char)*BUFFER_SIZE);
    size = 0;
    counter = BUFFER_SIZE;
        
    fp = fopen("users.txt","r");
    if(fp == NULL) {
        perror("Error opening file");
        return(-1);
    } else {
        while( ( ch = fgetc(fp) ) != EOF ) {
            pushData(ch);
        }
        fclose(fp);
    }
    while(1) {
        printf("server: waiting for connections...\n");
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&cli_add, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        printf("server: got connection from %s\n", s);
        if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        /* Received Password */
        char *authenticate,*receivedUsername, *receivedPassword,*context1;
        authenticate=strtok_r(buf," ",&context1);
        receivedUsername=strtok_r(NULL," ",&context1);
        receivedPassword=context1;
        users[0].user=malloc(sizeof(char)*strlen(receivedUsername));
        users[0].user=receivedUsername;
        char *pos;
        if ((pos=strchr(context1, '\n')) != NULL)
            *pos = '\0';
        strcpy(receivedUsername1,receivedUsername);
        printf("Phase 1:The Health Center Server has received request from a patient with username %s and password %s.		\n",receivedUsername,receivedPassword);
        char *ch1 = (char*) calloc(strlen(userData)+1, sizeof(char));
        strncpy(ch1,userData,strlen(userData));
        char *patient1,*patient2,*password1,*password2,*context;
        patient1=strtok_r(ch1," ",&context);
        password1=strtok_r(NULL,"\n",&context);
        patient2=strtok_r(NULL," ",&context);
        password2=context;
        
        /* Removal of \n */
        if ((pos=strchr(password2, '\n')) != NULL)
            *pos = '\0';
        
        /* Comparing the received Password with the server resources */
        if(strcmp(users[0].user,patient1)==0) {
            if(strcmp(receivedPassword,password1)==0) {
                users[0].user=malloc(sizeof(char)*strlen(receivedUsername));
                users[0].user=receivedUsername;
                flag=1;
            } else {
                flag=0;
            }
        } else if(strcmp(users[0].user,patient2)==0) {
            if(strcmp(receivedPassword,password2)==0) {
                users[1].user=malloc(sizeof(char)*strlen(receivedUsername));
                users[1].user=receivedUsername;
                flag=1;
            } else {
                flag=0;
            }
        }
        if(flag==1) {
            printf("Phase 1: The Health Center Server sends the response SUCCESS to patient with username %s.\n",receivedUsername1);
            if (send(new_fd, "SUCCESS", 7, 0) == -1)
                perror("send");
            /* Start of Phase 2 */
            
            if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            if(strcmp(buf,"available")==0){
                printf("Phase 2: The Health Center Server, receives a request for available time slots from patients with port number %d and IP address %s.\n",ntohs(cli_add.sin_port),s);
                    
                populateString(0);
                printf("Phase 2: The Health Center Server sends available time slots to patient with username %s\n",receivedUsername1);
                if (send(new_fd, str3, strlen(str3), 0) == -1)
                    perror("send");
                
                buf[numbytes]='\0';
                //Listening for tokens
                if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
                    perror("recv");
                    exit(1);
                }
                printf("Phase 2: The Health Center Server receives a request for appointment %c from patient with port number %d and username %s. \n", buf[10],ntohs(cli_add.sin_port),receivedUsername1);
                    localFlag=setElement(buf[10]);
                docInfo[0]='\0';
                strcat(docInfo,avals[localFlag].doc2);
                strcat(docInfo," ");
                strcat(docInfo,avals[localFlag].portno);
                if(localFlag==0){
                    printf("Phase 2:The Health Center Server rejects the following	appointment %s to patient with username %s\n",avals[localFlag].index,receivedUsername1);
                    if (send(new_fd, "not available",13, 0) == -1)
                        perror("send");
                    continue;	//exit(0);
                }
                else
                    printf("Phase 2:The Health Center Server confirms the following	appointment %s to patient with username %s\n",avals[localFlag].index,receivedUsername1);
                /* Sending Doctor and port information */
                if (send(new_fd, docInfo, strlen(docInfo), 0) == -1)
                    perror("send");
                
            }
            
            
        } else if(flag==0) {
            printf("Phase 1: The Health Center Server sends the response FAILURE to patient with username %s.\n",receivedUsername1);
            //close(sockfd);
            if (send(new_fd, "FAILURE", 7, 0) == -1)
                perror("send");
        }
      close(new_fd);
    }
    return 0;
}
