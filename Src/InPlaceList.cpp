/////////////////////////////////////////////////////////////////////////////
// InPlaceList.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceList.h"
#include "GridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboEdit

CComboEdit::CComboEdit()
{
}

CComboEdit::~CComboEdit()
{
}

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CComboEdit::PreTranslateMessage(MSG* pMsg) 
{
	// Make sure that the keystrokes continue to the appropriate handlers
	if (pMsg->message == WM_KEYDOWN ||
		pMsg->message == WM_CHAR)
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}	

	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;

	return CEdit::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CComboEdit, CEdit)
	//{{AFX_MSG_MAP(CComboEdit)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboEdit message handlers

void CComboEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
    CInPlaceList* pOwner = (CInPlaceList*) GetOwner();  // This MUST be a CInPlaceList
    if (pOwner)
        pOwner->EndEdit();	
}

void CComboEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
		 nChar == VK_DOWN  || nChar == VK_UP   ||
		 nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(GetKeyState(VK_CONTROL) < 0 && GetDlgCtrlID() == IDC_COMBOEDIT))
    {
        CWnd* pOwner = GetOwner();
        if (pOwner)
            pOwner->SendMessage(WM_KEYDOWN, nChar, nRepCnt+ (((DWORD)nFlags)<<16));
        return;
    }

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CComboEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		CWnd* pOwner = GetOwner();
		if (pOwner)
			pOwner->PostMessage(WM_CHAR, nChar, nRepCnt + (((DWORD)nFlags)<<16));
		return;
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

IMPLEMENT_DYNAMIC(CInPlaceList, CComboBox)

/////////////////////////////////////////////////////////////////////////////
// CInPlaceList

CInPlaceList::CInPlaceList(CWnd* pParent, CRect& rect, DWORD dwStyle, 
						   UINT nID, int nRow, int nColumn, 
						   UINT nFirstChar, CString sInitText, 
						   CString values)
{
	m_nNumLines		= 4;
	m_sInitText		= sInitText;
 	m_nRow			= nRow;
 	m_nCol			= nColumn;
 	m_nLastChar		= 0; 
	m_bExitOnArrows = FALSE;

	// Create the combobox
 	DWORD dwComboStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|
 					     CBS_AUTOHSCROLL | dwStyle;
	int nHeight = rect.Height();
	rect.bottom = rect.bottom + m_nNumLines*nHeight + ::GetSystemMetrics(SM_CYHSCROLL);
	if (!Create(dwComboStyle, rect, pParent, nID)) return;

	// Add the strings
	CString	text,val,key;
	int		i,n,m,nLen = values.GetLength();
	for (n = 0, m = -1; n < nLen; n = m + 1) {
		if ((m = values.Find("\n", n)) < 0)
			m = nLen + 1;
		text = values.Mid(n, m - n);
		if ((i = text.Find("\t")) >= 0)
		{
			val = text.Left(i);
			key = text.Mid(i + 1);
		}
		else {
			val = text;
			key = text;
		}
		// Добавление строки
		AddString(val);
		val.MakeLower();
		m_mapKey.SetAt(val, key);
	}

	SetFont(pParent->GetFont());

	// Set the initial text
	if (SelectString(-1, m_sInitText) == CB_ERR)
		SetWindowText(m_sInitText);

    // Subclass the combobox edit control if style includes CBS_DROPDOWN
    if ((dwStyle & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST)
    {
        m_edit.SubclassDlgItem(IDC_COMBOEDIT, this);
 	    SetFocus();
        switch (nFirstChar)
        {
            case VK_LBUTTON:
            case VK_RETURN:   m_edit.SetSel((int)_tcslen(m_sInitText), -1); return;
            case VK_BACK:     m_edit.SetSel((int)_tcslen(m_sInitText), -1); break;
	        case VK_TAB:
            case VK_DOWN:
            case VK_UP:
            case VK_RIGHT:
            case VK_LEFT:
            case VK_NEXT:
            case VK_PRIOR:
            case VK_HOME:
			case VK_SPACE:
            case VK_END:      m_edit.SetSel(0,-1); return;
            default:          m_edit.SetSel(0,-1);
        }
        SendMessage(WM_CHAR, nFirstChar);
    }
    else
 	    SetFocus();
}

CInPlaceList::~CInPlaceList()
{
}

int CInPlaceList::EndEdit(BOOL bValidate)
{
	// Get text value
    CString text;
	GetWindowText(text);
    if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
    {
		text.MakeLower();
		if (!m_mapKey.Lookup(text, text))
			GetWindowText(text);
	}

    // Send Notification to parent
    GV_DISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetSafeHwnd();
    dispinfo.hdr.idFrom   = GetDlgCtrlID();
 
    dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
    dispinfo.item.row     = m_nRow;
    dispinfo.item.col     = m_nCol;
    dispinfo.item.szText  = text;
    dispinfo.item.lParam  = (LPARAM) m_nLastChar; 
 
    CWnd* pOwner = GetOwner();
    if (IsWindow(pOwner->GetSafeHwnd()))
	{
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

void CInPlaceList::PostNcDestroy() 
{
	CComboBox::PostNcDestroy();

	delete this;
}

BEGIN_MESSAGE_MAP(CInPlaceList, CComboBox)
    ON_MESSAGE(GVM_END_EDIT,OnEndEdit)
	//{{AFX_MSG_MAP(CInPlaceList)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInPlaceList message handlers

LRESULT CInPlaceList::OnEndEdit(WPARAM wParam, LPARAM lParam)
{
	if (wParam > 0)	return EndEdit(TRUE);
	else			return EndEdit(FALSE);
}

UINT CInPlaceList::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;
}

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == TRUE
void CInPlaceList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CGridCtrl*	pGrid = (CGridCtrl* )GetParent();

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

	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Need to keep a lookout for Tabs, Esc and Returns.

void CInPlaceList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CGridCtrl*	pGrid = (CGridCtrl* )GetParent();

	if (nChar == VK_ESCAPE)
		SetWindowText(m_sInitText);	// restore previous text

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
	
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CInPlaceList::OnSelendok() 
{
    CWnd* pOwner = GetOwner();
    if (!IsWindow(pOwner->GetSafeHwnd()))
		return;

	// Get text value
    CString text;
	GetWindowText(text);
    if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
    {
		text.MakeLower();
		if (!m_mapKey.Lookup(text, text))
			GetWindowText(text);
	}
	// Send Notification to parent
    GV_DISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetSafeHwnd();
    dispinfo.hdr.idFrom   = GetDlgCtrlID();
    dispinfo.hdr.code     = GVN_EDITSELENDOK;

    dispinfo.item.mask    = LVIF_TEXT;
    dispinfo.item.row     = m_nRow;
    dispinfo.item.col     = m_nCol;
    dispinfo.item.szText  = text;
    dispinfo.item.lParam  = (LPARAM)GetCurSel();

    pOwner->SendMessage(WM_NOTIFY, 
		GetDlgCtrlID(), (LPARAM)&dispinfo );
}
