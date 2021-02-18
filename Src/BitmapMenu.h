
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 by Warren Gardner (warren555@hotmail.com).
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments. You must obtain the author's consent before 
// you can include this code in a software library. If this source code is 
// used in a commercial application please send me an e-mail so I can check 
// out your product.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// You can send me bug reports, bug fixes, enhancements, etc. or post them 
// to the codeproject: http://www.codeproject.com.
//
///////////////////////////////////////////////////////////////////////////////


#if !defined __BITMAPMENU_H__
#define __BITMAPMENU_H__


// Ignore long label warning when using the STL
#pragma warning( disable : 4786)

#include <set>


///////////////////////////////////////////////////////////////////////////////
//
// Class: BitmapMenu<T_FrameWnd>
//
// This class draws tool bar buttons onto menus.
//
///////////////////////////////////////////////////////////////////////////////


template <class T_FrameWnd>
class BitmapMenu : public T_FrameWnd
{
protected:
	BitmapMenu();
	virtual ~BitmapMenu();

	// Add and remove menu bitmaps by adding and removing tool bars.
	void AddToolBar(CToolBar *toolBar) { m_toolBars.insert(toolBar); }
	void RemoveToolBar(CToolBar *toolBar) { m_toolBars.erase(toolBar); }

	// Menu message handlers.
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, 
								 BOOL bSysMenu);
	afx_msg void OnMeasureItem(int nIDCtl, 
							   LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

private:
	// Given a menu item ID, get the corresponding image list and image ID.
	void GetImage(UINT menuID, int &imageID, CImageList *&imageList);

	// Draw a raised button.
	void DrawButton(CImageList *imageList, CDC *dc, CSize &buttonSize);

	// Draw a disabled menu item by converting a color bitmap to monochrome.
	void DrawDisabled(CImageList *imageList, CDC *dc, int imageID, 
					  CPoint &position, CSize &size);

	// Menu bitmaps are retrieved from this set of tool bars.
	std::set<CToolBar*> m_toolBars;

	// Number of pixels between the start of the text area 
	// and the first character of menu text.
	const UINT m_textSpace;
};

#endif // __BITMAPMENU_H__
