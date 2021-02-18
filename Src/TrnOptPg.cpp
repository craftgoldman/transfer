// TrnOptPg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnOptPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnOptPage property page

IMPLEMENT_DYNCREATE(CTrnOptPage, CPropertyPage)

CTrnOptPage::CTrnOptPage() : CPropertyPage(CTrnOptPage::IDD)
{
	//{{AFX_DATA_INIT(CTrnOptPage)
	m_DirPpl = _T("");
	m_DirLog = _T("");
	m_DirMsg = _T("");
	m_FileLog= _T("");
	m_FileMsg= _T("");
	m_PplExp = _T("");
	m_PplImp = _T("");
	m_UseExp = FALSE;
	m_UseImp = FALSE;
	//}}AFX_DATA_INIT
}

CTrnOptPage::~CTrnOptPage()
{
}

void CTrnOptPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnOptPage)
	DDX_Text(pDX, IDC_DIRPPL, m_DirPpl);
	DDX_Text(pDX, IDC_DIRLOG, m_DirLog);
	DDX_Text(pDX, IDC_DIRMSG, m_DirMsg);
	DDX_Text(pDX, IDC_FILELOG, m_FileLog);
	DDX_Text(pDX, IDC_FILEMSG, m_FileMsg);
	DDX_CBString(pDX, IDC_PPLEXP, m_PplExp);
	DDX_CBString(pDX, IDC_PPLIMP, m_PplImp);
	DDX_Check(pDX, IDC_USEEXP, m_UseExp);
	DDX_Check(pDX, IDC_USEIMP, m_UseImp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrnOptPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTrnOptPage)
	ON_CBN_SELENDOK(IDC_PPLEXP, OnSelendokPplexp)
	ON_CBN_SELENDOK(IDC_PPLIMP, OnSelendokPplimp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnOptPage message handlers

BOOL CTrnOptPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	int	i;
	// Заполнение списка профилей импорта
	CComboBox* pCBImp = (CComboBox*) GetDlgItem(IDC_PPLIMP);
	for (i = 0; i < arPPL.GetSize(); i++)
	{
		if (arPPL[i].iKind == 1)
			pCBImp->AddString(arPPL[i].sFile);
	}
	// Установка текущего выбора
	pCBImp->SelectString(0, TrnParm.sPplImp);
	i = FindPPL(TrnParm.sPplImp);
	if (i >= 0)
		SetDlgItemText(IDC_PPLIMP_N, arPPL[i].sName);

	// Заполнение списка профилей экспорта
	CComboBox* pCBExp = (CComboBox*) GetDlgItem(IDC_PPLEXP);
	for (i = 0; i < arPPL.GetSize(); i++)
	{
		if (arPPL[i].iKind == 2)
			pCBExp->AddString(arPPL[i].sFile);
	}
	// Установка текущего выбора
	pCBExp->SelectString(0, TrnParm.sPplExp);
	i = FindPPL(TrnParm.sPplExp);
	if (i >= 0)
		SetDlgItemText(IDC_PPLEXP_N, arPPL[i].sName);

	return TRUE;
}

void CTrnOptPage::OnSelendokPplexp() 
{
	CString		sText;
	CComboBox*	pCBExp = (CComboBox*) GetDlgItem(IDC_PPLEXP);
	pCBExp->GetLBText(pCBExp->GetCurSel(), sText);
	int i = FindPPL(sText);
	if (i >= 0)
		SetDlgItemText(IDC_PPLEXP_N, arPPL[i].sName);
}

void CTrnOptPage::OnSelendokPplimp() 
{
	CString		sText;
	CComboBox*	pCBImp = (CComboBox*) GetDlgItem(IDC_PPLIMP);
	pCBImp->GetLBText(pCBImp->GetCurSel(), sText);
	int i = FindPPL( sText );
	if (i >= 0)
		SetDlgItemText(IDC_PPLIMP_N, arPPL[i].sName);
}
