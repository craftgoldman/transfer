// TrnTabPg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnTabPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnTabPage property page

IMPLEMENT_DYNCREATE(CTrnTabPage, CResizablePage)

CTrnTabPage::CTrnTabPage(UINT nIDCaption) : CResizablePage(CTrnTabPage::IDD, nIDCaption)
{
	//{{AFX_DATA_INIT(CTrnTabPage)
	m_sOwner = _T("");
	m_sTable = _T("");
	m_nProcess = 0;
	m_sSQLCond = _T("");
	//}}AFX_DATA_INIT
	m_sColumns = _T("");
	m_sODBCDSN = _T("");
	m_db = NULL;
}

CTrnTabPage::~CTrnTabPage()
{
	if (m_db != NULL)
	{
		if (m_db->IsOpen())
			m_db->Close();
		delete m_db;
	}
}

void CTrnTabPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnTabPage)
	DDX_Text(pDX, IDC_DST_OWNER, m_sOwner);
	DDX_Control(pDX, IDC_DST_TABLE, m_cbTable);
	DDX_CBString(pDX, IDC_DST_TABLE, m_sTable);
	DDX_CBIndex(pDX, IDC_DST_PROCESS, m_nProcess);
	DDX_Text(pDX, IDC_DST_CONDITION, m_sSQLCond);
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_TABLE_MAP, m_Grid);
}


BEGIN_MESSAGE_MAP(CTrnTabPage, CResizablePage)
	ON_MESSAGE(WM_USER, OnUserMsg)
	//{{AFX_MSG_MAP(CTrnTabPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_CBN_DROPDOWN(IDC_DST_TABLE, OnDropdownDstTable)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnTabPage message handlers

BOOL CTrnTabPage::OnInitDialog() 
{
	CResizablePage::OnInitDialog();

	AddAnchor(IDC_DST_TABLE, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_DST_CAPTION, TOP_RIGHT);
	AddAnchor(IDC_DST_PROCESS, TOP_RIGHT);
	AddAnchor(IDC_DST_CONDITION, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_ADD,    TOP_RIGHT);
	AddAnchor(IDC_INSERT, TOP_RIGHT);
	AddAnchor(IDC_DELETE, TOP_RIGHT);
	AddAnchor(IDC_TABLE_MAP, TOP_LEFT, BOTTOM_RIGHT);

	// ���������� ������ �������
	FillColumnsList();

	return TRUE;
}

BOOL CTrnTabPage::OnKillActive() 
{
	// �������� ������
	UpdateData();
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return FALSE;
	}
	// �������� ������ � ������
	if (!CheckRowData()) return FALSE;
	// ������������ ������ ������ �����
	int		nFirst = m_Grid.GetFixedRowCount();
	int		nCount = m_Grid.GetRowCount();
	int		nLen   = 0;
	CString	str,sCol,sFlag,sType,sLen,sDec,sSrc,sVal;
	for (int row = nFirst; row < nCount; row++)
	{
		sCol  = m_Grid.GetItemText(row, 1);
		sFlag = m_Grid.GetItemText(row, 2);
		sType = m_Grid.GetItemText(row, 3);
		sLen  = m_Grid.GetItemText(row, 4);
		sDec  = m_Grid.GetItemText(row, 5);
		sSrc  = m_Grid.GetItemText(row, 6);
		sVal  = m_Grid.GetItemText(row, 7);
		if (sFlag != _T("*"))
			sCol = sFlag + _T(":") + sCol;
		if (sType != _T("*"))
		{
			sCol += _T("(") + sType;
			if (!sLen.IsEmpty()) sCol += _T(",") + sLen;
			if (!sDec.IsEmpty()) sCol += _T(",") + sDec;
			sCol += ")";
		}
		if (!sVal.IsEmpty())  sCol += _T("=") + sSrc + sVal;
		if (row < nCount - 1) sCol += _T(";");
		if (nLen + sCol.GetLength() > 80)
		{
			nLen = 0;
			str += _T("\r\n");
		}
		nLen += sCol.GetLength();
		str  += sCol;
	}
	m_sColumns = str;

	return CResizablePage::OnKillActive();
}

