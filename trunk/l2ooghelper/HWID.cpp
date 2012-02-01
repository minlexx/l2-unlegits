#include "stdafx.h"
#include "Logger.h"
#include "base64.h"
#include "HWID.h"

//#define VERIFY_HWID

#ifndef VERIFY_HWID
bool verifyHWID()
{
	return true;
}

bool verifyTimeLimit()
{
	return true;
}
#else

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
		//char to_cmp[12];
		//strncpy( to_cmp, (const char *)(MBR + pbsi[i].FsOffs), 3 );
		//to_cmp[3] = 0;
		//printf( "Cmp %d: [%s]==[%s]...\n", i, pbsi[i].Fs, to_cmp );
		if( strncmp( pbsi[i].Fs, (const char *)(MBR + pbsi[i].FsOffs), strlen( pbsi[i].Fs ) ) == 0 )
		{
			// we found a valid signature
			//printf( "found FS: %s;  index %d\n", pbsi[i].Fs, i );
			break;
		}
	}

	if( i >= max_pbsi )
	{
		log_error( LOG_ERROR, "ERROR: Cannot get serial number of this file system!\n" );
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
	if( RegOpenKeyExA( HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System", 0, KEY_READ, &hKey ) == 0 )
	{
		DWORD dwType = REG_SZ;
		DWORD dwCbData = sizeof(systemBIOSdate);
		// bios date
		if( RegQueryValueExA( hKey, "SystemBiosDate", NULL, &dwType, (LPBYTE)systemBIOSdate, &dwCbData ) == 0 )
		{
			strcat( out_ver, systemBIOSdate );
			strcat( out_ver, "; " );
		} else log_error( LOG_ERROR, "RegQueryValueExA(): error!\n" );
		// bios version
		dwType = REG_MULTI_SZ;
		dwCbData = sizeof(systemBIOSversion);
		if( RegQueryValueExA( hKey, "SystemBiosVersion", NULL, &dwType, (LPBYTE)systemBIOSversion, &dwCbData ) == 0 )
		{
			strcat( out_ver, systemBIOSversion );
			char *pstr = systemBIOSversion + strlen(systemBIOSversion) + 1;
			if( *pstr )
			{
				strcat( out_ver, "; " );
				strcat( out_ver, pstr );
			}
		} else log_error( LOG_ERROR, "RegQueryValueExA(): error!\n" );
		RegCloseKey( hKey );
	}
	else log_error( LOG_ERROR, "RegOpenKeyExA(): error!\n" );
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
	else log_error( LOG_ERROR, "GetVersionExA(): error!\n" );
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
		} else log_error( LOG_ERROR, "RegQueryValueExA(): error!\n" );
		// name
		dwType = REG_SZ;
		dwCbData = sizeof(str);
		if( RegQueryValueExA( hKey, "ProcessorNameString", NULL, &dwType, (LPBYTE)str, &dwCbData ) == 0 )
		{
			strcat( cpu, str );
		} else log_error( LOG_ERROR, "RegQueryValueExA(): error!\n" );
		// id
		dwType = REG_SZ;
		dwCbData = sizeof(str);
		if( RegQueryValueExA( hKey, "Identifier", NULL, &dwType, (LPBYTE)str, &dwCbData ) == 0 )
		{
			strcat( cpu, str );
		} else log_error( LOG_ERROR, "RegQueryValueExA(): error!\n" );
		RegCloseKey( hKey );
	}
	else log_error( LOG_ERROR, "RegOpenKeyExA(): error!\n" );
}

int calcHWID( char *out )
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
	base64_encode_string( globalIDString, strlen(globalIDString), based );

	strcpy( out, based );

	return 1;
}

// Bunny
/*static const char g_hardcoded_HWID[] = 
	"RDBDQTk0NkR8RDBDQTk0NkR8TlRGU3wwNy8yMC8wNzsg////////IC0gNzAwMDcyMDsgQklPUyBEYXRl"
	"OiAwNy8yMC8wNyAxMToyMjo1OCBWZXI6IDA4LjAwLjEwfDUuMS4yNjAwIHNwIDMuMHxHZW51aW5lSW50"
	"ZWwgICAgICAgICAgICAgICBJbnRlbChSKSBQZW50aXVtKFIpIEQgQ1BVIDMuMDBHSHp4ODYgRmFtaWx5"
	"IDE1IE1vZGVsIDYgU3RlcHBpbmcgMg==";*/

// me
/*static const char g_hardcoded_HWID[] = 
	"MDRCNDhEM0V8MDRCNDhEM0V8TlRGU3wwNi8yOS8wOTsgMDYyOTA5IC0gMjAwOTA2Mjk7IEJJT1MgRGF0"
	"ZTogMDYvMjkvMDkgMjA6Mjc6MjkgVmVyOiAwOC4wMC4xNXw1LjEuMjYwMCBzcCAzLjB8QXV0aGVudGlj"
	"QU1EIEFNRCBBdGhsb24odG0pIElJIFgyIDI0MCBQcm9jZXNzb3J4ODYgRmFtaWx5IDE2IE1vZGVsIDYg"
	"U3RlcHBpbmcgMg==";*/

// Disilon
static const char g_hardcoded_HWID[] = 
	"MDQzNjVBNTJ8MDQzNjVBNTJ8TlRGU3wwMi8xOS8wOTsgQV9NX0lfIC0gMjAwMDkxOTsgQklPUyBEYXRl"
	"OiAwMi8xOS8wOSAxNzo1NzozNCBWZXI6IDA4LjAwLjEyfDUuMS4yNjAwIHNwIDIuMHxHZW51aW5lSW50"
	"ZWwgSW50ZWwoUikgQ29yZShUTSkyIER1byBDUFUgICAgIEU4NDAwICBAIDMuMDBHSHp4ODYgRmFtaWx5"
	"IDYgTW9kZWwgMjMgU3RlcHBpbmcgNg==";

bool verifyHWID()
{
	char hwid[4096] = {0};
	memset( hwid, 0, sizeof(hwid) );

	calcHWID( hwid );
	if( strcmp( hwid, g_hardcoded_HWID ) == 0 ) return true;

	log_error( LOG_ERROR, "failed to verify HWID!\n" );

	return false;
}

bool verifyTimeLimit()
{
	SYSTEMTIME st;
	GetLocalTime( &st );
	if( st.wDay >= 30 || st.wMonth >= 10 || st.wYear >= 2010 )
	{
		log_error( LOG_ERROR, "Usage time limit exceeded [ hi, Dis? :) ]\n" );
		return false;
	}
	return true;
}

#endif
