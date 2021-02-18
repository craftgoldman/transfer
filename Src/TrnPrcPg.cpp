// TrnPrcPg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnPrcPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnPrcPage property page

IMPLEMENT_DYNCREATE(CTrnPrcPage, CPropertyPage)

CTrnPrcPage::CTrnPrcPage() : CPropertyPage(CTrnPrcPage::IDD)
{
	//{{AFX_DATA_INIT(CTrnPrcPage)
	m_DefType = FALSE;
	m_AutoExc = FALSE;
	m_DelFile = FALSE;
	m_WriteOpr= FALSE;
	m_WriteMsg= FALSE;
	m_ShowErr = FALSE;
	m_TraceOpr= FALSE;
	m_TraceCmd= FALSE;
	m_TraceSel= FALSE;
	//}}AFX_DATA_INIT
}

CTrnPrcPage::~CTrnPrcPage()
{
}

void CTrnPrcPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnPrcPage)
	DDX_Check(pDX, IDC_DEFTYPE, m_DefType);
	DDX_Check(pDX, IDC_AUTOEXC, m_AutoExc);
	DDX_Check(pDX, IDC_DELFILE, m_DelFile);
	DDX_Check(pDX, IDC_WRITEOPR,m_WriteOpr);
	DDX_Check(pDX, IDC_WRITEMSG,m_WriteMsg);
	DDX_Check(pDX, IDC_SHOWERR, m_ShowErr);
	DDX_Check(pDX, IDC_TRACEOPR,m_TraceOpr);
	DDX_Check(pDX, IDC_TRACECMD,m_TraceCmd);
	DDX_Check(pDX, IDC_TRACESEL,m_TraceSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrnPrcPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTrnPrcPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnPrcPage message handlers
