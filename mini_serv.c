#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*
The struct contains a client id and a char array of the message.
*/
typedef struct s_client
{
    int id;
    char msg[696969];
}   t_client;

/*A couple of global variables, apparently there is a max of 128 clients.*/
t_client clients[128];
/*Integer values of file descriptors and possibly the id-s of next client.*/
int socket_fd = 0, connection_fd = 0, maximum_fd = 0, idNext = 0;
/*The fd_set variable type used in select.*/
fd_set read_fds, write_fds, active_fds;
/*Sockaddr in is used in the bind socket.*/
struct sockaddr_in servaddr, cli;
char readbuff[696969], writebuff[696969];

void error(char *msg)
{
	if (msg)
		write(2, msg, strlen(msg));
	else
    	write(2, "Fatal error\n", 13);
    exit(1);
}

void	sendAll(int connection_fd)
{
	for (int fdId = 0; fdId <= maximum_fd; fdId++)
	{
		if (FD_ISSET(fdId, &write_fds) && fdId != connection_fd)
			send(fdId, writebuff, strlen(writebuff), 0);
	}
}

int main(int argc, char **argv)
{
	socklen_t len;
    if (argc != 2)
        error("Wrong number of arguments\n");
	// socket create and verification 
	socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
	if (socket_fd == -1)
		error(NULL);
    maximum_fd = socket_fd;
	bzero(&servaddr, sizeof(servaddr)); 
	bzero(&clients, sizeof(clients));
	bzero(&writebuff, sizeof(writebuff));
	bzero(&readbuff, sizeof(readbuff));
	// assign IP, PORT
    int port = atoi(argv[1]);
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port); 
	// Binding newly created socket to given IP and verification 
	if ((bind(socket_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		error(NULL);
	if (listen(socket_fd, 128) != 0)
		error(NULL);
    FD_ZERO(&active_fds);
    FD_SET(socket_fd, &active_fds);
	
	while (6969)
	{
		read_fds = write_fds = active_fds;
		if (select(maximum_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
			continue;
		for (int fdId = 0; fdId <= maximum_fd; fdId++)
		{
			if (FD_ISSET(fdId, &read_fds) && fdId == socket_fd)
			{
				connection_fd = accept(socket_fd, (struct sockaddr *)&cli, &len);
				if (connection_fd < 0)
					continue;
				maximum_fd = connection_fd > maximum_fd ? connection_fd : maximum_fd;
				sprintf(writebuff, "server: client %d just arrived\n", idNext);
				sendAll(connection_fd);
				bzero(&writebuff, strlen(writebuff));
				clients[connection_fd].id = idNext++;
				FD_SET(connection_fd, &active_fds);
			}
			else if (FD_ISSET(fdId, &read_fds))
			{
				int readCount = recv(fdId, &readbuff, sizeof(readbuff), 0);
				if (readCount == 0)
				{
					FD_CLR(fdId, &active_fds);
					sprintf(writebuff, "server: client %d just left\n", clients[fdId].id);
					sendAll(fdId);
					bzero(&writebuff, strlen(writebuff));
					bzero(&clients[fdId], sizeof(t_client));
					close(fdId);
				}
				else
				{
					for (int i = 0, j = strlen(clients[fdId].msg); readbuff[i]; i++, j++)
					{
						clients[fdId].msg[j] = readbuff[i];
						if (readbuff[i] == '\n')
						{
							clients[fdId].msg[j] = '\0';
							sprintf(writebuff, "client %d: %s\n", clients[fdId].id, clients[fdId].msg);
							sendAll(fdId);
							bzero(writebuff, strlen(writebuff));
							bzero(&clients[fdId].msg, strlen(clients[fdId].msg));
							j = -1;
						}
					}
					bzero(readbuff, sizeof(readbuff));
				}
			}
		}
	}
}
