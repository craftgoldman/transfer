// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_ShowDlg = FALSE;
	m_Registration = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Check(pDX, IDC_SHOWDLG, m_ShowDlg);
	DDX_Control(pDX, IDC_TRN_WEBLINK, m_TrnWebLink);
	DDX_Control(pDX, IDC_GLOBE_ANI, m_GlbAnimate);
	DDX_Text(pDX, IDC_REGISTRATION, m_Registration);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set URL link
	m_TrnWebLink.SetUnderline(CHyperLink::ulAlways);
	m_TrnWebLink.SetURL(_T("http://craftsystem.ai"));

	// Start animate control
	m_GlbAnimate.Open(IDR_GLOBE_AVI);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
