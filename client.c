/*
    Author: Lute Lillo Portero
    Date: 12/12/2021
    Client - Server Database
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "util.h"
#define PORT_NUM 20047


typedef struct student{
    char lname[10];
    char fname[10];
    char initial;
    unsigned long SID;
    float gpa;
}student_record;

void print_get(int init){
    int g;
    if(init){
        printf("| SID   | Lname     | Fname     | M | GPA  |\n");
    }
    printf("+");
    
    for(g = 0; g < 7; g++){
        printf("-");
    }
    printf("+");
    
    for(g = 0; g < 11; g++){
        printf("-");
    }
    printf("+");
    
    for(g = 0; g < 11; g++){
        printf("-");
    }
    printf("+");
    
    for(g = 0; g < 3; g++){
        printf("-");
    }
    printf("+");
    
    for(g = 0; g < 6; g++){
        printf("-");
    }
    printf("+\n");
}

/* Handle Control + C signal termination */
void sigint_handler(int sig){
    write(STDERR_FILENO, " -> Use 'close' to terminate client. Enter Command > ", 53);
}

int main(int argc, char *argv[]){
    
    int sockid;
    int port_num;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[2000];
    int printing = 0;
    size_t length;
    
    port_num = PORT_NUM;
    
    /* Create Socket */
    sockid = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockid < 0){
        error("Error: Creation socket");
    }
    
    /* Takes host domain name and resolves it to an IP address */
    server = gethostbyname("localhost");
    
    if(server == NULL){
        fprintf(stderr, "Error: No server found\n");
        exit(0);
    }
    
    /* Build server address structure */
    /*TODO: DISCUSSION SECTION TO CORRECT BUG*/
    memset((char*) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
    server_addr.sin_port = htons(port_num);
    
    /* Connect socket */
    if (connect(sockid, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        error("Error: connecting");
    }
    
    for (;;)
    {
        signal(SIGINT, sigint_handler);

        printf("Enter Command > ");

        /* Gets command from client */
        fgets(buffer, sizeof(buffer)-1, stdin);
        
        length = strlen(buffer);

        write_length(sockid, length);
        writen(sockid, buffer, length);
        
        if(!strncmp(buffer, "close", 5)){
            printf("Closing connection\n");
            close(sockid);
            break;
        }
        
        if(!strncmp(buffer, "get", 3)){
            printing = 1;
        }

        length = read_length(sockid);
        
        if (length >= sizeof(buffer)){
            error("Response message exceeds the buffer size");
        }
        
        /* Reads response back from server */
        readn(sockid, buffer, length);
        buffer[length] = '\0';
        if(printing){
            print_get(1);
        }
        
        printf("%s", buffer);
        
        if(printing){
            print_get(0);
            printing = 0;
        }
 
        
    }
    return 0;
}
