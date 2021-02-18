#if !defined(_TRNTABPG_H)
#define _TRNTABPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResizablePage.h"
#include "GridCtrl.h"

// TrnTabPg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnTabPage dialog

class CTrnTabPage : public CResizablePage
{
	DECLARE_DYNCREATE(CTrnTabPage)

// Construction
public:
	CString m_sColumns;
	CString m_sODBCDSN;
	CTrnTabPage(UINT nIDCaption = 0);
	~CTrnTabPage();

// Dialog Data
	//{{AFX_DATA(CTrnTabPage)
	enum { IDD = IDD_TRN_TABLEDEST };
	CComboBox	m_cbTable;
	CString		m_sOwner;
	CString		m_sTable;
	int			m_nProcess;
	CString		m_sSQLCond;
	//}}AFX_DATA
	CGridCtrl m_Grid;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnTabPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);
	BOOL FillColumnsList();
	BOOL CheckDataType(int iRow, CString sType);
	BOOL CheckCellData(int iRow, int iCol, CString data, CString &strErr);
	BOOL CheckRowData();
	// Generated message map functions
	//{{AFX_MSG(CTrnTabPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	afx_msg void OnDropdownDstTable();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT		m_nColCount;
	CDatabase*	m_db;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNTABPG_H)
