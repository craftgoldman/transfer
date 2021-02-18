#if !defined(_TRNEXCPG_H)
#define _TRNEXCPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrnExcPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnExcPage dialog

class CTrnExcPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrnExcPage)

// Construction
public:
	CTrnExcPage();
	~CTrnExcPage();

// Dialog Data
	//{{AFX_DATA(CTrnExcPage)
	enum { IDD = IDD_TRN_EXCHANGE };
	int		m_Depart;
	int		m_ExcTime;
	int		m_IntTime;
	CString	m_FileImp;
	CString	m_FileExp;
	CString	m_FileEmp;
	CString	m_NameArc;
	CString	m_DirPut;
	CString	m_DirGet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnExcPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrnExcPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNEXCPG_H)
