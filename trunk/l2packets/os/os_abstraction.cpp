#include "stdafx.h"
#include "os_abstraction.h"

// in Lunux must include <sys/time.h>

// returns number of milliseconds of current time
// in windows - since Windows Start (uptime)
// in Lunux - in current day
unsigned long long int OS_GetTickCount()
{
#if defined(_WIN32) || defined(WIN32) || defined(WINVER) || defined(_WIN32_WINNT)
	return (unsigned long long int)GetTickCount();
#else
	timeval tv;
	if( gettimeofday( &tv, NULL ) == 0 )
	{
		return (unsigned long long int)( ((unsigned long long int)tv.tv_sec*1000) + tv.tv_usec/1000 );
	}
	else return 0; // error
#endif
}

/** gettimeofday() description: The gettimeofday() function shall obtain the current time,
expressed as seconds and microseconds since the Epoch, and store it in the timeval structure
pointed to by tp. The resolution of the system clock is unspecified.
If tzp is not a null pointer, the behavior is unspecified. **/

/** struct timeval {
time_t          tv_sec;         // seconds
suseconds_t     tv_usec;        // microseconds
}; **/



/** http://www.linuxmanpages.com/man2/gettimeofday.2.php
uses Libc, .....
{$IFDEF LINUX}
function GetTickCount: Cardinal;
var
tv: timeval;
begin
gettimeofday(tv, nil);
{$RANGECHECKS OFF}
Result := int64(tv.tv_sec) * 1000 + tv.tv_usec div 1000;
end;
{$ENDIF}
**/

/** Another idea:
Implement GetTickCount in Linux
To implement the GetTickCount API when porting code from Windows to Linux, use the following code:

long getTickCount()
{
    tms tm;
    return times(&tm);
}

**/
