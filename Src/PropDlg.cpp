// PropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "PropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropDlg

IMPLEMENT_DYNAMIC(CPropDlg, CResizableSheet)

CPropDlg::CPropDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CResizableSheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPropDlg::CPropDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CResizableSheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPropDlg::~CPropDlg()
{
}


BEGIN_MESSAGE_MAP(CPropDlg, CResizableSheet)
	//{{AFX_MSG_MAP(CPropDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropDlg message handlers

BOOL CPropDlg::OnInitDialog() 
{
	EnableStackedTabs(FALSE);

	CResizableSheet::OnInitDialog();

	// set minimal size
	CRect rc;
	GetWindowRect(&rc);

	SetMinTrackSize(CSize(rc.Width(), rc.Height()));

	return TRUE;
}
