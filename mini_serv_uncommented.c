#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct s_client
{
    int id;
    char msg[696969];
}   t_client;

t_client clients[128];
int socket_fd = 0, connection_fd = 0, maximum_fd = 0, idNext = 0;
fd_set read_fds, write_fds, active_fds;
struct sockaddr_in server_address, client_address;
char read_buffer[696969], write_buffer[696969];

void error(char *msg)
{
	if (msg)
		write(2, msg, strlen(msg));
	else
    	write(2, "Fatal error\n", 13);
    exit(1);
}

void	send_all(int connection_fd)
{
	for (int fd_id = 0; fd_id <= maximum_fd; fd_id++)
	{
		if (FD_ISSET(fd_id, &write_fds) && fd_id != connection_fd)
			send(fd_id, write_buffer, strlen(write_buffer), 0);
	}
}

int main(int argc, char **argv)
{
	socklen_t len;
    if (argc != 2)
        error("Wrong number of arguments\n");
	socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
	if (socket_fd == -1)
		error(NULL);
    maximum_fd = socket_fd;
	bzero(&server_address, sizeof(server_address)); 
	bzero(&clients, sizeof(clients));
	bzero(&write_buffer, sizeof(write_buffer));
	bzero(&read_buffer, sizeof(read_buffer));
    int port = atoi(argv[1]);
	server_address.sin_family = AF_INET; 
	server_address.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	server_address.sin_port = htons(port); 
	if ((bind(socket_fd, (const struct sockaddr *)&server_address, sizeof(server_address))) != 0)
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
		for (int fd_id = 0; fd_id <= maximum_fd; fd_id++)
		{
			if (FD_ISSET(fd_id, &read_fds) && fd_id == socket_fd)
			{
				connection_fd = accept(socket_fd, (struct sockaddr *)&client_address, &len);
				if (connection_fd < 0)
					continue;
				maximum_fd = connection_fd > maximum_fd ? connection_fd : maximum_fd;
				sprintf(write_buffer, "server: client %d just arrived\n", idNext);
				send_all(connection_fd);
				bzero(&write_buffer, strlen(write_buffer));
				clients[connection_fd].id = idNext++;
				FD_SET(connection_fd, &active_fds);
			}
			else if (FD_ISSET(fd_id, &read_fds))
			{
				int readCount = recv(fd_id, &read_buffer, sizeof(read_buffer), 0);
				if (readCount == 0)
				{
					FD_CLR(fd_id, &active_fds);
					sprintf(write_buffer, "server: client %d just left\n", clients[fd_id].id);
					send_all(fd_id);
					bzero(&write_buffer, strlen(write_buffer));
					bzero(&clients[fd_id], sizeof(t_client));
					close(fd_id);
				}
				else
				{
					for (int i = 0, j = strlen(clients[fd_id].msg); read_buffer[i]; i++, j++)
					{
						clients[fd_id].msg[j] = read_buffer[i];
						if (read_buffer[i] == '\n')
						{
							clients[fd_id].msg[j] = '\0';
							sprintf(write_buffer, "client %d: %s\n", clients[fd_id].id, clients[fd_id].msg);
							send_all(fd_id);
							bzero(write_buffer, strlen(write_buffer));
							bzero(&clients[fd_id].msg, strlen(clients[fd_id].msg));
							j = -1;
						}
					}
					bzero(read_buffer, sizeof(read_buffer));
				}
			}
		}
	}
}
