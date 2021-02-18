#if !defined(_CARDPPL_H)
#define _CARDPPL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResizableFormView.h"
#include "Sxbutton.h"

// CardPPL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardPPLView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCardPPLView : public CResizableFormView
{
protected:
	CCardPPLView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCardPPLView)

// Form Data
public:
	//{{AFX_DATA(CCardPPLView)
	enum { IDD = IDD_CARDPPL_FORM };
	CComboBox	m_cbSRC;
	CComboBox	m_cbDST;
	CEdit		m_EditFile;
	CEdit		m_EditName;
	CListCtrl	m_PPLList;
	CSXButton	m_bOK;
	CSXButton	m_bSAVE;
	CSXButton	m_bCANCEL;
	CSXButton	m_bADD;
	CSXButton	m_bINSERT;
	CSXButton	m_bEDIT;
	CSXButton	m_bDELETE;
	CString		m_PPLFile;
	CString		m_PPLName;
	int			m_PPLKind;
	CString		m_PPLParm;
	CString		m_PPLSrc;
	CString		m_PPLSrcUID;
	CString		m_PPLSrcPWD;
	CString		m_PPLDst;
	CString		m_PPLDstUID;
	CString		m_PPLDstPWD;
	//}}AFX_DATA

// Attributes
public:

protected:
	INT			m_PPLNum;
	INT			m_nState;
	BOOL		m_bListFocus;
	BOOL		m_bHeadChanged;
	BOOL		m_bFileChanged;
	CString		m_strTitle;
	PPL_INFO	m_PPLInfo;
	CSize		m_OrigSize;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardPPLView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LRESULT OnFormGetState(WPARAM wParam, LPARAM lParam);
	LRESULT OnFormSetState(WPARAM wParam, LPARAM lParam);
	LRESULT OnFormUpdateData(WPARAM wParam, LPARAM lParam);
	LRESULT OnFormQuerySave(WPARAM wParam, LPARAM lParam);
	BOOL QuerySave(BOOL bQuestion=TRUE);
	void MarkTitle();
	void ClearPPLInfo();
	BOOL CheckPPLFile();
	BOOL InsertTRN(int iItem, CString sName, int iSchema);
	BOOL ReadPPLInfo();
	BOOL WritePPLInfo();
	void OnDataInsert();
	virtual ~CCardPPLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCardPPLView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillfocusPplFile();
	afx_msg void OnSetfocusPplList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusPplList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickPplList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkPplList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnOk();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnCancel();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnInsert();
	afx_msg void OnBtnEdit();
	afx_msg void OnBtnDelete();
	afx_msg void OnEditFile();
	afx_msg void OnEditParm();
	afx_msg void OnHeadChanged();
	afx_msg void OnFileChanged();
	afx_msg void OnDataSave();
	afx_msg void OnDataAdd();
	afx_msg void OnDataRetrieve();
	afx_msg void OnDataEdit();
	afx_msg void OnDataDelete();
	afx_msg void OnDataClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_CARDPPL_H)
