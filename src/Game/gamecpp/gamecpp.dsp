# Microsoft Developer Studio Project File - Name="gamecpp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GAMECPP - WIN32 FINAL RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gamecpp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gamecpp.mak" CFG="GAMECPP - WIN32 FINAL RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gamecpp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gamecpp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gamecpp - Win32 Final Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SOF/Code2/gamecpp", BXJAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gamecpp - Win32 Release"

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
# ADD CPP /nologo /W3 /Gm /Gi /GX /Zi /O2 /Ob2 /I "..\qcommon" /I "..\player" /D "NDEBUG" /D "_KEF_" /D "WIN32" /D "_WINDOWS" /D "_SOF_" /D "__GAME__" /D "_RAVEN_" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib /nologo /base:"0x50000000" /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libcmt" /out:"../release/gamex86.dll"
# SUBTRACT LINK32 /map /nodefaultlib

!ELSEIF  "$(CFG)" == "gamecpp - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /Gi /GX /ZI /Od /I "..\qcommon" /I "..\player" /D "_DEBUG" /D "_KEF_" /D "WIN32" /D "_WINDOWS" /D "_SOF_" /D "__GAME__" /D "_RAVEN_" /FR /Fp".\Debug/gamecpp.pch" /YX /Fo".\Debug/" /Fd".\Debug/" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib /nologo /subsystem:windows /dll /pdb:"../Debug/gamex86.pdb" /debug /machine:I386 /def:".\game.def" /out:"..\debug\gamex86.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gamecpp - Win32 Final Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gamecpp___Win32_Final_Release"
# PROP BASE Intermediate_Dir "gamecpp___Win32_Final_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Final"
# PROP Intermediate_Dir "Final"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Gm /Gi /GX /Zi /O2 /I "..\qcommon" /I "..\player" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_SOF_" /D "__GAME__" /FR /YX /FD /c
# ADD CPP /nologo /W3 /Gi /GX /O2 /Ob2 /I "..\qcommon" /I "..\player" /D "NDEBUG" /D "_FINAL_" /D "WIN32" /D "_WINDOWS" /D "_SOF_" /D "__GAME__" /D "_RAVEN_" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../smartheap/shlw32m.lib kernel32.lib user32.lib /nologo /subsystem:windows /dll /profile /map:"..\Release/gamex86.map" /debug /machine:I386 /out:"../release/gamex86.dll"
# ADD LINK32 kernel32.lib user32.lib /nologo /base:"0x50000000" /subsystem:windows /dll /pdb:none /map:"..\Final/gamex86.map" /machine:I386 /nodefaultlib:"libcmt" /out:"../Final/gamex86.dll"
# SUBTRACT LINK32 /profile /debug

!ENDIF 

# Begin Target

# Name "gamecpp - Win32 Release"
# Name "gamecpp - Win32 Debug"
# Name "gamecpp - Win32 Final Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\ai.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_actions.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_body.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_bodycow.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_bodydekker.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_bodydog.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_bodyhuman.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_bodynoghoul.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_decisions.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_path_pre.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_pathfinding.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_senses.cpp
# End Source File
# Begin Source File

SOURCE=.\CWeaponInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\dm.cpp
# End Source File
# Begin Source File

SOURCE=.\dm_arsenal.cpp
# End Source File
# Begin Source File

SOURCE=.\dm_assassin.cpp
# End Source File
# Begin Source File

SOURCE=.\dm_ctf.cpp
# End Source File
# Begin Source File

SOURCE=.\dm_none.cpp
# End Source File
# Begin Source File

SOURCE=.\dm_real.cpp
# End Source File
# Begin Source File

SOURCE=.\dm_standard.cpp
# End Source File
# Begin Source File

SOURCE=.\ds.cpp
# End Source File
# Begin Source File

SOURCE=.\fx_effects.cpp
# End Source File
# Begin Source File

SOURCE=.\fx_tempents.cpp
# End Source File
# Begin Source File

SOURCE=.\g_bosnia.cpp
# End Source File
# Begin Source File

SOURCE=.\g_castle.cpp
# End Source File
# Begin Source File

SOURCE=.\g_chase.cpp
# End Source File
# Begin Source File

SOURCE=.\g_cmds.cpp
# End Source File
# Begin Source File

SOURCE=.\g_combat.cpp
# End Source File
# Begin Source File

SOURCE=.\g_environ.cpp
# End Source File
# Begin Source File

SOURCE=.\g_func.cpp
# End Source File
# Begin Source File

SOURCE=.\g_generic.cpp
# End Source File
# Begin Source File

