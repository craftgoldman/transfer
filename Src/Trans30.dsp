# Microsoft Developer Studio Project File - Name="Trans30" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Trans30 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Trans30.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Trans30.mak" CFG="Trans30 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Trans30 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Trans30 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Trans30 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Fp"Release/Trans32.pch" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/Trans33.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ntl.lib /nologo /subsystem:windows /machine:I386 /out:"Release/Trans33.exe" /libpath:"Include\ntl"

!ELSEIF  "$(CFG)" == "Trans30 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"Debug/Trans32.pch" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/Trans33.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ntl.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/Trans33.exe" /pdbtype:sept /libpath:"Include\ntl"

!ENDIF 

# Begin Target

# Name "Trans30 - Win32 Release"
# Name "Trans30 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Base64.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CardPPL.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Crc32Static.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomizeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DataVal.cpp
# End Source File
# Begin Source File

SOURCE=.\GetPPL.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceList.cpp
# End Source File
# Begin Source File

SOURCE=.\ListPPL.cpp
# End Source File
# Begin Source File

SOURCE=.\LzwCom.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Masked.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Progress.cpp
# End Source File
# Begin Source File

SOURCE=.\PropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableFormView.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableGrip.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableMinMax.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizablePage.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizablePageEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableSheetEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableState.cpp
# End Source File
# Begin Source File

SOURCE=.\rsa.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Sxbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\time64.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolbarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\tools.cpp
# End Source File
# Begin Source File

SOURCE=.\Trans30.cpp
# End Source File
# Begin Source File

SOURCE=.\Trans30.rc
# End Source File
# Begin Source File

SOURCE=.\Transfer.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnCmdPg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnExcPg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnGenPg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnIntPg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnOptPg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnParm.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnPrcPg.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnStart.cpp
# End Source File
# Begin Source File

SOURCE=.\TrnTabPg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\Base64.h
# End Source File
# Begin Source File

SOURCE=.\BitmapMenu.h
# End Source File
# Begin Source File

SOURCE=.\CardPPL.h
# End Source File
# Begin Source File

SOURCE=.\CellRange.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\Crc32Static.h
# End Source File
# Begin Source File

SOURCE=.\CustomizeDialog.h
# End Source File
# Begin Source File

SOURCE=.\DataVal.h
# End Source File
# Begin Source File

SOURCE=.\GetPPL.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceList.h
# End Source File
# Begin Source File

SOURCE=.\ListPPL.h
# End Source File
# Begin Source File

SOURCE=.\LzwCom.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Masked.h
# End Source File
# Begin Source File

SOURCE=.\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\MsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgBar.h
# End Source File
# Begin Source File

SOURCE=.\Progress.h
# End Source File
# Begin Source File

SOURCE=.\PropDlg.h
# End Source File
# Begin Source File

SOURCE=.\ResizableDialog.h
# End Source File
# Begin Source File

SOURCE=.\ResizableFormView.h
# End Source File
# Begin Source File

SOURCE=.\ResizableGrip.h
# End Source File
# Begin Source File

SOURCE=.\ResizableLayout.h
# End Source File
# Begin Source File

SOURCE=.\ResizableMinMax.h
# End Source File
# Begin Source File

SOURCE=.\ResizablePage.h
# End Source File
# Begin Source File

SOURCE=.\ResizablePageEx.h
# End Source File
# Begin Source File

SOURCE=.\ResizableSheet.h
# End Source File
# Begin Source File

SOURCE=.\ResizableSheetEx.h
# End Source File
# Begin Source File

SOURCE=.\ResizableState.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Strings.h
# End Source File
# Begin Source File

SOURCE=.\Sxbutton.h
# End Source File
# Begin Source File

SOURCE=.\time64.h
# End Source File
# Begin Source File

SOURCE=.\ToolbarEx.h
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# Begin Source File

SOURCE=.\Trans30.h
# End Source File
# Begin Source File

SOURCE=.\Transfer.h
# End Source File
# Begin Source File

SOURCE=.\TrnCmdPg.h
# End Source File
# Begin Source File

SOURCE=.\TrnExcPg.h
# End Source File
# Begin Source File

SOURCE=.\TrnGenPg.h
# End Source File
# Begin Source File

SOURCE=.\TrnIntPg.h
# End Source File
# Begin Source File

SOURCE=.\TrnOptPg.h
# End Source File
# Begin Source File

SOURCE=.\TrnParm.h
# End Source File
# Begin Source File

SOURCE=.\TrnPrcPg.h
# End Source File
# Begin Source File

SOURCE=.\TrnStart.h
# End Source File
# Begin Source File

SOURCE=.\TrnTabPg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Add.ico
# End Source File
# Begin Source File

SOURCE=.\res\binary.bin
# End Source File
# Begin Source File

SOURCE=.\res\Cancel.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cardexp.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cardfile.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cardimp.ico
# End Source File
# Begin Source File

SOURCE=.\res\Conv.ico
# End Source File
# Begin Source File

SOURCE=.\res\conv1.ico
# End Source File
# Begin Source File

SOURCE=.\res\convs.ico
# End Source File
# Begin Source File

SOURCE=.\res\Crdfiles.ico
# End Source File
# Begin Source File

SOURCE=.\res\Data.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Delete.ico
# End Source File
# Begin Source File

SOURCE=.\res\Edit.ico
# End Source File
# Begin Source File

SOURCE=.\res\Help.ico
# End Source File
# Begin Source File

SOURCE=.\res\Insert.ico
# End Source File
# Begin Source File

SOURCE=.\res\No.ico
# End Source File
# Begin Source File

SOURCE=.\res\Ok.ico
# End Source File
# Begin Source File

SOURCE=.\res\Paddoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Save.ico
# End Source File
# Begin Source File

SOURCE=.\res\SmallPPL.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Trans.ico
# End Source File
# Begin Source File

SOURCE=.\res\Trans30.ico
# End Source File
# Begin Source File

SOURCE=.\res\Trans30.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Eartha.avi
# End Source File
# End Target
# End Project
