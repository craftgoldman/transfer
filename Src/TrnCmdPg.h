#if !defined(_TRNCMDPG_H)
#define _TRNCMDPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResizablePage.h"
#include "GridCtrl.h"

// TrnCmdPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnCmdPage dialog

class CTrnCmdPage : public CResizablePage
{
	DECLARE_DYNCREATE(CTrnCmdPage)

// Construction
public:
	CString m_sTabCmd;
	CTrnCmdPage(UINT nIDCaption = 0);
	~CTrnCmdPage();

// Dialog Data
	//{{AFX_DATA(CTrnCmdPage)
	enum { IDD = IDD_TRN_TABLECMD };
	//}}AFX_DATA
	CGridCtrl m_Grid;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnCmdPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL FillCommandsList();
	BOOL CheckCondition(int iRow, CString sCond);
	BOOL CheckCellData(int iRow, int iCol, CString data, CString &strErr);
	BOOL CheckRowData();
	// Generated message map functions
	//{{AFX_MSG(CTrnCmdPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT m_nRowHeight;
	UINT m_nColCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNCMDPG_H)
