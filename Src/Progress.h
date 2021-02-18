#if !defined(_PROGRESS_H)
#define _PROGRESS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProgBar.h"

// Progress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
	void SetBreakEnable(BOOL bEnable);
	void CheckState();
	void SetStatusText(LPCTSTR szText);
	void SetMessageText(LPCTSTR szText);
	void SetSubText(LPCTSTR szText);
	void SetRange(int nLower, int nUpper);
	void SetPos(int nPos);
	BOOL IsBreaked();
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESSDLG };
	CProgressBar m_cProgress;
	CString	m_sText;
	CString	m_sStatus;
	CButton	m_bBREAK;
	CString	m_sSubText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bBreaked;
	BOOL m_bMinimized;
	HWND m_hwndMain;
	CString m_sAppTitle;

	void SetPctTitle(HWND hwnd);
	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	afx_msg void OnClose();
	afx_msg void OnBreak();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_PROGRESS_H)
