// TrnCmdPg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnCmdPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnCmdPage property page

IMPLEMENT_DYNCREATE(CTrnCmdPage, CResizablePage)

CTrnCmdPage::CTrnCmdPage(UINT nIDCaption) : CResizablePage(CTrnCmdPage::IDD, nIDCaption)
{
	//{{AFX_DATA_INIT(CTrnCmdPage)
	//}}AFX_DATA_INIT
	m_sTabCmd = _T("");
}

CTrnCmdPage::~CTrnCmdPage()
{
}

void CTrnCmdPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnCmdPage)
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_TABLE_CMD, m_Grid);
}


BEGIN_MESSAGE_MAP(CTrnCmdPage, CResizablePage)
	//{{AFX_MSG_MAP(CTrnCmdPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnCmdPage message handlers

BOOL CTrnCmdPage::OnInitDialog() 
{
	CResizablePage::OnInitDialog();

	AddAnchor(IDC_ADD,    TOP_RIGHT);
	AddAnchor(IDC_INSERT, TOP_RIGHT);
	AddAnchor(IDC_DELETE, TOP_RIGHT);
	AddAnchor(IDC_TABLE_CMD, TOP_LEFT, BOTTOM_RIGHT);

	// Заполнение списка команд
	FillCommandsList();

	return TRUE;
}

BOOL CTrnCmdPage::OnKillActive() 
{
	// Принятие данных
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return FALSE;
	}
	// Проверка данных в строке
	if (!CheckRowData()) return FALSE;
	// Формирование строки списка команд
	int		nFirst = m_Grid.GetFixedRowCount();
	int		nCount = m_Grid.GetRowCount() - 1;
	int		nFound;
	CString	sCmd;
	// OnStart
	if ((nFound = m_Grid.FindRow(2, "OnStart", nFirst, nCount)) > 0)
		sCmd += "OnStart(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// OnEnd
	if ((nFound = m_Grid.FindRow(2, "OnEnd", nFirst, nCount)) > 0)
		sCmd += "OnEnd(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// CallExec
	if ((nFound = m_Grid.FindRow(2, "CallExec", nFirst, nCount)) > 0)
		sCmd += "CallExec(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// CallBack
	if ((nFound = m_Grid.FindRow(2, "CallBack", nFirst, nCount)) > 0)
		sCmd += "CallBack(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// OnInsert
	if ((nFound = m_Grid.FindRow(2, "OnInsert", nFirst, nCount)) > 0)
		sCmd += "OnInsert(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// OnUpdate
	if ((nFound = m_Grid.FindRow(2, "OnUpdate", nFirst, nCount)) > 0)
		sCmd += "OnUpdate(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// OnDelete
	if ((nFound = m_Grid.FindRow(2, "OnDelete", nFirst, nCount)) > 0)
		sCmd += "OnDelete(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// ReInsert
	if ((nFound = m_Grid.FindRow(2, "ReInsert", nFirst, nCount)) > 0)
		sCmd += "ReInsert(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// ReUpdate
	if ((nFound = m_Grid.FindRow(2, "ReUpdate", nFirst, nCount)) > 0)
		sCmd += "ReUpdate(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// ReDelete
	if ((nFound = m_Grid.FindRow(2, "ReDelete", nFirst, nCount)) > 0)
		sCmd += "ReDelete(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
	// OnChange
	while ((nFound = m_Grid.FindRow(2, "OnChange", nFirst, nCount)) > 0)
	{
		sCmd += "OnChange." + m_Grid.GetItemText(nFound, 3) + 
				"(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
		nFirst = nFound + 1;
	}
	// ReChange
	while ((nFound = m_Grid.FindRow(2, "ReChange", nFirst, nCount)) > 0)
	{
		sCmd += "ReChange." + m_Grid.GetItemText(nFound, 3) + 
				"(" + m_Grid.GetItemText(nFound, 1) + ")\r\n";
		nFirst = nFound + 1;
	}
	sCmd.TrimRight();
	m_sTabCmd = sCmd;

	return CResizablePage::OnKillActive();
}

