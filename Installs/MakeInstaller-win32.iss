[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={#AppName}
AppName={#AppName}
AppVersion={#Version}
AppVerName={#AppName} {#Version}
DefaultDirName={pf}\{#AppName}
DisableProgramGroupPage=yes
OutputBaseFilename="{#AppName}-win32_{#Version}"
Compression=lzma2/ultra
InternalCompressLevel=ultra
PrivilegesRequired=admin
SolidCompression=yes
UninstallDisplayIcon={app}\{#AppExeName}
SetupIconFile="{#SourcePath}\..\Resources\app.ico"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Dirs]
Name: "{app}\Templates"; Flags: uninsneveruninstall

[Files]
Source: "{#ReleaseDir}\iconengines\*"; DestDir: "{app}\iconengines"; Flags: ignoreversion 
Source: "{#ReleaseDir}\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion 
Source: "{#ReleaseDir}\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion 
Source: "{#ReleaseDir}\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#ReleaseDir}\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion
Source: "{#ReleaseDir}\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion
Source: "{#ReleaseDir}\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\..\Localization\localization_en.qm"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\..\Localization\localization_ru.qm"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#ReleaseDir}\{#AppExeName}"; DestDir: "{app}"; Flags: ignoreversion                
Source: "{#SourcePath}\..\Docs\stations.db"; DestDir: "{app}"; Flags: onlyifdoesntexist uninsneveruninstall        
Source: "{#SourcePath}\..\Docs\stations.db"; DestName: "stations_empty.db"; DestDir: "{app}"; Flags: ignoreversion uninsneveruninstall                
Source: "{#SourcePath}\..\dbupdate\*"; DestDir: "{app}\dbupdate"; Flags: onlyifdoesntexist recursesubdirs             

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\{#AppName}"; Filename: "{app}\{#AppExeName}"
Name: "{commondesktop}\{#AppName}"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[Run]
; Parameters: "-l {language}" ;
Filename: "{app}\{#AppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(AppName, '&', '&&')}}"; Parameters: "-l ru"; Flags: runascurrentuser nowait postinstall skipifsilent
