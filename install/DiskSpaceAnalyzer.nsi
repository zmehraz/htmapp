; InstallWinglibScriptEngine.nsi
;
;--------------------------------

!define APPNAME "Disk Space Analyzer ${PROC}"

!define FILENAME "DiskSpaceAnalyzer"

!define APPKEY "${FILENAME}_${PROC}_${BUILD}"

; The name of the installer

; The file to write
!ifdef DVER
	!define APPVNAME "${APPNAME} ${DVER}"
!else
	!define APPVNAME "${APPNAME}"
!endif

Name "${APPVNAME}"

!ifdef FVER
	OutFile "${OUTROOT}\Install${FILENAME}${POSTFIX}_${FVER}_${PROC}_${BUILD}.exe"
!else
	OutFile "${OUTROOT}\Install${FILENAME}${POSTFIX}_${PROC}_${BUILD}.exe"
!endif

; The default installation director
!if "${PROC}" == "x64"
	InstallDir "$PROGRAMFILES64\${APPNAME}"
!else
	InstallDir "$PROGRAMFILES\${APPNAME}"
!endif

; Request application privileges for Windows Vista
RequestExecutionLevel admin

; Installation directory key
InstallDirRegKey HKLM "SOFTWARE\${APPKEY}" "Install_Dir"

;--------------------------------
; Pages

Page license
LicenseData "License.txt"
Page components
Page directory
Page instfiles


UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "${APPVNAME} (required)"

  SectionIn RO

!if "${PROC}" == "x64"
	SetRegView 64
!endif
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  SetOverwrite on
  File "License.txt"
  File "${OUTROOT}\${FILENAME}${POSTFIX}.exe"
  
  ; Qt + Webkit dependencies, this sux :(
  File "${OUTROOT}\libgcc_s_dw2-1.dll"
  File "${OUTROOT}\mingwm10.dll"
  File "${OUTROOT}\phonon4.dll"
  File "${OUTROOT}\QtCore4.dll"
  File "${OUTROOT}\QtGui4.dll"
  File "${OUTROOT}\QtNetwork4.dll"
  File "${OUTROOT}\QtWebKit4.dll"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\${APPKEY}" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "DisplayName" "${APPVNAME}"
!ifdef DVER
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "DisplayVersion" "${DVER}"
!endif
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "InstallLocation" '$INSTDIR'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\${APPNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Disk Space Analyzer.lnk" "$INSTDIR\${FILENAME}${POSTFIX}.exe"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

;--------------------------------
; Uninstaller

Section "Uninstall"
  
!if "${PROC}" == "x64"
	SetRegView 64
!endif

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}"
  DeleteRegKey HKLM "SOFTWARE\${APPKEY}"

  ; Remove files and uninstaller
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\License.txt"
  Delete "$INSTDIR\${FILENAME}${POSTFIX}.exe"  
  
  ; Remove Qt/WebKit dependencies
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"  
  Delete "$INSTDIR\mingwm10.dll"  
  Delete "$INSTDIR\phonon4.dll"  
  Delete "$INSTDIR\QtCore4.dll"  
  Delete "$INSTDIR\QtGui4.dll"  
  Delete "$INSTDIR\QtNetwork4.dll"  
  Delete "$INSTDIR\QtWebKit4.dll"  

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APPNAME}\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${APPNAME}"
  RMDir "$INSTDIR"
  
SectionEnd

;--------------------------------
; Remove previous version

Function .onInit
!if "${PROC}" == "x64"
	SetRegView 64
!endif
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "UninstallString"
  ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPKEY}" "InstallLocation"
  StrCmp $R0 "" done
    MessageBox MB_YESNOCANCEL|MB_ICONQUESTION \
			   "A previous version of ${APPNAME} was found.$\n$\nIt is recommended that you uninstall it first.$\n$\nDo you want to do that now?" \
			   /SD IDYES IDNO done IDYES uninst
      Abort
uninst:
    ExecWait '$R0 /S _?=$R1'
done: 
FunctionEnd

