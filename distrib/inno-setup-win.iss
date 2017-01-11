#define MyAppName "GPStudio"
#define MyAppVersion "1.20"
#define MyAppPublisher "Dream Institut Pascal"
#define MyAppURL "http://gpstudio.univ-bpclermont.fr/"
#define MyAppExeName "gpnode_gui.exe"

#define VCmsg "Installing Microsoft Visual C++ Redistributable...."
#define DreamCamDrivermsg "Installing DreamCAM libUSB drivers...."

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{F60553B8-A18D-4484-BC93-C8B70B82BC61}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir=.
OutputBaseFilename=setup-gpstudio_win64-qt5-v{#MyAppVersion}
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
Uninstallable=yes
LicenseFile=gpstudio_win64-qt5\license.txt
ChangesEnvironment=yes
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]  
; doc files
Source: "gpstudio_win64-qt5\doc\*.pdf"; DestDir: "{app}\doc"; Flags: ignoreversion
   
Source: "gpstudio_win64-qt5\license.txt"; DestDir: "{app}"; Flags: ignoreversion; DestName: "license.txt"
Source: "gpstudio_win64-qt5\readme.txt"; DestDir: "{app}"; Flags: ignoreversion; DestName: "readme.txt"
Source: "gpstudio_win64-qt5\install.txt"; DestDir: "{app}"; Flags: ignoreversion; DestName: "install.txt"
Source: "gpstudio_win64-qt5\changelog.txt"; DestDir: "{app}"; Flags: ignoreversion; DestName: "changelog.txt"

; bin files
Source: "gpstudio_win64-qt5\bin\gpnode_gui.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "gpstudio_win64-qt5\bin\gpviewer.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "gpstudio_win64-qt5\bin\*.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "gpstudio_win64-qt5\bin\*.bat"; DestDir: "{app}\bin"; Flags: ignoreversion     
Source: "gpstudio_win64-qt5\bin\iconengines\*"; DestDir: "{app}\bin\iconengines"; Flags: ignoreversion
Source: "gpstudio_win64-qt5\bin\imageformats\*"; DestDir: "{app}\bin\imageformats"; Flags: ignoreversion
Source: "gpstudio_win64-qt5\bin\platforms\*"; DestDir: "{app}\bin\platforms"; Flags: ignoreversion

; doc files
Source: "gpstudio_win64-qt5\doc\*.pdf"; DestDir: "{app}\doc"; Flags: ignoreversion

; scripts files
Source: "gpstudio_win64-qt5\scripts\*"; DestDir: "{app}\scripts"; Flags: ignoreversion recursesubdirs

; support files  
Source: "gpstudio_win64-qt5\support\*"; DestDir: "{app}\support"; Flags: ignoreversion recursesubdirs

; thirdparts files
Source: "gpstudio_win64-qt5\thirdparts\*"; DestDir: "{app}\thirdparts"; Flags: ignoreversion recursesubdirs

[Icons]  
Name: "{group}\{#MyAppName}\gpnode"; Filename: "{app}\bin\gpnode_gui.exe"; WorkingDir: "{app}\bin"; Comment: "GPStudio node editor"
Name: "{group}\{#MyAppName}\gpviewer"; Filename: "{app}\bin\gpviewer.exe"; WorkingDir: "{app}\bin"; Comment: "GPStudio viewer and debugger"
Name: "{group}\{#MyAppName}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"

Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\bin\gpnode_gui.exe"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\bin\gpnode_gui.exe"; Tasks: desktopicon

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\bin"; \
    Check: NotOnPathAlready('{app}\bin')

[Code]
function NotOnPathAlready(Param: string): Boolean;
var
  BinDir, Path: String;
begin
  Log('Checking if '+Param+' dir is already on the %PATH%');
  if RegQueryStringValue(HKEY_LOCAL_MACHINE,'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path) then
  begin // Successfully read the value
    Log('HKLM\Environment\PATH = ' + Path);
    BinDir := ExpandConstant(Param);
    Log('Looking for '+Param+' dir in %PATH%: ' + BinDir + ' in ' + Path);
    if Pos(LowerCase(BinDir), Lowercase(Path)) = 0 then
    begin
      Log('Did not find '+Param+' dir in %PATH% so will add it');
      Result := True;
    end
    else
    begin
      Log('Found '+Param+' dir in %PATH% so will not add it again');
      Result := False;
    end
  end
  else // The key probably doesn't exist
  begin
    Log('Could not access HKCU\Environment\PATH so assume it is ok to add it');
    Result := True;
  end;
end;

[Run]                                                                                                                              
Filename: {sys}\rundll32.exe; Parameters: "setupapi,InstallHinfSection DefaultInstall 128 {app}\thirdparts\dreamcam_usb\libusb_device.inf"; WorkingDir: {app}\thirdparts\dreamcam_usb; Flags: 32bit; StatusMsg: "{#DreamCamDrivermsg}"
; Filename: "{app}\thirdparts\vcredist_x86.exe"; StatusMsg: "{#VCmsg}"; Parameters: "/passive /verysilent /norestart /q:a /c:""VCREDI~3.EXE /q:a /c:""""msiexec /i vcredist.msi /qn"""" """

Filename: "{app}\bin\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
