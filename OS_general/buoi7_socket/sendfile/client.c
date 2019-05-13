#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h>


#define PORT 8080

int main()
{
    int client_fd, new_socket;
    struct sockaddr_in serv_addr; 

    // create socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
    // init struct sockadd_in
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

    // Convert IPv4 and IPv6 addresses from text to binary form ( AF_INET : IPv4)
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
		return -1; 
	} 

    char* hello = "Hello, i'm client\n";
    char buffer[1024] = "";
    int valread = 0;

    // read and write
    send(client_fd , hello , strlen(hello) , 0 ); 
	printf("Hello message sent\n"); 

	valread = read( client_fd , buffer, 1024); 
	printf("%s\n",buffer ); 

    return 0;
}