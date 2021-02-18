#if !defined(_INPLACEEDIT_H)
#define _INPLACEEDIT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#include "GridCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit window
 
class CInPlaceEdit : public CMaskEdit
{
    DECLARE_DYNAMIC(CInPlaceEdit)

// Construction
public:
	CInPlaceEdit(CWnd* pParent,         // parent
                 CRect& rect,           // dimensions & location
                 DWORD dwStyle,         // window/edit style
                 UINT nID,              // control ID
                 int nRow, int nColumn, // row and column
				 UINT nFirstChar,       // first character to pass
                 CString sInitText,     // initial selection
                 GV_COLUMN& colProp);   // column properties

// Attributes
public:
 
// Operations
public:
 
// Overrides
     // ClassWizard generated virtual function overrides
     //{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
 
// Implementation
public:
     virtual ~CInPlaceEdit();
protected:
	BOOL SetEditText(CString sText);
	BOOL EndEdit(BOOL bValidate = TRUE);
 
// Generated message map functions
protected:
	LRESULT OnEndEdit(WPARAM wParam, LPARAM lParam);
    //{{AFX_MSG(CInPlaceEdit)
	afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    int     m_nRow;
    int     m_nColumn;
    CString m_sInitText;
    UINT    m_nLastChar;
    BOOL    m_bExitOnArrows;
    CRect   m_Rect;
};
 
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_INPLACEEDIT_H)
