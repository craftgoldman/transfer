#if !defined(_PROGBAR_H)
#define _PROGBAR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressBar window

class CProgressBar : public CProgressCtrl
{
// Construction
public:
	CProgressBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void OnDraw(CDC* pDC);
	virtual ~CProgressBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgressBar)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	COLORREF cbFore;
	COLORREF cbBack;
	COLORREF cbNTxt;
	COLORREF cbHTxt;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_PROGBAR_H)
