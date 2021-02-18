// TrnParm.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnParm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnParmDlg dialog


CTrnParmDlg::CTrnParmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrnParmDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrnParmDlg)
	//}}AFX_DATA_INIT
	m_strParm = _T("");
}


void CTrnParmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnParmDlg)
	DDX_Control(pDX, IDOK, m_bOK);
	DDX_Control(pDX, IDCANCEL, m_bCANCEL);
	DDX_Control(pDX, IDC_ADD, m_bADD);
	DDX_Control(pDX, IDC_INSERT, m_bINSERT);
	DDX_Control(pDX, IDC_DELETE, m_bDELETE);
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_PARM_LIST, m_Grid);
}


BEGIN_MESSAGE_MAP(CTrnParmDlg, CDialog)
	//{{AFX_MSG_MAP(CTrnParmDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnParmDlg message handlers

BOOL CTrnParmDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// ��������� ����������� ��� ������
	m_bOK.SetIcon(IDI_BOK, 16, 16);
	m_bOK.SetDefaultButton();
	m_bCANCEL.SetIcon(IDI_BCANCEL, 16, 16);
	m_bADD.SetIcon(IDI_BADD, 16, 16);
	m_bINSERT.SetIcon(IDI_BINSERT, 16, 16);
	m_bDELETE.SetIcon(IDI_BDELETE, 16, 16);
	// ���������� ������ ����������
	FillParmList();

	return TRUE;
}

void CTrnParmDlg::OnOK() 
{
	// �������� ������
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return;
	}
	// �������� ������ � ������
	if (!CheckRowData()) return;
	// ������������ ������ ����������
	int		nFirst = m_Grid.GetFixedRowCount();
	int		nCount = m_Grid.GetRowCount();
	int		nLen = 0;
	CString	tmp,svl,sPrm;
	for (int row = nFirst; row < nCount; row++)
	{
		tmp	= m_Grid.GetItemText(row, 3) + " " +
			  m_Grid.GetItemText(row, 1) + "(" +
			  m_Grid.GetItemText(row, 2) + ")";
		svl = m_Grid.GetItemText(row, 4);
		if (!svl.IsEmpty())   tmp += "=" + svl;
		if (row < nCount - 1) tmp += ",";
		if (nLen + tmp.GetLength() > 80)
		{
			nLen  = 0;
			sPrm += "\r\n";
		}
		nLen += tmp.GetLength();
		sPrm += tmp;
	}
	m_strParm = sPrm;

	CDialog::OnOK();
}

void CTrnParmDlg::OnAdd() 
{
	// �������� ������
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return;
	}
	// �������� ������ � ������
	if (!CheckRowData()) return;
	// ������� ������
	int		row = m_Grid.GetRowCount();
	CString	tmp;
	tmp.Format("%i", row);
	row = m_Grid.InsertRow(tmp);
	// ��������� ���������� ��������
	m_Grid.SetItemText(row, 3, _T("char"));
	// ��������� ������� ������
	m_Grid.SetFocusCell(row, 1);
	m_Grid.SetSelectedRange(row, 1, row, m_nColCount - 1);
	m_Grid.Invalidate();
	// ����� ������ ��������������
	m_Grid.BeginEditting();
}

void CTrnParmDlg::OnInsert() 
{
	// �������� ������
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return;
	}
	// �������� ������ � ������
	if (!CheckRowData()) return;
	// ������� ������
	CCellID	cell = m_Grid.GetFocusCell();
	int		row;
	CString	tmp;
	tmp.Format("%i", cell.row);
	row = m_Grid.InsertRow(tmp, cell.row);
	// ��������� ���������� ��������
	m_Grid.SetItemText(row, 3, _T("char"));
	// ��������� ������� ������
	m_Grid.SetFocusCell(row, 1);
	m_Grid.SetSelectedRange(row, 1, row, m_nColCount - 1);
	// ������������� �����
	int	nCount = m_Grid.GetRowCount();
	for (; row < nCount; row++) {
		tmp.Format("%i", row);
		m_Grid.SetItemText(row, 0, tmp);
	}
	m_Grid.Invalidate();
	// ����� ������ ��������������
	m_Grid.BeginEditting();
}