SOURCE=.\g_ghoul.cpp
# End Source File
# Begin Source File

SOURCE=.\g_iraq.cpp
# End Source File
# Begin Source File

SOURCE=.\g_items.cpp
# End Source File
# Begin Source File

SOURCE=.\g_lightmodels.cpp
# End Source File
# Begin Source File

SOURCE=.\g_main.cpp
# End Source File
# Begin Source File

SOURCE=.\g_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\g_monster.cpp
# End Source File
# Begin Source File

SOURCE=.\g_newyork.cpp
# End Source File
# Begin Source File

SOURCE=.\g_obj.cpp
# End Source File
# Begin Source File

SOURCE=.\g_phys.cpp
# End Source File
# Begin Source File

SOURCE=.\g_player.cpp
# End Source File
# Begin Source File

SOURCE=.\g_save.cpp
# End Source File
# Begin Source File

SOURCE=.\g_siberia.cpp
# End Source File
# Begin Source File

SOURCE=.\g_skilllevels.cpp
# End Source File
# Begin Source File

SOURCE=.\g_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\g_spawn.cpp
# End Source File
# Begin Source File

SOURCE=.\g_svcmds.cpp
# End Source File
# Begin Source File

SOURCE=.\g_target.cpp
# End Source File
# Begin Source File

SOURCE=.\g_tokyo.cpp
# End Source File
# Begin Source File

SOURCE=.\g_trigger.cpp
# End Source File
# Begin Source File

SOURCE=.\g_uganda.cpp
# End Source File
# Begin Source File

SOURCE=.\g_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\m_ecto.cpp
# End Source File
# Begin Source File

SOURCE=.\m_female.cpp
# End Source File
# Begin Source File

SOURCE=.\m_generic.cpp
# End Source File
# Begin Source File

SOURCE=.\m_heliactions.cpp
# End Source File
# Begin Source File

SOURCE=.\m_heliai.cpp
# End Source File
# Begin Source File

SOURCE=.\m_meso.cpp
# End Source File
# Begin Source File

SOURCE=.\m_snowcatactions.cpp
# End Source File
# Begin Source File

SOURCE=.\m_snowcatai.cpp
# End Source File
# Begin Source File

SOURCE=.\m_tankactions.cpp
# End Source File
# Begin Source File

SOURCE=.\m_tankai.cpp
# End Source File
# Begin Source File

SOURCE=..\ghoul\matrix4.cpp
# End Source File
# Begin Source File

SOURCE=.\mp_ents.cpp
# End Source File
# Begin Source File

SOURCE=.\p_body.cpp
# End Source File
# Begin Source File

SOURCE=.\p_client.cpp
# End Source File
# Begin Source File

SOURCE=.\p_hud.cpp
# End Source File
# Begin Source File

SOURCE=.\p_trail.cpp
# End Source File
# Begin Source File

SOURCE=.\p_view.cpp
# End Source File
# Begin Source File

SOURCE=.\pt_listpointer.cpp
# End Source File
# Begin Source File

SOURCE=.\q_sh_fx.cpp
# End Source File
# Begin Source File

SOURCE=.\q_shared.cpp
# End Source File
# Begin Source File

SOURCE=..\smartheap\SMRTHEAP.C

!IF  "$(CFG)" == "gamecpp - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gamecpp - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gamecpp - Win32 Final Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\test.cpp
# End Source File
# Begin Source File

SOURCE=..\ghoul\vect3.cpp
# End Source File
# Begin Source File

SOURCE=.\w_equip.cpp
# End Source File
# Begin Source File

SOURCE=.\w_fire.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\actionids.h
# End Source File
# Begin Source File

SOURCE=.\ai.h
# End Source File
# Begin Source File

SOURCE=.\ai_actions.h
# End Source File
# Begin Source File

SOURCE=.\ai_body.h
# End Source File
# Begin Source File

SOURCE=.\ai_bodycow.h
# End Source File
# Begin Source File

SOURCE=.\ai_bodydekker.h
# End Source File
# Begin Source File

SOURCE=.\ai_bodydog.h
# End Source File
# Begin Source File

SOURCE=.\ai_bodyhuman.h
# End Source File
# Begin Source File

SOURCE=.\ai_bodynoghoul.h
# End Source File
# Begin Source File

SOURCE=.\ai_decisions.h
# End Source File
# Begin Source File

SOURCE=.\ai_path_pre.h
# End Source File
# Begin Source File

SOURCE=.\ai_pathfinding.h
# End Source File
# Begin Source File

