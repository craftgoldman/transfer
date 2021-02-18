#if !defined(_CHILDFRM_TRANS30_H)
#define _CHILDFRM_TRANS30_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChildFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChildFrame frame

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();           // protected constructor used by dynamic creation

protected:
	static CMenu menu;

// Attributes
public:
	CMDIChildWnd* m_pParentWnd;
	BOOL m_bOpen;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CChildFrame();

	// Generated message map functions
	afx_msg LRESULT OnFormGetState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFormSetState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFormUpdateData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFormUpdateMenu(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFormQuerySave(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_CHILDFRM_TRANS30_H)
