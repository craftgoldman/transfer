#if !defined(_TRNINTPG_H)
#define _TRNINTPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrnIntPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnIntPage dialog

class CTrnIntPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrnIntPage)

// Construction
public:
	CTrnIntPage();
	~CTrnIntPage();

// Dialog Data
	//{{AFX_DATA(CTrnIntPage)
	enum { IDD = IDD_TRN_INTERFACE };
	BOOL	m_SplashW;
	BOOL	m_Minimize;
	BOOL	m_SysTray;
	BOOL	m_SavePos;
	BOOL	m_SaveMDI;
	BOOL	m_MaxiMDI;
	//}}AFX_DATA
	CString	m_LangID;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnIntPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrnIntPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokLangid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNINTPG_H)
