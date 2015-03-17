# Microsoft Developer Studio Project File - Name="wingraph" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wingraph - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wingraph.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wingraph.mak" CFG="wingraph - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wingraph - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wingraph - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wingraph - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wingraph - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wingraph - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wingraph - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wingraph - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wingraph___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "wingraph___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebug"
# PROP Intermediate_Dir "UDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wingraph - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wingraph___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "wingraph___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "URelease"
# PROP Intermediate_Dir "URelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "wingraph - Win32 Release"
# Name "wingraph - Win32 Debug"
# Name "wingraph - Win32 Unicode Debug"
# Name "wingraph - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Affine.cpp
# End Source File
# Begin Source File

SOURCE=..\areafill.cpp
# End Source File
# Begin Source File

SOURCE=..\axis.cpp
# End Source File
# Begin Source File

SOURCE=..\Background.cpp
# End Source File
# Begin Source File

SOURCE=..\basicdib.cpp
# End Source File
# Begin Source File

SOURCE=..\BitmapInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\BitmapMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\Canvas.cpp
# End Source File
# Begin Source File

SOURCE=..\CheckMark.cpp
# End Source File
# Begin Source File

SOURCE=..\Color.cpp
# End Source File
# Begin Source File

SOURCE=..\curve.cpp
# End Source File
# Begin Source File

SOURCE=..\DDB.cpp
# End Source File
# Begin Source File

SOURCE=..\dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Dib.cpp
# End Source File
# Begin Source File

SOURCE=..\DIBSection.cpp
# End Source File
# Begin Source File

SOURCE=..\EditView.cpp
# End Source File
# Begin Source File

SOURCE=..\emf.cpp
# End Source File
# Begin Source File

SOURCE=..\filedialog.cpp
# End Source File
# Begin Source File

SOURCE=..\FontText.cpp
# End Source File
# Begin Source File

SOURCE=..\Framewnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\listview.cpp
# End Source File
# Begin Source File

SOURCE=..\LogWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\Lookup.cpp
# End Source File
# Begin Source File

SOURCE=..\Octree.cpp
# End Source File
# Begin Source File

SOURCE=..\outputsetup.cpp
# End Source File
# Begin Source File

SOURCE=..\pagecanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\Rop.cpp
# End Source File
# Begin Source File

SOURCE=..\ScrollCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\Status.cpp
# End Source File
# Begin Source File

SOURCE=..\Toolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\treeview.cpp
# End Source File
# Begin Source File

SOURCE=..\win.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Affine.h
# End Source File
# Begin Source File

SOURCE=..\areafill.h
# End Source File
# Begin Source File

SOURCE=..\axis.h
# End Source File
# Begin Source File

SOURCE=..\Background.h
# End Source File
# Begin Source File

SOURCE=..\basicdib.h
# End Source File
# Begin Source File

SOURCE=..\BitmapInfo.h
# End Source File
# Begin Source File

SOURCE=..\BitmapMenu.h
# End Source File
# Begin Source File

SOURCE=..\Canvas.h
# End Source File
# Begin Source File

SOURCE=..\CheckMark.h
# End Source File
# Begin Source File

SOURCE=..\Color.h
# End Source File
# Begin Source File

SOURCE=..\curve.h
# End Source File
# Begin Source File

SOURCE=..\DDB.h
# End Source File
# Begin Source File

SOURCE=..\dialog.h
# End Source File
# Begin Source File

SOURCE=..\Dib.h
# End Source File
# Begin Source File

SOURCE=..\DIBSection.h
# End Source File
# Begin Source File

SOURCE=..\EditView.h
# End Source File
# Begin Source File

SOURCE=..\emf.h
# End Source File
# Begin Source File

SOURCE=..\filedialog.h
# End Source File
# Begin Source File

SOURCE=..\fonttext.h
# End Source File
# Begin Source File

SOURCE=..\Framewnd.h
# End Source File
# Begin Source File

SOURCE=..\GDIObject.h
# End Source File
# Begin Source File

SOURCE=..\Image.h
# End Source File
# Begin Source File

SOURCE=..\listview.h
# End Source File
# Begin Source File

SOURCE=..\LogWindow.h
# End Source File
# Begin Source File

SOURCE=..\Lookup.h
# End Source File
# Begin Source File

SOURCE=..\mdichild.h
# End Source File
# Begin Source File

SOURCE=..\MVC.h
# End Source File
# Begin Source File

SOURCE=..\Octree.h
# End Source File
# Begin Source File

SOURCE=..\outputsetup.h
# End Source File
# Begin Source File

SOURCE=..\pagecanvas.h
# End Source File
# Begin Source File

SOURCE=..\Pen.h
# End Source File
# Begin Source File

SOURCE=..\Rop.h
# End Source File
# Begin Source File

SOURCE=..\ScrollCanvas.h
# End Source File
# Begin Source File

SOURCE=..\Status.h
# End Source File
# Begin Source File

SOURCE=..\Toolbar.h
# End Source File
# Begin Source File

SOURCE=..\treeview.h
# End Source File
# Begin Source File

SOURCE=..\win.h
# End Source File
# End Group
# End Target
# End Project
