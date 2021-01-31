#include "serial_win.h"

#ifdef __WIN32__

int connect_port( char* comPort, int baud )
{
    char options[ 256 ];

    switch ( baud )
    {
    case 300:
        strcpy( options, "baud=300" );
        break;
    
    case 1200:
        strcpy( options, "baud=1200" );
        break;
    
    case 2400:
        strcpy( options, "baud=2400" );
        break;
    
    case 4800:
        strcpy( options, "baud=4800" );
        break;

    case 9600:
        strcpy( options, "baud=9600" );
        break;

    case 19200:
        strcpy( options, "baud=19200" );
        break;

    case 38400:
        strcpy( options, "baud=38400" );
        break;

    case 57600:
        strcpy( options, "baud=57600" );
        break;

    case 115200:
        strcpy( options, "baud=115200" );
        break;
    
    default:
        strcpy( options, "baud=9600" );
        break;
    }

    strcat( options, " data=8" );
    strcat( options, " parity=n" );
    strcat( options, " stop=1" );
    strcat( options, " xon=off to=off odsr=off dtr=on rts=on" );

    char port[12] = "\\\\.\\";
    strcat( port, comPort );

    HANDLE device = CreateFile( port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

    DCB port_settings;
    memset( &port_settings, 0, sizeof( port_settings ) );
    port_settings.DCBlength = sizeof( port_settings );

    BuildCommDCBA( options, &port_settings );

    SetCommState( device, &port_settings );

    COMMTIMEOUTS Cptimeouts;

    Cptimeouts.ReadIntervalTimeout = MAXDWORD;
    Cptimeouts.ReadTotalTimeoutConstant = 0;
    Cptimeouts.ReadTotalTimeoutMultiplier = 0;
    Cptimeouts.WriteTotalTimeoutConstant = 0;
    Cptimeouts.WriteTotalTimeoutMultiplier = 0;

    SetCommTimeouts( device, &Cptimeouts );

    write_port( ( int ) device, "" );

    return ( int ) device;
}


int read_port( int device, unsigned char *buffer, int size )
{
    DWORD error;

    ReadFile( ( HANDLE ) device, buffer, size, &error , NULL );

    return ( int ) error;
}

int write_port(int device, const char *string)
{
    int error;
    int bytesWritten;
    error = WriteFile( ( HANDLE ) device, string, strlen( string ), ( LPDWORD ) &bytesWritten, NULL );

    if ( error < 0 )
        return -1;

	return -1;
}

void close_port( int device )
{
    CloseHandle( ( HANDLE ) device );
}

void flush_port( int device )
{
    PurgeComm( ( HANDLE ) device, PURGE_RXCLEAR | PURGE_RXABORT );
    PurgeComm( ( HANDLE ) device, PURGE_TXCLEAR | PURGE_TXABORT );
}

#endif