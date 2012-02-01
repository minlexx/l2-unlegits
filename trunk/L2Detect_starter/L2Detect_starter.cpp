#include "stdafx.h"
#include "ChooseDlg.h"

#define PROCESS_NAME  "l2.exe"
#define DLL_NAME      "L2Detect.dll"
#define DLL_AUTH_NAME "L2Detect_auth.dll"

//I could just use PROCESS_ALL_ACCESS but it's always best to use the absolute bare minimum of priveleges, so that your code works in as
//many circumstances as possible.
#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)
 
BOOL WriteProcessBYTES( HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize );
int  RunProcess( HANDLE *phProcess, HANDLE *phThread );
BOOL InjectDLL( HANDLE hProcess, char *dllName );

extern "C" __declspec(dllimport) int getHWID( char *);

bool validateAuthDll()
{
	HINSTANCE hInstDLL = LoadLibrary( TEXT( DLL_AUTH_NAME ) );
	if( hInstDLL )
	{
		FreeLibrary( hInstDLL );
		return true;
	}
	MessageBox( NULL,
		TEXT("The application has failed to start because required DLL ")
		TEXT( DLL_AUTH_NAME )
		TEXT( " was not found.\n" )
		TEXT( "Reinstall application or contact application author." )
		,TEXT("DLL Error"), MB_ICONSTOP );
	return false;
}


bool IsWindowsNT()
{
	OSVERSIONINFO osinfo;
	memset( &osinfo, 0, sizeof(osinfo) );
	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx( &osinfo );
	if( osinfo.dwMajorVersion < 5 ) return false;
	return true;
}

/*void run_connect()
{
	SOCKET s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	addr.sin_addr.s_addr = inet_addr( "81.30.199.5" );
	//addr.sin_port = htons( 80 );
	addr.sin_port = htons( 7777 );
	//
	int r = connect( s, (const sockaddr *)&addr, sizeof(addr) );
	if( r == -1 )
	{
		MessageBox( NULL, TEXT("connect() failed!"), TEXT("Error!"), MB_ICONSTOP );
		closesocket( s );
		return;
	}
	MessageBox( NULL, TEXT("connect() OK!"), TEXT("OK!"), MB_ICONINFORMATION );
	shutdown( s, 0 );
	closesocket( s );
}*/

int WINAPI _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	InitCommonControls();

	//if( !validateAuthDll() ) return 1; // тут тоже пока отпаяем
	
	INT_PTR r = ChooseMode( hInstance );
	if( r == IDNO ) // outgame
	{
		WSADATA wsadata;
		WSAStartup( MAKEWORD(2,0), &wsadata );

		TCHAR tszEvtName[256];
		wsprintf( tszEvtName, TEXT("L2Detect Outgame Event_%d"), (unsigned)GetCurrentProcessId() );

		//MessageBox( NULL, tszEvtName, TEXT("ggg"), 0 );

		HANDLE hEvtDbg = CreateEvent( NULL, FALSE, FALSE, tszEvtName );
		HINSTANCE hDLL = LoadLibrary( TEXT( DLL_NAME ) );

		if( hDLL == NULL )
		{
			MessageBox( NULL,
				TEXT("Cannot find required DLL: ")
				TEXT( DLL_NAME ),
				TEXT("Error starting L2Detect:"), MB_ICONSTOP );
			CloseHandle( hEvtDbg );
			WSACleanup();
			return 0;
		}

		/*r = IDYES;
		while( r == IDYES )
		{
			r = MessageBox( NULL, TEXT("Run connect()?"), TEXT("OK"), MB_ICONQUESTION | MB_YESNO );
			if( r == IDYES ) run_connect();
		}*/

		WaitForSingleObject( hEvtDbg, INFINITE );
		Sleep( 1000 );

		//FreeLibrary( hDLL );
		CloseHandle( hEvtDbg );
		WSACleanup();
		return 0;
	}
	else if( r == IDYES ) // ingame
	{
		if( IsWindowsNT() )
		{
			HANDLE hProcess = NULL, hThread = NULL;
			int res = RunProcess( &hProcess, &hThread );
			if( res == 0 ) return 0;
			if( res == 1 )
			{
				InjectDLL( hProcess, DLL_NAME );
				ResumeThread( hThread );
				CloseHandle( hProcess );
				CloseHandle( hThread );
			}
			else
			{
				MessageBox( 0, TEXT("Cannot start process!"), TEXT("Error!"), MB_ICONSTOP );
			}
		}
		else
		{
			MessageBox( 0,
				TEXT("Required Windows 2000 or later!\n")
				TEXT("Your system does not support this method!"),
				TEXT("OS Error!"),
				MB_ICONSTOP );
		}
	}// if( r == IDNO )

	return 0;
}


int RunProcess( HANDLE *phProcess, HANDLE *phThread )
{
	TCHAR lpstrFilter[] = TEXT("Executables\0*.exe\0All files\0*.*\0\0");
	TCHAR lpstrFile[512] = {0};
	TCHAR dir[512] = {0};
	//TCHAR curDir[512] = {0};

	//GetCurrentDirectory( sizeof(curDir)/sizeof(curDir[0]), curDir );
	_tcscpy( lpstrFile, _T("l2.exe") );

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = lpstrFilter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = sizeof(lpstrFile)/sizeof(lpstrFile[0]);
	ofn.lpstrTitle = TEXT("Choose program to run");
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST;

	BOOL ret = GetOpenFileName( &ofn );
	if( !ret ) return 0;

	//SetCurrentDirectory( procDir );
	GetCurrentDirectory( sizeof(dir)/sizeof(dir[0]), dir );
	//SetCurrentDirectory( curDir );

	PROCESS_INFORMATION pi;
	memset( &pi, 0, sizeof(pi) );
	STARTUPINFO si;
	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	ret = CreateProcess( NULL, lpstrFile, NULL, NULL, FALSE,
		CREATE_SUSPENDED, NULL, dir, &si, &pi );
	if( !ret ) return -1;
	//
	(*phProcess) = pi.hProcess;
	(*phThread) = pi.hThread;
	return 1;
}

BOOL InjectDLL( HANDLE hProcess, char *dllName )
{
   HANDLE Proc = hProcess;
   LPVOID RemoteString, LoadLibAddy;

   LoadLibAddy = (LPVOID)GetProcAddress( GetModuleHandle( TEXT("kernel32.dll") ), "LoadLibraryA" );

   RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(DLL_NAME)+1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
   if( !RemoteString ) MessageBox( 0, TEXT("VirtualAllocEx() returned NULL!"), TEXT("Error (InjectDLL)"), MB_ICONSTOP );
   if( WriteProcessMemory(Proc, (LPVOID)RemoteString, dllName, strlen(dllName), NULL) == FALSE )
	   MessageBox( 0, TEXT("WriteProcessMemory() returned FALSE!"), TEXT("Error (InjectDLL)"), MB_ICONSTOP );
   if( CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL) == NULL )
	   MessageBox( 0, TEXT("CreateRemoteThread() returned NULL!"), TEXT("Error (InjectDLL)"), MB_ICONSTOP );

   return TRUE;
}

