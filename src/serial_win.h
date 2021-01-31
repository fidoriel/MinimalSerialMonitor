#ifndef serial_win_h
#define serial_win_h

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __WIN32__

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int connect_port( char* comPort, int baud );
int read_port(int device, unsigned char *buffer, int size);
int write_port(int device, const char *string);
void close_port( int device );
void flush_port( int device );

#ifdef __cplusplus
}
#endif

#endif

#endif