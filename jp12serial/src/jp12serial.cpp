/* 
   
   This file is part of the JP1.x serial (RS-232) library used by various
   applications in the JP1 remote control user community to communicate with
   JP1.x (Flash) remote controls.  These applications include the 'IR.exe'
   utility, the 'Remote Master' IR utility (aka 'RMIR'), and several
   command-line utilities.

   Authors/Copyright/License:
 
      The JP1.x serial library was written by Greg Bush ('gfb107') and
      modified by several others, including 'binky123', Chris Nappi
      ('xnappo'), Bengt Martensson ('Barf'), 'osolfan', Bill Marr
      ('WagonMaster'), Kevin Timmerman and Graham Dixon ('mathdon').
      Mac OS X support by Michael Dreher ('MikeT').
 
      Copyright (C) 2006-2010 Greg Bush (et al)
      
      The JP1.x serial library is free software: you can redistribute it
      and/or modify it under the terms of the GNU General Public License as
      published by the Free Software Foundation, either version 3 of the
      License, or (at your option) any later version.
 
      The JP1.x serial library is distributed in the hope that it will be
      useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
      General Public License for more details.
 
      You should have received a copy of the GNU General Public License along
      with the JP1.x serial library.  If not, see
      <http://www.gnu.org/licenses/>.

   Files:

      jp12serial.cpp -- main library source code

      jp12serial.hpp -- main library header file

      jp12serial.dll -- Dynamic Link Library (DLL) for use by Windows 
                        applications

      jp12serial.so  -- shared object (library) file for use by 
                        Unix/Linux/Solaris/etc applications

   Additional files used only by Windows:

      ftdi.cpp       -- source code of FTDI interface functions

	  ftdi.h         -- header file of FTDI interface functions

	  ftd2xx.h       -- FTDI-provided file for interfacing to ftd2xx.dll

	  StdAfx.h       -- minimal header file of common settings

	  The files ftdi.cpp and ftdi.h are also used by IRScope.

   Building Instructions:

      This source code is intended to be compiled into both a Dynamic Link
      Library ('.dll') file for Windows users and a shared object library file
      ('.so') for Unix/Linux/Solaris/etc users.  

      Compile command:

         Windows + MinGW:  g++ -I "c:\Program Files\Java\jdk1.6.0_12\include" 
		     -I "c:\Program Files\Java\jdk1.6.0_12\include\win32" -shared
			 -o jp12serial.dll jp12serial.cpp ftdi.cpp -Wl,--kill-at

			 (amend the JDK Include parameters as appropriate)

         Linux + GCC:  gcc -shared -o libjp12serial.so jp12serial.cpp
      
   Miscellaneous Information:

      To download the latest version of this library and for other
      JP1{.x}-related information and files, please visit the forums at Rob
      Crowe's excellent JP1 site:
      
         http://www.hifi-remote.com/forums/

   Version History / Change Log:

      Note: Prior to version 0.14, no change log was kept.

	  Version 0.19 (released 21 Aug 2012)
	     Support for JP1.4 and JP2 remotes added by Dave Reed (3FG).  Includes a minor change
		 from the beta version issued 14 Dec 2011 as 0.18f_JP2 to handle an issue for these
		 remotes when using transistor-based interfaces.
 
      Version 0.18 (released 30 Oct 2010):
		 Fixed initialization of serial port, replaced "options.c_lflag |= IEXTEN"
         by "options.c_lflag &= ~IEXTEN;".
         Added #ifdefs for serial flags which are not available on OS X. Cleared flags
		 ONLCR and OXTABS. Courtesy of Michael Dreher ('MikeT')

         Cleaned up error handling for hSerial: always use INVALID_HANDLE_VALUE instead of NULL,
         courtesy of Michael Dreher ('MikeT')
 
		 Dynamically searches for plugged-in cables because there are no predefined or builtin
         serial / parallel devices and the name of the USB devices contain the serial number of
         the FTDI chip, courtesy of Michael Dreher ('MikeT').
 
	  Version 0.17 (released 3 Jul 2010):
         Amended getSignature() to return both JP1 and JP1.x styles of
		 signature.  This function is used by RMIR but not IR.exe, and
		 without this change the Kevin Timmerman JP1 EEPROM Programming
		 adapter (see note on Version 0.15 below) would not work with
		 RMIR.

      Version 0.16 (released 11 Feb 2010):
         Changes made by Kevin Timmerman and Graham Dixon to speed up
		 finding the remote.  On initial access during a session, priority
		 in the search order of COM ports is given to ports with FTDI chips
		 (excluding IR Widgets). The port located on initial start-up is
		 remembered and used in subsequent accesses.

      Version 0.15 (released 26 Jan 2010):
	     Changes made by Kevin Timmerman to allow use of the JP1 EEPROM Programming
		   adapter.  http://www.compendiumarcana.com/jp1epa

		 - Global variable "blockSize" size added. This is used when writing
		   data to the remote with the 'S' command (writeBlock() function).
		   It is set to 128 for flash remotes, or 32 for EEPROM remotes. The
		   type of remote is determined by the first char of the response to
		   the 'I' command (getIdentity() function). The EEPROM programming
		   adapter will respond with "eeprom", so 'e' will set the block
		   size to 32. 'M' or 'S' will set the block size to 128. The smaller
		   block size for EE is required due to the speed of EE and the limited
		   buffer in the EE adapter.

		 - The erasePages() function will not be called by writeRemote() if
		   the pageSize is 1 or less. EEPROM do not need page erasure and have
		   one byte write granularity. 

	     - Cleaned up formatting and structure in some places.

      Version 0.14 (released 13 Sep 2009):
  
         Except where noted otherwise, these changes were done by Bill Marr
         ('WagonMaster').
      
         - Fixed a bug whereby the routine used to delay ('sleep') in a
           non-Windows (Unix, Linux, Solaris, et al) environment was off by a
           factor of 1000, erroneously sleeping for the specified number of
           MICROseconds instead of the specified number of MILLIseconds.
           
         - Fixed a bug whereby the code intended to set and hold a UART
           'break' condition in a non-Windows (Unix, Linux, Solaris, et al)
           environment was erroneously using code to set the 'break'
           condition, sleep for a while (250 milliseconds in many
           implementations), then release the 'break' condition.
           
           This problem was substantially masked by the 'sleep' bug mentioned
           above, which was causing the process of putting a remote control
           into 'serial communication' mode to succeed where it would have
           otherwise failed with this bug in place.
           
           A corresponding fix was also needed to release the UART 'break'
           condition when commanded.
           
         - Properly preserve and restore the port settings of any opened
           RS-232 serial port.  Previously, the port's settings were
           overwritten, corrupting the settings for all ports (and therefore
           potentially all devices connected to them, like a dial-up modem)
           which may have been opened in the search for a valid JP1.x (Flash)
           remote control.
           
         - Fix a problem whereby error messages in a non-Windows (Unix, Linux,
           Solaris, et al) environment were erroneously showing up split
           across lines and out of sequence.
           
           The fix involves replacing 'perror()' with 'strerror()'.
           
         - Enhanced error reporting for serial port initialization.
           
         - Added support for Solaris environments, courtesy of Bengt
           Martensson ('Barf') and 'osolfan'.
           
         - Added a new routine to determine if the specified string parameter
           looks like a valid serial port name, for any of the supported
           platforms, courtesy of Bengt Martensson ('Barf').
           
           (At the moment, no current utilities use this routine, but future
           improvements to some of the command-line utilities will use it.)
           
         - In the routine to read a block of memory from the remote control,
           explicitly initialize that block to all zeroes, courtesy of Bengt
           Martensson ('Barf').
           
         - Remove code to define and initialize a couple of variables
           ('readAddress' and 'addressLimit') that were not used anywhere,
           courtesy of Bengt Martensson ('Barf')
           
         - Explicit variable casting was added in several spots to eliminate
           compile-time warnings, courtesy of Bengt Martensson ('Barf').
           
         - Explicitly specify 'void' parameter lists in routines.
           
         - Use '#ifndef WIN32' in lieu of '#ifdef __linux' so that building on
           Unix platforms other than Linux works as expected, courtesy of
           Bengt Martensson ('Barf').

         - Provide for conditional compilation in environments that don't have
           the Java SDK/JDK (and therefore the header files) installed.  (The
           default is to expect the Java headers to be present and therefore
           to build the Java routines.)
           
         - Move some definitions from the C source file to the header file,
           for better separation and in anticipation of future improvements.
           
         - Various cosmetic cleanups, including comment changes, additions,
           etc.

         - Allow use of Microsoft Visual C++ as well as MinGW when compiling,
           courtesy of Graham Dixon ('mathdon').

         - Replace uses of non-standard "or" operator (which doesn't compile
           under Microsoft Visual C++) with conventional "||" operator,
           courtesy of Graham Dixon ('mathdon').

*/