BOOL CTrnTabPage::OnSetActive() 
{
	if (m_Grid.GetFocusCell().row < 0)
		return TRUE;
	// ����� ������ ��������������
	m_Grid.BeginEditting();

	return CResizablePage::OnSetActive();
}

void CTrnTabPage::OnAdd() 
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
	m_Grid.SetItemText(row, 2, "*");
	m_Grid.SetItemText(row, 3, "*");
	m_Grid.SetItemText(row, 6, "^");
	// ���������� ����� � ��������
	UINT stLen = m_Grid.GetItemState(row, 4);
	UINT stDec = m_Grid.GetItemState(row, 5);
	m_Grid.SetItemState(row, 4, stLen|GVIS_READONLY);
	m_Grid.SetItemState(row, 5, stDec|GVIS_READONLY);
	// ��������� ������� ������
	m_Grid.SetFocusCell(row, 1);
	m_Grid.SetSelectedRange(row, 1, row, m_nColCount - 1);
	m_Grid.Invalidate();
	// ����� ������ ��������������
	m_Grid.BeginEditting();
}

void CTrnTabPage::OnInsert() 
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
	m_Grid.SetItemText(row, 2, "*");
	m_Grid.SetItemText(row, 3, "*");
	m_Grid.SetItemText(row, 6, "^");
	// ���������� ����� � ��������
	UINT stLen = m_Grid.GetItemState(row, 4);
	UINT stDec = m_Grid.GetItemState(row, 5);
	m_Grid.SetItemState(row, 4, stLen|GVIS_READONLY);
	m_Grid.SetItemState(row, 5, stDec|GVIS_READONLY);
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

void CTrnTabPage::OnDelete() 
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