void CTrnParmDlg::OnDelete() 
{
	CCellID	cell = m_Grid.GetFocusCell();
	if (cell.row < 0) return;

	// ���������� ��������������
	m_Grid.EndEditting(FALSE);
	// �������� ������
	m_Grid.DeleteRow(cell.row, FALSE);
	// ������������� �����
	int		nFirst = m_Grid.GetFixedRowCount();
	int		nCount = m_Grid.GetRowCount();
	CString	tmp;
	for (int row = nFirst; row < nCount; row++) {
		tmp.Format("%i", row);
		m_Grid.SetItemText(row, 0, tmp);
	}
    m_Grid.Invalidate();
	// ��������� ������� ������
	if (cell.row == nCount) cell.row--;
	if (cell.row >= nFirst)
	{
		m_Grid.SetFocusCell(cell.row, cell.col);
		m_Grid.SetSelectedRange(cell.row, 1, cell.row, m_nColCount - 1);
		// ����� ������ ��������������
		m_Grid.BeginEditting();
	}
}

BOOL CTrnParmDlg::FillParmList()
{
	CString tmp;
	// ��������� ���������� �������
	m_Grid.SetRowResize(FALSE );
	m_Grid.SetEditMode(GVEM_FULL_LIST);
	// ��������� ������� �������
	CString sNum,sCod,sNam,sTyp,sVal;
	sNum.LoadString(TAB_FLD_COLNUM);
	sCod.LoadString(TAB_FLD_CODE);
	sNam.LoadString(TAB_FLD_NAME);
	sTyp.LoadString(TAB_FLD_COLTYPE);
	sVal.LoadString(TAB_FLD_COLVAL);
	m_Grid.DeleteAllItems();
	m_Grid.InsertColumn(sNum);
	m_Grid.InsertColumn(sCod, DT_LEFT);
	m_Grid.InsertColumn(sNam, DT_LEFT);
	m_Grid.InsertColumn(sTyp, DT_LEFT);
	m_Grid.InsertColumn(sVal, DT_LEFT);
	m_Grid.SetColumnWidth(0, 30);
	m_Grid.SetColumnWidth(1, 70);
	m_Grid.SetColumnWidth(2, 130);
	m_Grid.SetColumnWidth(3, 70);
	m_Grid.SetColumnWidth(4, 70);
	// ��������� ����� �������
	m_Grid.SetColumnEditStyle(1, GVCE_EDIT);
	m_Grid.SetColumnEditStyle(2, GVCE_EDIT);
	m_Grid.SetColumnEditStyle(3, GVCE_LISTBOX);
	m_Grid.SetColumnEditStyle(4, GVCE_EDIT);
	m_Grid.SetColumnProp(3, GVCV_VALUES, 
	"char\ninteger\nnumeric\ndate\ntime\ntimestamp");
	// �������� ����������
	m_Grid.SetRowCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnCount(1);
	m_nColCount = m_Grid.GetColumnCount();

	// ���������� ������� ����������
	CFldArray arPrm;
	if (!FillParmArray(m_strParm, arPrm))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(CString(GetErrMsg(ER_SNTX_PARAMS)), 
					MB_OK|MB_ICONEXCLAMATION);
	}
	// ���������� ������ � ������
	int row = m_Grid.GetFixedRowCount();
	int i, n = arPrm.GetSize();
	for (i = 0; i < n; i++, row++)
	{
		// ������� ������ � �������
		tmp.Format("%i", row);
		row = m_Grid.InsertRow(tmp);
		// ���������� ������ � ������
		m_Grid.SetItemText(row, 1, arPrm[i].sID);
		m_Grid.SetItemText(row, 2, arPrm[i].sName);
		m_Grid.SetItemText(row, 3, arPrm[i].sType);
		m_Grid.SetItemText(row, 4, arPrm[i].sValue);
	};
	// ��������� ������� ������
	if (m_Grid.GetRowCount() > 1) {
		m_Grid.SetFocusCell(1, 1);
		m_Grid.SetSelectedRange(1, 1, 1, m_nColCount - 1);
		// ����� ������ ��������������
		m_Grid.BeginEditting();
	}
	return TRUE;
}

