#ifndef serial_unix_h
#define serial_unix_h

#include <stdio.h>
#include <string.h>

#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)

#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

int connect_port( char* comPort, int baud );
int read_port(int device, unsigned char *buffer, int size);
int write_port(int device, const char *string);
void close_port( int device );
void flush_port( int device );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

#endif