// 
// Define this whenever the Java SDK/JDK is available.  By undefining it (or 
// simply commenting it out), it is possible to build this library without the 
// Java header files, but if you do so it will NOT work with RMIR and the 
// compiled binary SHOULD NOT BE ISSUED FOR GENERAL USE. 
// 

#define JAVA_SDK_AVAILABLE

#ifdef WIN32
#define BUILD_DLL
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef WIN32
#include "stdafx.h"
// Uncomment next line if using MessageBox to debug
// #pragma comment(lib,"user32.lib");
#include "ftdi.h"
#else
#include <dirent.h>
#endif

#include "jp12serial.hpp"

#ifdef JAVA_SDK_AVAILABLE
#include "com_hifiremote_jp1_io_JP12Serial.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)	// Supress security warnings for unsafe string copy
#endif

#ifndef WIN32
#include <unistd.h>                   /* UNIX standard function definitions */
#include <fcntl.h>                    /* File control definitions           */
#include <errno.h>                    /* Error number definitions           */
#include <termios.h>                  /* POSIX terminal control definitions */
//#include <termio.h>                   /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <time.h>
// 
// Mimic versions of the Windows API routines for non-Windows platforms
// (e.g. Linux)...
// 

int ReadFile( int handle, unsigned char *buff, int len, int *bytesRead, int *overlapped )
{
  int totalbytes = 0;
  int bytes = read( handle, buff, len );
  while ( bytes > 0 )
  {
    totalbytes += bytes;
    bytes = read( handle, buff + totalbytes, len - totalbytes );
  }
  *bytesRead = totalbytes;
  if ( bytes < 0 )
    return 0;
  return 1;
}

int WriteFile( int handle, const unsigned char *buff, int len, int *bytesWritten, int *overlapped )
{
  *bytesWritten = write( handle, buff, len );
  if ( *bytesWritten < 0 )
    return 0;
  return 1;
}

int CloseHandle( int handle )
{
  if ( close( handle ) < 0 )
    return 0;
  return 1;
}

void Sleep( int millisec )
{
  usleep(1000 * millisec);
  return;
}

#endif

bool jp2_14GetInfoAndSig();

#define READBLOCKSIZE (0x80)
EXPORT HANDLE hSerial = INVALID_HANDLE_VALUE;
static char activePort[MAX_DEV_FNAME_LEN] = "";
static unsigned char cmdBuff[ 0x20 ];
static unsigned char jp2info[ 18 ];
static char signature[ 27 ];
static int eepromAddress = 0;
static int eepromSize = 0;
static int pageSize = 0;
static int blockSize = 0;

#ifdef WIN32
HINSTANCE m_hFTDILib = 0;

// 
// This variable is a Device Control Block (DCB) which holds the
// communications port state when it is first opened.  It is used to restore
// the state of any comm port which has been opened and had its settings
// altered.
// 
DCB saved_comm_state_dcb;

#else

// 
// This variable is a terminal I/O structure which holds the communications
// port state when it is first opened.  It is used to restore the state of any
// comm port which has been opened and had its settings altered.
// 
struct termios termios_saved;

#endif

#ifdef __APPLE__ //AKA Mac OS X
#define FTDI_SERIAL_DEVNAME "cu.usbserial"
//#define FTDI_SERIAL_DEVNAME "tty.usbserial"
#else
const char *portNames[] =
{
#ifdef WIN32
  "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
  "COM10", "COM11", "COM12", "COM13", "COM14", "COM15", "COM16", "COM17", "COM18", "COM19",
  "COM20", "COM21", "COM22", "COM23", "COM24", "COM25", "COM26", "COM27", "COM28", "COM29",
#endif
#ifdef __linux
  "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4",
  "/dev/ttyS5", "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8", "/dev/ttyS9",
  "/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB2", "/dev/ttyUSB3",
  "/dev/ttyUSB4", "/dev/ttyUSB5", "/dev/ttyUSB6", "/dev/ttyUSB7",
  "/dev/ttyUSB8", "/dev/ttyUSB9",
#endif
#ifdef __solaris
  "/dev/ttya", "/dev/ttyb", "/dev/cua/a", "/dev/cua/b",
#endif
  NULL
};

const unsigned NUMPORTS = sizeof(portNames) / sizeof(char *);

const char *orderedPortNames[NUMPORTS];
#endif

// Do not change the order of the Connection types as this enum value is exported
// in versions later than 0.18_JP2.  Add new values at the end.
enum ConnectType { NONE, JP2_14, JP13, JP12, JP11, JP12Orig } Connection;


void jp12PrintLastError( void )
{
#ifdef WIN32
  LPVOID lpMsgBuf;
  DWORD dw = GetLastError();

  FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                 FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 dw,
                 MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                 ( LPTSTR )&lpMsgBuf,
                 0, NULL );

  printf( "Error %d: %s\n", (int)dw, (char*)lpMsgBuf );

  LocalFree(lpMsgBuf);
#else
  printf("%s\n", strerror(errno));
#endif
}

int jp12EscapeComm( int func )
{
  if(hSerial == INVALID_HANDLE_VALUE)
    return 0;

#ifdef WIN32
  if ( !EscapeCommFunction( hSerial, func ))
#else
  int status;
  ioctl( hSerial, TIOCMGET, &status );
  switch ( func )
  {
    case CLRDTR:
      status &= ~TIOCM_DTR;
      break;
    case SETDTR:
      status |= TIOCM_DTR;
      break;
    case CLRRTS:
      status &= ~TIOCM_RTS;
      break;
    case SETRTS:
      status |= TIOCM_RTS;
      break;
    case CLRBREAK: 
      return (ioctl(hSerial, TIOCCBRK, 0) == 0);
    case SETBREAK: 
      return (ioctl(hSerial, TIOCSBRK, 0) == 0);
    default:
      printf( "Unknown function %i\n", func );
      return 0;
  }
  if ( ioctl( hSerial, TIOCMSET, &status ) != 0 )
#endif
  {
    printf( "EscapeCommFunction( %i ) failed: ", func );
    jp12PrintLastError();
    return 0;
  }
  return 1;
}

