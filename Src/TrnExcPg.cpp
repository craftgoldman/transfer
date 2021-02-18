// TrnExcPg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnExcPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnExcPage property page

IMPLEMENT_DYNCREATE(CTrnExcPage, CPropertyPage)

CTrnExcPage::CTrnExcPage() : CPropertyPage(CTrnExcPage::IDD)
{
	//{{AFX_DATA_INIT(CTrnExcPage)
	m_Depart = -1;
	m_ExcTime = 0;
	m_IntTime = 0;
	m_FileImp = _T("");
	m_FileExp = _T("");
	m_FileEmp = _T("");
	m_NameArc = _T("");
	m_DirPut = _T("");
	m_DirGet = _T("");
	//}}AFX_DATA_INIT
}

CTrnExcPage::~CTrnExcPage()
{
}

void CTrnExcPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnExcPage)
	DDX_CBIndex(pDX, IDC_DEPART, m_Depart);
	DDX_Text(pDX, IDC_EXCTIME, m_ExcTime);
	DDX_Text(pDX, IDC_INTTIME, m_IntTime);
	DDX_Text(pDX, IDC_FILEIMP, m_FileImp);
	DDX_Text(pDX, IDC_FILEEXP, m_FileExp);
	DDX_Text(pDX, IDC_FILEEMP, m_FileEmp);
	DDX_Text(pDX, IDC_NAMEARC, m_NameArc);
	DDX_Text(pDX, IDC_DIRPUT, m_DirPut);
	DDX_Text(pDX, IDC_DIRGET, m_DirGet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrnExcPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTrnExcPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnExcPage message handlers

