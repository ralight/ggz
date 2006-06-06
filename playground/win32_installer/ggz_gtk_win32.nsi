;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

!include "MUI.nsh"

;--------------------------------
;General

;Name and file
Name "GGZ Gaming Zone GTK Client"
OutFile "ggz_gtk_win32.exe"

;Default installation folder
InstallDir "$PROGRAMFILES\GGZ Gaming Zone"
  
;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\GGZ Gaming Zone\GTK" ""

;--------------------------------
;Variables

Var MUI_TEMP
Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "../COPYING"
;!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
 
;--------------------------------
; Start Menu folder page configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\GGZ Gaming Zone" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "GGZ Gaming Zone"

!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

;--------------------------------
; Install and end pages
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;--------------------------------
; Uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages
 
!insertmacro MUI_LANGUAGE "English"


;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy
	SetOutPath "$INSTDIR"

;-------------------------------------------------------------------
	!include gtk_client_install.nsh
	!include gtk_games_install.nsh
;-------------------------------------------------------------------
  
	;Store installation folder
	WriteRegStr HKCU "Software\GGZ Gaming Zone" "" $INSTDIR
  
	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
  
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
	;Create shortcuts
	CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
	CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
	!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd


;;--------------------------------
;;Descriptions
;
;  ;Language strings
;  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."
;
;  ;Assign language strings to sections
;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
;!insertmacro MUI_FUNCTION_DESCRIPTION_END
; 
;--------------------------------
;Uninstaller Section

Section "Uninstall"
;-------------------------------------------------------------------
	!include gtk_client_uninstall.nsh
	!include gtk_games_uninstall.nsh
;-------------------------------------------------------------------

	Delete "$INSTDIR\Uninstall.exe"

	RMDir "$INSTDIR"
  
	!insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
    
	Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  
	;Delete empty start menu parent diretories
	StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
 
	startMenuDeleteLoop:
		ClearErrors
		RMDir $MUI_TEMP
		GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    
		IfErrors startMenuDeleteLoopDone
  
		StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
	startMenuDeleteLoopDone:

	DeleteRegKey /ifempty HKCU "Software\GGZ Gaming Zone"
SectionEnd