int jp12Init( char* portName )
{
	if(hSerial != INVALID_HANDLE_VALUE)
        	return 0;

#ifdef WIN32
	hSerial = CreateFile( portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0 );
	if ( hSerial == INVALID_HANDLE_VALUE )
	{
		return 0;
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof( dcbSerialParams );

	if ( !GetCommState(hSerial, &dcbSerialParams))
	{
		CloseHandle( hSerial );
	        hSerial = INVALID_HANDLE_VALUE;
		return 0;
	} else {
		memcpy(&saved_comm_state_dcb, &dcbSerialParams, sizeof dcbSerialParams);
	}

	dcbSerialParams.BaudRate = 38400;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = TWOSTOPBITS;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
	dcbSerialParams.fRtsControl = RTS_CONTROL_ENABLE;
	dcbSerialParams.fOutX = FALSE;
	dcbSerialParams.fInX = FALSE;

	if ( !SetCommState( hSerial, &dcbSerialParams ))
	{
		CloseHandle( hSerial );
	        hSerial = INVALID_HANDLE_VALUE;
		return 0;
	}

	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;

	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if ( !SetCommTimeouts( hSerial, &timeouts ))
	{
		//    
		// Restore the state for the comm port we're about to close...
		//    
		if (!SetCommState(hSerial, &saved_comm_state_dcb))
		{
			printf("Error restoring original state for comm port '%s': ", portName);
			jp12PrintLastError();
		}
		CloseHandle( hSerial );
	        hSerial = INVALID_HANDLE_VALUE;
		return 0;
	}

#else

  hSerial = open( portName, O_RDWR | /* O_NOCTTY | */ O_NDELAY );
  if ( hSerial == INVALID_HANDLE_VALUE )
  {
    return 0;
  }

  struct termios options;
  if (tcgetattr( hSerial, &options ) < 0) {   /* Get the current options for the port*/
     printf("Error ('%s') reading comm port '%s' attributes!\n", strerror(errno), portName);
     CloseHandle(hSerial);
     hSerial = INVALID_HANDLE_VALUE;
     return 0;
  }
  else memcpy(&termios_saved, &options, sizeof(options));   // preserve port settings

  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~PARODD;
  options.c_cflag &= ~HUPCL;
  options.c_cflag &= ~CSIZE;
  options.c_cflag &= ~CRTSCTS;
  options.c_cflag |= CREAD;
  options.c_cflag |= CLOCAL;
  options.c_cflag |= CS8;
  options.c_cflag |= CSTOPB;
  
  options.c_iflag &= ~BRKINT;
  options.c_iflag &= ~PARMRK;
  options.c_iflag &= ~INPCK;
  options.c_iflag &= ~ISTRIP;
  options.c_iflag &= ~INLCR;
  options.c_iflag &= ~IGNCR;
  options.c_iflag &= ~ICRNL; 
  options.c_iflag &= ~IXON;
  options.c_iflag &= ~IXOFF;
  options.c_iflag &= ~IXANY;
  options.c_iflag &= ~IMAXBEL;
  options.c_iflag &= ~IGNPAR;
#ifndef __solaris
  options.c_iflag &= ~IUTF8;
#endif
#ifdef IUCLC
  options.c_iflag &= ~IUCLC;
#endif
  options.c_iflag |= IGNBRK;

  options.c_oflag &= ~OPOST;
  options.c_oflag &= ~ONLCR;
#ifdef OXTABS
  options.c_oflag &= ~OXTABS;
#endif
#ifdef OLCUC
  options.c_oflag &= ~OLCUC;
#endif
  options.c_oflag &= ~OCRNL; 
  options.c_oflag &= ~ONOCR;
  options.c_oflag &= ~ONLRET;
  options.c_oflag &= ~OFILL;
  options.c_oflag &= ~OFDEL;
  options.c_oflag |= ONLCR;
  //options.c_oflag |= NL0;
  //options.c_oflag |= CR0;
  //options.c_oflag |= TAB0;
  //options.c_oflag |= BS0;
  //options.c_oflag |= VT0;
  //options.c_oflag |= FF0;
   
  options.c_lflag &= ~ISIG;
  options.c_lflag &= ~ICANON;
  options.c_lflag &= ~ECHO;
  options.c_lflag &= ~ECHONL;
#ifdef XCASE
  options.c_lflag &= ~XCASE;
#endif
  options.c_lflag &= ~TOSTOP;
  options.c_lflag &= ~ECHOPRT;
  options.c_lflag &= ~IEXTEN; // this is needed on a Mac
  options.c_lflag |= ECHOE;
  options.c_lflag |= ECHOK;
  options.c_lflag |= NOFLSH;
  options.c_lflag |= ECHOCTL;
  options.c_lflag |= ECHOKE;
  
	
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = 1;
  /*
   * Set the baud rates to 38400...
   */
  cfsetispeed( &options, B38400 );
  cfsetospeed( &options, B38400 );

  /*
   * Set the new options for the port...
   */

  tcsetattr( hSerial, TCSANOW, &options);

  fcntl( hSerial, F_SETFL, 0 );

//  jp12EscapeComm( SETDTR );
//  jp12EscapeComm( SETRTS );

#endif

  // calling resetRemote() here causes a quick LED blink
  // and then another reset into bootloader mode and
  // then another reset into remote mode followed by 2 LED blnks.
  // This is because fmIR calls ifStart/ReadOrWrite/ifStop for
  // each upload/download. ifStart calls openRemote() which in turn
  // calls jp12Init(). openRemote() then calls jp12Connect() which
  // does a reset into bootloader mode. ifStop calls closeRemote()
  // which resets remote and remote goes into remote mode followed
  // by 2 LED blinks.
  // resetRemote();                // reset remote and restart as a remote
  return 1;
}


// ******************************** Connection ********************************

// Reset JP1.2/JP1.3 remote and go into serial mode
// Works with JP1.2 design with no IDC-5 connection
// Works with JP1.2 remote and JP1.x design with IDC-5 connection
// Does NOT work with JP1.3 remote and JP1.x design with IDC-5 connection
//   because IDC-5 is High and puts JP1.3 remote into Tool mode
int jp12Connect( void )
{
	if ( !jp12EscapeComm( CLRRTS ))   // OFF/-10V/1 makes IDC-5 High
		return 0;
	if ( !jp12EscapeComm( CLRBREAK )) // OFF/-10V/1 makes IDC-4 High
		return 0;

	Sleep( 100 );
	if ( !jp12EscapeComm( CLRDTR ))   // OFF/-10V/1, discharge capacitor
		return 0;
	Sleep( 10 );                      // allow capacitor to discharge
	if ( !jp12EscapeComm( SETBREAK )) // ON/+10V/0 makes IDC-4 Low
		return 0;                       // JP1.3 remote tests for Low 56 cycles after Power On Reset
										// so need to set break ON early
	if ( !jp12EscapeComm( SETDTR ))   // ON/+10V/0, Reset pulse delay(10-20ms) from capacitor charge up
		return 0;                       // this generates the result pulse width

	Sleep( 100 );  // wait at least 20ms+50ms with IDC-4 Low for JP1.2 to go into BootLoader mode
	if ( !jp12EscapeComm( CLRBREAK )) // OFF/-10V/1 makes IDC-4 High
		return 0;

	Sleep( 10 );
		return 1;
}

int JP2_14StartOrStopComm( bool start )  {
	unsigned char buffer[20];
	static unsigned char initComm[4] ={0x00, 0x02, 0x51, 0x53};
	static unsigned char exitComm[4] ={0x00, 0x02, 0x52, 0x50};
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;
	DWORD bytesRead = 0;
	ReadFile( hSerial, buffer, 10, &bytesRead, NULL );  //try to clear out spurious bytes seen with transistor based serial interface 
														// the above line will cause a delay of 50+10*10 mS = 150mS
	DWORD bytesWritten = 0;
	if ( start ) {
		if ( !WriteFile( hSerial, initComm, 4, &bytesWritten, NULL ))
			return 0;
	} else {
		if ( !WriteFile( hSerial, exitComm, 4, &bytesWritten, NULL ))
			return 0;
	}
	if (!start) return 1;  //Probably it won't respond since we've returned it to remote mode! 
	bytesRead = 0;
	if (!ReadFile(hSerial, cmdBuff, 4, &bytesRead, NULL) || (bytesRead != 4) || (cmdBuff[2] != 0))
		return 0;
	return 1;
}

//JP 1.4 and JP2 remotes simply require a 35 mS Low pulse of RTS (reset -- IDC-2)
// to go into host communication mode.  It will return to ordinary remote
// mode when JP2_14StartOrStopComm( false) is executed. 
int jp2_14Connect( void ) {
	if ( !jp12EscapeComm( SETRTS ))   // ON/+10V/0 makes IDC-2 Low
		return 0;
	Sleep( 35 );
	if ( !jp12EscapeComm( CLRRTS )) // OFF/-10V/1 makes IDC-2 High
		return 0;
	// Sleep( 35 );  subsumed into jp2_14GetInfoAndSig.ReadFile
	return jp2_14GetInfoAndSig();
}

// Reset JP1.3 remote and go into serial mode
// Works with JP1.x design with IDC-5 connection
// Works with JP1.2 design with no IDC-5 connection
// Does NOT work for JP1.2 remote with JP1.x design with IDC-5
//   connection because IDC-5 is Low and puts JP1.2 remote into BDM mode
int jp13Connect( void )
{
	if ( !jp12EscapeComm( SETRTS ))   // ON/+10V/0 makes IDC-5 Low
		return 0;
	if ( !jp12EscapeComm( CLRBREAK )) // OFF/-10V/1 makes IDC-4 High
		return 0;

	Sleep( 100 );
	if ( !jp12EscapeComm( CLRDTR ))   // OFF/-10V/1, discharge capacitor
		return 0;
	Sleep( 10 );                      // allow capacitor to discharge
	if ( !jp12EscapeComm( SETBREAK )) // ON/+10V/0 makes IDC-4 Low
		return 0;                       // JP1.3 remote tests for Low 56 cycles after Power On Reset
										// so need to set break ON early

	if ( !jp12EscapeComm( SETDTR ))   // ON/+10V/0, Reset pulse delay(10-20ms) from capacitor charge up
		return 0;                       // this generates the result pulse width

	Sleep( 100 );  // wait at least 50ms with IDC-4 Low for JP1.3 to go into BootLoader mode
	if ( !jp12EscapeComm( CLRBREAK )) // OFF/-10V/1 makes IDC-4 High
		return 0;

	Sleep( 10 );

	return 1;
}

// Reset JP1.1 remote and go into serial mode
int jp11Connect( void )
{
	if ( !jp12EscapeComm( CLRDTR ))   // OFF/-10V/1, discharge capacitor
		return 0;
	Sleep( 10 );                      // allow capacitor to discharge
	if ( !jp12EscapeComm( SETRTS ))   // ON/+10V/0 makes IDC-5 /START Low
		return 0;
	if ( !jp12EscapeComm( SETDTR ))   // ON/+10V/0, Reset pulse delay(10-20ms) from capacitor charge up
		return 0;                       // this generates the result pulse width
	Sleep( 25 );                      // wait at least 20ms to allow for max reset pulse delay
	if ( !jp12EscapeComm( CLRRTS ))   // OFF/-10V/1 makes IDC-5 /START High
		return 0;
	Sleep( 10 );
	return 1;
}

// Reset JP1.2/JP1.3 remote and go into serial mode(Using Original circuit design)
// JP1.3 remote tests for Low 56 cycles after Power On Reset so need to set break ON early
// JP1.2 remote tests for Low 50ms after Power On Reset
int jp12OriginalConnect( void )
{
	if ( !jp12EscapeComm( SETBREAK )) // ON/+10V/0 makes IDC-4 Low
		return 0;
	if ( !jp12EscapeComm( SETRTS ))   // ON/+10V/0 makes IDC-2 /RESET Low
		return 0;
	Sleep( 10 );
	if ( !jp12EscapeComm( CLRRTS ))   // OFF/-10V/1 makes IDC-2 /RESET High
		return 0;
	Sleep( 100 ); // wait at least 50ms with IDC-4 Low for JP1.2 to go into BootLoader mode
	if ( !jp12EscapeComm( CLRBREAK )) // OFF/-10V/1 makes IDC-4 High
		return 0;
	Sleep( 10 );
	return 1;
}


// ************************************ Flash Protocol *********************************

int jp12Test( void )
{
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;

	DWORD bytesRead = 0;
	if ( !ReadFile( hSerial, cmdBuff, 20, &bytesRead, NULL ))
		return 0;

	cmdBuff[ 0 ] = 'E';
	DWORD bytesWritten = 0;
	if ( !WriteFile( hSerial, cmdBuff, 1, &bytesWritten, NULL ))
		return 0;

	bytesRead = 0;
	if ( !ReadFile( hSerial, cmdBuff, 1, &bytesRead, NULL ))
		return 0;

	if ( bytesRead == 0 )
		return 0;

	if ( cmdBuff[ 0 ] != 6 )
		return 0;

	return 1;
}

unsigned char jp12ComputeCheckSum( const unsigned char *data, int length )
{
	unsigned char sum = 0;
	while(length--) sum ^= *data++;
	return sum;
}

int jp12GetIdentity( unsigned char *buffer )
{

	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;

	cmdBuff[ 0 ] = 'I';
	DWORD bytesWritten = 0;
	if ( ! WriteFile( hSerial, cmdBuff, 1, &bytesWritten, NULL ))
		return 0;

	DWORD bytesRead = 0;
	if ( !ReadFile( hSerial, buffer, 20, &bytesRead, NULL ))
		return 0;

	int len = bytesRead - 1;
	if ( buffer[ len ] != jp12ComputeCheckSum( buffer, len ))
		return 0;

	return len;
}

int jp12GetVersion( unsigned char *buffer )
{
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;

	cmdBuff[ 0 ] = 'V';
	DWORD bytesWritten = 0;
	if ( ! WriteFile( hSerial, cmdBuff, 1, &bytesWritten, NULL ))
	{
		printf( "Error sending 'V'ersion command: " );
		jp12PrintLastError();
		return 0;
	}

	DWORD bytesRead = 0;
	if ( !ReadFile( hSerial, buffer, 20, &bytesRead, NULL ))
	{
		printf( "Error reading 'V'ersion results: " );
		jp12PrintLastError();
		return 0;
	}

	int len = bytesRead - 1;
	if ( buffer[ len ] != jp12ComputeCheckSum( buffer, len ))
	{
		printf( "Version checksum mismatch!\n" );
		return 0;
	}
	return len;
}

bool jp2_14GetInfoAndSig()
{
	unsigned char buffer[48];
	unsigned char getInfo[4] ={0x00, 0x02, 0x50, 0x52};
	unsigned char getSig[8] ={0x00, 0x06, 0x01, 0x00, 0x00, 0x00, 0x2C, 0x00};

	if (!JP2_14StartOrStopComm( true )) 
		return false;
	DWORD bytesWritten = 0;
	if ( ! WriteFile( hSerial, getInfo, 4, &bytesWritten, NULL )) 
		return false;
	DWORD bytesRead = 0;
	if ( !ReadFile( hSerial, buffer, 8, &bytesRead, NULL ) || (buffer[ 2 ] != 0) )
		return false;
	getSig[3] = buffer[5];
	getSig[4] = buffer[6];
	getSig[7] = jp12ComputeCheckSum( getSig, 7 );
	if ( ! WriteFile( hSerial, getSig, 8, &bytesWritten, NULL )) 
		return false;
	bytesRead = 0;
	if ( !ReadFile( hSerial, buffer, 48, &bytesRead, NULL ) || (buffer[ 2 ] != 0) )
		return false;
	if ( bytesRead != 48 )
		return false;
	for ( int i = 0; i < 26; i++ ) {
		signature[ i ] = buffer[ i + 3 ] & 0x7F;
	}
	for ( int i = 0; i < 18; i++ ) {
		jp2info[ i ] = buffer[ i + 29 ];
	}
	signature[ 26 ] = '\0';
	eepromAddress = ( (int) buffer[37] << 8) + (int) buffer[38];
	eepromSize = ( (int) buffer[39] << 8) + (int) buffer[40] - eepromAddress + 1;
	if (signature[ 0 ] == 0x33) {  //the digit "3"
		pageSize  = 0x080;	//S3F80 can erase a sector of 128 bytes	
	} else {
		pageSize  = 0x0200; // 512  MAXQ610 erases a minimum of 512 bytes, but can write 1 word
	}
	blockSize = 0x0080; // 128 
	return true;
}

int readJP2_14Block( int address, int blockLength, unsigned char *buffer )
{
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;

		cmdBuff[ 0 ] = 0;						
		cmdBuff[ 1 ] = 6;						
		cmdBuff[ 2 ] = 0x01;  //Read command
		cmdBuff[ 3 ] = address >> 8;
		cmdBuff[ 4 ] = address & 0x00FF;
		cmdBuff[ 5 ] = blockLength >> 8;
		cmdBuff[ 6 ] = blockLength & 0x00FF;
		cmdBuff[ 7 ] = jp12ComputeCheckSum( cmdBuff, 7 );
	
	DWORD bytesWritten = 0;  // send command
	if ( !WriteFile( hSerial, cmdBuff, 8, &bytesWritten, NULL ) || bytesWritten != 8)
		return 0;
	
	DWORD bytesRead = 0; // get acknowldegement
	if ( !ReadFile( hSerial, cmdBuff, 3, &bytesRead, NULL) || (bytesRead != 3) || (cmdBuff[2] != 0)) {
		ReadFile( hSerial, cmdBuff, 1, &bytesRead, NULL); //read out the checksum
		return 0;
	}
	unsigned char ackChkSum = jp12ComputeCheckSum(cmdBuff,3); 
	bytesRead = 0;  //get data
	if ( !ReadFile( hSerial, buffer, blockLength, &bytesRead, NULL) || (bytesRead != (DWORD)blockLength))
		return 0;

	ackChkSum ^= jp12ComputeCheckSum( buffer, blockLength);
	bytesRead = 0;	// get checksum
	unsigned char tempByte;
	if ( !ReadFile( hSerial, &tempByte, 1, &bytesRead, NULL) || (bytesRead != 1) || (tempByte != ackChkSum))
		return 0;

	return bytesRead;
}

int readBlock( int address, int blockLength, unsigned char *buffer )
{
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;

	// fill command
		cmdBuff[ 0 ] = 'R';
		cmdBuff[ 1 ] = address >> 8;
		cmdBuff[ 2 ] = address & 0xFF;
		cmdBuff[ 3 ] = blockLength;
		cmdBuff[ 4 ] = jp12ComputeCheckSum( cmdBuff, 4 );
	for (int i=0; i<blockLength; i++)
		buffer[i] = 0;
	
	// send command
	DWORD bytesWritten = 0;
	if ( !WriteFile( hSerial, cmdBuff, 5, &bytesWritten, NULL ) || bytesWritten != 5)
		return 0;
	
	// get result
	DWORD bytesRead = 0;
	if ( !ReadFile( hSerial, cmdBuff, 1, &bytesRead, NULL ) || bytesRead != 1)
		return 0;
	if ( cmdBuff[ 0 ] != 'r' )
		return 0;
	bytesRead = 0;
	if ( !ReadFile( hSerial, buffer, blockLength, &bytesRead, NULL) || (bytesRead != (DWORD)blockLength))
		return 0;

	DWORD resultLength = 0;
	if ( !ReadFile( hSerial, cmdBuff, 1, &resultLength, NULL ) || (resultLength != 1))
		return 0;
	if ( cmdBuff[ 0 ] != ( jp12ComputeCheckSum( buffer, blockLength ) ^ 'r' ))
		return 0;
	return bytesRead;
} 
 
int erasePages( int address )
{
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;
	int cmdLen, rdLen;
	if ( Connection == JP2_14 ) {
		cmdLen = 8;
		rdLen = 4;
		cmdBuff[ 0 ] = 0;
		cmdBuff[ 1 ] = 6;
		cmdBuff[ 2 ] = 3;  // erase command
		cmdBuff[ 3 ] = address >> 8;
		cmdBuff[ 4 ] = address & 0x00FF;
		cmdBuff[ 5 ] = (address + pageSize) >> 8;
		cmdBuff[ 6 ] = (address + pageSize) & 0x00FF;
		cmdBuff[ 7 ] = jp12ComputeCheckSum( cmdBuff, 7 );
	} else  {
		int count = 1;
		cmdLen = 6;
		rdLen = 1;
		cmdBuff[ 0 ] = 'C';
		cmdBuff[ 1 ] = address >> 8;
		cmdBuff[ 2 ] = address & 0x00FF;
		cmdBuff[ 3 ] = count >> 8;
		cmdBuff[ 4 ] = count & 0x00FF;
		cmdBuff[ 5 ] = jp12ComputeCheckSum( cmdBuff, 5 );
	}
	DWORD bytesWritten = 0;
	if ( !WriteFile( hSerial, cmdBuff, cmdLen, &bytesWritten, NULL ))
		return 0;

	DWORD bytesRead = 0;
	if ( !ReadFile( hSerial, cmdBuff, rdLen, &bytesRead, NULL ))
		return 0;
	if ( Connection == JP2_14 ) {
		if ( cmdBuff[ 2] != 0 )
		return 0;
	} else {
		if ( cmdBuff[ 0 ] != 6 )
		return 0;
	}
	return 1;
}

int writeBlock( int addr, const unsigned char *data, int length )
{
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;
	int cmdLen, rdLen;
	/// fill in the command header
	if ( Connection == JP2_14 ) {
		cmdBuff[ 0 ] = (length + 4) >> 8;
		cmdBuff[ 1 ] = (length + 4) & 0x00FF;
		cmdBuff[ 2 ] = 0x02;  //Write command
		cmdBuff[ 3 ] = addr >> 8;
		cmdBuff[ 4 ] = addr & 0x00FF;
		cmdLen = 5;
		rdLen = 4;
	}
	else {
		cmdBuff[ 0 ] = 'S';
		cmdBuff[ 1 ] = addr >> 8;
		cmdBuff[ 2 ] = addr & 0x00FF;
		cmdBuff[ 3 ] = length;
		cmdLen = 4;
		rdLen = 1;
	}
							   // compute the checksum
	unsigned char cs = jp12ComputeCheckSum( cmdBuff, cmdLen ) ^ jp12ComputeCheckSum( data, length );
								  
	DWORD bytesWritten = 0;        // send the command header
	if ( !WriteFile( hSerial, cmdBuff, cmdLen, &bytesWritten, NULL ))
		return 0;
							   
	bytesWritten = 0;				// send the data
	if ( !WriteFile( hSerial, data, length, &bytesWritten, NULL ))
		return 0;
							   
	bytesWritten = 0;				// send the checksum
	if ( !WriteFile( hSerial, &cs, 1, &bytesWritten, NULL ))
		return 0;
						   
	DWORD bytesRead = 0;			// get the result
	if ( !ReadFile( hSerial, cmdBuff, rdLen, &bytesRead, NULL ))
		return 0;
	if ( Connection == JP2_14 ) {
		if ( cmdBuff[2] != 0 ) 
			return 0;
	}
	else if ( cmdBuff[ 0 ] != 0x06 )
			return 0;
	return 1;
}

#ifdef WIN32
void reorderPorts(int port, bool tail)
{
	// Moves specified port to head of search list if tail == false, to tail if tail == true

	// Pointer to the specifed index in portNames[]
	const char *const p = portNames[port-1];

	// Find the orderedPortNames[] array index of the specified port
	int i;
	for (i=0; i < NUMPORTS && orderedPortNames[i] != p; i++);
	// About if port not found
	if (i == NUMPORTS) return;

	OutputDebugString("Moving "); OutputDebugString(p); OutputDebugString(tail ? " to the tail\n" : " to the head\n");

	if (tail)
	{
		// Move down folowing ports, don't touch the NULL at the end of list!
		for (; i < NUMPORTS - 2; ++i) orderedPortNames[i] = orderedPortNames[i+1];
		// Put the specified port at the tail of the array
		orderedPortNames[i] = p;
	} else {
		// Move up preceding ports
		for (; i > 0; --i) orderedPortNames[i] = orderedPortNames[i-1];
		// Put the specified port at the head of the array
		orderedPortNames[i] = p;
	}

}
#endif

// ****************************** Exported high level functions ********************************

static int isPortAvailable(const char *portName )
{
  if(hSerial != INVALID_HANDLE_VALUE)
    return 0;

#ifdef WIN32
  char testName[ 80 ];
  strcpy( testName , "\\\\.\\\\" );
  strcat( testName, portName );
  hSerial = CreateFile( testName,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        0 );
  if ( hSerial == INVALID_HANDLE_VALUE )
  {
    return 0;
  }
#else
  hSerial = open( portName, O_RDWR | /* O_NOCTTY | */ O_NDELAY );
  if ( hSerial == INVALID_HANDLE_VALUE )
  {
    return 0;
  }
#endif

  CloseHandle( hSerial );
  hSerial = INVALID_HANDLE_VALUE;
  return 1;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jobjectArray JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getPortNames( JNIEnv *env, jobject jobj )
{
  int count = 0;

#ifdef __APPLE__ //AKA Mac OS X
  // dynamically collect port names like "/dev/cu.usbserial-@6008i8L" "/dev/tty.usbserial-@6008i8L"
  size_t compareLen = strlen(FTDI_SERIAL_DEVNAME);
  DIR* dirp = opendir("/dev");
  dirent* dp;
  while ((dp = readdir(dirp)) != NULL)
    if (!strncmp(dp->d_name, FTDI_SERIAL_DEVNAME, compareLen)) {
      // it is not necessary to check if the port is available because the directory is
      // dynamically created, when the device is available
      ++count;
    }
  (void)closedir(dirp);
  dirp = NULL;
#else
  int bit = 1;
  int portFlags = 0;
  for ( int i = 0; portNames[ i ] != NULL ; ++i )
  {
    if ( isPortAvailable( portNames[ i ]))
    {
      ++count;
      portFlags |= bit;
    }
    bit <<= 1;
  }
#endif

  jobjectArray result;
  jclass strArrCls = env->FindClass( "Ljava/lang/String;" );
  if ( strArrCls == NULL )
    return NULL;                                        /* exception thrown*/

  result = env->NewObjectArray( count, strArrCls, NULL);
  if ( result == NULL )
    return NULL;                              /* out of memory error thrown*/

  int index = 0;

#ifdef __APPLE__ //AKA Mac OS X
  dirp = opendir("/dev");
  while ((index < count) && (dp = readdir(dirp)) != NULL)
    if (!strncmp(dp->d_name, FTDI_SERIAL_DEVNAME, compareLen)) {
      char path[MAX_DEV_FNAME_LEN] = "/dev/";
      strcat(path, dp->d_name);
      jstring name = env->NewStringUTF(path);
      if ( name == NULL )
        return NULL;                          /* out of memory error thrown*/

      env->SetObjectArrayElement( result, index++, name );
    }
  (void)closedir(dirp);
  dirp = NULL;
#else
  bit = 1;
  for ( int i = 0; portNames[ i ] != NULL; i++)
  {
    if (( portFlags & bit ) != 0 )
    {
      jstring name = env->NewStringUTF( portNames[ i ]);
      if ( name == NULL )
        return NULL;                          /* out of memory error thrown*/

      env->SetObjectArrayElement( result, index++, name );
    }
    bit <<= 1;
  }
#endif

  return result;
}
#endif


EXPORT bool serialPortLike(const char *s)
{
#ifdef WIN32
  return strncasecmp(s, "COM", 3) == 0;
#else
  #ifdef __APPLE__ //AKA Mac OS X
    // the port names are like "cu.usbserial-@6008i8L" or "tty.usbserial-@6008i8L"
    return strncmp(s, "/dev/tty", 8) == 0 || strncmp(s, "/dev/cu", 7) == 0;
  #else
    return strncmp(s, "/dev/tty", 8) == 0 || strncmp(s, "/dev/cua", 8) == 0;
  #endif
#endif
}


EXPORT const char *getInterfaceName( void )
{
  return "JP1.X Serial";
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jstring JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getInterfaceName( JNIEnv *env, jobject jobj )
{
  return env->NewStringUTF( getInterfaceName());
}
#endif


EXPORT const char *getInterfaceVersion( void )
{
  return "0.19";
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jstring JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getInterfaceVersion( JNIEnv *env, jobject jobj )
{
  return env->NewStringUTF( getInterfaceVersion());
}
#endif

bool openPort( const char *portName)
{
	Connection = NONE;
	pageSize = blockSize = eepromSize = eepromAddress = 0;

	if( portName[0] == 0 ) return false;

	//printf( "Trying port %s\n", portName );

	char new_portName[MAX_DEV_FNAME_LEN] = "";

#ifdef WIN32	
	strcpy(new_portName,"\\\\.\\");
	strncat(&new_portName[0], portName, 5);
#else
	//strcpy(new_portName,portNames[ port ]);
	strcpy(new_portName, portName);
#endif

	// ("Port %s \n",new_portName);
	if ( !jp12Init( new_portName )) {
		return false;
	}
	//try JP2_14 first because it only requires a 35mS low pulse of reset
	//to identify itself
	if ( jp2_14Connect() ) {
		Connection = JP2_14;
		return true;  	// init test succeeded
	}

	// Try JP1.3 Connect next so IDC-5 is Low
	// IDC-5 High puts JP1.3 Remote in Tool(I2C) mode and
	// subsequent pulses on IDC-4(SCL) and IDC-6(SDA) are
	// mistakenly treated as I2C and can erase flash contents
	// IDC-5 Low and subsequent pulses won't affect JP1.2 Remotes
	if ( !jp13Connect()) {
		closeRemote();
		return false;
	}
	if ( jp12Test()) {
		Connection = JP13;					// 'E' Test passed
	} else {
		if ( !jp12Connect() ) {
			closeRemote();
			return false;
		}
		if ( jp12Test()) {
			Connection = JP12;				// 'E' Test passed
		} else {
			if ( !jp11Connect()) {
				closeRemote();
				return false;
			}
			if ( jp12Test()) {
				Connection = JP11;			// 'E' Test passed
			} else {
				if ( !jp12OriginalConnect()) {
					closeRemote();
					return false;
				}
				if ( jp12Test()) {
					Connection = JP12Orig;	// 'E' Test passed
				} else {
					closeRemote();
					return false;
				}
			}
		}
	}
	int len = jp12GetIdentity( cmdBuff );
	if ( len == 0 ) {
		closeRemote();
		return false;
	}

	if ( cmdBuff[ 0 ] == 'M' ) {			// JP1 Flash Remote
		pageSize  = 0x0200; // 512
		blockSize = 0x0080; // 128
	} else if ( cmdBuff[ 0 ] == 'S' ) {		// JP1 Flash Remote
		pageSize  = 0x0080; // 128
		blockSize = 0x0080; // 128
	} else if ( cmdBuff[ 0 ] == 'e' ) {		// JP1 EEPROM Adapter (new in rev 0.15)
		pageSize  = 0x0001; //   1
		blockSize = 0x0020; //  32
	}

	len = jp12GetVersion( cmdBuff );
	if ( len < 3 )
	{
		closeRemote();
		return false;
	}

	eepromSize = ( cmdBuff[ 0 ] & 0xFF ) << 7;
	eepromAddress = (( cmdBuff[ 1 ] & 0xFF ) << 8 ) | ( cmdBuff[ 2 ] & 0xFF );

	return true;
}

EXPORT const char *openRemote( const char *portName )
{
	// If a port was specified, use it, don't try others
	if ( portName != NULL )
	{
		strcpy(activePort, portName);
		return openPort( activePort ) ? activePort : NULL;
	}

	// If an active port was found, try it
	if ( activePort[0] )
	{
		//OutputDebugString("Last active port was "); OutputDebugString(activePort); OutputDebugString(", trying it...\n");

		if( openPort( activePort ) )
		{
			return activePort;
		}
	}

#ifndef __APPLE__ //AKA Mac OS X
	// Initialise the port list that will be used as processing order
	int i;
	for (i = 0; i < NUMPORTS; ++i)
		orderedPortNames[i] = portNames[i];
#endif

#ifdef WIN32
	// Identify non-Widget FTDI ports and give them priority

	int num;
	long comPort;

	//	The library ftd2xx.dll is part of the FTDI CDM driver suite for Windows
	//  but will not be present under Linux
	m_hFTDILib = LoadLibrary("ftd2xx.dll");
	if (m_hFTDILib && SetFunctions(m_hFTDILib)
		&& (num = GetNumFTDIDevices()) > 0)
	{
		int iTest;
		for (i = num; i > 0; i--)
		{
			// iTest returns -1 if COM port cannot be determined,
			// otherwise 0 if non-Widget, 1 if Widget
			iTest = TestFTDIDevice(i-1, &comPort); 
			if ((iTest >= 0) && (comPort < NUMPORTS-1)) reorderPorts(comPort, iTest > 0);
		}
	}
#endif


#ifdef __APPLE__ //AKA Mac OS X
	size_t compareLen = strlen(FTDI_SERIAL_DEVNAME);
	DIR* dirp = opendir("/dev");
	dirent* dp;
	while ((dp = readdir(dirp)) != NULL)
		if (!strncmp(dp->d_name, FTDI_SERIAL_DEVNAME, compareLen)) {
		// it is not necessary to check if the port is available because the directory entry is
		// dynamically created, when the device is available
			char path[MAX_DEV_FNAME_LEN] = "/dev/";
			strcat(path, dp->d_name);
			strcpy(activePort, path);
			if( openPort( activePort) )
				break;
		}
	(void)closedir(dirp);
	dirp = NULL;
#else
	// Try all ports in the list
	int port = 0;
	do {
		// Stop at end of port list
		if( !orderedPortNames[ port ] )
		{
			strcpy(activePort, "");
			break;
		}

		strcpy(activePort, orderedPortNames[ port++ ]);

		//OutputDebugString("Trying "); OutputDebugString(activePort); OutputDebugString("\n");

		// Stop when open succeeds
	} while( !openPort( activePort) );
#endif

	return activePort;
}


#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jstring JNICALL Java_com_hifiremote_jp1_io_JP12Serial_openRemote( JNIEnv *env, jobject obj, jstring jPortName )
{
  const char *portName = NULL;
  if ( jPortName != NULL )
  {
    portName = env->GetStringUTFChars( jPortName, NULL );
    if ( portName == NULL )
      return NULL;
  }
  return env->NewStringUTF( openRemote( portName ) );
}
#endif


EXPORT int closeRemote( void )
{
	if(hSerial == INVALID_HANDLE_VALUE)
		return 0;

	resetRemote(); // reset remote and restart as a remote

	//    
	// Restore the state for the comm port we're about to close...
	//    
#ifdef WIN32
	if (m_hFTDILib) {
		FreeLibrary(m_hFTDILib);
	}
	if (!SetCommState(hSerial, &saved_comm_state_dcb)) {
		printf("Error restoring original state for comm port: ");
		jp12PrintLastError();
	}
#else
  if (tcsetattr(hSerial, TCSANOW, &termios_saved) < 0) {
     printf("Error ('%s') restoring serial port settings!\n", strerror(errno));
     return 0;
  }
#endif

	if ( !CloseHandle( hSerial ))
        {
	        hSerial = INVALID_HANDLE_VALUE;
		return 0;
        }
        hSerial = INVALID_HANDLE_VALUE;
	return 1;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT void JNICALL Java_com_hifiremote_jp1_io_JP12Serial_closeRemote( JNIEnv *env, jobject obj )
{
  closeRemote();
}
#endif


// Reset remote and restart as a remote
EXPORT int resetRemote( void )
{
	if (Connection == JP2_14) {
		JP2_14StartOrStopComm( false );
		return 1;
	}
	if (Connection == JP13 || Connection == JP12 || Connection == JP11) {
		// JP1.x /RESET on DTR. Current Design with Resistor and Capacitor on DTR.
		// DTR transition from -10V to +10V causes reset pulse.
		// Need IDC-4 High and IDC-5 High to ensure a Restart on JP1.2 remote with JP1.x design
		// Need IDC-4 High and IDC-5 Low  to ensure a Restart on JP1.3 remote with JP1.x design
		if ( !jp12EscapeComm( CLRBREAK )) // OFF/-10V/1 makes IDC-4 High
			return 0;
		// IDC-5 not set so it should be the same state it was (High or Low) when the
		// remote was opened
		//  if ( !jp12EscapeComm( CLRRTS ))   // OFF/-10V/1 makes IDC-5 High
		//    return 0;

		if ( !jp12EscapeComm( CLRDTR ))   // OFF/-10V/1
			return 0;
		Sleep( 10 );
		if ( !jp12EscapeComm( SETDTR ))   // ON/+10V/0
			return 0;
		Sleep( 100 );  // wait at least 20ms+50ms with IDC-4 High for JP1.2 to go into Restart mode
	}

	if (Connection == JP12Orig) {
		// JP1.2 Original Design with /RESET directly on RTS.
		// RTS transition from 0 to 1 causes reset.
		// Need IDC-4 High to ensure a Restart. IDC-5 is not used.
		if ( !jp12EscapeComm( CLRBREAK )) // OFF/-10V/1 makes IDC-4 High
			return 0;

		if ( !jp12EscapeComm( SETRTS ))   // ON/+10V/0 makes IDC-2 /RESET Low
			return 0;
		Sleep( 10 );
		if ( !jp12EscapeComm( CLRRTS ))   // OFF/-10V/1 makes IDC-2 /RESET High
			return 0;
		Sleep( 100 );  // wait at least 50ms with IDC-4 High for JP1.2 to go into Restart mode
	}

	return 1;
}

EXPORT int getJP12InterfaceType( void )
{
	return Connection;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jint JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getJP12InterfaceType( JNIEnv *, jobject )
{
  return getJP12InterfaceType();
}
#endif

EXPORT const char *getRemoteSignature( void )
{
	if ( Connection != JP2_14 )  {  //JP2_14 sig is read during GetInfo
		unsigned char firstBytes[10];
		if ( readRemote( eepromAddress, firstBytes, 10 ) != 10 )
			return NULL;
		int sigOffset = ( firstBytes[0] + firstBytes[1] == 0xFF ) ? 2 : 0;
		unsigned char byte;
		int i = 0;
		for ( ; i < 8 && ( byte = firstBytes[ i + sigOffset ], byte >= 0x20 && byte < 0x80 ); i++ ) {
			signature[ i ] = byte;
		}
		for ( ; i < 8; i++ ) {
			signature[ i ] = '_';
		}
		signature[ 8 ] = '\0';
	}
	return signature;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jstring JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getRemoteSignature( JNIEnv *env, jobject jobj )
{
  return env->NewStringUTF( getRemoteSignature());
}
#endif


EXPORT int getRemoteEepromAddress( void )
{
  return eepromAddress;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jint JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getRemoteEepromAddress( JNIEnv *, jobject )
{
  return getRemoteEepromAddress();
}
#endif


EXPORT int getRemoteEepromSize( void )
{
  return eepromSize;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jint JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getRemoteEepromSize( JNIEnv *env, jobject jobj )
{
  return getRemoteEepromSize();
}
#endif


EXPORT int readRemote( int address, unsigned char *buffer, int length )
{
	// only allow reading within the upgrade area
	if ( (address < eepromAddress) ||
		((address + length) > (eepromAddress + eepromSize)))
	{
		return -1;
	}

	int offset = 0;
	int blockLength = READBLOCKSIZE;
	while ( offset < length )
	{
		if (( offset + blockLength ) > length )
			blockLength = length - offset;
		if (Connection == JP2_14) {
			if ( !readJP2_14Block( address + offset, blockLength, buffer + offset ))
				return -1;
		} else {
			if       ( !readBlock( address + offset, blockLength, buffer + offset ))
				return -1;
		}
		offset += blockLength;
	}
	return offset;

}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jint JNICALL Java_com_hifiremote_jp1_io_JP12Serial_readRemote( JNIEnv *env, jobject obj, jint address, jbyteArray jBuffer, jint length )
{
  unsigned char *buffer = ( unsigned char * )env->GetByteArrayElements( jBuffer, 0 );
  int rc = readRemote( address, buffer, length );
  env->ReleaseByteArrayElements( jBuffer, (jbyte *)buffer, 0 );
  return rc;
}
#endif

EXPORT bool getJP2info( unsigned char *buffer, int length )
{
	memcpy( buffer, jp2info, sizeof( jp2info ) < length ? sizeof( buffer ) : length );
	return true;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jboolean JNICALL Java_com_hifiremote_jp1_io_JP12Serial_getJP2info( JNIEnv *env, jobject obj, jbyteArray jBuffer, jint length )
{
  unsigned char *buffer = ( unsigned char * )env->GetByteArrayElements( jBuffer, 0 );
  bool b = getJP2info( buffer, length );
  env->ReleaseByteArrayElements( jBuffer, (jbyte *)buffer, 0 );
  return b;
}
#endif

EXPORT int writeRemote( int address, const unsigned char *buffer, int length )
{
	// only allow writing within the upgrade area
	if ( (address < eepromAddress) ||
	   ((address + length) > (eepromAddress + eepromSize)))
	{
		return -1;
	}

	// must have valid page and block sizes
	if( (pageSize < 1) || (blockSize < 1) )
	{
		return -1;
	}

	// Only allow writing on page boundaries, and in pages
	if ((( address % pageSize ) != 0 ) || (( length % pageSize ) != 0 ))
		return -1;

	int offset = 0;
	int blockLength = blockSize;
	while ( offset < length )
	{
		// EEPROM has a page size of 1 and does not need page erasure
		if ((( offset % pageSize ) == 0) && (pageSize > 1))
			erasePages( address + offset );

		if (( offset + blockLength ) > length )
			blockLength = length - offset;

		if ( !writeBlock( address + offset, buffer + offset, blockLength ))
			return -1;

		offset += blockLength;
	}

	return offset;
}

#ifdef JAVA_SDK_AVAILABLE
JNIEXPORT jint JNICALL Java_com_hifiremote_jp1_io_JP12Serial_writeRemote( JNIEnv *env, jobject obj, jint address, jbyteArray jBuffer, jint length )
{
  unsigned char *buffer = ( unsigned char * )env->GetByteArrayElements( jBuffer, 0 );
  int rc = writeRemote( address, buffer, length );
  env->ReleaseByteArrayElements( jBuffer, (jbyte *)buffer, 0 );
  return rc;
}
#endif
