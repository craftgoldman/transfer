
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


#include "stdafx.h"
#include "winuser2.h"
#include "BitmapMenu.h"



//
// Constructor and destructor.
//

template <class T_FrameWnd> BitmapMenu<T_FrameWnd>::BitmapMenu()
: T_FrameWnd()
, m_textSpace(3)
{
	ASSERT(m_toolBars.empty());
}

template <class T_FrameWnd> BitmapMenu<T_FrameWnd>::~BitmapMenu()
{
	m_toolBars.clear();
}



//
// When a menu is about to popup, insert callbacks so that the menu items 
// can be sized correctly and the bitmaps can be drawn.
//

template <class T_FrameWnd> 
void BitmapMenu<T_FrameWnd>::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, 
											 BOOL bSysMenu)
{
	// Call the base class function.
	T_FrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// If there are no tool bars, then do not add callbacks to the menu.
	if (m_toolBars.empty()) return;

	// Callbacks will be generated for every menu item so that the 
	// menus can be sized correctly and the bitmaps can be drawn.
	MENUITEMINFO_WIN50 menuItemInfo;
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask  = MIIM_BITMAP;
	menuItemInfo.hbmpItem = HBMMENU_CALLBACK;
	for (UINT i=0; i<pPopupMenu->GetMenuItemCount(); ++i)
		SetMenuItemInfo(pPopupMenu->m_hMenu, i, TRUE, &menuItemInfo);

	// Draw images in the same space occupied by checkmarks.
	MENUINFO_WIN50 menuInfo;
	menuInfo.cbSize = sizeof(menuInfo);
	menuInfo.fMask = MIM_STYLE;
	GetMenuInfoWin50(pPopupMenu->m_hMenu, &menuInfo);
    menuInfo.dwStyle |= MNS_CHECKORBMP;
	SetMenuInfoWin50(pPopupMenu->m_hMenu, &menuInfo);

	return;
}



//
// Set the size of a menu item. The size must be large enough to accommodate 
// the tool bar bitmap.
//

template <class T_FrameWnd> 
void BitmapMenu<T_FrameWnd>::OnMeasureItem(int nIDCtl, 
									LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// Call the base class function.
	if (lpMeasureItemStruct->CtlType != ODT_MENU)
	{
		T_FrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
		return;
	}

	ASSERT(!m_toolBars.empty());

	// Find the size of a tool bar button.
	IMAGEINFO imageInfo;
	CToolBar *toolBar = *(m_toolBars.begin());
	CImageList *imageList = toolBar->GetToolBarCtrl().GetImageList();
	imageList->GetImageInfo(0, &imageInfo);
	CSize size = CRect(imageInfo.rcImage).Size();
	
	// Add at least a one pixel border around the bitmap so there is room 
	// to draw a raised button. Also, put some extra horizontal space 
	// between the bitmap and the menu text.
	lpMeasureItemStruct->itemHeight = 
		__max(size.cy+2, lpMeasureItemStruct->itemHeight+3);
	double ratio = (size.cx+2) / (double) (size.cy+2);
	lpMeasureItemStruct->itemWidth = 
		ratio * lpMeasureItemStruct->itemHeight + m_textSpace;

	return;
}



//
// Draw a tool bar bitmap on the menu. The bitmap will be drawn gray if the
// corresponding button is currently disabled.
//

template <class T_FrameWnd> 
void BitmapMenu<T_FrameWnd>::OnDrawItem(int nIDCtl, 
										LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Call the base class function.
	T_FrameWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);

	// The remainder of this function pertains to menus.
	if (lpDrawItemStruct->CtlType != ODT_MENU) return;

	// Get information about the menu item to be drawn.
	bool isDisabled = ((lpDrawItemStruct->itemState & ODS_GRAYED) != 0);
	bool isSelected = ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0);
	CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);

	// Get the image that corresponds to this menu item.
	int imageID;
	CImageList *imageList;
	GetImage(lpDrawItemStruct->itemID, imageID, imageList);
	if (imageID == -1) return;

	// Get the size and position of the button and bitmap.
	IMAGEINFO imageInfo;
	imageList->GetImageInfo(imageID, &imageInfo);
	CSize bitmapSize = CRect(imageInfo.rcImage).Size();
	CSize buttonSize(lpDrawItemStruct->rcItem.right - m_textSpace - 2,
					 lpDrawItemStruct->rcItem.bottom);
	CPoint bitmapPosition((buttonSize.cx - bitmapSize.cx) / 2 + 1,
						  (buttonSize.cy - bitmapSize.cy) / 2 + 1);
	ASSERT(bitmapPosition.x >= 0 && bitmapPosition.y >= 0);

	// The item is disabled, so draw a monochrome bitmap.
	if (isDisabled)	
	{
		DrawDisabled(imageList, dc, imageID, bitmapPosition, bitmapSize);
		return;
	}

	// If the item is selected, draw a raised button.
	if (isSelected) DrawButton(imageList, dc, buttonSize);

	// Draw the bitmap on the menu.
	imageList->Draw(dc, imageID, bitmapPosition, ILD_TRANSPARENT);

	return;
}



