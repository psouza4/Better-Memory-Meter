; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=Better Memory Meter
AppVerName=Better Memory Meter version 2.00
AppPublisher=Peter Souza IV
AppPublisherURL=http://www.PeterSouza.com/
AppSupportURL=http://www.PeterSouza.com/
AppUpdatesURL=http://www.PeterSouza.com/software/BetterMemoryMeter/
DefaultDirName={pf}\Peter Souza IV\Better Memory Meter
DefaultGroupName=Better Memory Meter
OutputBaseFilename=BMMSetup
OutputDir=!  Compiled
SetupIconFile=ico224.ico
Compression=lzma/ultra
SolidCompression=yes
UninstallDisplayName=Better Memory Meter version 2.00
VersionInfoCompany=Peter Souza IV
VersionInfoCopyright=2003-2010 Peter Souza IV
VersionInfoDescription=Better Memory Meter
VersionInfoTextVersion=2.00.10510.576
VersionInfoVersion=2.00.10510.576
MinVersion=4.0,4.0
;WizardImageFile=side.bmp
WizardSmallImageFile=corner.bmp
;InfoBeforeFile=info_before.rtf
DiskSpanning=no
DiskSliceSize=300000


[Languages]
Name: english; MessagesFile: compiler:Default.isl; LicenseFile: license.txt

[Messages]
BeveledLabel=Better Memory Meter

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}

[Files]
; Better Memory Meter:
Source: ..\Release\Better Memory Meter.exe; DestDir: {app}; Flags: ignoreversion

; Icon resources:
Source: uninstall.ico; DestDir: {app}; Flags: ignoreversion


[Icons]
Name: {group}\Better Memory Meter; Filename: {app}\Better Memory Meter.exe
Name: {group}\Visit my website; Filename: http://www.PeterSouza.com/
Name: {group}\{cm:UninstallProgram,Better Memory Meter}; Filename: {uninstallexe}; IconFilename: {app}\uninstall.ico
Name: {commondesktop}\Better Memory Meter; Filename: {app}\Better Memory Meter.exe; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Better Memory Meter; Filename: {app}\Better Memory Meter.exe; Tasks: quicklaunchicon

[Run]
Filename: {app}\Better Memory Meter.exe; Description: {cm:LaunchProgram,Better Memory Meter}; Flags: nowait postinstall skipifsilent