BOOL CTrnTabPage::FillColumnsList()
{
	CString	tmp;
	// ��������� ���������� �������
	m_Grid.SetRowResize(FALSE );
	m_Grid.SetEditMode(GVEM_FULL_LIST);
	// ��������� ������� �������
	CString sNum,sFld,sFlg,sTyp,sLen,sDec,sVal,sSrc;
	sNum.LoadString(TAB_FLD_COLNUM);
	sFld.LoadString(TAB_FLD_COLNAME);
	sFlg.LoadString(TAB_FLD_COLFLAG);
	sTyp.LoadString(TAB_FLD_COLTYPE);
	sLen.LoadString(TAB_FLD_COLLEN);
	sDec.LoadString(TAB_FLD_COLDEC);
	sVal.LoadString(TAB_FLD_COLVAL);
	m_Grid.DeleteAllItems();
	m_Grid.InsertColumn(sNum);
	m_Grid.InsertColumn(sFld, DT_LEFT);
	m_Grid.InsertColumn(sFlg, DT_CENTER);
	m_Grid.InsertColumn(sTyp, DT_LEFT);
	m_Grid.InsertColumn(sLen, DT_RIGHT);
	m_Grid.InsertColumn(sDec, DT_RIGHT);
	m_Grid.InsertColumn("<-", DT_CENTER);
	m_Grid.InsertColumn(sVal, DT_LEFT);
	m_Grid.SetColumnWidth(0, 30);
	m_Grid.SetColumnWidth(1, 80);
	m_Grid.SetColumnWidth(2, 40);
	m_Grid.SetColumnWidth(3, 80);
	m_Grid.SetColumnWidth(4, 50);
	m_Grid.SetColumnWidth(5, 40);
	m_Grid.SetColumnWidth(6, 40);
	m_Grid.SetColumnWidth(7, 90);
	// ��������� ����� �������
	m_Grid.SetColumnEditStyle(1, GVCE_EDIT);
	m_Grid.SetColumnEditStyle(2, GVCE_LISTBOX);
	m_Grid.SetColumnEditStyle(3, GVCE_LISTBOX);
	m_Grid.SetColumnEditStyle(4, GVCE_EDIT);
	m_Grid.SetColumnEditStyle(5, GVCE_EDIT);
	m_Grid.SetColumnEditStyle(6, GVCE_LISTBOX);
	m_Grid.SetColumnEditStyle(7, GVCE_EDIT);
	// ��������� ����� ��������������
	m_Grid.SetColumnMaskType(4, MASK_NUMERIC, 3, 0, FALSE);
	m_Grid.SetColumnMaskType(5, MASK_NUMERIC, 2, 0, FALSE);
	// ��������� ������ ������
	m_Grid.SetColumnProp(2, GVCV_VALUES, 
		_T(" \t*\nN\tN\nK\tK\nF\tF\nT\tT"));
	// ��������� ������ ����� ������
	tmp  = _T(" \t*\n");
	tmp += _T("Char\tA\nVarChar\tVA\nLongVarChar\tLVA\n");
	tmp += _T("Decimal\tDC\nNumeric\tN\nSmallInt\tSI\nInteger\tI\n");
	tmp += _T("Real\tR\nFloat\tF\nDouble\tDB\n");
	tmp += _T("Bit\tBT\nTinyInt\tTI\nBigInt\tBI\n");
	tmp += _T("Binary\tB\nVarBinary\tVB\nLongVarBinary\tLVB\n");
	tmp += _T("Date\tD\nTime\tT\nTimestamp\tTS\n");
	m_Grid.SetColumnProp(3, GVCV_VALUES, tmp);
	// ��������� ������ ����� ����������
	m_Grid.SetColumnProp(6, GVCV_VALUES, 
		_T(" \t^\n+\t+\n:\t:\n&\t&\n$\t$\n~\t~\n<\t<\n%\t%\n@\t@"));
	// �������� ����������
	m_Grid.SetRowCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnCount(1);
	m_nColCount = m_Grid.GetColumnCount();

	// ���������� ������� �����
	CFldArray arFld;
	if (!FillFieldArray(m_sColumns, arFld))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		CString strErr = GetErrMsg(ER_SNTX_MAPTAB);
		AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
	}
	// ���������� ������ � ������
	int	row = m_Grid.GetFixedColumnCount();
	int	i, n = arFld.GetSize();
	for (i = 0; i < n; i++, row++)
	{
		// ������� ������ � �������
		tmp.Format("%i", row);
		row = m_Grid.InsertRow(tmp);
		// ��������� �����
		switch (arFld[i].iFlag)
		{
			case FLD_DATA:     sFlg = _T("*"); break;
			case FLD_KEY:      sFlg = _T("K"); break;
			case FLD_NOCHANGE: sFlg = _T("N"); break;
			case FLD_FRUNBACK: sFlg = _T("F"); break;
			case FLD_TRIGGER:  sFlg = _T("T"); break;
			default:           sFlg = _T("*"); break;
		}
		// ��������� ���������
		switch (arFld[i].iSrc)
		{
			case DATA_LOCAL:   sSrc = _T("^"); break;
			case DATA_AUTO:    sSrc = _T("+"); break;
			case DATA_PARAM:   sSrc = _T(":"); break;
			case DATA_CONST:   sSrc = _T("&"); break;
			case DATA_TABSRC:  sSrc = _T("$"); break;
			case DATA_SUBSRC:  sSrc = _T("~"); break;
			case DATA_REF:     sSrc = _T("<"); break;
			case DATA_FUNC:    sSrc = _T("%"); break;
			case DATA_PROC:    sSrc = _T("@"); break;
			default:           sSrc = _T("^"); break;
		}
		// ��������� ����� � ��������
		sLen.Empty();
		sDec.Empty();
		if (arFld[i].iSize > 0)
			sLen.Format("%i", arFld[i].iSize);
		if (arFld[i].iPrec > 0)
			sDec.Format("%i", arFld[i].iPrec);
		// ���������� ������ � ������
		m_Grid.SetItemText(row, 1, arFld[i].sName);
		m_Grid.SetItemText(row, 2, sFlg);
		m_Grid.SetItemText(row, 3, arFld[i].sType);
		m_Grid.SetItemText(row, 4, sLen);
		m_Grid.SetItemText(row, 5, sDec);
		m_Grid.SetItemText(row, 6, sSrc);
		m_Grid.SetItemText(row, 7, arFld[i].sValue);
		// �������� ���� ������
		CheckDataType(row, arFld[i].sType);
	};
	// ��������� ������� ������
	if (m_Grid.GetRowCount() > 1) {
		m_Grid.SetFocusCell(1, 1);
		m_Grid.SetSelectedRange(1, 1, 1, m_nColCount - 1);
	}
	return TRUE;
}

