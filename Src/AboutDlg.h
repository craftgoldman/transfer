#if !defined(AFX_ABOUTDLG_H__1DC7BE44_5F30_48A9_B83C_ED20DD91CD1D__INCLUDED_)
#define AFX_ABOUTDLG_H__1DC7BE44_5F30_48A9_B83C_ED20DD91CD1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDlg.h : header file
//

#include "HyperLink.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

class CAboutDlg : public CDialog
{
// Construction
public:
	CAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	BOOL			m_ShowDlg;
	CHyperLink		m_TrnWebLink;
	CAnimateCtrl	m_GlbAnimate;
	CString			m_Registration;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDLG_H__1DC7BE44_5F30_48A9_B83C_ED20DD91CD1D__INCLUDED_)
