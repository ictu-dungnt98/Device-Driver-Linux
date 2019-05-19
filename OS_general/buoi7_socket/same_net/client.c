#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h>


#define PORT 		8080
/* get by phone ^^ */
#define SERVER_IP	"192.168.43.219"
int main()
{
    int client_fd, new_socket;
    struct sockaddr_in serv_addr; 
 
	char* hello = "Hello, i'm client\n";
    char buffer[1024] = "";
    int valread = 0;

    // init struct sockadd_in
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
    /* Convert IPv4 from text into a network address and restore to serv_addr.sin_addr */
	if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr)<=0)  
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

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
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	}
	/* int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	 * USE: connects the socket referred to by the file
     *      descriptor `sockfd` to the address specified by `addr`.  The `addrlen`
     *      argument specifies the size of `addr`.
	 * parameter:
	 *     sockfd  [0]: socket file descriptor, that is returned of socket function.
	 *     addr    [1]: server address that client connect to.
	 *     addrlen [2]: argument specifies the size of `addr`.
	 * return:
	 *     0 for success, -1 if error.
	 */
	if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
		return -1; 
	} 

    /*
	 * ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	 * USE: are used to transmit a message to another socket.
	 * parameter:
	 *     client_fd [0]: file descriptor that is returned by socket function
	 *     buf       [1]: message we wanna send
	 *     len		 [2]: strlen(buf)
	 *     flags		 [3]: The flags argument is the bitwise OR of zero or more flags
	 * return: 
	 * 	   On success, these calls return the number of bytes sent. If error, return -1.
	 */
    send(client_fd , hello , strlen(hello) , 0 ); 
	printf("Hello message sent\n"); 
	/* ssize_t read(int fd, void *buf, size_t count);
	 * read() attempts to read up to count bytes from file descriptor fd into the buffer starting at buf.
	 * parameter:
	 *     client_fd [0]: file descriptor that is return by socket function.
	 *     buf       [1]: buffer store message recieved from socket
	 *     count     [2]: the number bytes we wanno read.
	 */
	valread = read( client_fd , buffer, 1024); 
	printf("%s\n",buffer ); 

    return 0;
}