BOOL CTrnCmdPage::OnSetActive() 
{
	if (m_Grid.GetFocusCell().row < 0)
		return TRUE;
	// Вызов режима редактирования
	m_Grid.BeginEditting();

	return CResizablePage::OnSetActive();
}

void CTrnCmdPage::OnAdd() 
{
	// Принятие данных
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return;
	}
	// Проверка данных в строке
	if (!CheckRowData()) return;
	// Вставка строки
	int		row = m_Grid.GetRowCount();
	CString	tmp;
	tmp.Format("%i", row);
	row = m_Grid.InsertRow(tmp);
	m_Grid.SetRowHeight(row, m_nRowHeight);
	m_Grid.ResetScrollBars();
	// Установка начального значения
	m_Grid.SetItemText(row, 2, "OnStart");
	// Запрещение ввода имени поля
	UINT stFld = m_Grid.GetItemState(row, 3);
	m_Grid.SetItemState(row, 3, stFld|GVIS_READONLY);
	// Установка текущей строки
	m_Grid.SetFocusCell(row, 1);
	m_Grid.SetSelectedRange(row, 1, row, m_nColCount - 1);
	m_Grid.Invalidate();
	// Вызов режима редактирования
	m_Grid.BeginEditting();
}

void CTrnCmdPage::OnInsert() 
{
	// Принятие данных
	if (!m_Grid.EndEditting())
	{
		m_Grid.SetFocus();
		return;
	}
	// Проверка данных в строке
	if (!CheckRowData()) return;
	// Вставка строки
	CCellID	cell = m_Grid.GetFocusCell();
	int		row;
	CString	tmp;
	tmp.Format("%i", cell.row);
	row = m_Grid.InsertRow(tmp, cell.row);
	m_Grid.SetRowHeight(row, m_nRowHeight);
	m_Grid.ResetScrollBars();
	// Установка начального значения
	m_Grid.SetItemText(row, 2, "OnStart");
	// Запрещение ввода имени поля
	UINT stFld = m_Grid.GetItemState(row, 3);
	m_Grid.SetItemState(row, 3, stFld|GVIS_READONLY);
	// Установка текущей строки
	m_Grid.SetFocusCell(row, 1);
	m_Grid.SetSelectedRange(row, 1, row, m_nColCount - 1);
	// Перенумерация строк
	int	nCount = m_Grid.GetRowCount();
	for (; row < nCount; row++) {
		tmp.Format("%i", row);
		m_Grid.SetItemText(row, 0, tmp);
	}
	m_Grid.Invalidate();
	// Вызов режима редактирования
	m_Grid.BeginEditting();
}

void CTrnCmdPage::OnDelete() 
{
	CCellID	cell = m_Grid.GetFocusCell();
	if (cell.row < 0) return;

	// Завершение редактирования
	m_Grid.EndEditting(FALSE);
	// Удаление строки
	m_Grid.DeleteRow(cell.row, FALSE);
	// Перенумерация строк
	int		nFirst = m_Grid.GetFixedRowCount();
	int		nCount = m_Grid.GetRowCount();
	CString	tmp;
	for (int row = nFirst; row < nCount; row++) {
		tmp.Format("%i", row);
		m_Grid.SetItemText(row, 0, tmp);
	}
    m_Grid.Invalidate();
	// Установка текущей строки
	if (cell.row == nCount) cell.row--;
	if (cell.row >= nFirst)
	{
		m_Grid.SetFocusCell(cell.row, cell.col);
		m_Grid.SetSelectedRange(cell.row, 1, cell.row, m_nColCount - 1);
		// Вызов режима редактирования
		m_Grid.BeginEditting();
	}
}

