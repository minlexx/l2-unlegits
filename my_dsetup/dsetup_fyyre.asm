;wildcard, by Fyyre
;http://fyyre.l2-fashion.de
;

format PE GUI 4.0 DLL at 00400000h
entry DllEntryPoint

ERROR_INVALID_DATA	equ	13

include '%fasminc%\win32a.inc'

section '.text' code readable executable

proc DllEntryPoint
  cmp	dword [IsCorePatched], 1
  je	@F
  call	PatchCoreDll
@@:
  cmp	dword [IsEnginePatched], 1
  je	@F
  call	verify_opcodes
@@:
  mov	eax, 1
  ret
  call	dword [L2Detect_fake]  ; minlexx ;) if we don't call it, it won't appear in imports section =[
endp

proc PatchCoreDll
  mov	eax, [?StaticInit@FName@@SAXXZ]     ;check for non-Themida opcodes
  mov	eax, [eax]
  cmp	eax, 06AEC8B55h
  je	@F
  ret
@@:
  pushad
  mov	ebp, esp
  lea	edx, [ebp-4]
  push	edx			     ;old protect
  push	PAGE_READWRITE		     ;0x40
  push	4			     ;one dword
  mov	ebx, [?GL2UseGameGuard@@3HA] ;address of export
  push	ebx
  push	-1			     ;(-1) == GetCurrentProcess
  call	[VirtualProtectEx]
; test  al, al
; jz    @F                           ;should never fail...
;
  mov	dword [ebx], 0h 		     ;set ?GL2UseGameGuard@3HLA to false
  mov	[IsCorePatched], 1		     ;mark as complete
  mov	esp, ebp
  popad 				     ;restore registers, etc.
  ret
endp

proc verify_opcodes
  push	edi
  mov	edi, [?Destroy@UNetworkHandler@@UAEXXZ] 	 ;ExRestart client is always the function above this one,
							 ;for CT2.2 and below...
  cmp	dword [edi], 0F98B5753h
  jne	@F
  call	PatchEngineDll

@@:
  pop	edi
  ret
endp

proc PatchEngineDll			     ;locate ExRestartClient & IsRASConnected
  push	  edi				     ;both are non-exported.
  push	  ecx

  std					     ;set the directional flag, search backwards in memory.
  mov	  ecx, 0FFFFh
  mov	  eax, 06AEC8B55h
  ;edi == address of UNetworkHandler::Destroy ... no need to set it again.

@@:
  repne   scasb
  jcxz	  error_handler 		     ;if ecx == zero, our search fails.
  cmp	  dword [edi+1], eax
  jne	  @B
  inc	  edi
  cmp	  dword [edi+18h], 007B0EC81h	     ;sub esp, 7B0h == IsRASConnected
  jne	  _is_ct2_2_or_lower		     ;if not present, client is CT2.2 or lower.
  mov	  dword [edi], 0C0FEC033h	     ;replace function start with xor eax, eax .. inc al
  add	  edi, 4			     ;move forward by one dword
  mov	  byte [edi], 0C3h		     ;retn - replace function start with -->> return true;

  mov	  ecx, 0FFFFh
@@:
  repne   scasb
  jcxz	  error_handler
  cmp	  dword [edi+1], eax
  jne	  @B
  inc	  edi

_is_ct2_2_or_lower:
  mov	  dword [edi], 0C0FEC033h
  add	  edi, 4
  mov	  byte [edi], 0C3h

@@:
  cld					     ;fix the directional flag back.
  pop	  ecx				     ;restore saved registers.
  pop	  edi

  mov	[IsEnginePatched], 1
  ret
endp

proc error_handler
;when ecx == 0 we display a message box/ExitProcess, searching beyond the code section is pointless
;in this situation and will only end up crashing.
;
  cmp	eax, 250C862Bh
  je	@F

  push	0
  push	error_caption
  push	error_text
  push	MB_OK
  call	[MessageBoxA]
  push	ERROR_INVALID_DATA
  call	[ExitProcess]

@@:
  xor	ecx, ecx
  mov	bl, 0
  ret
endp

include 'inc\exported.inc'

section '.data' data readable writeable
cmpb1			dd	0
IsCorePatched		dd	0
IsEnginePatched 	dd	0
			dw	0
error_text		db	'byte signature not found =( contact me -->> fyyre@nightmail.ru', 0
			dw	0
error_caption		db	'error_handler', 0
			dw	0
Fyyre			du	'Fyyre', 0   ; =)

section '.idata' import data readable

  library kernel32,'kernel32.dll',\
	  user32, 'user32.dll',\
	  core,'core.dll',\
	  engine,'engine.dll',\
	  l2detect,'L2Detect.dll'

  include '%fasminc%\API\KERNEL32.INC'
  include '%fasminc%\API\USER32.INC'
  include 'inc\core.inc'
  include 'inc\engine.inc'
	import l2detect, L2Detect_fake,'fake' ; minlexx ;)

section '.edata' export data  readable executable
  export 'dsetup_fyyre.dll',\
       DirectXDeviceDriverSetupA,'DirectXDeviceDriverSetupA',\
       DirectXDeviceDriverSetupW,'DirectXDeviceDriverSetupW',\
       DirectXRegisterApplicationA,'DirectXRegisterApplicationA',\
       DirectXRegisterApplicationW,'DirectXRegisterApplicationW',\
       DirectXSetupA,'DirectXSetupA',\
       DirectXSetupIsJapan,'DirectXSetupIsJapan',\
       DirectXSetupIsJapanNec,'DirectXSetupIsJapanNec',\
       DirectXSetupW,'DirectXSetupW',\
       DirectXUnRegisterApplication,'DirectXUnRegisterApplication',\
       DirectXSetupSetCallback,'DirectXSetupSetCallback',\
       DirectXSetupGetVersion,'DirectXSetupGetVersion',\
       DirectXSetupCallback,'DirectXSetupCallback',\
       DirectXSetupGetFileVersion,'DirectXSetupGetFileVersion',\
       DirectXLoadString,'DirectXLoadString',\
       DirectXSetupIsEng,'DirectXSetupIsEng',\
       DirectXSetupShowEULA,'DirectXSetupShowEULA',\
       DirectXSetupGetEULAA,'DirectXSetupGetEULAA',\
       DirectXSetupGetEULAW,'DirectXSetupGetEULAW'

section '.reloc' data discardable fixups
section '.rsrc' data readable resource from 'wildcard.res'