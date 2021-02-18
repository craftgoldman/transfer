#if !defined(_TRNSTART_H)
#define _TRNSTART_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Sxbutton.h"
#include "GridCtrl.h"
#include "Trans30.h"	// Added by ClassView

// TrnStart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnStartDlg dialog

class CTrnStartDlg : public CDialog
{
// Construction
public:
	CTrnStartDlg(CWnd* pParent = NULL);	// standard constructor
	~CTrnStartDlg();

	// Dialog Data
	//{{AFX_DATA(CTrnStartDlg)
	enum { IDD = IDD_TRN_START };
	CCheckListBox	m_lbTrnList;
	CSXButton		m_bOK;
	CSXButton		m_bCANCEL;
	CButton			m_bSELECTALL;
	CButton			m_bDESELECT;
	//}}AFX_DATA
	CGridCtrl m_Grid;

public:
	CString		m_sTitle;	// Строка заголовка
	CFldArray*	m_pArPrm;	// Массив описаний параметров
	CTrnArray*	m_pArTrn;	// Массив описаний трансфертов

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrnStartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL CheckValType(UINT nType, CString strVal);
	BOOL FillParmList();

	// Generated message map functions
	//{{AFX_MSG(CTrnStartDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectall();
	afx_msg void OnDeselect();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNSTART_H)
