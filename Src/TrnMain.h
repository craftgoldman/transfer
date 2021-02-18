#if !defined(_TRNMAIN_H)
#define _TRNMAIN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrnMain.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrnMainPage dialog

class CTrnMainPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrnMainPage)

// Construction
public:
	CTrnMainPage();
	~CTrnMainPage();

// Dialog Data
	//{{AFX_DATA(CTrnMainPage)
	enum { IDD = IDD_TRN_MAIN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrnMainPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrnMainPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRNMAIN_H)
