// Serial port related header file

#include <errno.h>  /*Error number definition*/
#include <fcntl.h>  /*File Control Definition*/
#include <stdio.h>  /*Standard input and output definitions*/
#include <stdlib.h> /*Standard function library definition*/
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h> /*PPSIX Terminal Control Definition*/
#include <unistd.h>  /*Unix Standard Function Definitions*/

// macro definition

#define FALSE -1
#define TRUE 0

/*******************************************************************
 * Name: UART0_Open
 * Function: Open the serial port and return to the serial device file description
 * Entry parameters: fd : file descriptor port : serial port number (ttyS0, ttyS1, ttyS2)
 * Export parameters: correctly returned to 1, error returned to 0
 *******************************************************************/

int UART0_Open(int fd, char *port)
{
    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

    if (FALSE == fd) {
        perror("Can't Open Serial Port");

        return (FALSE);
    }

    // Restore the serial port is blocked
    if (fcntl(fd, F_SETFL, 0) < 0) {
        printf("fcntl failed!\n");

        return (FALSE);
    } else {
        printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));
    }

    // Test whether it is a terminal device
    if (0 == isatty(STDIN_FILENO)) {
        printf("standard input is not a terminal device\n");
        return (FALSE);
    } else {
        printf("isatty success!\n");
    }

    printf("fd->open=%d\n", fd);

    return fd;
}

/*******************************************************************
 * Name: UART0_Close
 * Function: Close the serial port and return to the serial device file description
 * Entry parameters: fd : file descriptor port : serial port number (ttyS0, ttyS1, ttyS2)
 * Export parameters: void
 *******************************************************************/

void UART0_Close(int fd) { close(fd); }

/*******************************************************************
 * Name: UART0_Set
 * Function: Set serial data bit, stop bit and check digit
 * Entry parameters: fd serial file descriptor
 * speed serial port speed
 * flow_ctrl data flow control
 * databits data bit takes 7 or 8
 * stopbits stop bit value is 1 or 2
 * parity Validation type The value is N, E, O,, S
 *Export parameters: Return correctly to 1, error returns to 0
 *******************************************************************/

int UART0_Set(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)

{
    int i;
    int status;

    int speed_arr[] = {B115200, B19200, B9600, B4800, B2400, B1200, B300};
    int name_arr[] = {115200, 19200, 9600, 4800, 2400, 1200, 300};

    struct termios options;

    /*tcgetattr(fd,&options) gets the relevant parameters of the object pointed to by fd and saves
     * them in options. This function can also test whether the configuration is correct, whether
     * the serial port is available, etc. If the call succeeds, the function returns a value of 0.
     * If the call fails, the function returns a value of 1.
     */

    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return (FALSE);
    }

    // Set the serial port input baud rate and output baud rate

    for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
        if (speed == name_arr[i]) {
            cfsetispeed(&options, speed_arr[i]);

            cfsetospeed(&options, speed_arr[i]);
        }
    }

    // Modify the control mode to ensure that the program does not occupy the serial port
    options.c_cflag |= CLOCAL;

    // Modify the control mode, so that the input data can be read from the serial port
    options.c_cflag |= CREAD;

    // Set the data flow control
    switch (flow_ctrl) {
        case 0:  // no flow control
            options.c_cflag &= ~CRTSCTS;
            break;

        case 1:  // Use hardware flow control
            options.c_cflag |= CRTSCTS;
            break;

        case 2:  // use software flow control
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
    }

    // Set the data bit
    // Block other flags
    options.c_cflag &= ~CSIZE;
    switch (databits) {
        case 5:
            options.c_cflag |= CS5;
            break;

        case 6:
            options.c_cflag |= CS6;
            break;

        case 7:
            options.c_cflag |= CS7;
            break;

        case 8:
            options.c_cflag |= CS8;
            break;

        default:
            fprintf(stderr, "Unsupported data size\n");
            return (FALSE);
    }

    // Set the check digit
    switch (parity) {
        case 'n':
        case 'N':  // no parity.
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;

        case 'o':
        case 'O':  // set to odd parity
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;

        case 'e':
        case 'E':  // set to even parity
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;

        case 's':
        case 'S':  // set to space
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;

        default:
            fprintf(stderr, "Unsupported parity\n");
            return (FALSE);
    }

    // Set the stop bit
    switch (stopbits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;

        case 2:
            options.c_cflag |= CSTOPB;
            break;

        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return (FALSE);
    }

    // Modify the output mode, raw data output
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // options.c_lflag &= ~(ISIG | ICANON);

    // Set the waiting time and minimum receiving characters
    Options.c_cc[VTIME] = 1; /* Read a character waiting 1*(1/10)s */

    Options.c_cc[VMIN] = 1; /* The minimum number of characters read is 1 */

    /* If the data overflow occurs, receive the data,
    but no longer read Refresh the received data but do not read */
    tcflush(fd, TCIFLUSH);

    // Activate the configuration(set the modified termios data to the serial port)

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("com set error!\n");
        return (FALSE);
    }

    return (TRUE);
}