BOOL CTrnCmdPage::FillCommandsList()
{
	CString	tmp;
	m_nRowHeight = 32;
	// Установка параметров таблицы
	m_Grid.SetEditMode(GVEM_FULL_LIST);
	// Установка колонок таблицы
	CString	sNum,sCmd,sEvt,sFld;
	sNum.LoadString(TAB_FLD_COLNUM);
	sCmd.LoadString(TAB_FLD_COMMAND);
	sEvt.LoadString(TAB_FLD_EVENT);
	sFld.LoadString(TAB_FLD_COLNAME);
	m_Grid.DeleteAllItems();
	m_Grid.InsertColumn(sNum);
	m_Grid.InsertColumn(sCmd, DT_LEFT);
	m_Grid.InsertColumn(sEvt, DT_CENTER);
	m_Grid.InsertColumn(sFld, DT_LEFT);
	m_Grid.SetColumnWidth(0, 30);
	m_Grid.SetColumnWidth(1, 240);
	m_Grid.SetColumnWidth(2, 80);
	m_Grid.SetColumnWidth(3, 100);
	// Установка стиля колонок
	m_Grid.SetColumnEditStyle(1, GVCE_EDIT, GVCS_MULTILINE);
	m_Grid.SetColumnEditStyle(2, GVCE_LISTBOX);
	m_Grid.SetColumnEditStyle(3, GVCE_EDIT);
	// Установка списка событий
	tmp  = "OnStart\nOnEnd\nCallExec\nCallBack\n";
	tmp += "OnInsert\nOnUpdate\nOnDelete\n";
	tmp += "ReInsert\nReUpdate\nReDelete\n";
	tmp += "OnChange\nReChange";
	m_Grid.SetColumnProp(2, GVCV_VALUES, tmp);
	// Фиксация заголовков
	m_Grid.SetRowCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnCount(1);
	m_nColCount = m_Grid.GetColumnCount();

	// Заполнение массива команд
	CCmdArray arCmd;
	if (!FillCmdArray(m_sTabCmd, arCmd))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		CString strErr = GetErrMsg(ER_SNTX_CMDTAB);
		AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
	}
	// Заполнение данных в списке
	int	row = m_Grid.GetFixedRowCount();
	int	i, n = arCmd.GetSize();
	for (i = 0; i < n; i++, row++)
	{
		tmp.Format("%i", row);
		row = m_Grid.InsertRow(tmp);
		m_Grid.SetItemText(row, 1, arCmd[i].sCmd);
		m_Grid.SetItemText(row, 2, arCmd[i].sEvent);
		m_Grid.SetItemText(row, 3, arCmd[i].sField);
		if (arCmd[i].iEvent != EV_ON_CHANGE)
			m_Grid.SetItemState(row, 3, GVIS_READONLY);
	}
	// Установка размера строк
	m_Grid.AutoSizeRows();
	// Установка текущей строки
	if (m_Grid.GetRowCount() > 1) {
		m_Grid.SetFocusCell(1, 1);
		m_Grid.SetSelectedRange(1, 1, 1, m_nColCount - 1);
	}
	return TRUE;
}

BOOL CTrnCmdPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// Обработка сообщений от списка
	if (wParam == IDC_TABLE_CMD)
	{
	    NM_GRIDVIEW	*nmgv = (NM_GRIDVIEW *)lParam;
		// Проверка данных в ячейке
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
		// Проверка данных в строке
		if (nmgv->hdr.code == GVN_ROWCHANGING)
		{
			if (!CheckRowData()) {
				*pResult = 1;
				return TRUE;
			}
		}
		// Установка имени поля
		// в зависимости от условия
		if (nmgv->hdr.code == GVN_EDITSELENDOK &&
			nmgv->iCol == 2)
		{
			CheckCondition(nmgv->iRow, nmgv->szText);
			m_Grid.Invalidate();
		}
	}

	return CResizablePage::OnNotify(wParam, lParam, pResult);
}

BOOL CTrnCmdPage::CheckCondition(int iRow, CString sCond)
{
	if (!m_Grid.IsValid(iRow, 0))
		return FALSE;

	UINT stFld = m_Grid.GetItemState(iRow, 3);
	if (sCond == "OnChange" || sCond == "ReChange")
	{
		// Разрешение имени поля
		m_Grid.SetItemState(iRow, 3, stFld & ~GVIS_READONLY);
	}
	else
	{
		// Запрещение имени поля
		m_Grid.SetItemState(iRow, 3, stFld | GVIS_READONLY);
		m_Grid.SetItemText(iRow, 3, "");
	}
	return TRUE;
}

