#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h>

#define PORT 8080

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1; 
    int addrlen = sizeof(address); 

	int valread = 0;
    char buffer[1024] = "";
    char* hello = "Hello i'm socket server\n";

    /* int socket(int domain, int type, int protocol);
	 * #include <sys/socket.h>
	 * USE: create an endpoint for communication and return file descriptor (fd)
	 * parameter:
	 *     domain   [0]: Local communication(AF_UNIX) , Internet protocols (AF_INET, AF_INET6)
	 *     type     [1]: how data transfer inside socket
	 * 	   protocol [2]: protocol for communication
	 * return: 
	 *     On success, a file descriptor for the new socket is returned.
	 *     On error, -1 is returned, and errno is set appropriately.
	 */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
    // Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 

    memset(&address, '0', sizeof(address)); 
    address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 

	/* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	 *
	 * When a socket is created with socket(2), it exists in a name space (address family)
	 * but has no address assigned to it.
	 * bind() assigns the address specified by `addr` to the socket referred to by the file descriptor `sockfd`.
	 * `addrlen` specifies the size, in bytes, of the address structure pointed to by `addr`.
	 * Traditionally, this operation is called “assigning a name to a socket”.
	 * return:
	 *     0 is success, -1 if error.
	 */
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) // Forcefully attaching socket to the port 8080 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
    // listen request connect from client to server
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}

    // accept connect 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 

    // read and write
    valread = read( new_socket , buffer, 1024); 
	printf("%s\n",buffer ); 
	
	send(new_socket , hello , strlen(hello) , 0 ); 
	printf("Hello message sent\n"); 

    return 0;
}