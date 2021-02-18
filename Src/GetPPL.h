#if !defined(_GETPPL_H)
#define _GETPPL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sxbutton.h"

// GetPPL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetPPLDlg dialog

class CGetPPLDlg : public CDialog
{
// Construction
public:
	int m_nIndex;
	int m_nKind;
	CGetPPLDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetPPLDlg)
	enum { IDD = IDD_PPL_CHOOSE };
	CListBox	m_lbPPLList;
	CSXButton	m_bOK;
	CSXButton	m_bCANCEL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetPPLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPPLDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkListppl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_GETPPL_H)
