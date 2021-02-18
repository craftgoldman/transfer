
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 by Nikolay Denisov. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Please email bug reports, bug fixes, enhancements, requests and
// comments to: nick@actor.ru
//
// --------------------------------------------------------------------------
//
// Changes made by Warren Gardner:
//		- Changed file name from GlobalData.h to winuser2.h to reflect the 
//		  fact that all these definitions originally come from the Windows
//		  header file winuser.h.
//		- Removed all definitions that were not being used by the class
//		  BitmapMenu<T_FrameWnd>.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __WINUSER2_H__
#define __WINUSER2_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// Copied from the latest Platform SDK available.
// This should help if you don't have SDK installed on your PC.

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// W98 specific:

#if ( WINVER < 0x0500 )

#define MIIM_BITMAP         0x00000080
#define MIIM_FTYPE          0x00000100

#define HBMMENU_CALLBACK    ((HBITMAP) -1)

struct MENUITEMINFO_WIN50 : MENUITEMINFO
{
    HBITMAP  hbmpItem;
};

#define MIM_STYLE       0x00000010
#define MNS_CHECKORBMP  0x04000000

struct MENUINFO_WIN50
{
    DWORD   cbSize;
    DWORD   fMask;
    DWORD   dwStyle;
    UINT    cyMax;
    HBRUSH  hbrBack;
    DWORD   dwContextHelpID;
    ULONG   dwMenuData;
};

BOOL GetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi );
BOOL SetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi );

#else

#define MENUINFO_WIN50      MENUINFO
#define MENUITEMINFO_WIN50  MENUITEMINFO

#define GetMenuInfoWin50    GetMenuInfo
#define SetMenuInfoWin50    SetMenuInfo

#endif  // WINVER < 0x0500


/////////////////////////////////////////////////////////////////////////////
// Inline functions

#if ( WINVER < 0x0500 )
inline FARPROC GetProcAddress( LPCTSTR lpModuleName, LPCSTR lpProcName )
{
    HMODULE hModule = ::GetModuleHandle( lpModuleName );
    ASSERT( hModule != 0 );

    return ::GetProcAddress( hModule, lpProcName );
}

inline BOOL GetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi )
{
    typedef BOOL ( WINAPI* GETMENUINFO )( HMENU hmenu, const MENUINFO_WIN50* lpcmi );
    GETMENUINFO pfGetMenuInfo = ( GETMENUINFO )::GetProcAddress( _T("USER32"), "GetMenuInfo" );
    ASSERT( pfGetMenuInfo != 0 );

    return pfGetMenuInfo( hmenu, lpcmi );
}

inline BOOL SetMenuInfoWin50( HMENU hmenu, const MENUINFO_WIN50* lpcmi )
{
    typedef BOOL ( WINAPI* SETMENUINFO )( HMENU hmenu, const MENUINFO_WIN50* lpcmi );
    SETMENUINFO pfSetMenuInfo = ( SETMENUINFO )::GetProcAddress( _T("USER32"), "SetMenuInfo" );
    ASSERT( pfSetMenuInfo != 0 );

    return pfSetMenuInfo( hmenu, lpcmi );
}

#endif  // WINVER < 0x0500

/////////////////////////////////////////////////////////////////////////////
#endif  // __WINUSER2_H__
