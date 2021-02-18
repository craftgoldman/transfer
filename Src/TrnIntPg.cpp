// TrnIntPg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnIntPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnIntPage property page

IMPLEMENT_DYNCREATE(CTrnIntPage, CPropertyPage)

CTrnIntPage::CTrnIntPage() : CPropertyPage(CTrnIntPage::IDD)
{
	//{{AFX_DATA_INIT(CTrnIntPage)
	m_SplashW = FALSE;
	m_Minimize= FALSE;
	m_SysTray = FALSE;
	m_SavePos = FALSE;
	m_SaveMDI = FALSE;
	m_MaxiMDI = FALSE;
	//}}AFX_DATA_INIT
	m_LangID = _T("");
}

CTrnIntPage::~CTrnIntPage()
{
}

void CTrnIntPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnIntPage)
	DDX_Check(pDX, IDC_SPLASHW, m_SplashW);
	DDX_Check(pDX, IDC_MINIMIZE,m_Minimize);
	DDX_Check(pDX, IDC_SYSTRAY, m_SysTray);
	DDX_Check(pDX, IDC_SAVEPOS, m_SavePos);
	DDX_Check(pDX, IDC_SAVEMDI, m_SaveMDI);
	DDX_Check(pDX, IDC_MAXIMDI, m_MaxiMDI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrnIntPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTrnIntPage)
	ON_CBN_SELENDOK(IDC_LANGID, OnSelendokLangid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnIntPage message handlers

BOOL CTrnIntPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Установка текущего языка
	CComboBox* pCBLang = (CComboBox*) GetDlgItem(IDC_LANGID);
	pCBLang->SelectString(0, m_LangID);

	return TRUE;
}

void CTrnIntPage::OnSelendokLangid() 
{
	// Установка текущего языка
	CComboBox* pCBLang = (CComboBox*) GetDlgItem(IDC_LANGID);
	pCBLang->GetWindowText(m_LangID);
	m_LangID = m_LangID.Left(3);
}
