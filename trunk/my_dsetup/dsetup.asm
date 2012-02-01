;wildcard, by Fyyre
;http://fyyre.l2-fashion.de
;modified to not disable GameGuard, by minlexx ;)
; just to link to L2Detect.dll
;

format PE GUI 4.0 DLL at 00400000h
entry DllEntryPoint

include '%fasminc%\win32a.inc'

section '.text' code readable executable

proc DllEntryPoint
  mov	eax, 1
  ret
  ; next will never be executed, but reference to L2Detect.dll will remain )
  call	dword [L2Detect_fake]  ; minlexx ;) if we don't call it, it won't appear in imports section =[
endp

; include standard dsetup.dll exports (c) Fyyre
include 'inc\exported.inc'

section '.idata' import data readable

  library kernel32,'kernel32.dll',\
	  l2detect,'l2detect.dll' ; minlexx ;)

  include '%fasminc%\API\KERNEL32.INC'
  import l2detect, L2Detect_fake,'fake' ; minlexx ;)

section '.edata' export data  readable executable
  export 'wildcard.dll',\
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