BOOL CTrnCmdPage::CheckCellData(int iRow, int iCol, CString data, CString &strErr)
{
	// Проверка имени поля
	if (iCol == 3) {
		LPCTSTR cSet;
		// Проверка на недопустимые символы
		cSet = " \t~`!@#$%^&*()-+=|\\<>?/,.";
		if (data.FindOneOf(cSet) >= 0) {
			strErr = GetErrMsg(ER_BAD_VAL, data);
			return FALSE;
		}
		// Имя поля не может начинаться с цифры
		cSet = "1234567890";
		if (data.Left(1).FindOneOf(cSet) >= 0) {
			strErr = GetErrMsg(ER_BAD_VAL, data);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CTrnCmdPage::CheckRowData()
{
	int		nRow;
	CString	strErr;
	CCellID	cell = m_Grid.GetFocusCell();
	if (cell.row >= m_Grid.GetFixedRowCount())
	{
		// Значение команды - обязательно
		if (m_Grid.GetItemText(cell.row, 1).IsEmpty())
		{
			strErr = GetErrMsg(ER_NEED_COMMAND);
			cell.col = 1;
			goto err;
		}
		CString	cond = m_Grid.GetItemText(cell.row, 2);
		CString	sfld = m_Grid.GetItemText(cell.row, 3);
		if (cond == "OnChange" || cond == "ReChange")
		{
			// Имя поля - обязательно
			if (m_Grid.GetItemText(cell.row, 3).IsEmpty())
			{
				strErr = GetErrMsg(ER_NEED_FIELD);
				cell.col = 3;
				goto err;
			}
			// Проверка имени поля на уникальность
			// Поиск с 1 строки до текущей
			if ((nRow = m_Grid.FindRow(3, sfld, 1, cell.row - 1)) > 0)
			{
				// Если условия совпадают
				if (m_Grid.GetItemText(nRow, 2) == cond)
				{
					cell.col = 3;
					strErr = GetErrMsg(ER_DOUBLE_FIELD, sfld);
					goto err;
				}
				// Если найдено еще раз...
				else if (m_Grid.FindRow(3, sfld, 1, nRow - 1) > 0)
				{
					cell.col = 3;
					strErr = GetErrMsg(ER_DOUBLE_FIELD, sfld);
					goto err;
				}
			}
			// Поиск с текущей строки до конца
			else if ((nRow = m_Grid.FindRow(3, sfld, cell.row + 1,
							 m_Grid.GetRowCount() - 1)) > 0)
			{
				// Если условия совпадают
				if (m_Grid.GetItemText(nRow, 2) == cond)
				{
					cell.col = 3;
					strErr = GetErrMsg(ER_DOUBLE_FIELD, sfld);
					goto err;
				}
				// Если найдено еще раз...
				else if (m_Grid.FindRow(3, sfld, nRow + 1, 
						 m_Grid.GetRowCount() - 1) > 0)
				{
					cell.col = 3;
					strErr = GetErrMsg(ER_DOUBLE_FIELD, sfld);
					goto err;
				}
			}
		}
		else {
			// Проверка условия на уникальность
			if ((m_Grid.FindRow(2, cond, 1, cell.row - 1) > 0) ||
				(m_Grid.FindRow(2, cond, cell.row + 1,
				m_Grid.GetRowCount() - 1) > 0))
			{
				cell.col = 2;
				strErr = GetErrMsg(ER_DOUBLE_COND, cond);
				goto err;
			}
		}
	}
	return TRUE;

err:// Установка текущей колонки
	m_Grid.SetFocusCell(cell);
	// Вызов режима редактирования
	m_Grid.BeginEditting();
	// Сообщение об ошибке
	MessageBeep(MB_ICONEXCLAMATION);
	AfxMessageBox(strErr, MB_ICONEXCLAMATION);
	return FALSE;
}

void CTrnCmdPage::OnSize(UINT nType, int cx, int cy) 
{
	CResizablePage::OnSize(nType, cx, cy);
	
	m_Grid.ExpandColumnsToFit();
}
