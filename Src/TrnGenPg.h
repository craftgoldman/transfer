#if !defined(_TRNGENPG_H)
#define _TRNGENPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrnGenPg.h : header file
//

#include "ResizablePage.h"

/////////////////////////////////////////////////////////////////////////////
// CTrnGenPage dialog

class CTrnGenPage : public CResizablePage
{
	DECLARE_DYNCREATE(CTrnGenPage)

// Construction
public:
	CString m_sODBCDSN;
	CTrnGenPage();
	~CTrnGenPage();

// Dialog Data
	//{{AFX_DATA(CTrnGenPage)
	enum { IDD = IDD_TRN_GENERAL };
	int		m_nSchema;
	int		m_nConvert;
	CString	m_sName;
	CString	m_sMsg;
	CString	m_sTabSQL;
	CString	m_sSubSQL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnGenPage)
	public:
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL CheckSQLSelect(CString strSQL);
	CDatabase * m_db;
	// Generated message map functions
	//{{AFX_MSG(CTrnGenPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNGENPG_H)