/*******************************************************************
 * Name: UART0_Init()
 * Function: Serial port initialization
 * Entry parameters: fd : file descriptor
 * speed : serial port speed
 * flow_ctrl data flow control
 * databits data bit takes 7 or 8
 * stopbits stop bit value is 1 or 2
 * parity Validation type The value is N, E, O,, S
 *
 * Export parameters: correctly returned to 1, error returned to 0
 *******************************************************************/

int UART0_Init(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)

{
    int err;

    // Set the serial data frame format
    if (UART0_Set(fd, 9600, 0, 8, 1, 'N') == FALSE) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/*******************************************************************
 * Name: UART0_Recv
 * Function: Receive serial port data
 * Entry parameters: fd: file descriptor
 * rcv_buf : Receive data from the serial port into the rcv_buf buffer
 * data_len : the length of one frame of data
 * Export parameters: correctly returned to 1, error returned to 0
 *******************************************************************/
int UART0_Recv(int fd, char *rcv_buf, int data_len)

{
    int len, fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);

    time.tv_sec = 10;
    time.tv_usec = 0;

    // Use select to achieve serial communication
    fs_sel = select(fd + 1, &fs_read, NULL, NULL, &time);

    printf("fs_sel = %d\n", fs_sel);

    if (fs_sel) {
        len = read(fd, rcv_buf, data_len);
        printf("I am right!(version1.2) len = %d fs_sel = %d\n", len, fs_sel);
        return len;
    } else {
        printf("Sorry,I am wrong!");

        return FALSE;
    }
}

/********************************************************************
 * Name: UART0_Send
 * Function: Send data
 * Entry parameters: fd: file descriptor
 * send_buf: store serial port to send data
 * data_len : the number of data in one frame
 * Export parameters: correctly returned to 1, error returned to 0
 *******************************************************************/

int UART0_Send(int fd, char *send_buf, int data_len)

{
    int len = 0;
    len = write(fd, send_buf, data_len);

    if (len == data_len) {
        printf("send data is %s\n", send_buf);
        return len;
    } else {
        tcflush(fd, TCOFLUSH);
        return FALSE;
    }
}

int main(int argc, char **argv)

{
    int fd;  // file descriptor
    int err;

    // return the state of the calling function
    int len;
    int i;

    char rcv_buf[100];

    // char send_buf[20]="tiger john";

    char send_buf[20] = "tiger john";

    if (argc != 3) {
        printf("Usage: %s /dev/ttySn tx(send data) / rx(receive data) \n", argv[0]);

        return FALSE;
    }

    fd = UART0_Open(fd, argv[1]);  // Open the serial port and return the file descriptor

    do {
        err = UART0_Init(fd, 9600, 0, 8, 1, 'N');

        printf("Set Port Exactly!\n");

    } while (FALSE == err || FALSE == fd);

    if (0 == strcmp(argv[2], "tx")) {
        for (i = 0; i < 10; i++) {
            len = UART0_Send(fd, send_buf, 10);

            if (len > 0)
                printf(" %d time send %d data successful\n", i, len);
            else
                printf("send data failed!\n");

            sleep(2);
        }

        UART0_Close(fd);
    }
    else { /* rx */
        While(1) {  // loop read data
            len = UART0_Recv(fd, rcv_buf, 99);

            if (len > 0) {
                rcv_buf[len] = '\0';
                printf("receive data is %s\n", rcv_buf);
                printf("len = %d\n", len);
            } else {
                printf("cannot receive data\n");
            }

            sleep(2);
        }

        UART0_Close(fd);
    }
}
