// TrnStart.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnStart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnStartDlg dialog


CTrnStartDlg::CTrnStartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrnStartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrnStartDlg)
	//}}AFX_DATA_INIT
	m_sTitle = _T("");
	m_pArPrm = NULL;
	m_pArTrn = NULL;
}

CTrnStartDlg::~CTrnStartDlg()
{
}

void CTrnStartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnStartDlg)
	DDX_Control(pDX, IDC_LIST_TRN, m_lbTrnList);
	DDX_Control(pDX, IDOK, m_bOK);
	DDX_Control(pDX, IDCANCEL, m_bCANCEL);
	DDX_Control(pDX, IDC_SELECTALL, m_bSELECTALL);
	DDX_Control(pDX, IDC_DESELECT, m_bDESELECT);
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_PARM_LIST, m_Grid);
}


BEGIN_MESSAGE_MAP(CTrnStartDlg, CDialog)
	//{{AFX_MSG_MAP(CTrnStartDlg)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_DESELECT, OnDeselect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnStartDlg message handlers

BOOL CTrnStartDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// ��������� ��������� ����
	if (!m_sTitle.IsEmpty())
		SetWindowText(m_sTitle);
	// ��������� ����������� ��� ������
	m_bOK.SetIcon(IDI_BOK, 16, 16);
	m_bOK.SetDefaultButton();
	m_bCANCEL.SetIcon(IDI_BCANCEL, 16, 16);

	// ���������� ������ ����������
	FillParmList();

	// ���������� ������ �����������
	if (m_pArTrn == NULL) return TRUE;
	int	i, n = m_pArTrn->GetSize();
	for (i = 0; i < n; i++)
	{
		// ���������� ���������� � ������
		m_lbTrnList.AddString(" "+m_pArTrn->GetAt(i).sName);
		// ��������� �������
		if (!m_pArTrn->GetAt(i).bValid)
			m_lbTrnList.Enable(i, FALSE);
		else if (m_pArTrn->GetAt(i).bSelect)
			m_lbTrnList.SetCheck(i, 1);
	}
	return TRUE;
}

