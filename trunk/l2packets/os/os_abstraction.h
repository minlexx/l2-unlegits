#ifndef H_OS_ABSTRCTION
#define H_OS_ABSTRCTION

/** \file os_abstraction.h
* uses defines to determine under which OS it is compiled:
* ifdef WIN32 | _WIN32 ... - Windows
* else - Linux :)
*/

/** Os abstracted function GetTickCount() from Win32 API.\n
* Returns number of milliseconds of current time.\n
* in windows - since Windows Start (uptime)\n
* in Lunux - in current day\n
* \return number of milliseconds of current time
*/
unsigned long long int OS_GetTickCount();

#endif
