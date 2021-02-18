#if !defined(_PROPDLG_H)
#define _PROPDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropDlg.h : header file
//

#include "ResizableSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CPropDlg

class CPropDlg : public CResizableSheet
{
	DECLARE_DYNAMIC(CPropDlg)

// Construction
public:
	CPropDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropDlg)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropDlg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_PROPDLG_H)