SOURCE=.\ai_private.h
# End Source File
# Begin Source File

SOURCE=.\ai_public.h
# End Source File
# Begin Source File

SOURCE=.\ai_senses.h
# End Source File
# Begin Source File

SOURCE=..\client\amd3d.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\Angles.h
# End Source File
# Begin Source File

SOURCE=.\callback.h
# End Source File
# Begin Source File

SOURCE=..\client\cdaudio.h
# End Source File
# Begin Source File

SOURCE=..\client\client.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\configstring.h
# End Source File
# Begin Source File

SOURCE=..\client\console.h
# End Source File
# Begin Source File

SOURCE=.\CWeaponInfo.h
# End Source File
# Begin Source File

SOURCE=.\dm.h
# End Source File
# Begin Source File

SOURCE=..\strings\dm_arsenal.h
# End Source File
# Begin Source File

SOURCE=..\strings\dm_assassin.h
# End Source File
# Begin Source File

SOURCE=.\dm_ctf.h
# End Source File
# Begin Source File

SOURCE=..\strings\dm_generic.h
# End Source File
# Begin Source File

SOURCE=.\dm_private.h
# End Source File
# Begin Source File

SOURCE=.\ds.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\ef_flags.h
# End Source File
# Begin Source File

SOURCE=..\client\fffx.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\fields.h
# End Source File
# Begin Source File

SOURCE=.\fx_effects.h
# End Source File
# Begin Source File

SOURCE=.\g_ghoul.h
# End Source File
# Begin Source File

SOURCE=.\g_local.h
# End Source File
# Begin Source File

SOURCE=.\g_monster.h
# End Source File
# Begin Source File

SOURCE=.\g_obj.h
# End Source File
# Begin Source File

SOURCE=.\g_skilllevels.h
# End Source File
# Begin Source File

SOURCE=.\g_spawn.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=..\strings\general.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\GSQFile.h
# End Source File
# Begin Source File

SOURCE=..\ghoul\ighoul.h
# End Source File
# Begin Source File

SOURCE=..\client\input.h
# End Source File
# Begin Source File

SOURCE=..\strings\items.h
# End Source File
# Begin Source File

SOURCE=..\client\keys.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\m_ecto.h
# End Source File
# Begin Source File

SOURCE=.\m_female.h
# End Source File
# Begin Source File

SOURCE=.\m_generic.h
# End Source File
# Begin Source File

SOURCE=.\m_heliactions.h
# End Source File
# Begin Source File

SOURCE=.\m_heliai.h
# End Source File
# Begin Source File

SOURCE=.\m_meso.h
# End Source File
# Begin Source File

SOURCE=.\m_snowcatactions.h
# End Source File
# Begin Source File

SOURCE=.\m_snowcatai.h
# End Source File
# Begin Source File

SOURCE=.\m_tankactions.h
# End Source File
# Begin Source File

SOURCE=.\m_tankai.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\mathlib.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\matrix4.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\music.h
# End Source File
# Begin Source File

SOURCE=.\p_body.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\p_heights.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\palette.h
# End Source File
# Begin Source File

SOURCE=.\pcode.h
# End Source File
# Begin Source File

SOURCE=..\player\player.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\pmove.h
# End Source File
# Begin Source File

SOURCE=.\pt_listpointer.h
# End Source File
# Begin Source File

SOURCE=.\q_sh_fx.h
# End Source File
# Begin Source File

SOURCE=.\q_sh_interface.h
# End Source File
# Begin Source File

SOURCE=.\q_shared.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\qcommon.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\qfiles.h
# End Source File
# Begin Source File

SOURCE=..\client\ref.h
# End Source File
# Begin Source File

SOURCE=..\client\screen.h
# End Source File
# Begin Source File

SOURCE=..\strings\singleplr.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\snd_common.h
# End Source File
# Begin Source File

SOURCE=..\client\sound.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\timing.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\vect3.h
# End Source File
# Begin Source File

SOURCE=..\client\vid.h
# End Source File
# Begin Source File

SOURCE=..\player\w_public.h
# End Source File
# Begin Source File

SOURCE=..\player\w_types.h
# End Source File
# Begin Source File

SOURCE=..\qcommon\w_types.h
# End Source File
# Begin Source File

SOURCE=..\player\w_utils.h
# End Source File
# Begin Source File

SOURCE=..\player\w_weapons.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\game.def

!IF  "$(CFG)" == "gamecpp - Win32 Release"

!ELSEIF  "$(CFG)" == "gamecpp - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gamecpp - Win32 Final Release"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
