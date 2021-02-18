/////////////////////////////////////////////////////////////////////////////
// CustomizeDialog.h: interface for the CCustomizeDialog class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CUSTOMIZEDIALOG_H__
#define __CUSTOMIZEDIALOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum TextOptions
{
    toNone          = -1,
    toTextLabels    =  0,
    toTextOnRight   =  1,
    toNoTextLabels  =  2,
};

enum IconOptions
{
    ioNone          = -1,
    ioSmallIcons    =  0,
    ioLargeIcons    =  1,
};


// Customize dialog
#define IDD_CUSTOMIZE                   0x7900

// Customize dialog controls
#define IDC_CB_TEXTOPTIONS              1000
#define IDC_CB_ICONOPTIONS              1001


class CToolBarEx;

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDialog dialog

class CCustomizeDialog : public CWnd
{
    DECLARE_DYNAMIC( CCustomizeDialog );

// Construction
public:
    CCustomizeDialog( CToolBarEx* pToolBar,
                      TextOptions eTextOptions,
                      IconOptions eIconOptions );

// Operations
public:
    void SetTextOptions( TextOptions eTextOptions);
    void SetIconOptions( IconOptions eIconOptions);
    TextOptions  GetTextOptions() { return m_eTextOptions; }
    IconOptions GetIconOptions() { return m_eIconOptions; }

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCustomizeDialog)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Implementation data
protected:
    CToolBarEx*     m_pToolBar;

// Generated message map functions
protected:
	CComboBox m_wndIconOptions;
	CComboBox m_wndTextOptions;
	CStatic m_wndIconOptionsText;
	CStatic m_wndTextOptionsText;
    TextOptions m_eTextOptions;
    IconOptions m_eIconOptions;


    //{{AFX_MSG(CCustomizeDialog)
	//}}AFX_MSG
    afx_msg void OnTextOptions();
    afx_msg void OnIconOptions();
    LRESULT OnInitDialog( WPARAM wParam, LPARAM lParam );
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__CUSTOMIZEDIALOG_H__
