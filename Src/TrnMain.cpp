// TrnMain.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnMainPage property page

IMPLEMENT_DYNCREATE(CTrnMainPage, CPropertyPage)

CTrnMainPage::CTrnMainPage() : CPropertyPage(CTrnMainPage::IDD)
{
	//{{AFX_DATA_INIT(CTrnMainPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTrnMainPage::~CTrnMainPage()
{
}

void CTrnMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnMainPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrnMainPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTrnMainPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnMainPage message handlers
