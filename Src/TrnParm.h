#if !defined(_TRNPARM_H)
#define _TRNPARM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sxbutton.h"
#include "GridCtrl.h"

// TrnParm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnParmDlg dialog

class CTrnParmDlg : public CDialog
{
// Construction
public:
	CString m_strParm;
	CTrnParmDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTrnParmDlg)
	enum { IDD = IDD_TRN_PARM };
	CSXButton	m_bOK;
	CSXButton	m_bCANCEL;
	CSXButton	m_bADD;
	CSXButton	m_bINSERT;
	CSXButton	m_bDELETE;
	//}}AFX_DATA
	CGridCtrl m_Grid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrnParmDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL FillParmList();
	BOOL CheckCellData(int iRow, int iCol, CString data, CString& strErr);
	BOOL CheckRowData();

	// Generated message map functions
	//{{AFX_MSG(CTrnParmDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	UINT m_nColCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNPARM_H)
