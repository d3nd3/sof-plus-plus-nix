# Microsoft Developer Studio Project File - Name="player" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=player - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "player.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "player.mak" CFG="player - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "player - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "player - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "player - Win32 Final Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SOF/Code2/player", UDWAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "player - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /Ob2 /I "..\gamecpp" /I "..\qcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x40000000" /subsystem:windows /dll /profile /debug /machine:I386
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "player - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\gamecpp" /I "..\qcommon" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../Debug/player.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "player - Win32 Final Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "player___Win32_Final_Release"
# PROP BASE Intermediate_Dir "player___Win32_Final_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Final"
# PROP Intermediate_Dir "Final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\gamecpp" /I "..\qcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /W3 /Gi /GX /O2 /Ob2 /I "..\gamecpp" /I "..\qcommon" /D "NDEBUG" /D "_FINAL_" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../smartheap/shlw32m.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /map:"..\Release/player.map" /debug /machine:I386 /out:"../Release/player.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x40000000" /subsystem:windows /dll /pdb:none /map:"..\Final/player.map" /machine:I386 /out:"../Final/player.dll"
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "player - Win32 Release"
# Name "player - Win32 Debug"
# Name "player - Win32 Final Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=..\smartheap\SMRTHEAP.C

!IF  "$(CFG)" == "player - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "player - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "player - Win32 Final Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\w_equip_attack.cpp
# End Source File
# Begin Source File

SOURCE=.\w_equip_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\w_equipment.cpp
# End Source File
# Begin Source File

SOURCE=.\w_inven.cpp
# End Source File
# Begin Source File

SOURCE=.\w_models.cpp
# End Source File
# Begin Source File

SOURCE=.\w_network.cpp
# End Source File
# Begin Source File

SOURCE=.\w_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\w_weapons.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\qcommon\Angles.h
# End Source File
# Begin Source File

SOURCE=..\gamecpp\game.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\GSQFile.h
# End Source File
# Begin Source File

SOURCE=..\ghoul\ighoul.h
# End Source File
# Begin Source File

SOURCE=..\strings\items.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\matrix4.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\palette.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\pmove.h
# End Source File
# Begin Source File

SOURCE=..\gamecpp\q_shared.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\qcommon.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\qfiles.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\snd_common.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\vect3.h
# End Source File
# Begin Source File

SOURCE=.\w_equipment.h
# End Source File
# Begin Source File

SOURCE=.\w_network.h
# End Source File
# Begin Source File

SOURCE=.\w_public.h
# End Source File
# Begin Source File

SOURCE=.\w_types.h
# End Source File
# Begin Source File

SOURCE=.\w_utils.h
# End Source File
# Begin Source File

SOURCE=.\w_weapons.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\player.def
# End Source File
# End Group
# End Target
# End Project
