/////////////////////////////////////////////////////////////////////////////
// InPlaceEdit.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCHAR.h"
#include "InPlaceEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CInPlaceEdit, CMaskEdit)

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit

CInPlaceEdit::CInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, 
                           UINT nID, int nRow, int nColumn, 
                           UINT nFirstChar, CString sInitText, 
                           GV_COLUMN& colProp) 
{
    m_sInitText		= sInitText;
    m_nRow			= nRow;
    m_nColumn		= nColumn;
    m_nLastChar		= 0; 
    m_bExitOnArrows	= FALSE;
    m_Rect			= rect;	// For bizarre CE bug.

	// Установка стиля
    DWORD dwEditStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|
		ES_AUTOHSCROLL | dwStyle;
	// Создание окна
    if (!Create(dwEditStyle, rect, pParent, nID))
		return;

	// Установка маски и начального текста
    SetFont(pParent->GetFont());
	if (colProp.maskType == MASK_STRING)
	{
		SetMask(colProp.mask);
	}
	else
	{
		m_bSigned = colProp.isSigned;
		SetMask(colProp.maskType, 
		colProp.len, colProp.dec);
	}
	SetEditText(sInitText);
    SetFocus();

    // Обработка первой нажатой клавиши
    switch (nFirstChar){
        case VK_LBUTTON: 
        case VK_RETURN:   SetSel(-1, 0); return;
        case VK_BACK:     SetSel(-1, 0); break;
        case VK_TAB:
        case VK_DOWN: 
        case VK_UP:   
        case VK_RIGHT:
        case VK_LEFT:  
        case VK_NEXT:  
        case VK_PRIOR: 
        case VK_HOME:
        case VK_SPACE:
        case VK_END:      SetSel(0,-1); return;
        default:          SetSel(0,-1);
    }
    SendMessage(WM_CHAR, nFirstChar);
}

CInPlaceEdit::~CInPlaceEdit()
{
}

BEGIN_MESSAGE_MAP(CInPlaceEdit, CMaskEdit)
    ON_MESSAGE(GVM_END_EDIT,OnEndEdit)
    //{{AFX_MSG_MAP(CInPlaceEdit)
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers

LRESULT CInPlaceEdit::OnEndEdit(WPARAM wParam, LPARAM lParam)
{
	if (wParam > 0)	return EndEdit(TRUE);
	else			return EndEdit(FALSE);
}

UINT CInPlaceEdit::OnGetDlgCode() 
{
    return DLGC_WANTALLKEYS;
}

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == TRUE
void CInPlaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if ((nChar == VK_DOWN || nChar == VK_UP) &&
		(GetStyle() & ES_MULTILINE) != ES_MULTILINE)
    {
        m_nLastChar = nChar;
		EndEdit();
        return;
    }

    if ((nChar == VK_PRIOR || nChar == VK_NEXT) ||
		(nChar == VK_HOME  || nChar == VK_END   ||
		 nChar == VK_DOWN  || nChar == VK_UP    ||
		 nChar == VK_RIGHT || nChar == VK_LEFT) &&
        (m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
    {
        m_nLastChar = nChar;
		EndEdit();
        return;
    }

    CMaskEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CGridCtrl*	pGrid = (CGridCtrl* )GetParent();

	if (nChar == VK_ESCAPE)
		SetEditText(m_sInitText);	// restore previous text

	if (nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		m_nLastChar = nChar;
		if (pGrid->GetEditMode() == GVEM_NORMAL_GRID ||
			pGrid->GetEditMode() == GVEM_SINGLE_ROW)
			EndEdit();
		return;
	}
	if (nChar == VK_TAB)
	{
		m_nLastChar = nChar;
		EndEdit();
		return;
	}
    
    CMaskEdit::OnChar(nChar, nRepCnt, nFlags);
}

////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit overrides

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg) 
{
    // Catch the Alt key so we don't choke if focus is going to an owner drawn button
    if (pMsg->message == WM_SYSCHAR)
        return TRUE;
    
    return CWnd::PreTranslateMessage(pMsg);
}

// Auto delete
void CInPlaceEdit::PostNcDestroy() 
{
    CMaskEdit::PostNcDestroy();

    delete this;	
}

////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit implementation

BOOL CInPlaceEdit::SetEditText(CString sText)
{
	switch (GetMaskType())
	{
		case MASK_STRING:
			SetText(sText);
			break;
		case MASK_NUMERIC:
			SetNumber(strtod(sText ,NULL));
			break;
		case MASK_DATE:
		case MASK_TIME:
		case MASK_DATETIME:
		{
			COleDateTime dt;
			dt.ParseDateTime(sText);
			SetDateTime(dt);
			break;
		}
	}
	return TRUE;
}

int CInPlaceEdit::EndEdit(BOOL bValidate)
{
    // Send Notification to parent
    GV_DISPINFO dispinfo;
    
    dispinfo.hdr.hwndFrom = GetSafeHwnd();
    dispinfo.hdr.idFrom   = GetDlgCtrlID();
    
    dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
    dispinfo.item.row     = m_nRow;
    dispinfo.item.col     = m_nColumn;
    dispinfo.item.szText  = GetText();
    dispinfo.item.lParam  = (LPARAM) m_nLastChar; 
    
    CWnd* pOwner = GetOwner();
    if (pOwner) {
		// Посылка сообщения CHECKDATA
		if (bValidate)
		{
		    dispinfo.hdr.code = GVN_CHECKCELLDATA;
		    if (pOwner->SendMessage(WM_NOTIFY, 
				GetDlgCtrlID(), (LPARAM)&dispinfo ) != 0)
				return 1;
		}
		// Посылка сообщения ENDLABELEDIT
	    dispinfo.hdr.code = GVN_ENDLABELEDIT;
        if (pOwner->SendMessage(WM_NOTIFY, 
			GetDlgCtrlID(), (LPARAM)&dispinfo ) != 0)
			return 1;
	}
    // Close this window (PostNcDestroy will delete this)
    PostMessage(WM_CLOSE, 0, 0);

	return 0;
}
