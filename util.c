/*
    Author: Lute Lillo Portero
    Date: 12/12/2021
    Client - Server Database
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "util.h"

size_t read_length(int fd)
{
    size_t buffer;

    readn(fd, &buffer, sizeof(buffer));
    return ntohl(buffer);
}

void write_length(int fd, size_t length)
{
    size_t buffer;

    buffer = htonl(length);
    writen(fd, &buffer, sizeof(buffer));
}

/* fd is just the socket descriptor, */
void readn(int fd, void *buffer, size_t count)
{
    int read_count = 0;

    while (read_count < count)
    {
        int retval = read(fd, (char *)buffer + read_count, count - read_count);
        
        if (retval == 0)
            error("Peer closes the connection");
        else if (retval < 0)
            error("Socket error");
        else
            read_count += retval;
    }
}

void writen(int fd, void *buffer, size_t count)
{
    int written_count = 0;

    while (written_count < count)
    {
        int retval = write(fd, (char *)buffer + written_count, count - written_count);
        
        if (retval < 0)
            error("Socket error");
        else
            written_count += retval;
    }
}

void error(char *msg)
{
	perror(msg);
	exit(1);
}

