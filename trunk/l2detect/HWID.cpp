#include "stdafx.h"
#include "Logger.h"
#include "HWID.h"
#include "base64.h"
#include "L2Detect_auth.h"

const int max_pbsi = 3;

struct partial_boot_sector_info
{
	LPSTR Fs; // file system name
	DWORD FsOffs; // offset of file system name in the boot sector
	DWORD SerialOffs; // offset of the serialnumber in the boot sector
};

void read_MBR( unsigned char *outbuffer )
{
	//HANDLE hFile = CreateFileW( L"\\\\.\\PhysicalDrive0",
	HANDLE hFile = CreateFileW( L"\\\\.\\c:",
		GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		DWORD le = GetLastError();
		log_error( LOG_ERROR, "read_MBR(): CreateFile failed, errCode = %u\n", (unsigned)le );
		return;
	}
	DWORD dwRead = 0;
	if( ReadFile( hFile, outbuffer, 512, &dwRead, NULL ) == 0 )
	{
		DWORD le = GetLastError();
		log_error( LOG_ERROR, "read_MBR(): ReadFile failed, errCode = %u\n", (unsigned)le );
	}
	CloseHandle( hFile );
	log_error( LOG_DEBUG, "read_MBR(): ReadFile read %u bytes\n", (unsigned)dwRead );
}

void getHDDInfo( char *volumeSN, char *hwSN, char *fsName )
{
	char szVolumeName[256] = {0};
	char szFSName[256] = {0};
	unsigned long dwVolumeSN = 0, dwMaxComponentLen = 0, dwFSFlags = 0;
	if( !GetVolumeInformationA( "C:\\", szVolumeName, 256, &dwVolumeSN,
		&dwMaxComponentLen, &dwFSFlags, szFSName, 256 ) ) return;

	unsigned char MBR[512];
	memset( MBR, 0, sizeof(MBR) );
	read_MBR( MBR );

	partial_boot_sector_info pbsi[max_pbsi] =
	{
		{"FAT32", 0x52, 0x43},
		{"FAT",   0x36, 0x27},
		{"NTFS",  0x03, 0x48}
	};

	// try to search for a valid boot sector
	int i = 0;
	for( i=0; i<max_pbsi; i++)
	{
		if( strncmp( pbsi[i].Fs, (const char *)(MBR + pbsi[i].FsOffs), strlen( pbsi[i].Fs ) ) == 0 )
		{
			// we found a valid signature
			break;
		}
	}

	if( i >= max_pbsi )
	{
		log_error( LOG_ERROR, "getHDDInfo(): Cannot get serial number of this file system!\n" );
		return;
	}

	DWORD dwHWSN = 0;
	memcpy( (void *)(&dwHWSN), (const void *)(MBR + pbsi[i].SerialOffs), sizeof(dwHWSN) );

	// out data
	sprintf( volumeSN, "%08X", dwVolumeSN );
	sprintf( hwSN, "%08X", dwHWSN );
	strcpy( fsName, szFSName );
}

