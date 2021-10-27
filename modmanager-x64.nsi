; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Mod Manager"
!define PRODUCT_VERSION "1.0.0-beta.5"
!define PRODUCT_PUBLISHER "kaniol"
!define PRODUCT_WEB_SITE "www.github.com/kaniol-lck/modmanager"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\modmanager.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "src\modmanager.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "LICENSE"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\modmanager.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "TradChinese"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "ModManager-${PRODUCT_VERSION}-x64-Installer.exe"
InstallDir "$PROGRAMFILES\Mod Manager"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR\bin\bearer"
  SetOverwrite try
  File "deploy\bin\bearer\qgenericbearer.dll"
  SetOutPath "$INSTDIR\bin\iconengines"
  File "deploy\bin\iconengines\qsvgicon.dll"
  SetOutPath "$INSTDIR\bin\imageformats"
  File "deploy\bin\imageformats\qgif.dll"
  File "deploy\bin\imageformats\qicns.dll"
  File "deploy\bin\imageformats\qico.dll"
  File "deploy\bin\imageformats\qjpeg.dll"
  File "deploy\bin\imageformats\qsvg.dll"
  File "deploy\bin\imageformats\qtga.dll"
  File "deploy\bin\imageformats\qtiff.dll"
  File "deploy\bin\imageformats\qwbmp.dll"
  File "deploy\bin\imageformats\qwebp.dll"
  SetOutPath "$INSTDIR\bin"
  File "deploy\bin\libaria2-0.dll"
  File "deploy\bin\libcares-4.dll"
  File "deploy\bin\libcrypto-1_1-x64.dll"
  File "deploy\bin\libiconv-2.dll"
  File "deploy\bin\libintl-8.dll"
  File "deploy\bin\liblzma-5.dll"
  File "deploy\bin\libsqlite3-0.dll"
  File "deploy\bin\libssh2-1.dll"
  File "deploy\bin\libssl-1_1-x64.dll"
  File "deploy\bin\libstdc++-6.dll"
  File "deploy\bin\libwinpthread-1.dll"
  File "deploy\bin\libxml2-2.dll"
  File "deploy\bin\modmanager.exe"
  CreateDirectory "$SMPROGRAMS\Mod Manager"
  CreateShortCut "$SMPROGRAMS\Mod Manager\Mod Manager.lnk" "$INSTDIR\bin\modmanager.exe"
  CreateShortCut "$DESKTOP\Mod Manager.lnk" "$INSTDIR\bin\modmanager.exe"
  SetOutPath "$INSTDIR\bin\platforms"
  File "deploy\bin\platforms\qwindows.dll"
  SetOutPath "$INSTDIR\bin"
  File "deploy\bin\Qt5Core.dll"
  File "deploy\bin\Qt5Gui.dll"
  File "deploy\bin\Qt5Network.dll"
  File "deploy\bin\Qt5Svg.dll"
  File "deploy\bin\Qt5Widgets.dll"
  SetOutPath "$INSTDIR\bin\styles"
  File "deploy\bin\styles\qwindowsvistastyle.dll"
  SetOutPath "$INSTDIR\bin"
  File "deploy\bin\zlib1.dll"
  SetOutPath "$INSTDIR\ssl\certs"
  File "deploy\ssl\certs\ca-bundle.crt"
  File "deploy\ssl\certs\ca-bundle.trust.crt"
  SetOutPath "$INSTDIR\ssl"
  File "deploy\ssl\ct_log_list.cnf"
  File "deploy\ssl\ct_log_list.cnf.dist"
  File "deploy\ssl\openssl.cnf"
  File "deploy\ssl\openssl.cnf.dist"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\Mod Manager\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Mod Manager\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\bin\modmanager.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\modmanager.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从你的计算机移除。"
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，其及所有的组件？" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\ssl\openssl.cnf.dist"
  Delete "$INSTDIR\ssl\openssl.cnf"
  Delete "$INSTDIR\ssl\ct_log_list.cnf.dist"
  Delete "$INSTDIR\ssl\ct_log_list.cnf"
  Delete "$INSTDIR\ssl\certs\ca-bundle.trust.crt"
  Delete "$INSTDIR\ssl\certs\ca-bundle.crt"
  Delete "$INSTDIR\bin\zlib1.dll"
  Delete "$INSTDIR\bin\styles\qwindowsvistastyle.dll"
  Delete "$INSTDIR\bin\Qt5Widgets.dll"
  Delete "$INSTDIR\bin\Qt5Svg.dll"
  Delete "$INSTDIR\bin\Qt5Network.dll"
  Delete "$INSTDIR\bin\Qt5Gui.dll"
  Delete "$INSTDIR\bin\Qt5Core.dll"
  Delete "$INSTDIR\bin\platforms\qwindows.dll"
  Delete "$INSTDIR\bin\modmanager.exe"
  Delete "$INSTDIR\bin\libxml2-2.dll"
  Delete "$INSTDIR\bin\libwinpthread-1.dll"
  Delete "$INSTDIR\bin\libstdc++-6.dll"
  Delete "$INSTDIR\bin\libssl-1_1-x64.dll"
  Delete "$INSTDIR\bin\libssh2-1.dll"
  Delete "$INSTDIR\bin\libsqlite3-0.dll"
  Delete "$INSTDIR\bin\liblzma-5.dll"
  Delete "$INSTDIR\bin\libintl-8.dll"
  Delete "$INSTDIR\bin\libiconv-2.dll"
  Delete "$INSTDIR\bin\libcrypto-1_1-x64.dll"
  Delete "$INSTDIR\bin\libcares-4.dll"
  Delete "$INSTDIR\bin\libaria2-0.dll"
  Delete "$INSTDIR\bin\imageformats\qwebp.dll"
  Delete "$INSTDIR\bin\imageformats\qwbmp.dll"
  Delete "$INSTDIR\bin\imageformats\qtiff.dll"
  Delete "$INSTDIR\bin\imageformats\qtga.dll"
  Delete "$INSTDIR\bin\imageformats\qsvg.dll"
  Delete "$INSTDIR\bin\imageformats\qjpeg.dll"
  Delete "$INSTDIR\bin\imageformats\qico.dll"
  Delete "$INSTDIR\bin\imageformats\qicns.dll"
  Delete "$INSTDIR\bin\imageformats\qgif.dll"
  Delete "$INSTDIR\bin\iconengines\qsvgicon.dll"
  Delete "$INSTDIR\bin\bearer\qgenericbearer.dll"

  Delete "$SMPROGRAMS\Mod Manager\Uninstall.lnk"
  Delete "$SMPROGRAMS\Mod Manager\Website.lnk"
  Delete "$DESKTOP\Mod Manager.lnk"
  Delete "$SMPROGRAMS\Mod Manager\Mod Manager.lnk"

  RMDir "$SMPROGRAMS\Mod Manager"
  RMDir "$INSTDIR\ssl\certs"
  RMDir "$INSTDIR\ssl"
  RMDir "$INSTDIR\bin\styles"
  RMDir "$INSTDIR\bin\platforms"
  RMDir "$INSTDIR\bin\imageformats"
  RMDir "$INSTDIR\bin\iconengines"
  RMDir "$INSTDIR\bin\bearer"
  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