BOOL CTrnStartDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// ��������� ��������� �� ������
	if (wParam == IDC_PARM_LIST) 
	{
	    NM_GRIDVIEW	*nmgv = (NM_GRIDVIEW *)lParam;
		int n = m_Grid.GetFixedRowCount();
		// ��������� ����� ��� ��������
		if (nmgv->hdr.code == GVN_BEGINLABELEDIT &&
			nmgv->iCol == 2 && nmgv->iRow > 0 &&
			m_pArPrm != NULL)
		{
			switch (m_pArPrm->GetAt(nmgv->iRow-n).iSQLType)
			{
			case SQL_CHAR:
				m_Grid.SetColumnMaskType(2, MASK_STRING);
				break;
			case SQL_INTEGER:
				m_Grid.SetColumnMaskType(2, MASK_NUMERIC, 10);
				break;
			case SQL_NUMERIC:
				m_Grid.SetColumnMaskType(2, MASK_NUMERIC, 15, 4);
				break;
			case SQL_DATE:
				m_Grid.SetColumnMaskType(2, MASK_DATE);
				break;
			case SQL_TIME:
				m_Grid.SetColumnMaskType(2, MASK_TIME);
				break;
			case SQL_TIMESTAMP:
				m_Grid.SetColumnMaskType(2, MASK_DATETIME);
				break;
			}
		}
		// �������� ������ � ������
		if (nmgv->hdr.code == GVN_CHECKCELLDATA &&
			nmgv->iCol == 2 && nmgv->iRow > 0)
		{
			if (!CheckValType(m_pArPrm->GetAt(nmgv->iRow-n).iSQLType, 
				nmgv->szText))
			{
				CString strTitle,strErr;
				GetWindowText(strTitle);
				strErr = GetErrMsg(ER_BAD_DATA);
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox(strErr, strTitle, MB_ICONEXCLAMATION);
				*pResult = 1;
				return TRUE;
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CTrnStartDlg::OnOK() 
{
	// �������� ������
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return;
	}
	// ��������� �������� ����������
	int i, n, row, count;
	if (m_pArPrm != NULL)
	{
		n = m_pArPrm->GetSize();
		row = m_Grid.GetFixedRowCount();
		count = m_Grid.GetRowCount();
		for (i = 0; i < n && row < count; i++, row++)
		{
			if (m_Grid.GetItemText(row, 2).IsEmpty() &&
				m_pArPrm->GetAt(i).iSQLType != GVCT_CHAR)
			{
				m_Grid.SetFocusCell(row, 2);
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox(_T("������� ��������!"), 
					_T("���������"), MB_ICONEXCLAMATION);
				return;
			}
			m_pArPrm->ElementAt(i).sValue = m_Grid.GetItemText(row, 2);
		}
	}
	// ��������� ���������� �����������
	if (m_pArTrn != NULL)
	{
		n = m_pArTrn->GetSize();
		for (i = 0; i < n; i++)
		{
			if (m_lbTrnList.GetCheck(i) > 0)
				m_pArTrn->ElementAt(i).bSelect = TRUE;
			else
				m_pArTrn->ElementAt(i).bSelect = FALSE;
		}
	}

	CDialog::OnOK();
}

BOOL CTrnStartDlg::FillParmList()
{
	// ��������� ���������� �������
	m_Grid.SetRowResize(FALSE );
	m_Grid.SetEditMode(GVEM_FULL_LIST);
	// ��������� ������� �������
	CString sNum,sNam,sVal;
	sNum.LoadString(TAB_FLD_COLNUM);
	sNam.LoadString(TAB_FLD_NAME);
	sVal.LoadString(TAB_FLD_COLVAL);
	m_Grid.DeleteAllItems();
	m_Grid.InsertColumn(sNum);
	m_Grid.InsertColumn(sNam, DT_LEFT);
	m_Grid.InsertColumn(sVal, DT_LEFT);
	m_Grid.SetColumnWidth(0, 30);
	m_Grid.SetColumnWidth(1, 150);
	m_Grid.SetColumnWidth(2, 100);
	// ��������� ����� �������
	m_Grid.SetColumnEditStyle(2, GVCE_EDIT);
	// �������� ����������
	m_Grid.SetRowCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnCount(1);

	// ���������� ������ � ������
	if (m_pArPrm == NULL) return TRUE;
	CString tmp;
	int i, n = m_pArPrm->GetSize();
	int row = m_Grid.GetFixedRowCount();
	for (i = 0; i < n; i++, row++)
	{
		// ��������� ��������� ��������
		if (m_pArPrm->GetAt(i).iSQLType == SQL_INTEGER ||
			m_pArPrm->GetAt(i).iSQLType == SQL_NUMERIC)
		{
			CString	strSet = _T(" -1234567890.");
			m_pArPrm->ElementAt(i).sValue.Remove(' ');
			m_pArPrm->ElementAt(i).sValue.Remove(',');
			m_pArPrm->ElementAt(i).sValue.SpanIncluding(strSet);
		}
		// �������� ��������
		if (!m_pArPrm->GetAt(i).sValue.IsEmpty())
		{
			// ��������� ��������
			if (!CheckValType(m_pArPrm->GetAt(i).iSQLType, 
				m_pArPrm->GetAt(i).sValue))
				m_pArPrm->ElementAt(i).sValue.Empty();
		}
		// ���������� �������� �� ���������
		if (m_pArPrm->GetAt(i).sValue.IsEmpty())
			m_pArPrm->ElementAt(i).sValue = 
			SQLGetDefStrVal(m_pArPrm->GetAt(i).iSQLType);
		// ������� ������ � �������
		tmp.Format("%i", row);
		row = m_Grid.InsertRow(tmp);
		// ���������� ������ � ������
		m_Grid.SetItemText(row, 1, m_pArPrm->GetAt(i).sName);
		m_Grid.SetItemText(row, 2, m_pArPrm->GetAt(i).sValue);
		m_Grid.SetItemState(row, 1, GVIS_READONLY);
	};
	// ��������� ������� ������
	if (m_Grid.GetRowCount() > 1) {
		m_Grid.SetFocusCell(1, 2);
		m_Grid.SetSelectedRange(1, 1, 1, 2);
		// ����� ������ ��������������
		m_Grid.BeginEditting();
	}
	return TRUE;
}

void CTrnStartDlg::OnSelectall() 
{
	// ���������� ������� �� ��� ������
	int	i, n = m_lbTrnList.GetCount();
	for (i = 0; i < n; i++)
		if (m_lbTrnList.IsEnabled(i))
			m_lbTrnList.SetCheck(i, 1);
}

void CTrnStartDlg::OnDeselect() 
{
	// ����� ������� �� ���� �������
	int	i, n = m_lbTrnList.GetCount();
	for (i = 0; i < n; i++)
		if (m_lbTrnList.IsEnabled(i))
			m_lbTrnList.SetCheck(i, 0);
}

BOOL CTrnStartDlg::CheckValType(UINT nType, CString strVal)
{
	// �������� �������� �� ����
	switch (nType)
	{
	case GVCT_CHAR:
		break;
	case SQL_INTEGER:
	case SQL_NUMERIC:
	{
		CString	strSet = "-1234567890.";
		if (!strVal.SpanExcluding(strSet).IsEmpty())
			return FALSE;
		break;
	}
	case SQL_DATE:
	case SQL_TIME:
	case SQL_TIMESTAMP:
	{
		COleDateTime dt;
		if (!dt.ParseDateTime(strVal))
			return FALSE;
		break;
	}
	default:
		break;
	}
	return TRUE;
}