void getBIOSInfo( char *out_ver )
{
	out_ver[0] = 0;
	HKEY hKey = NULL;
	char systemBIOSdate[256];
	char systemBIOSversion[1024];
	//char out_ver[1024] = {0};
	memset( systemBIOSdate, 0, sizeof(systemBIOSdate) );
	memset( systemBIOSversion, 0, sizeof(systemBIOSversion) );
	LONG result = 0;
	// support Wow64
	if( (result = RegOpenKeyExA( HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System", 0, KEY_READ | KEY_WOW64_64KEY, &hKey )) == 0 )
	{
		DWORD dwType = REG_SZ;
		DWORD dwCbData = sizeof(systemBIOSdate);
		// bios date
		if( (result = RegQueryValueExA( hKey, "SystemBiosDate", NULL, &dwType, (LPBYTE)systemBIOSdate, &dwCbData )) == 0 )
		{
			strcat( out_ver, systemBIOSdate );
			strcat( out_ver, "; " );
		}
		else
		{
			ErrorLogger_LogLastError( "getBIOSInfo(): RegQueryValueExA()", (DWORD)result );
			// Windows x64 or 2003 versions? other reg keys:
			// HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS\\BIOSReleaseDate
			// HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS\\BIOSVersion
			HKEY hKey2 = NULL;
			result = RegOpenKeyExA( HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", 0, KEY_READ | KEY_WOW64_64KEY, &hKey2 );
			if( result == 0 )
			{
				dwType = REG_SZ;
				dwCbData = sizeof(systemBIOSdate);
				result = RegQueryValueExA( hKey2, "BIOSReleaseDate", NULL, &dwType, (LPBYTE)systemBIOSdate, &dwCbData );
				if( result == 0 )
				{
					strcat( out_ver, systemBIOSdate );
					strcat( out_ver, "; " );
				}
				else
					ErrorLogger_LogLastError( "getBIOSInfo(): RegQueryValueExA() (win64)", (DWORD)result );
				RegCloseKey( hKey2 );
			}
			else
				ErrorLogger_LogLastError( "getBIOSInfo(): RegOpenKeyExA() (win64)", (DWORD)result );
		}
		//
		// bios version
		dwType = REG_MULTI_SZ;
		dwCbData = sizeof(systemBIOSversion);
		if( (result = RegQueryValueExA( hKey, "SystemBiosVersion", NULL, &dwType, (LPBYTE)systemBIOSversion, &dwCbData )) == 0 )
		{
			strcat( out_ver, systemBIOSversion );
			char *pstr = systemBIOSversion + strlen(systemBIOSversion) + 1;
			if( *pstr )
			{
				strcat( out_ver, "; " );
				strcat( out_ver, pstr );
			}
		}
		else
		{
			ErrorLogger_LogLastError( "getBIOSInfo(): RegQueryValueExA()", (DWORD)result );
			// Windows x64 or 2003 versions? other reg keys
			// HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS\\BIOSReleaseDate
			// HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS\\BIOSVersion
			HKEY hKey2 = NULL;
			result = RegOpenKeyExA( HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", 0, KEY_READ | KEY_WOW64_64KEY, &hKey2 );
			if( result == 0 )
			{
				dwType = REG_SZ; // not REG_MULTI_SZ
				dwCbData = sizeof(systemBIOSversion);
				result = RegQueryValueExA( hKey2, "BIOSVersion", NULL, &dwType, (LPBYTE)systemBIOSversion, &dwCbData );
				if( result == 0 )
					strcat( out_ver, systemBIOSversion );
				else
					ErrorLogger_LogLastError( "getBIOSInfo(): RegQueryValueExA() (win64)", (DWORD)result );
				RegCloseKey( hKey2 );
			}
			else
				ErrorLogger_LogLastError( "getBIOSInfo(): RegOpenKeyExA() (win64)", (DWORD)result );
		}
		RegCloseKey( hKey );
	}
	else
		ErrorLogger_LogLastError( "getBIOSInfo(): RegOpenKeyExA", (DWORD)result );
}

void getOSInfo( char *os_info )
{
	OSVERSIONINFOEXA os;
	memset( &os, 0, sizeof(os) );
	os.dwOSVersionInfoSize = sizeof(os);
	if( GetVersionExA( (OSVERSIONINFOA *)&os ) )
	{
		sprintf( os_info, "%d.%d.%d sp %d.%d",
			os.dwMajorVersion, os.dwMinorVersion, os.dwBuildNumber,
			(int)os.wServicePackMajor, (int)os.wServicePackMinor );
	}
	else log_error( LOG_ERROR, "getOSInfo(): GetVersionExA(): error!\n" );
}

void getCPUInfo( char *cpu )
{
	cpu[0] = 0;
	HKEY hKey = NULL;
	char str[512] = {0};
	if( RegOpenKeyExA( HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey ) == 0 )
	{
		DWORD dwType = REG_SZ;
		DWORD dwCbData = sizeof(str);
		// vendor id
		if( RegQueryValueExA( hKey, "VendorIdentifier", NULL, &dwType, (LPBYTE)str, &dwCbData ) == 0 )
		{
			strcat( cpu, str );
			strcat( cpu, " " );
		} else log_error( LOG_ERROR, "getCPUInfo(): RegQueryValueExA(): error!\n" );
		// name
		dwType = REG_SZ;
		dwCbData = sizeof(str);
		if( RegQueryValueExA( hKey, "ProcessorNameString", NULL, &dwType, (LPBYTE)str, &dwCbData ) == 0 )
		{
			strcat( cpu, str );
		} else log_error( LOG_ERROR, "getCPUInfo(): RegQueryValueExA(): error!\n" );
		// id
		dwType = REG_SZ;
		dwCbData = sizeof(str);
		if( RegQueryValueExA( hKey, "Identifier", NULL, &dwType, (LPBYTE)str, &dwCbData ) == 0 )
		{
			strcat( cpu, str );
		} else log_error( LOG_ERROR, "getCPUInfo(): RegQueryValueExA(): error!\n" );
		RegCloseKey( hKey );
	}
	else log_error( LOG_ERROR, "getCPUInfo(): RegOpenKeyExA(): error!\n" );
}

void convert_to_urlsafe( char *s )
{
	/*
	* use '-' and '_' instead of '+' and '/'
	* no line feeds
	* padding character is '*' instead of '='
	*/
	while( (*s) )
	{
		if( (*s) == '+' ) (*s) = '-'; 
		if( (*s) == '/' ) (*s) = '_';
		if( (*s) == '=' ) (*s) = '*';
		s++;
	}
}

int getLocalHWID( char *out )
{
	char globalIDString[4096] = {0};
	char based[4096] = {0};
	char volumeSN[256] = {0}, hwSN[256] = {0}, fsName[256] = {0};
	char biosInfo[1024] = {0};
	char os_info[1024] = {0};
	char cpu_info[1024] = {0};

	getHDDInfo( volumeSN, hwSN, fsName );
	getBIOSInfo( biosInfo );
	getOSInfo( os_info );
	getCPUInfo( cpu_info );
	sprintf( globalIDString, "%s|%s|%s|%s|%s|%s", volumeSN, hwSN, fsName, biosInfo, os_info, cpu_info );
	log_error( LOG_DEBUG, "HWID: [%s]\n", globalIDString );
	base64_encode_string( globalIDString, strlen(globalIDString), based );

	convert_to_urlsafe( based );

	strcpy( out, based );
	return 1;
}

bool VerifyHWID()
{
	char localHWID[4096];
	char setHWID[4096];
	memset( localHWID, 0, sizeof(localHWID) );
	memset( setHWID, 0, sizeof(setHWID) );
	getLocalHWID( localHWID );
	//getHWID( setHWID );
	//log_error( LOG_DEBUG, "Verify HWID: [%s] == [%s] ?...\n", localHWID, setHWID );
	if( strcmp( localHWID, setHWID ) == 0 )
	{
		log_error( LOG_OK, "HWID verify OK\n" );
		return true;
	}
	log_error( LOG_ERROR, "HWID verify error!\n" );
	return false;
}
