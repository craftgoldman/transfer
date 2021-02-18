// ProgBar.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "ProgBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressBar

CProgressBar::CProgressBar()
{
	cbFore = ::GetSysColor(COLOR_ACTIVECAPTION);
	cbBack = ::GetSysColor(COLOR_WINDOW);
	cbNTxt = ::GetSysColor(COLOR_WINDOWTEXT);
	cbHTxt = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
}

CProgressBar::~CProgressBar()
{
}


BEGIN_MESSAGE_MAP(CProgressBar, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressBar)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressBar message handlers

void CProgressBar::OnPaint() 
{
	CPaintDC dc(this);
    CRect clipRect;

    if (dc.GetClipBox(&clipRect) != ERROR)
		OnDraw(&dc);
}

void CProgressBar::OnDraw(CDC *pDC)
{
	// Форматирование текста
	int nMinRange, nMaxRange;
	GetRange(nMinRange, nMaxRange);
	int nOffset = GetPos() - nMinRange;
	int nRange = nMaxRange - nMinRange;
	CString strText;
	strText.Format("%i %%", (nOffset*100)/nRange);
	// Вычисление высоты и ширины текста
	CSize st = pDC->GetOutputTextExtent(strText);
	CRect rct(0, 0, st.cx, st.cy);
	// Вычисление ProgressBar Rectangles
	CRect rc1,rc2;
	GetClientRect(&rc1);
	int nTextX = (rc1.Width() - rct.Width())/2;
	int nTextY = (rc1.Height() - rct.Height())/2;
	int nPBarX = (nOffset*rc1.Width())/nRange;
	rc2         = rc1;
	rc2.left   += nPBarX;
	rc1.right   = rc2.left;
	// Установка цвета
	pDC->SetBkColor(cbFore);
	pDC->SetTextColor(cbHTxt);
	// Вывод линейки ProgressBar
	pDC->ExtTextOut(nTextX, nTextY, ETO_OPAQUE|ETO_CLIPPED, 
					&rc1, strText, NULL);
	// Установка цвета
	pDC->SetBkColor(cbBack);
	pDC->SetTextColor(cbNTxt);
	// Вывод пустой части
	pDC->ExtTextOut(nTextX, nTextY, ETO_OPAQUE|ETO_CLIPPED,
					&rc2, strText, NULL);
}