//
// Given a menu ID, get the corresponding image. This routine returns
// an image list and an image ID which is the index of the image within 
// the image list. If there is no tool bar button corresponding to the
// given menu ID, then the returned image ID is -1.
//

template <class T_FrameWnd> 
void BitmapMenu<T_FrameWnd>::GetImage(UINT menuID, int &imageID, 
									  CImageList *&imageList)
{
	// Find the tool bar and button that correspond to the menu ID.
	int buttonID = imageID = -1;
	std::set<CToolBar*>::iterator toolBar;
	for (toolBar=m_toolBars.begin(); toolBar!=m_toolBars.end(); ++toolBar)
	{
		buttonID = (*toolBar)->CommandToIndex(menuID);
		if (buttonID != -1) break;
	}
	if (buttonID == -1) return;

	// Find the corresponding image ID.
	UINT commandID, style;
	(*toolBar)->GetButtonInfo(buttonID, commandID, style, imageID);

	// Find the corresponding image list.
	imageList = (*toolBar)->GetToolBarCtrl().GetImageList();
	ASSERT(imageList->GetSafeHandle() != NULL);

	return;
}



//
// Draw a raised button on the menu. This is done to indicate that the menu
// item is currently selected.
//

template <class T_FrameWnd> 
void BitmapMenu<T_FrameWnd>::DrawButton(CImageList *imageList, CDC *dc, 
										CSize &buttonSize)
{
	ASSERT(!m_toolBars.empty());

	// Clear the area where the button will be drawn.
	CBrush brush(GetSysColor(COLOR_MENU));
	CBrush *oldBrush = dc->SelectObject(&brush);
	BOOL result = dc->PatBlt(0, 0, buttonSize.cx+2, buttonSize.cy+1, PATCOPY);
	dc->SelectObject(oldBrush);

	// Draw the lighted side of the button.
	CPen lightPen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
	CPen *oldPen = dc->SelectObject(&lightPen);
	dc->MoveTo(1, buttonSize.cy);
	dc->LineTo(1, 0);
	dc->LineTo(buttonSize.cx, 0);
	dc->SelectObject(oldPen);

	// Draw the darkened side of the button.
	CPen darkPen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	oldPen = dc->SelectObject(&darkPen);
	dc->LineTo(buttonSize.cx, buttonSize.cy);
	dc->LineTo(0, buttonSize.cy);
	dc->SelectObject(oldPen);

	return;
}

//
// Draw an image in a disabled state by converting a color image to a 
// monochrome image. Pixels in the color image that are part of the 
// background or have the same color as the button highlight color are
// mapped to the monochrome background. All other pixels are mapped to
// the monochrome foreground.
//

template <class T_FrameWnd> 
void BitmapMenu<T_FrameWnd>::DrawDisabled(CImageList *imageList, CDC *dc, 
										  int imageID, CPoint &position, 
										  CSize &size)
{
	ASSERT(!m_toolBars.empty());

	// Create a color bitmap.
	CWindowDC windowDC(0);
	CDC colorDC;
	colorDC.CreateCompatibleDC(0);
	CBitmap colorBmp;
	colorBmp.CreateCompatibleBitmap(&windowDC, size.cx, size.cy);
	CBitmap *oldColorBmp = colorDC.SelectObject(&colorBmp);

	// Create a monochrome bitmap.
	CDC monoDC;
	monoDC.CreateCompatibleDC(0);
	CBitmap monoBmp;
	monoBmp.CreateCompatibleBitmap(&monoDC, size.cx, size.cy);
	CBitmap *oldMonoBmp = monoDC.SelectObject(&monoBmp);

	// Copy the toolbar button to the color bitmap, make all transparent 
	// areas the same color as the button highlight color.
	imageList->DrawIndirect(&colorDC, imageID, CPoint(0,0), 
							size, CPoint(0,0), ILD_NORMAL, 
							SRCCOPY, GetSysColor(COLOR_BTNHIGHLIGHT));

	// Copy the color bitmap into the monochrome bitmap. Pixels that 
	// have the button highlight color are mapped to the background.
	colorDC.SetBkColor(GetSysColor(COLOR_BTNHIGHLIGHT));
	monoDC.BitBlt(0, 0, size.cx, size.cy, &colorDC, 0, 0, SRCCOPY);

	// Draw the monochrome bitmap onto the menu.
	dc->BitBlt(position.x, position.y, size.cx, size.cy, 
			   &monoDC, 0, 0, SRCCOPY);

	// Delete the color DC and bitmap.
	colorDC.SelectObject(oldColorBmp);
	colorDC.DeleteDC();
	colorBmp.DeleteObject();

	// Delete the monochrome DC and bitmap.
	monoDC.SelectObject(oldMonoBmp);
	monoDC.DeleteDC();
	monoBmp.DeleteObject();

	return;
}



///////////////////////////////////////////////////////////////////////////////
//
// Generate code for the two most common frame windows through explicit 
// instantiation. 
//
///////////////////////////////////////////////////////////////////////////////

template BitmapMenu<CFrameWnd>;
template BitmapMenu<CMDIFrameWnd>;
