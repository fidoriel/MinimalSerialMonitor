/*
    This file ( serial_unix.h ) is part of MinimalSerialMonitor.
    (C) 31.1.2021 Fidoriel

    MinimalSerialMonitor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MinimalSerialMonitor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MinimalSerialMonitor.  If not, see <http://www.gnu.org/licenses/>.
*/

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