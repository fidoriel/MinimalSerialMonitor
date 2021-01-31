#include "serial_unix.h"

#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
 
int connect_port(char* comPort, int baud )
{
    int bbaud;
    switch ( baud )
    {
    case 300:
        bbaud = B300;
        break;
    
    case 1200:
        bbaud = B1200;
        break;
    
    case 2400:
        bbaud = B2400;
        break;
    
    case 4800:
        bbaud = B4800;
        break;

    case 9600:
        bbaud = B9600;
        break;

    case 19200:
        bbaud = B19200;
        break;

    case 38400:
        bbaud = B38400;
        break;

    case 57600:
        bbaud = B57600;
        break;

    case 115200:
        bbaud = B115200;
        break;
    
    default:
        bbaud = B9600;
        break;
    }

    int connection = open(comPort, O_RDWR | O_NDELAY);

    if (connection == -1)
    {
        perror(comPort);
        return -1;
    }

    flock(connection, LOCK_EX | LOCK_NB);

    int result, status;
    struct termios options;
    result = tcgetattr(connection, &options);
    if (result)
    {
        perror("tcgetattr failed");
        close(connection);
        return -1;
    }
    memset(&options, 0, sizeof(options));
    options.c_cflag = CS8 | 0 | 0 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;

    cfsetispeed(&options, bbaud);
    cfsetospeed(&options, bbaud);

    result = tcsetattr(connection, TCSANOW, &options);

	ioctl(connection, TIOCMGET, &status);
	status |= TIOCM_DTR;
  	status |= TIOCM_RTS; 
	ioctl(connection, TIOCMSET, &status);

    write(connection, "", 1);
  	return connection;
}

int read_port(int device, unsigned char *buffer, int size)
{
	int error = read(device, buffer, size);
	if(error < 0)
	{
		if(errno == EAGAIN)
            return -1;
	}

	return error;
}

int write_port(int device, const char *string)
{
  	while(*string != 0)
	{
		int error = write(device, (string++), 1);
		if (error < 0)
		{
			if(errno == EAGAIN)
				return 0;
			else
				return 1;
		}
	}
	return 1;
}

void close_port( int device )
{
    int status;

    ioctl( device, TIOCMGET, &status );

    status &= ~TIOCM_DTR;
    status &= ~TIOCM_RTS;

    ioctl( device, TIOCMSET, &status );

    close( device );

    flock(device, LOCK_UN);
}

void flush_port( int device )
{
    tcflush( device, TCIOFLUSH );
}

#endif