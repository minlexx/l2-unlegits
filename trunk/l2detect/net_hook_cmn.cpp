#include "stdafx.h"
#include "net_hook.h"
#include "Logger.h"

/** Validates istructure alignment. returns true on OK **/
bool Hook_ValidateAlign()
{
	size_t ss = sizeof(struct jmp_push_ret);
	if( ss != 6 ) /* FatalAppExit( 0, TEXT("Structure alignment error!") ); */
	{
#ifdef _DEBUG
		DebugBreak();
#endif
		return false;
	}
	ss = sizeof(jmp_jmp);
	if( ss != 5 ) /* FatalAppExit( 0, TEXT("Structure alignment error!") ); */
	{
#ifdef _DEBUG
		DebugBreak();
#endif
		return false;
	}
#ifdef _DEBUG
	log_error( LOG_DEBUG, "Hook_ValidateAlign(): returning true (sizeof(jmp_push_ret) = %u)\n",
		(unsigned int)sizeof(jmp_push_ret) );
	ErrorLogger_FlushLogFile();
#endif
	return true;
}



/////////////////////////////////////////////////////////////////////////////
//
// Функция ставит JMP-хук в функцию
//
// old_ptr - адрес старой функции
// new_ptr - адрес новой функции
// jmp_ptr - указатель на переменную, в которую запишется адрес перехода
//
void Hook_InterceptCall( DWORD old_ptr, DWORD new_ptr, DWORD *jmp_ptr )
{
	jmp_jmp jump;
	jump.instr_jmp = 0xE9;
	jump.jmp_arg = new_ptr - old_ptr - 5;

	DWORD oldProtect, prot;

	GetCurrentProcess();

	// Обычно страницы в этой области недоступны для записи
	// поэтому принудительно разрешаем запись
	prot = PAGE_EXECUTE_WRITECOPY; // PAGE_READWRITE;
	BOOL vp_res = VirtualProtectEx( (HANDLE)-1, (LPVOID)old_ptr, 5, prot, &oldProtect );
	if( !vp_res )
	{
		DWORD le = GetLastError();
		ErrorLogger_LogLastError( "Hook_InterceptCall(): VirtualProtectEx() failed", le );
		ErrorLogger_FlushLogFile();
		return;
	}

	// Пишем новый адрес
	//WriteProcessMemory(hprocess, (LPVOID)old_ptr, (void*)(&jump), 5, &written);
	unsigned char *po = (unsigned char *)old_ptr;
	unsigned char *pj = (unsigned char *)&jump;
	po[0] = pj[0];
	po[1] = pj[1];
	po[2] = pj[2];
	po[3] = pj[3];
	po[4] = pj[4];

	//восстанавливаем первоначальную защиту области по записи
	prot = oldProtect;
	VirtualProtectEx( (HANDLE)-1, (LPVOID)old_ptr, 5, prot, &oldProtect );

	(*jmp_ptr) = old_ptr + 5;
}


