void __cdecl srand( unsigned int seed )
{
	_getptd()->_holdrand = (unsigned long)seed;
}

int __cdecl rand( void )
{
	_ptiddata ptd = _getptd();
	return( ((ptd->_holdrand = ptd->_holdrand * 214013L + 2531011L) >> 16) & 0x7fff );
}

public rand
rand proc near
call    sub_77C29F25     // _getptd()?
mov     ecx, [eax+14h]   // offset of _holdrand?
imul    ecx, 343FDh      // 214013
add     ecx, 269EC3h     // 2531011
mov     [eax+14h], ecx   
mov     eax, ecx
shr     eax, 10h         // >> 16
and     eax, 7FFFh
retn
rand endp

public srand
srand proc near

arg_0= dword ptr  8

mov     edi, edi            // 3 lines - BP-based frame
push    ebp
mov     ebp, esp
call    sub_77C29F25        // _getptd()?
mov     ecx, [ebp+arg_0]    // ecx = seed (argument 0)
mov     [eax+14h], ecx      // _getptd()->_holdrand = seed
pop     ebp
retn
srand endp