BOOL CTrnParmDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// ��������� ��������� �� ������
	if (wParam == IDC_PARM_LIST) 
	{
	    NM_GRIDVIEW	*nmgv = (NM_GRIDVIEW *)lParam;
		// �������� ������ � ������
		if (nmgv->hdr.code == GVN_CHECKCELLDATA) 
		{
			CString	strErr,strTitle;
			if (!CheckCellData(nmgv->iRow, nmgv->iCol, 
				nmgv->szText, strErr))
			{
				GetWindowText(strTitle);
				MessageBeep(MB_ICONEXCLAMATION);
				MessageBox(strErr, strTitle, MB_ICONEXCLAMATION);
				*pResult = 1;
				return TRUE;
			}
		}
		// �������� ������ � ������
		if (nmgv->hdr.code == GVN_ROWCHANGING)
		{
			if (!CheckRowData()) {
				*pResult = 1;
				return TRUE;
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CTrnParmDlg::CheckCellData(int iRow, int iCol, CString data, CString &strErr)
{
	// ����������� ������������� �������
	if (data.FindOneOf(",") >= 0)
	{
		strErr = GetErrMsg(ER_BAD_CHAR, _T(","));
		return FALSE;
	}
	// �������� ��������������
	if (iCol == 1) {
		LPCTSTR cSet;
		// �������� �� ������������ �������
		cSet = " \t~`!@#$%^&*()-+=|\\<>?/";
		if (data.FindOneOf(cSet) >= 0)
		{
			strErr = GetErrMsg(ER_BAD_VAL, data);
			return FALSE;
		}
		// ������������� �� ����� ���������� � �����
		cSet = "1234567890";
		if (data.Left(1).FindOneOf(cSet) >= 0)
		{
			strErr = GetErrMsg(ER_BAD_VAL, data);
			return FALSE;
		}
	}
	// �������� ������������
	if (iCol == 2 && data.FindOneOf("()") >= 0)
	{
		strErr = GetErrMsg(ER_BAD_VAL, data);
		return FALSE;
	}
	return TRUE;
}

BOOL CTrnParmDlg::CheckRowData()
{
	CString	strErr,strTitle;
	CCellID	cell = m_Grid.GetFocusCell();
	if (cell.row >= m_Grid.GetFixedRowCount())
	{
		// �������� �������������� - �����������
		CString	sID = m_Grid.GetItemText(cell.row, 1);
		if (sID.IsEmpty())
		{
			strErr = GetErrMsg(ER_NEED_CODE);
			cell.col = 1;
			goto err;
		}
		// �������� �������������� �� ������������
		if ((m_Grid.FindRow(1, sID, 1, cell.row - 1) > 0) ||
			(m_Grid.FindRow(1, sID, cell.row + 1,
			 m_Grid.GetRowCount() - 1) > 0))
		{
			cell.col = 1;
			strErr = GetErrMsg(ER_DOUBLE_CODE, sID);
			goto err;
		}
		// �������� ������������ - �����������
		if (m_Grid.GetItemText(cell.row, 2).IsEmpty())
		{
			strErr = GetErrMsg(ER_NEED_NAME);
			cell.col = 2;
			goto err;
		}
	}
	return TRUE;

err:// ��������� ������� �������
	m_Grid.SetFocusCell(cell);
	// ����� ������ ��������������
	m_Grid.BeginEditting();
	// ��������� �� ������
	GetWindowText(strTitle);
	MessageBeep(MB_ICONEXCLAMATION);
	MessageBox(strErr, strTitle, MB_ICONEXCLAMATION);
	return FALSE;
}
