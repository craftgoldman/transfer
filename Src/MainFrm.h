// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_MAINFRM_TRANS30_H)
#define _MAINFRM_TRANS30_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BitmapMenu.h"
#include "ToolbarEx.h"

class CMainFrame : public BitmapMenu<CMDIFrameWnd>
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:
	void ShowAboutDlg();

protected:
	BOOL CreateToolbars();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	HACCEL		m_hWndAccel;
	CStatusBar	m_wndStatusBar;
	CToolBarEx	m_wndToolBar;
	CToolBarEx	m_proToolBar;
	CMenu		m_menuTray;
	BOOL		m_bCreated;
	BOOL		m_bSkipTst;
	BOOL		m_bInstErr;
	BOOL		m_bTrialEx;
	CString		m_strSerial;
	CString		m_strInput;

// Generated message map functions
protected:
	afx_msg LRESULT OnMDIUpdateBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayNotifyIcon(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTransfer();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnProfile();
	afx_msg void OnTransOptions();
	afx_msg void OnAppAbout();
	afx_msg void OnStartExchange();
	afx_msg void OnStopExchange();
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStartExchange(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStopExchange(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransfer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransOptions(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewProfileTbr();
	afx_msg void OnUpdateProfileTbr(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusbar();
	afx_msg void OnUpdateStatusbar(CCmdUI* pCmdUI);
	afx_msg void OnCustomizeToolbar();
	afx_msg void OnCustomizeProfileTbr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_MAINFRM_TRANS30_H)
