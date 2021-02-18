#if !defined(_TRNOPTPG_H)
#define _TRNOPTPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrnOptPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnOptPage dialog

class CTrnOptPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrnOptPage)

// Construction
public:
	CTrnOptPage();
	~CTrnOptPage();

// Dialog Data
	//{{AFX_DATA(CTrnOptPage)
	enum { IDD = IDD_TRN_OPTIONS };
	CString	m_DirPpl;
	CString	m_DirLog;
	CString	m_DirMsg;
	CString	m_FileLog;
	CString	m_FileMsg;
	CString	m_PplExp;
	CString	m_PplImp;
	BOOL	m_UseExp;
	BOOL	m_UseImp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnOptPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrnOptPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokPplexp();
	afx_msg void OnSelendokPplimp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNOPTPG_H)
