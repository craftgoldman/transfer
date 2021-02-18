#if !defined(_TRNPRCPG_H)
#define _TRNPRCPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrnPrcPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnPrcPage dialog

class CTrnPrcPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrnPrcPage)

// Construction
public:
	CTrnPrcPage();
	~CTrnPrcPage();

// Dialog Data
	//{{AFX_DATA(CTrnPrcPage)
	enum { IDD = IDD_TRN_PROCESS };
	BOOL	m_DefType;
	BOOL	m_AutoExc;
	BOOL	m_DelFile;
	BOOL	m_WriteOpr;
	BOOL	m_WriteMsg;
	BOOL	m_ShowErr;
	BOOL	m_TraceOpr;
	BOOL	m_TraceCmd;
	BOOL	m_TraceSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnPrcPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrnPrcPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNPRCPG_H)
