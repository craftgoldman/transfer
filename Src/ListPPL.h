#if !defined(_LISTPPL_H)
#define _LISTPPL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListPPL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListPPLView view

class CListPPLView : public CListView
{
protected:
	CListPPLView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CListPPLView)

// Attributes
public:

protected:
	CString	m_strTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListPPLView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	LRESULT OnFormQuerySave(WPARAM wParam, LPARAM lParam);
	LRESULT OnFormUpdateData(WPARAM wParam, LPARAM lParam);
	void UpdateCardData(CChildFrame* pWnd);
	virtual ~CListPPLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CListPPLView)
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDataAdd();
	afx_msg void OnDataRetrieve();
	afx_msg void OnDataSave();
	afx_msg void OnDataEdit();
	afx_msg void OnDataDelete();
	afx_msg void OnDataFirst();
	afx_msg void OnDataPrev();
	afx_msg void OnDataNext();
	afx_msg void OnDataLast();
	afx_msg void OnDataClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HWND		m_hChildFrm;
	CImageList	m_imgSmall;
	CImageList	m_imgNormal;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_LISTPPL_H)