BOOL CTrnTabPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// ��������� ��������� �� ������
	if (wParam == IDC_TABLE_MAP) 
	{
	    NM_GRIDVIEW	*nmgv = (NM_GRIDVIEW *)lParam;
		// �������� ������ � ������
		if (nmgv->hdr.code == GVN_CHECKCELLDATA)
		{
			CString	strErr;
			if (!CheckCellData(nmgv->iRow, nmgv->iCol, 
				nmgv->szText, strErr))
			{
				MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(strErr, MB_ICONEXCLAMATION);
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
		// ��������� ����� ����� � �������� 
		// � ����������� �� ���� ������
		if (nmgv->hdr.code == GVN_EDITSELENDOK &&
			nmgv->iCol == 3)
		{
			CheckDataType(nmgv->iRow, nmgv->szText);
			m_Grid.Invalidate();
		}
	}

	return CResizablePage::OnNotify(wParam, lParam, pResult);
}

BOOL CTrnTabPage::CheckDataType(int iRow, CString sType)
{
	if (!m_Grid.IsValid(iRow, 0))
		return FALSE;

	UINT stLen = m_Grid.GetItemState(iRow, 4);
	UINT stDec = m_Grid.GetItemState(iRow, 5);
	if (sType == "N"  ||
		sType == "DC")
	{
		// ���������� ����� � ��������
		m_Grid.SetItemState(iRow, 4, stLen & ~GVIS_READONLY);
		m_Grid.SetItemState(iRow, 5, stDec & ~GVIS_READONLY);
	}
	else if (sType == "F"   ||
			 sType == "A"   ||
			 sType == "VA"  ||
			 sType == "B"   ||
			 sType == "VB")
	{
		// ���������� �����
		m_Grid.SetItemState(iRow, 4, stLen & ~GVIS_READONLY);
		// ���������� ��������
		m_Grid.SetItemState(iRow, 5, stDec | GVIS_READONLY);
		m_Grid.SetItemText(iRow, 5, "");
	}
	else
	{
		// ���������� ����� � ��������
		m_Grid.SetItemState(iRow, 4, stLen | GVIS_READONLY);
		m_Grid.SetItemState(iRow, 5, stDec | GVIS_READONLY);
		m_Grid.SetItemText(iRow, 4, "");
		m_Grid.SetItemText(iRow, 5, "");
	}
	return TRUE;
}

BOOL CTrnTabPage::CheckCellData(int iRow, int iCol, CString data, CString &strErr)
{
	// ����������� ������������� ����� � �������
	if (data.FindOneOf(";") >= 0) {
		strErr = GetErrMsg(ER_BAD_CHAR, _T(";"));
		return FALSE;
	}
	// �������� ����� ����
	if (iCol == 1) {
		LPCTSTR cSet;
		// �������� �� ������������ �������
		cSet = " \t~`!@#$%^&*()-+=|\\<>?/,";
		if (data.FindOneOf(cSet) >= 0) {
			strErr = GetErrMsg(ER_BAD_VAL, data);
			return FALSE;
		}
		// ��� ���� �� ����� ���������� � �����
		cSet = "1234567890";
		if (data.Left(1).FindOneOf(cSet) >= 0) {
			strErr = GetErrMsg(ER_BAD_VAL, data);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CTrnTabPage::CheckRowData()
{
	CString	strErr;
	CCellID	cell = m_Grid.GetFocusCell();
	if (cell.row >= m_Grid.GetFixedRowCount())
	{
		// �������� ����� ���� - �����������
		CString sFld = m_Grid.GetItemText(cell.row, 1);
		if (sFld.IsEmpty())
		{
			strErr = GetErrMsg(ER_NEED_FIELD);
			cell.col = 1;
			goto err;
		}
		// �������� ���� �� ������������
		if ((m_Grid.FindRow(1, sFld, 1, cell.row - 1) > 0) ||
			(m_Grid.FindRow(1, sFld, cell.row + 1,
			 m_Grid.GetRowCount() - 1) > 0))
		{
			cell.col = 1;
			strErr = GetErrMsg(ER_DOUBLE_FIELD, sFld);
			goto err;
		}
		// �������� �� ���� ������
		CString sType = m_Grid.GetItemText(cell.row, 3);
		// �������� �����
		if ((sType == "N"   ||
			 sType == "DC"  ||
			 sType == "F"   ||
			 sType == "A"   ||
			 sType == "VA"  ||
			 sType == "B"   ||
			 sType == "VB") &&
			(m_Grid.GetItemText(cell.row, 4).IsEmpty() ||
			 m_Grid.GetItemText(cell.row, 4) == "0"))
		{
			strErr = GetErrMsg(ER_NEED_VALUE);
			cell.col = 4;
			goto err;
		}
		// �������� ��������
		if ((sType == "N"   ||
			 sType == "DC") &&
			m_Grid.GetItemText(cell.row, 5).IsEmpty())
			m_Grid.SetItemText(cell.row, 5, "0");
	}
	return TRUE;

err:// ��������� ������� �������
	m_Grid.SetFocusCell(cell);
	// ����� ������ ��������������
	m_Grid.BeginEditting();
	// ��������� �� ������
	MessageBeep(MB_ICONEXCLAMATION);
	AfxMessageBox(strErr, MB_ICONEXCLAMATION);
	return FALSE;
}

void CTrnTabPage::OnDropdownDstTable() 
{
	CString	strTable,strErr;

	// ������� ������ � ����� �����
	UpdateData(TRUE);
	// ������� ����������� ������
	strTable = m_sTable;
	for (int i = m_cbTable.GetCount() - 1; i >= 0; i--)
	{
		m_cbTable.DeleteString(i);
	}
	// ������������ �������
	m_sTable = strTable;
	UpdateData(FALSE);
	// ���������� ������ ������
	if (m_db == NULL)
		m_db = new CDatabase;
	if (!m_db->IsOpen())
	{
		// ���������� � ���������� ODBC
		BeginWaitCursor();
		m_db->SetLoginTimeout(5);
		try
		{
			m_db->OpenEx(m_sODBCDSN, CDatabase::noOdbcDialog);
		}
		catch (CDBException* e)
		{
			EndWaitCursor();
			// ��������� �� ������
			strErr = m_sODBCDSN + "\n" + e->m_strError;
			strErr = GetErrMsg(ER_CONN_DST, strErr);
			PostMessage(WM_USER, 0, (LPARAM)LPCTSTR(strErr));
			return;
		}
	}
	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, 
		m_db->m_hdbc, &hstmt) != SQL_SUCCESS)
	{
		EndWaitCursor();
		// ��������� �� ������
		strErr = GetErrMsg(ER_CRT_SQL);;
		PostMessage(WM_USER, 0, (LPARAM)LPCTSTR(strErr));
		return;
	}
	// ����������� ��������� ��������� ��� ������
	SDWORD lValue = SQLGetInfoLong(m_db->m_hdbc, SQL_OWNER_USAGE);
	// ���� �������������� �������� ��� ������
	if (lValue & SQL_OU_TABLE_DEFINITION)
	{
		CString sOwner = (m_sOwner.IsEmpty())?SQLGetInfoString(m_db->m_hdbc,SQL_USER_NAME):m_sOwner;

		// ������ �� ������ ������ ��������� sOwner
		if (SQLTables(hstmt, NULL, 0, 
			(SQLCHAR*)LPCTSTR(sOwner), SQL_NTS, NULL, 0, 
			(SQLCHAR*)"TABLE", SQL_NTS) != SQL_SUCCESS)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			EndWaitCursor();
			// ��������� �� ������
			strErr = GetErrMsg(ER_DESC_CATTAB);
			PostMessage(WM_USER, 0, (LPARAM)LPCTSTR(strErr));
			return;
		}
	}
	else
	{
		// ������ �� ������ ������ ������
		if (SQLTables(hstmt, NULL, 0, NULL, 0, NULL, 0, 
			(SQLCHAR*)"TABLE", SQL_NTS) != SQL_SUCCESS)
		{
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			EndWaitCursor();
			// ��������� �� ������
			strErr = GetErrMsg(ER_DESC_CATTAB);
			PostMessage(WM_USER, 0, (LPARAM)LPCTSTR(strErr));
			return;
		}
	}
	SQLCHAR*	szCatalog;
	SQLCHAR*	szSchema;
	SQLCHAR*	szTable;
	SQLINTEGER	nmCatalog, nmSchema, nmTable;
	// ����������� ������������ ����� �����
	nmCatalog = SQLGetInfoShort(m_db->m_hdbc, SQL_MAX_CATALOG_NAME_LEN);
	nmSchema  = SQLGetInfoShort(m_db->m_hdbc, SQL_MAX_SCHEMA_NAME_LEN);
	nmTable   = SQLGetInfoShort(m_db->m_hdbc, SQL_MAX_TABLE_NAME_LEN);
	// ��������� ������ ��� �����
	szCatalog = (SQLCHAR *) malloc((nmCatalog+1) * sizeof(SQLCHAR));
	szSchema = (SQLCHAR *) malloc((nmSchema+1) * sizeof(SQLCHAR));
	szTable = (SQLCHAR *) malloc((nmTable+1) * sizeof(SQLCHAR));
	// ������������� �����
	SQLBindCol(hstmt, 1, SQL_C_CHAR, szCatalog, nmCatalog+1, &nmCatalog);
	SQLBindCol(hstmt, 2, SQL_C_CHAR, szSchema, nmSchema+1, &nmSchema);
	SQLBindCol(hstmt, 3, SQL_C_CHAR, szTable, nmTable+1, &nmTable);
	// ���� �� ������ ������
	while (SQLFetch(hstmt) == SQL_SUCCESS)
	{
		// ���������� ������� � ������
		m_cbTable.AddString(CString(szTable));
	}
	// ����������� ����������� SQL ���������
	SQLFreeStmt(hstmt, SQL_UNBIND);
	SQLFreeStmt(hstmt, SQL_DROP);
	// ������������ ������
	free(szCatalog);
	free(szSchema);
	free(szTable);
	EndWaitCursor();
}

LRESULT CTrnTabPage::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	if (lParam != NULL && lParam != 0)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		LPCTSTR szErr = (LPCTSTR)lParam;
		AfxMessageBox(szErr);
	}
	return 0;
}

void CTrnTabPage::OnSize(UINT nType, int cx, int cy) 
{
	CResizablePage::OnSize(nType, cx, cy);
	
	m_Grid.ExpandColumnsToFit();
}
