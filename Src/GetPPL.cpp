// GetPPL.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "GetPPL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetPPLDlg dialog


CGetPPLDlg::CGetPPLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetPPLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetPPLDlg)
	//}}AFX_DATA_INIT
	m_nKind = -1;
	m_nIndex = -1;
}


void CGetPPLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetPPLDlg)
	DDX_Control(pDX, IDC_LISTPPL, m_lbPPLList);
	DDX_Control(pDX, IDOK, m_bOK);
	DDX_Control(pDX, IDCANCEL, m_bCANCEL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetPPLDlg, CDialog)
	//{{AFX_MSG_MAP(CGetPPLDlg)
	ON_LBN_DBLCLK(IDC_LISTPPL, OnDblclkListppl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetPPLDlg message handlers

BOOL CGetPPLDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Установка изображений для кнопок
	m_bOK.SetIcon(IDI_BOK, 16, 16);
	m_bOK.SetDefaultButton();
	m_bCANCEL.SetIcon(IDI_BCANCEL, 16, 16);
	// Заполнение списка профилей
	int	idx, iSel = 0;
	for (int i = 0; i < arPPL.GetSize(); i++)
	{
		if (m_nKind < 0 || arPPL[i].iKind == m_nKind)
		{
			idx = m_lbPPLList.AddString(arPPL[i].sName);
			m_lbPPLList.SetItemData(idx, i);
			// Установка профиля по умолчанию
			if (m_nKind == 1 && arPPL[i].sFile == TrnParm.sPplImp ||
				m_nKind == 2 && arPPL[i].sFile == TrnParm.sPplExp)
				iSel = idx;
		}
	}
	if (arPPL.GetSize() > 0)
	{
		m_lbPPLList.SetCurSel(iSel);
		m_nIndex = m_lbPPLList.GetItemData(iSel);
	}

	return TRUE;
}

void CGetPPLDlg::OnOK() 
{
	if (m_lbPPLList.GetCount() > 0)
	{
		int i = m_lbPPLList.GetCurSel();
		m_nIndex = m_lbPPLList.GetItemData(i);
	}

	CDialog::OnOK();
}

void CGetPPLDlg::OnDblclkListppl() 
{
	if (m_lbPPLList.GetCount() > 0)
	{
		int	i = m_lbPPLList.GetCurSel();
		m_nIndex = m_lbPPLList.GetItemData(i);
	}

	EndDialog(IDOK);
}
