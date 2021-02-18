// TrnGenPg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "TrnGenPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrnGenPage property page

IMPLEMENT_DYNCREATE(CTrnGenPage, CResizablePage)

CTrnGenPage::CTrnGenPage() : CResizablePage(CTrnGenPage::IDD)
{
	//{{AFX_DATA_INIT(CTrnGenPage)
	m_nSchema = 0;
	m_nConvert = 0;
	m_sName = _T("");
	m_sMsg = _T("");
	m_sTabSQL = _T("");
	m_sSubSQL = _T("");
	//}}AFX_DATA_INIT
	m_sODBCDSN = _T("");
	m_db = NULL;
}

CTrnGenPage::~CTrnGenPage()
{
	if (m_db != NULL)
	{
		if (m_db->IsOpen())
			m_db->Close();
		delete m_db;
	}
}

void CTrnGenPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrnGenPage)
	DDX_CBIndex(pDX, IDC_SCHEMA, m_nSchema);
	DDX_CBIndex(pDX, IDC_CONVERT, m_nConvert);
	DDX_Text(pDX, IDC_TRNNAME, m_sName);
	DDX_Text(pDX, IDC_STRMSG, m_sMsg);
	DDX_Text(pDX, IDC_TAB_SQL, m_sTabSQL);
	DDX_Text(pDX, IDC_SUB_SQL, m_sSubSQL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrnGenPage, CResizablePage)
	//{{AFX_MSG_MAP(CTrnGenPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrnGenPage message handlers


BOOL CTrnGenPage::OnKillActive() 
{
	UpdateData();
	CString	strErr;
	if (m_sName.IsEmpty())
	{
		strErr = GetErrMsg(ER_NEED_NAME);
		goto err;
	}
	if (m_sTabSQL.IsEmpty())
	{
		strErr = GetErrMsg(ER_NEED_SQLTAB);
		goto err;
	}
	if (!CheckSQLSelect(m_sTabSQL))
		return FALSE;
	if (m_nSchema == TRN_COMPLEX)
	{
		if (m_sSubSQL.IsEmpty())
		{
			strErr = GetErrMsg(ER_NEED_SQLSUB);
			goto err;
		}
		if (!CheckSQLSelect(m_sSubSQL))
			return FALSE;
	}
	else
	{
		if (!m_sSubSQL.IsEmpty())
		{
			strErr = GetErrMsg(ER_IGNORE_SQLSUB);
			goto err;
		}
	}
	return CResizablePage::OnKillActive();

err:
	MessageBeep(MB_ICONEXCLAMATION);
	AfxMessageBox(strErr, MB_ICONEXCLAMATION);
	return FALSE;
}

BOOL CTrnGenPage::CheckSQLSelect(CString strSQL)
{
	// Проверка SQL-оператора
	CString	tmp,strErr;
	BOOL	bRet = TRUE;
	int		i, n;

	strSQL.MakeLower();
	// Проверка выражения SELECT
	if (m_nSchema == TRN_COPY ||
		m_nSchema == TRN_COMPLEX)
	{
		if ((i = strSQL.Find(_T("select"))) >= 0)
		{
			// Проверка следующей лексемы
			n = GetLex(strSQL, i+6, tmp);
			// Проверка на префикс таблицы
			if (strSQL.Mid(n,1) == _T("."))
				n = GetLex(strSQL, n+1, tmp);
			if (tmp != _T("*"))
			{
				strErr = GetErrMsg(ER_MAIN_TABLE);
				bRet = FALSE; goto err;
			}
			// Проверка следующей лексемы
			GetLex(strSQL, n, tmp);
			if (tmp == _T(","))
			{
				strErr = GetErrMsg(ER_MAIN_TABLE);
				bRet = FALSE; goto err;
			}
		}
	}
	// Разбор ссылок и замена их на '?'
	if (!ParseRefExp(strSQL, strErr))
		goto err;
	// Соединение с источником ODBC
	BeginWaitCursor();
	if (m_db == NULL)
		m_db = new CDatabase;
	if (!m_db->IsOpen())
	{
		m_db->SetLoginTimeout(5);
		try
		{
			m_db->OpenEx(m_sODBCDSN, 
			CDatabase::noOdbcDialog);
		}
		catch (CDBException* e)
		{
			strErr = m_sODBCDSN + "\n" + e->m_strError;
			strErr = GetErrMsg(ER_CONN_SRC, strErr);
			goto err;
		}
	}
	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, 
		m_db->m_hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		goto err;
	}
	// Подготовка SQL-оператора
	if (SQLPrepare(hstmt, (SQLTCHAR*)LPCTSTR(strSQL), 
		SQL_NTS) == SQL_ERROR)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		bRet = FALSE;
		goto err;
	}
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	EndWaitCursor();
	return TRUE;

err:
	// Сообщение об ошибке
	EndWaitCursor();
	MessageBeep(MB_ICONEXCLAMATION);
	AfxMessageBox(strErr);
	return bRet;
}

void CTrnGenPage::OnOK() 
{
#ifdef _TRIAL_VER
	if (m_nSchema == TRN_COMPLEX)
	{
		CString sErr;
		// Ограничение по схеме в Trial версии
		sErr.FormatMessage(IDS_ER_TRIAL_COMPLEX);
		sErr.Insert(0, ":\n");
		sErr.Insert(0, LPCTSTR(m_sName));
		AfxMessageBox(sErr);
	}
#endif

	CResizablePage::OnOK();
}

BOOL CTrnGenPage::OnInitDialog() 
{
	CResizablePage::OnInitDialog();

	AddAnchor(IDC_TRNNAME, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_SC_TEXT, TOP_RIGHT);
	AddAnchor(IDC_SCHEMA,  TOP_RIGHT);
	AddAnchor(IDC_CV_TEXT, TOP_RIGHT);
	AddAnchor(IDC_CONVERT, TOP_RIGHT);
	AddAnchor(IDC_STRMSG, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_TAB_SQL, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_SUB_TXT, BOTTOM_LEFT);
	AddAnchor(IDC_SUB_SQL, BOTTOM_LEFT, BOTTOM_RIGHT);

	return TRUE;
}
