// Serial port related header file
#include <errno.h>  /*Error number definition*/
#include <fcntl.h>  /*File Control Definition*/
#include <stdio.h>  /*Standard input and output definitions*/
#include <stdlib.h> /*Standard function library definition*/
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <termios.h> /*PPSIX Terminal Control Definition*/
#include <unistd.h>  /*Unix Standard Function Definitions*/

#define FALSE -1
#define TRUE 0

static void *thread1_handle(void *arg);
static void *thread2_handle(void *arg);

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

void UART0_Close(int fd)
{
    close(fd);
}

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

    /* tcgetattr(fd,&options): gets the relevant parameters of the object pointed to by fd and saves
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
    options.c_cc[VTIME] = 1; /* Read a character waiting 1*(1/10)s */

    options.c_cc[VMIN] = 1; /* The minimum number of characters read is 1 */

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
    return UART0_Set(fd, speed, flow_ctrl, databits, stopbits, parity);
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

    /* init fs_read */
    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);

    time.tv_sec = 10; /* wait 10s for read data */
    time.tv_usec = 0;

    /* Use select to achieve serial communication */
    fs_sel = select(fd + 1, &fs_read, NULL, NULL, &time);

    if (fs_sel) {
        len = read(fd, rcv_buf, data_len);
        printf("Data received with len = %d fs_sel = %d\n", len, fs_sel);
        return len;
    } else {
        printf("Sorry, It's timeout. Can not get any data!\n");
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

    pthread_t thread1, thread2;
    char rcv_buf[100];
    char send_buf[20] = "dungnt98-author";

    /* return the state of the calling function */
    int len;
    int i;

    if (argc != 3) {
        printf("Usage: %s /dev/ttySn\n", argv[0]);
        return FALSE;
    }

    /* Open /dev/tty* file for work with */
    fd = UART0_Open(fd, argv[1]);  // Open the serial port and return the file descriptor

    /* Setting uart attributes */
    do {
        err = UART0_Init(fd, 9600, 0, 8, 1, 'N');
        printf("Set Port Exactly!\n");
    } while (FALSE == err || FALSE == fd);

    /* implement handler */
    if (0 != pthread_create(&thread1, NULL, thread1_handle, &fd)) {
        perror("Can not create thread transmiter\n");
        exit(-1);
    }

    if (0 != pthread_create(&thread2, NULL, thread2_handle, &fd)) {
        perror("Can not create thread receiver\n");
        exit(-1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}

void clean_stdin(void)
{
    int c;

    printf("clean stdin ");

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

    printf("done\n");
}

static void *thread1_handle(void *arg)
{
    int cmd;
    char command[256];
    int fd = (*(int*)arg);

    while (1) {
        memset(command, 0, sizeof(command));
        cmd = 0;

        printf("Select command to send\n");
        scanf("%d", &cmd);

        clean_stdin();

        switch (cmd) {
            case 1: {
                sprintf(command, "%s", "Trong Dung HTN k15A");
            } break;

            case 2: {
                sprintf(command, "%s", "Do An Tot Nghiep 2021");
            } break;

            default:
                break;
        }

        UART0_Send(fd, command, strlen(command));
        fsync(fd);
    }

    return 0;
}

static void *thread2_handle(void *arg)
{
    char buff[512];
    int rx_data_len;
    int fd = (*(int*)arg);

    printf("thread2 run\n");

    while (1) {
        memset(buff, 0, sizeof(buff));
        rx_data_len = 0;

        rx_data_len = read(fd, buff, 512);

        if (rx_data_len > 0)
            printf("%s", buff);

        usleep(10000);
    }

    printf("thread2 terminate\n");

    return 0;
}

