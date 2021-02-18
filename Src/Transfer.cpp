// Transfer.cpp: implementation of the CTransfer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Transfer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CTransfer, CObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransfer::CTransfer()
{
	nKind      = 0;
	pDlg       = NULL;
	pSrc       = NULL;
	pDst       = NULL;
	pTrn       = NULL;
	pPrm       = NULL;
	bValid     = FALSE;
	bShowErr   = TRUE;
	bBreaked   = FALSE;
	bComplex   = FALSE;
	bMessage   = FALSE;
	bTxnCommit = FALSE;
	bTabSearch = FALSE;
	bSubSearch = FALSE;
	bTabRowCmd = FALSE;
	bSubRowCmd = FALSE;
	bTabFldCmd = FALSE;
	bSubFldCmd = FALSE;
	hSrcTabSQL = SQL_NULL_HSTMT;
	hDstTabSQL = SQL_NULL_HSTMT;
	hDstTabIns = SQL_NULL_HSTMT;
	hDstTabUpd = SQL_NULL_HSTMT;
	hDstTabDel = SQL_NULL_HSTMT;
	hSrcSubSQL = SQL_NULL_HSTMT;
	hDstSubSQL = SQL_NULL_HSTMT;
	hDstSubIns = SQL_NULL_HSTMT;
	hDstSubUpd = SQL_NULL_HSTMT;
	hDstSubDel = SQL_NULL_HSTMT;
}

CTransfer::CTransfer(int nKindTrans,
					 ODBC_INFO *pSrcInfo,
					 ODBC_INFO *pDstInfo,
					 TRN_INFO  *pTrnInfo,
					 CFldArray *pArrParm)
{
	nKind      = nKindTrans;
	pDlg       = NULL;
	pSrc       = pSrcInfo;
	pDst       = pDstInfo;
	pTrn       = pTrnInfo;
	pPrm       = pArrParm;
	bValid     = FALSE;
	bShowErr   = TRUE;
	bBreaked   = FALSE;
	bComplex   = FALSE;
	bMessage   = FALSE;
	bTxnCommit = FALSE;
	bTabSearch = FALSE;
	bSubSearch = FALSE;
	bTabRowCmd = FALSE;
	bSubRowCmd = FALSE;
	bTabFldCmd = FALSE;
	bSubFldCmd = FALSE;
	hSrcTabSQL = SQL_NULL_HSTMT;
	hDstTabSQL = SQL_NULL_HSTMT;
	hDstTabIns = SQL_NULL_HSTMT;
	hDstTabUpd = SQL_NULL_HSTMT;
	hDstTabDel = SQL_NULL_HSTMT;
	hSrcSubSQL = SQL_NULL_HSTMT;
	hDstSubSQL = SQL_NULL_HSTMT;
	hDstSubIns = SQL_NULL_HSTMT;
	hDstSubUpd = SQL_NULL_HSTMT;
	hDstSubDel = SQL_NULL_HSTMT;
}

CTransfer::~CTransfer()
{
	// Уничтожение дексрипторов SQL
	FreeAllStmt();
	// Очистка данных в массивах
	ClearAllArr();
	// Удаление диалога
	if (pDlg != NULL) delete pDlg;
}

/////////////////////////////////////////////////////////////////////
// Обработка ошибки трансферта

void CTransfer::ErrorHandle(ER_TYPE errType, CString &strErr, UINT &nErr)
{
	// Параметры:
	// errType - тип ошибки
	// strErr  - строка ошибки
	// nErr    - счетчик ошибок

	// Увеличить счетчик ошибок
	nErr++;
	// Получить полный текст ошибки
	strErr = GetErrMsg(errType, strErr);
	// Форматирование текста ошибки
	strErr = FormatErr(strErr);
	// Запись текста ошибки в файл
	if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
	// Показ сообщения об ошибке
	if (bShowErr) ShowErr(pTrn->sName, strErr);
}

/////////////////////////////////////////////////////////////////////
// Обработка предупреждения трансферта

void CTransfer::WarningHandle(CString &strErr, UINT &nMsg)
{
	// Параметры:
	// strErr  - строка предупреждения
	// nErr    - счетчик предупреждений

	// Увеличить счетчик предупреждений
	nMsg++;
	if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
}

/////////////////////////////////////////////////////////////////////
// Освобождение всех дексрипторов SQL

void CTransfer::FreeAllStmt() 
{
	FreeSQLStmt(hSrcTabSQL);
	FreeSQLStmt(hDstTabSQL);
	FreeSQLStmt(hDstTabIns);
	FreeSQLStmt(hDstTabUpd);
	FreeSQLStmt(hDstTabDel);
	FreeSQLStmt(hSrcSubSQL);
	FreeSQLStmt(hDstSubSQL);
	FreeSQLStmt(hDstSubIns);
	FreeSQLStmt(hDstSubUpd);
	FreeSQLStmt(hDstSubDel);
}

/////////////////////////////////////////////////////////////////////
// Очистка данных во всех массивах

void CTransfer::ClearAllArr() 
{
	// Очистка массивов полей и параметров
	ClearFldArr(arStrMsgPrm);
	ClearFldArr(arSrcTabPrm);
	ClearFldArr(arSrcTabFld);
	ClearFldArr(arSrcSubQry);
	ClearFldArr(arSrcSubPrm);
	ClearFldArr(arSrcSubFld);
	ClearFldArr(arDstTabPrm);
	ClearFldArr(arDstTabFld);
	ClearFldArr(arDstSubQry);
	ClearFldArr(arDstSubPrm);
	ClearFldArr(arDstSubFld);
	ClearFldArr(arTabInsPrm);
	ClearFldArr(arSubInsPrm);
	ClearFldArr(arTabUpdPrm);
	ClearFldArr(arSubUpdPrm);
	ClearFldArr(arTabDelPrm);
	ClearFldArr(arSubDelPrm);
	// Очистка буферов строк
	ClearBufArr(arTabRowBuf);
	ClearBufArr(arSubRowBuf);
	// Очистка массивов команд
	ClearCmdArr(arDstTabCmd);
	ClearCmdArr(arDstSubCmd);
}

/////////////////////////////////////////////////////////////////////
// Очистка массива полей (параметров)

void CTransfer::ClearFldArr(CFldArray &arFld) 
{
	if (arFld.GetSize() == 0) return;

	for (int i = arFld.GetSize() - 1; i >= 0; i--)
	{
		arFld[i].vData.Clear();
	}
	arFld.RemoveAll();
	arFld.FreeExtra();
	arFld.SetSize(0);
}

/////////////////////////////////////////////////////////////////////
// Очистка буфера строк

void CTransfer::ClearBufArr(CBufArray &arBuf) 
{
	if (arBuf.GetSize() == 0) return;

	for (int i = arBuf.GetSize() - 1; i >= 0; i--)
	{
		if (arBuf[i].pArSrcFld != NULL)
		{
			// Удаление данных источника
			ClearFldArr(*arBuf[i].pArSrcFld);
			delete arBuf[i].pArSrcFld;
			arBuf[i].pArSrcFld = NULL;
		}
		if (arBuf[i].pArDstFld != NULL)
		{
			// Удаление данных приемника
			ClearFldArr(*arBuf[i].pArDstFld);
			delete arBuf[i].pArDstFld;
			arBuf[i].pArDstFld = NULL;
		}
	}
	arBuf.RemoveAll();
	arBuf.FreeExtra();
	arBuf.SetSize(0);
}

/////////////////////////////////////////////////////////////////////
// Очистка массива команд (и их параметров)

void CTransfer::ClearCmdArr(CCmdArray &arCmd) 
{
	if (arCmd.GetSize() == 0) return;

	for (int i = arCmd.GetSize() - 1; i >= 0; i--)
	{
		// Уничтожение декриптора SQL
		if (arCmd[i].hstmt != SQL_NULL_HSTMT)
		{
			::SQLFreeStmt(arCmd[i].hstmt, SQL_CLOSE);
			::SQLFreeStmt(arCmd[i].hstmt, SQL_UNBIND);
			::SQLFreeStmt(arCmd[i].hstmt, SQL_RESET_PARAMS);
			::SQLFreeStmt(arCmd[i].hstmt, SQL_DROP);
			arCmd[i].hstmt = SQL_NULL_HSTMT;
		}
		// Уничтожение массива подкоманд
		if (arCmd[i].pArCmd != NULL)
		{
			arCmd[i].pArCmd->RemoveAll();
			arCmd[i].pArCmd->FreeExtra();
			delete arCmd[i].pArCmd;
			arCmd[i].pArCmd = NULL;
		}
		// Уничтожение массива параметров
		if (arCmd[i].pArPrm != NULL)
		{
			for (int j = arCmd[i].pArPrm->GetSize() - 1; j >= 0; j--)
			{
				if (arCmd[i].pArPrm->GetAt(j) != NULL)
				{
					ClearFldArr(*arCmd[i].pArPrm->ElementAt(j));
					delete arCmd[i].pArPrm->ElementAt(j);
					arCmd[i].pArPrm->ElementAt(j) = NULL;
				}
			}
			arCmd[i].pArPrm->RemoveAll();
			arCmd[i].pArPrm->FreeExtra();
			delete arCmd[i].pArPrm;
			arCmd[i].pArPrm = NULL;
		}
	}
	arCmd.RemoveAll();
	arCmd.FreeExtra();
	arCmd.SetSize(0);
}

/////////////////////////////////////////////////////////////////////
// Установить значение по умолчанию для элементов массива

void CTransfer::ResetFldArr(CFldArray &arFld) 
{
	for (int i = arFld.GetSize() - 1; i >= 0; i--)
	{
		arFld[i].vData.SetDefault();
	}
}

/////////////////////////////////////////////////////////////////////
// Добавить строку данных в буфер

void CTransfer::AddBufRow(CBufArray &arBuf, CFldArray &arSrcFld, 
						  CFldArray &arDstFld, long nRow, 
						  ROW_STATUS nStatus, BOOL bRunCmd, 
						  BOOL bEvChanged) 
{
	ROW_INFO rowInfo;
	rowInfo.nRow		= nRow;
	rowInfo.nStatus		= nStatus;
	rowInfo.bRunCmd		= bRunCmd;
	rowInfo.bEvChanged	= bEvChanged;
	rowInfo.pArSrcFld	= new CFldArray;
	rowInfo.pArDstFld	= new CFldArray;

	// Перенос данных источника
	int n = arSrcFld.GetSize();
	rowInfo.pArSrcFld->SetSize(n);
	for (int i = 0; i < n; i++)
	{
		rowInfo.pArSrcFld->ElementAt(i).bMod  = arSrcFld[i].bMod;
		rowInfo.pArSrcFld->ElementAt(i).vData = arSrcFld[i].vData;
	}
	// Перенос данных приемника
	int m = arDstFld.GetSize();
	rowInfo.pArDstFld->SetSize(m);
	for (int j = 0; j < m; j++)
	{
		rowInfo.pArDstFld->ElementAt(j).bMod  = arDstFld[j].bMod;
		rowInfo.pArDstFld->ElementAt(j).vData = arDstFld[j].vData;
	}
	// Добавление элемента
	arBuf.Add(rowInfo);
}

/////////////////////////////////////////////////////////////////////
// Освобождение дескриптора SQL оператора

void CTransfer::FreeSQLStmt(SQLHSTMT &hstmt, BOOL bDrop) 
{
	if (hstmt != NULL && hstmt != SQL_NULL_HSTMT)
	{
		::SQLFreeStmt(hstmt, SQL_CLOSE);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
		if (bDrop)
		{
			::SQLFreeStmt(hstmt, SQL_DROP);
			hstmt = SQL_NULL_HSTMT;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Показ сообщения об ошибке

void CTransfer::ShowErr(LPCTSTR szTitle, LPCTSTR szError) 
{
	RETCODE nRet;
	CString strAdd,strErr(szError);
	strAdd.LoadString(IDS_MS_SHOW_ERR);
	strErr += strAdd;
	MessageBeep(MB_ICONEXCLAMATION);
	nRet = AfxGetApp()->m_pMainWnd->MessageBox(strErr,szTitle,
					MB_YESNO|MB_DEFBUTTON1|MB_ICONEXCLAMATION);
	AfxGetApp()->m_pMainWnd->UpdateWindow();
	if (pDlg != NULL) pDlg->UpdateWindow();
	if (nRet == IDNO) bShowErr = FALSE;
}

/////////////////////////////////////////////////////////////////////
// Показ сообщения (с возможностью прерывания)

int CTransfer::ShowMsg(LPCTSTR szTitle, LPCTSTR szMessage) 
{
	RETCODE nRet;
	CString strAdd,strMsg(szMessage);
	strAdd.LoadString(IDS_MS_SHOW_MSG);
	strMsg += strAdd;
	MessageBeep(MB_ICONEXCLAMATION);
	nRet = AfxGetApp()->m_pMainWnd->MessageBox(strMsg, szTitle,
				MB_YESNOCANCEL|MB_DEFBUTTON3|MB_ICONEXCLAMATION);
	AfxGetApp()->m_pMainWnd->UpdateWindow();
	if (pDlg != NULL) pDlg->UpdateWindow();
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// Поиск параметра в массиве по ID

int CTransfer::FindParam(CFldArray &arPrm, LPCTSTR szID) 
{
	int	i, n = arPrm.GetSize();
	for (i = 0; i < n; i++)
	{
		if (arPrm[i].sID.CompareNoCase(szID) == 0)
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////
// Поиск поля в массиве по имени

int CTransfer::FindField(CFldArray &arFld, LPCTSTR szName) 
{
	int	i, n = arFld.GetSize();
	for (i = 0; i < n; i++)
	{
		if (arFld[i].sName.CompareNoCase(szName) == 0)
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////
// Поиск поля в массиве по ссылке на источник

int CTransfer::FindSrcRef(CFldArray &arFld, REF_TYPE iRefType, 
						  LPCTSTR szRef) 
{
	int	i, n = arFld.GetSize();
	for (i = 0; i < n; i++)
	{
		if (iRefType == REF_TAB && arFld[i].iSrc == DATA_TABSRC ||
			iRefType == REF_SUB && arFld[i].iSrc == DATA_SUBSRC)
		{
			if (arFld[i].sValue.CompareNoCase(szRef) == 0)
				return i;
		}
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////
// Поиск команды в массиве только по идентификатору

int CTransfer::FindEvent(CCmdArray &arCmd, EV_TYPE iEventID) 
{
	int	i, n = arCmd.GetSize();
	for (i = 0; i < n; i++)
	{
		if (arCmd[i].iEvent == iEventID) return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////
// Поиск команды в массиве по идентификатору и имени поля

int CTransfer::FindSQLCmd(CCmdArray &arCmd, EV_TYPE iEventID, 
						  LPCTSTR szField) 
{
	int	i, n = arCmd.GetSize();
	for (i = 0; i < n; i++)
	{
		if (arCmd[i].iEvent == iEventID)
		{
			if (iEventID == EV_ON_CHANGE)
			{
				if (arCmd[i].sField.CompareNoCase(szField) != 0)
					continue;
			}
			return i;
		}
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////
// Определение основной таблицы SQL запроса

BOOL CTransfer::TableInSQL(CString strSQL, CString &strTable, 
						   CString &strErr) 
{
	CString	tmp;
	// Прочитать первую лексему
	int	i = GetLex(strSQL, 0, tmp);
	if (tmp == _T("select"))
	{
		// Проверка лексемы после SELECT
		i = GetLex(strSQL, i, strTable);
		// Проверка на выражение <table.*>
		if (strSQL.Mid(i,  1) == _T(".") &&
			strSQL.Mid(i+1,1) == _T("*"))
		{
			// Проверка следующей лексемы
			GetLex(strSQL, i+2, tmp);
			if (tmp == _T(",")) goto err;
		}
		else
		{
			// Получить имя таблицы из FROM
			if ((i = strSQL.Find(_T("from"))) >= 0)
			{
				// Предполагаемая таблица
				i = GetLex(strSQL, i+4, strTable);
				// Проверка следующей лексемы
				GetLex(strSQL, i, tmp);
				if (tmp == _T(",")) goto err;
			}
			else goto err;
		}
	}
	else
	{
		goto err;
	}
	return TRUE;

err:
	strErr = GetErrMsg(ER_MAIN_TABLE);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Сравнение данных в массиве полей (параметров) по ссылке

BOOL CTransfer::CompareRef(CFldArray &arFld, OP_TYPE iOpType, 
						   MOD_FLAG &iMFlag, CString &strErr) 
{
	iMFlag = MOD_NO; // Not modified
	CDataValue dv;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// Пропуск неизменяемых полей
		if ((arFld[i].iFlag == FLD_NOCHANGE) &&
			(iOpType == OP_UPDATE)) continue;

		// Инициализация буфера по типу поля
		dv.GetBuffer(arFld[i].iSQLType, arFld[i].iSize, TRUE);
		// Установка временного значения
		switch (arFld[i].iSrc)		// Тип источника
		{
		case DATA_LOCAL:			// Значение поля
		{
			// Локальное значение не сравнивается
			break;
		}
		case DATA_AUTO:				// Автоинкремент
		{
			// Автоинкрементное значение не сравнивается
			break;
		}
		case DATA_PARAM:			// Параметр
		{
			if (!dv.ConvertInto((LPCTSTR)pPrm->GetAt(arFld[i].iRef).sValue))
			{
				strErr = GetErrMsg(ER_CONV_PARAM, 
									pPrm->GetAt(arFld[i].iRef).sID);
				return FALSE;
			}
			break;
		}
		case DATA_CONST:			// Константа
		{
			strErr = GetErrMsg(ER_CONV_CONST, arFld[i].sValue);
			return FALSE;
		}
		case DATA_TABSRC:			// Основная таблица источника
		{
			if (!dv.ConvertInto(arSrcTabFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arSrcTabFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_SUBSRC:			// Подчиненная таблица источника
		{
			if (!dv.ConvertInto(arSrcSubFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arSrcSubFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_TABDST:			// Основная таблица приемника
		{
			if (!dv.ConvertInto(arDstTabFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstTabFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_SUBDST:			// Подчиненная таблица приемника
		{
			if (!dv.ConvertInto(arDstSubFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstSubFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_FUNC:				// Встроенная функция
		{
			strErr = GetErrMsg(ER_CONV_FUNC, arFld[i].sValue);
			return FALSE;
		}
		case DATA_PROC:				// Системная процедура
		{
			strErr = GetErrMsg(ER_CONV_PROC, arFld[i].sValue);
			return FALSE;
		}
		}
		// Сравнение значений полей
		if (dv != arFld[i].vData)
		{
			// Установка флага модификации поля
			arFld[i].bMod = TRUE;
			// Установка флага модификации строки
			// строго по возрастанию приоритета
			switch (arFld[i].iFlag)
			{
			case FLD_DATA:
			{
				if (iMFlag < MOD_YES) iMFlag = MOD_YES; break;
			}
			case FLD_EVCHANGE:
			{
				if (iMFlag < MOD_CMD) iMFlag = MOD_CMD; break;
			}
			case FLD_FRUNBACK:
			{
				// Флаг выполнения/отката
				// Если значение преобразуется в число,
				// то результат будет отличным от нуля
				if (dv.GetDOUBLE() > arFld[i].vData.GetDOUBLE())
				{
					if (iMFlag < MOD_EXEC) iMFlag = MOD_EXEC;
				}
				else if (dv.GetDOUBLE() < arFld[i].vData.GetDOUBLE())
				{
					if (iMFlag < MOD_BACK) iMFlag = MOD_BACK;
				}
				break;
			}
			case FLD_TRIGGER:
			{
				if (iMFlag < MOD_TRG) iMFlag = MOD_TRG; break;
			}
			default:
			{
				if (iMFlag < MOD_YES) iMFlag = MOD_YES; break;
			}
			}
		}
		else
		{
			arFld[i].bMod = FALSE;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Установка ссылок на приемник для ключевых полей полей

BOOL CTransfer::SetDstRefs(CFldArray &arFld, REF_TYPE iRefType, 
						   BOOL bKeysOnly,   CString &strErr) 
{
	// iRefType - тип ссылки: 0 - таблица, 1 - субтаблица
	int	i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		if (!bKeysOnly || arFld[i].iFlag == FLD_KEY)
		{
			switch (iRefType)
			{
			case REF_TAB:				// Осн.таблица приемника
				arFld[i].iSrc = DATA_TABDST;
				arFld[i].iRef = FindSrcRef(arDstTabFld, iRefType, 
											arFld[i].sName);
				if (arFld[i].iRef >= 0)	// Установка имени поля
					arFld[i].sValue = arDstTabFld.GetAt(
										arFld[i].iRef).sName;
				break;
			case REF_SUB:				// Под.таблица приемника
				arFld[i].iSrc = DATA_SUBDST;
				arFld[i].iRef = FindSrcRef(arDstSubFld, iRefType, 
											arFld[i].sName);
				if (arFld[i].iRef >= 0)	// Установка имени поля
					arFld[i].sValue = arDstSubFld.GetAt(
										arFld[i].iRef).sName;
				break;
			default:					// Осн.таблица приемника
				arFld[i].iSrc = DATA_TABDST;
				arFld[i].iRef = FindSrcRef(arDstTabFld, iRefType, 
											arFld[i].sName);
				if (arFld[i].iRef >= 0)	// Установка имени поля
					arFld[i].sValue = arDstTabFld.GetAt(
										arFld[i].iRef).sName;
				break;
			}
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDDSTREF, arFld[i].sName);
				return FALSE;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Установка ссылок на источник данных в массиве полей

BOOL CTransfer::SetFldRefs(CFldArray &arFld, REF_TYPE iRefType, 
						   CString &strErr) 
{
	// iRefType - тип ссылки: 0 - таблица, 1 - субтаблица
	int	i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		switch (arFld[i].iSrc)		// Тип источника
		{
		case DATA_LOCAL:			// Значение поля
		{
			if (arFld[i].sValue.IsEmpty())
			{
				// Установка значения по умолчанию
				arFld[i].vData.SetDefault();
			}
			else
			{
				// Установка локального значения
				if (!arFld[i].vData.ConvertInto((LPCTSTR)arFld[i].sValue))
				{
					strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
					return FALSE;
				}
			}
			break;
		}
		case DATA_AUTO:				// Автоинкремент
		{
			// Установка значения по умолчанию
			arFld[i].vData.SetDefault();

			break;
		}
		case DATA_PARAM:			// Параметр
		{
			arFld[i].iRef = FindParam(*pPrm, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_PARAM, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_CONST:			// Константа
		{
			strErr = GetErrMsg(ER_FIND_CONST, arFld[i].sValue);
			return FALSE;
		}
		case DATA_REF:				// Ссылка
		{
			switch (iRefType)
			{
			case REF_TAB:			// Осн.таблица источника
				arFld[i].iSrc = DATA_TABSRC;
				arFld[i].iRef = FindField(arSrcTabFld, arFld[i].sValue);
				break;
			case REF_SUB:			// Под.таблица источника
				arFld[i].iSrc = DATA_SUBSRC;
				arFld[i].iRef = FindField(arSrcSubFld, arFld[i].sValue);
				break;
			default:				// Осн.таблица источника
				arFld[i].iSrc = DATA_TABSRC;
				arFld[i].iRef = FindField(arSrcTabFld, arFld[i].sValue);
				break;
			}
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDSRCTAB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_TABSRC:			// Осн.таблица источника
		{
			arFld[i].iRef = FindField(arSrcTabFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDSRCTAB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_SUBSRC:			// Под.таблица источника
		{
			arFld[i].iRef = FindField(arSrcSubFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDSRCSUB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_TABDST:			// Осн.таблица приемника
		{
			arFld[i].iRef = FindField(arDstTabFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDDSTTAB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_SUBDST:			// Под.таблица приемника
		{
			arFld[i].iRef = FindField(arDstSubFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDDSTSUB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_FUNC:				// Встроенная функция
		{
			// Поиск встроенной функции
			long n = arFld[i].sValue.Find('(');
			long m = arFld[i].sValue.ReverseFind(')');
			CString sFnc = arFld[i].sValue.Left(n);

			if      (sFnc.CompareNoCase("SQLGetVal") == 0) arFld[i].iRef = FNC_SQLGETVAL;
			else if (sFnc.CompareNoCase("SQLMaxVal") == 0) arFld[i].iRef = FNC_SQLMAXVAL;
			else if (sFnc.CompareNoCase("SQLMinVal") == 0) arFld[i].iRef = FNC_SQLMINVAL;
			else if (sFnc.CompareNoCase("SQLAvgVal") == 0) arFld[i].iRef = FNC_SQLAVGVAL;
			else if (sFnc.CompareNoCase("SQLSumVal") == 0) arFld[i].iRef = FNC_SQLSUMVAL;
			else if (sFnc.CompareNoCase("SQLVarVal") == 0) arFld[i].iRef = FNC_SQLVARVAL;
			else if (sFnc.CompareNoCase("SQLCount")  == 0) arFld[i].iRef = FNC_SQLCOUNT;
			else
			{
				strErr = GetErrMsg(ER_FIND_FUNC, arFld[i].sValue);
				return FALSE;
			}
			// Выделение списка параметров функции
			if (n >= 0 && m >= 0)
				arFld[i].sValue = arFld[i].sValue.Mid(n + 1, m - n - 1);

			break;
		}
		case DATA_PROC:				// Системная процедура
		{
			// Поиск системной процедуры
			strErr = GetErrMsg(ER_FIND_PROC, arFld[i].sValue);
			return FALSE;
		}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Установка ключевых полей в массиве
// по уникальному индексу таблицы

BOOL CTransfer::SetFldKeys(ODBC_INFO *pODBC, CFldArray &arFld, 
						   CString strOwner, CString strTable, 
						   CString &strErr) 
{
	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// Если поддерживается Owner для таблиц
	if (pODBC->bOuTableDef && !strOwner.IsEmpty())
	{
		// Определение уникального индекса
		// в схеме владельца Owner
		if (::SQLStatistics(hstmt, NULL, 0, 
			(SQLCHAR*)LPCTSTR(strOwner), SQL_NTS, 
			(SQLCHAR*)LPCTSTR(strTable), SQL_NTS, 
			SQL_INDEX_UNIQUE, SQL_QUICK) != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			strErr = GetErrMsg(ER_DEF_INDEX, strErr);
			return FALSE;
		}
	}
	else
	{
		// Определение уникального индекса
		if (::SQLStatistics(hstmt, NULL, 0, NULL, 0, 
			(SQLCHAR*)LPCTSTR(strTable), SQL_NTS, 
			SQL_INDEX_UNIQUE, SQL_QUICK) != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			strErr = GetErrMsg(ER_DEF_INDEX, strErr);
			return FALSE;
		}
	}
	int		i;
	TCHAR*	szColName;
	short	nDataType;
	long	nmColName,cbColName,cbDataType;
	BOOL	bKeyFound = FALSE;
	// Определение максимальной длины наименования поля
	nmColName = pODBC->iMaxColNameLen + 1;
	// Выделение памяти для наименования поля
	szColName = (TCHAR*)malloc((nmColName)*sizeof(TCHAR));
	// Связывание полей запроса
	::SQLBindCol(hstmt, 7, SQL_C_SHORT, &nDataType, 0, &cbDataType);
	::SQLBindCol(hstmt, 9, SQL_C_CHAR, szColName, nmColName, &cbColName);
	// Чтение индексных полей
	while (::SQLFetch(hstmt) == SQL_SUCCESS)
	{
		// SQL_INDEX_CONTENT (3)
		if (nDataType != 3) continue;
		if ((i = FindField(arFld, (LPCTSTR)szColName)) >= 0)
		{
			bKeyFound = TRUE;
			arFld[i].iFlag = FLD_KEY;
		}
		else
		{
			strErr = GetErrMsg(ER_FIND_FLDKEY, szColName);
			::SQLFreeStmt(hstmt, SQL_CLOSE);
			::SQLFreeStmt(hstmt, SQL_UNBIND);
			::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			free(szColName);
			return FALSE;
		}
	}
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	free(szColName);

	if (!bKeyFound)
	{
		strErr = GetErrMsg(ER_NO_KEYS);
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Установка данных в массиве полей (параметров) по ссылке

BOOL CTransfer::SetFldData(CFldArray &arFld, OP_TYPE iOpType, 
						   CString &strErr) 
{
	BOOL bError = FALSE;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// При ошибке сброс значения остальных полей
		if (bError)
		{
			arFld[i].vData.SetDefault();
			continue;
		}

		if (iOpType == OP_UPDATE)
		{
			// Пропуск неизменяемых и 
			// автоинкрементных полей
			if (arFld[i].iFlag == FLD_NOCHANGE || 
				arFld[i].iSrc  == DATA_AUTO)
				continue;
		}

		// Установка значения
		switch (arFld[i].iSrc)		// Тип источника
		{
		case DATA_LOCAL:			// Значение поля
		{
			if (!arFld[i].vData.ConvertInto((LPCTSTR)arFld[i].sValue))
			{
				strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
				strErr = arFld[i].sName + _T(" - ") + strErr;
				bError = TRUE;
			}
			break;
		}
		case DATA_AUTO:				// Автоинкремент
		{
			CString sValue;
			
			// Получение максимального значения поля в таблице основного приемника
			if (ExecSQLMax(pDst, arFld[i], strErr) == SQL_ERROR)
			{
				bError = TRUE;
				continue;
			}

			// Вычисление значения
			double nMaxV, nAuto;
			nAuto = atof((LPCTSTR)arFld[i].sValue);
			nMaxV = atof((LPCTSTR)arFld[i].vData.GetString());
			nMaxV+= nAuto;
			sValue.Format("%f", nMaxV);
			// Установка значения
			if (!arFld[i].vData.ConvertInto((LPCTSTR)sValue))
			{
				strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
				strErr = arFld[i].sName + _T(" - ") + strErr;
				bError = TRUE;
			}
			break;
		}
		case DATA_PARAM:			// Параметр
		{
			if (!arFld[i].vData.ConvertInto((LPCTSTR)
				pPrm->ElementAt(arFld[i].iRef).sValue))
			{
				strErr = GetErrMsg(ER_CONV_PARAM, 
								pPrm->GetAt(arFld[i].iRef).sID);
				bError = TRUE;
			}
			break;
		}
		case DATA_CONST:			// Константа
		{
			strErr = GetErrMsg(ER_CONV_CONST, arFld[i].sValue);
			bError = TRUE;
			break;
		}
		case DATA_TABSRC:			// Основная таблица источника
		{
			if (!arFld[i].vData.ConvertInto(
				arSrcTabFld.ElementAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arSrcTabFld.GetAt(arFld[i].iRef).sName);
				bError = TRUE;
			}
			break;
		}
		case DATA_SUBSRC:			// Подчиненная таблица источника
		{
			if (!arFld[i].vData.ConvertInto(
				arSrcSubFld.ElementAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arSrcSubFld.GetAt(arFld[i].iRef).sName);
				bError = TRUE;
			}
			break;
		}
		case DATA_TABDST:			// Основная таблица приемника
		{
			if (!arFld[i].vData.ConvertInto(
				arDstTabFld.ElementAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstTabFld.GetAt(arFld[i].iRef).sName);
				bError = TRUE;
			}
			break;
		}
		case DATA_SUBDST:			// Подчиненная таблица приемника
		{
			if (!arFld[i].vData.ConvertInto(
				arDstSubFld.ElementAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstSubFld.GetAt(arFld[i].iRef).sName);
				bError = TRUE;
			}
			break;
		}
		case DATA_FUNC:				// Встроенная функция
		{
			CString sValue;
			
			// Выполнение SQL функции и сохранение результата в sValue
			if (ExecSQLFunc(pDst, (FNC_TYPE)arFld[i].iRef, arFld[i].sValue, sValue, strErr) == SQL_ERROR)
			{
				bError = TRUE;
				continue;
			}

			// Установка значения
			if (!arFld[i].vData.ConvertInto((LPCTSTR)sValue))
			{
				strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
				strErr = arFld[i].sName + _T(" - ") + strErr;
				bError = TRUE;
			}
			break;
		}
		case DATA_PROC:				// Системная процедура
		{
			strErr = GetErrMsg(ER_CONV_PROC, arFld[i].sValue);
			bError = TRUE;
		}
		}
	}
	return bError ? FALSE : TRUE;
}

/////////////////////////////////////////////////////////////////////
// Установка данных в массиве полей (параметров) из приемника

BOOL CTransfer::SetDstData(CFldArray &arFld, CString &strErr) 
{
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// Установка значения
		switch (arFld[i].iDst)		// Тип приемника
		{
		case DATA_TABDST:			// Основная таблица приемника
		{
			if (!arFld[i].vData.ConvertInto(
				arDstTabFld.ElementAt(arFld[i].iIdx).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstTabFld.GetAt(arFld[i].iIdx).sName);
				return FALSE;
			}
			break;
		}
		case DATA_SUBDST:			// Подчиненная таблица приемника
		{
			if (!arFld[i].vData.ConvertInto(
				arDstSubFld.ElementAt(arFld[i].iIdx).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstSubFld.GetAt(arFld[i].iIdx).sName);
				return FALSE;
			}
			break;
		}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Конвертация текстовых полей

BOOL CTransfer::ConvertTxt(CFldArray &arFld, OP_TYPE iOpType,
						   UINT iConvert) 
{
	CString s;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// Пропуск неизменяемых полей
		if ((arFld[i].iFlag == FLD_NOCHANGE) &&
			(iOpType == OP_UPDATE)) continue;

		// Конвертация значения
		switch (arFld[i].iSrc)	// Тип источника
		{
		case DATA_TABSRC:		// Основная таблица источника
		case DATA_SUBSRC:		// Подчиненная таблица источника
		{
			if (arFld[i].vData.m_iType == SQL_C_CHAR)
			{
				s = arFld[i].vData.GetCHAR();
				switch (iConvert)
				{
				case TRN_UNCHANGED:	{ continue; }
				case TRN_OEMTOANSI:	{ s.OemToAnsi(); break; }
				case TRN_ANSITOOEM:	{ s.AnsiToOem(); break; }
				}
				arFld[i].vData.SetCHAR(s);
			}
			break;
		}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка массива колонок SQL Select (без изменения строки SQL)

BOOL CTransfer::ParseColumns(ODBC_INFO *pODBC, CFldArray &arFld, 
							 CString strSQL,   CString &strErr) 
{
	arFld.RemoveAll();
	// Разбор выражения и замена ссылок на '?'
	// без создания массива параметров
	if (!ParseSQLParams(pODBC, NULL, strSQL, strErr))
	{
		strErr = GetErrMsg(ER_PARSE_PARAMS, strErr);
		return FALSE;
	}
	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// Подготовка SQL оператора
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strSQL), 
		SQL_NTS) != SQL_SUCCESS)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		goto err;
	}
	// Определение числа полей
	short j,nCols;
	if (::SQLNumResultCols(hstmt, &nCols) != SQL_SUCCESS)
	{
		SQLGetError(hstmt, strErr);
		strErr = GetErrMsg(ER_DESC_FIELDS, strErr);
		goto err;
	}
	if (nCols == 0)
	{
		strErr = GetErrMsg(ER_NO_FIELDS);
		goto err;
	}
	SQLCHAR*	szColName;
	SQLSMALLINT	nmColName, nmColLen;
	SQLSMALLINT	nDataType, nColDec, fNullable;
	SQLUINTEGER	nColSize;
	// Определение максимальной длины наименования поля
	nmColName = pODBC->iMaxColNameLen + 1;
	// Выделение памяти для наименования поля
	szColName = (SQLCHAR*)malloc((nmColName)*sizeof(SQLCHAR));
	// Считывание описания результирующих полей
	for (j = 0; j < nCols; j++)
	{
		// Заполнение описания поля
		if (::SQLDescribeCol(hstmt, j + 1, szColName, 
			nmColName, &nmColLen, &nDataType, 
			&nColSize, &nColDec, &fNullable) != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			strErr = GetErrMsg(ER_DESC_FIELDS, strErr);
			free(szColName);
			goto err;
		}
		//if (nDataType == SQL_VARCHAR && nColSize > 255)
		//{
		//	nDataType = SQL_LONGVARCHAR;
		//}
		arFld.SetSize(j + 1);
		arFld[j].sName    = CString(szColName);
		arFld[j].iSQLType = nDataType;
		arFld[j].iSize    = nColSize;
		arFld[j].iPrec    = nColDec;
		arFld[j].fNull    = fNullable;
		arFld[j].sType    = SQLGetTypeDef(*pODBC, 
							nDataType).sTypeName;
		// Инициализация буфера данных
		arFld[j].vData.GetBuffer(arFld[j].iSQLType, 
								 arFld[j].iSize, TRUE);
	}
	// Уничтожение дексриптора SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	// Освобождение памяти
	free(szColName);
	return TRUE;

err:
	// Уничтожение дексриптора SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Разбор параметров SQL-выражения

BOOL CTransfer::ParseSQLParams(ODBC_INFO *pODBC, CFldArray *pArPrm, 
							   CString &strSQL,  CString &strMsg) 
{
	if (pODBC == NULL) return FALSE;
	if (strSQL.IsEmpty())
	{
		strMsg = GetErrMsg(ER_NO_EXPR);
		return FALSE;
	}
	// Очистка массива параметров
	if (pArPrm != NULL) pArPrm->RemoveAll();
	// Удаление всех символов '?'
	strSQL.Remove('?');

	CString	s,sProcName,sRef,sType,sLen,sDec;
	int		i,n,l,iSrc,iRef,iSize,iPrec;
	UINT	iType;
	BOOL	bProc;

	// Получить лексему
	i = GetLex(strSQL, 0, s);
	// Удаление символов {}
	if (s.Compare(_T("{")) == 0)
	{
		strSQL.TrimLeft('{');
		strSQL.TrimRight('}');
		// Получить лексему снова
		i = GetLex(strSQL, 0, s);
	}
	// Установка признака процедуры
	if (s.CompareNoCase(_T("call")) == 0)
	{
		bProc = TRUE;
		// Получение имени процедуры
		GetLex(strSQL, i, sProcName);
		// Установка Escape-выражения ODBC
		strSQL = _T("{") + strSQL + _T("}");
	}
	else
	{
		bProc = FALSE;
	}
	// Предобработка SQL-выражения и
	// формирование массива параметров
	for (i = 0, n = 0, l = 0;; i++)
	{
		// Поиск префикса ссылки
		if ((n = strSQL.FindOneOf(_T("^:&$~#!@"))) < 0)
			break;
		// Выделение префикса
		s = strSQL.Mid(n, 1);
		// Определение значения ссылки
		GetWord(strSQL, n+1, sRef);
		// Замена ссылки на знак ? в SQL операторе
		strSQL.Delete(n, sRef.GetLength()+1);
		strSQL.Insert(n, '?');
		// Установка источника
		if (s == _T("^"))		iSrc = DATA_LOCAL;
		else if (s == _T("+"))	iSrc = DATA_AUTO;
		else if (s == _T(":"))	iSrc = DATA_PARAM;
		else if (s == _T("&"))
		{
			iSrc = DATA_CONST;
			// Ссылка на константу пока недоступна!
			strMsg = GetErrMsg(ER_BAD_REF, s);
			return FALSE;
		}
		else if (s == _T("$"))	iSrc = DATA_TABSRC;
		else if (s == _T("~"))	iSrc = DATA_SUBSRC;
		else if (s == _T("#"))	iSrc = DATA_TABDST;
		else if (s == _T("!"))	iSrc = DATA_SUBDST;
		else
		{
			strMsg = GetErrMsg(ER_BAD_REF, s);
			return FALSE;
		}
		// Установка ссылки
		switch (iSrc)				// Тип источника
		{
		case DATA_LOCAL:			// Значение
		{
			break;
		}
		case DATA_AUTO:				// Автоинкремент
		{
			break;
		}
		case DATA_PARAM:			// Параметр
		{
			if ((iRef = FindParam(*pPrm, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_PARAM, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_TABSRC:			// Осн.таблица источника
		{
			if ((iRef = FindField(arSrcTabFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDSRCTAB, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_SUBSRC:			// Под.таблица источника
		{
			if ((iRef = FindField(arSrcSubFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDSRCSUB, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_TABDST:			// Осн.таблица приемника
		{
			if ((iRef = FindField(arDstTabFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDDSTTAB, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_SUBDST:			// Под.таблица приемника
		{
			if ((iRef = FindField(arDstSubFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDDSTSUB, sRef);
				return FALSE;
			}
			break;
		}
		}
		// Проверить, не указан ли тип
		l = GetLex(strSQL, n + 1, s);
		if (s == _T("("))
		{
			// Получить сокращение типа
			l = GetLex(strSQL, l, sType);
			sType.MakeUpper();
			// Проверка следующей лексемы
			l = GetLex(strSQL, l, s);
			// Если указан размер
			if (s == _T(","))
			{
				// Получить размер поля
				l = GetLex(strSQL, l, sLen);
				// Проверка следующей лексемы
				l = GetLex(strSQL, l, s);
				// Если указана точность
				if (s == _T(","))
				{
					// Получить точность поля
					l = GetLex(strSQL, l, sDec);
					// Проверка следующей лексемы
					l = GetLex(strSQL, l, s);
					if (s != _T(")"))
					{
						strMsg = GetErrMsg(ER_SNTX_PARAM, sRef);
						return FALSE;
					}
				}
				else if (s != _T(")"))
				{
					strMsg = GetErrMsg(ER_SNTX_PARAM, sRef);
					return FALSE;
				}
			}
			else if (s != _T(")"))
			{
				strMsg = GetErrMsg(ER_SNTX_PARAM, sRef);
				return FALSE;
			}
			// Установка типа параметра
			if ((iType = SQLGetTypeID(sType)) == 0)
			{
				strMsg = GetErrMsg(ER_BAD_TYPE, sType);
				return FALSE;
			}
			// Конвертирование типа параметра
			if (!SQLConvertType(*pODBC, iType, iType))
			{
				strMsg = GetErrMsg(ER_CONV_PARTYPE, sRef);
				return FALSE;
			}
			// Установка размера и точности
			iSize = atoi(sLen);
			iPrec = atoi(sDec);
			// Удаление информации о типе
			strSQL.Delete(n + 1, l - n - 1);
		}
		// Тип явно не указан, а функция
		// описания типа параметра недоступна
		else if (bProc && !pODBC->bProcColumns ||
				!bProc && !pODBC->bDescParam)
		{
			// Если не разрешено использование
			// типа по умолчанию
			if (TrnParm.iDefType <= 0)
			{
				strMsg = GetErrMsg(ER_DEF_REFTYPE, sRef);
				return FALSE;
			}
			// Попытка установки типа по ссылке
			switch (iSrc)				// Тип источника
			{
			case DATA_PARAM:			// Параметр
			{
				if (!SQLConvertType(*pODBC, 
					pPrm->GetAt(iRef).iSQLType, iType))
				{
					strMsg = GetErrMsg(ER_CONV_PARTYPE, 
									pPrm->GetAt(iRef).sName);
					return FALSE;
				}
				iSize = pPrm->GetAt(iRef).iSize;
				iPrec = pPrm->GetAt(iRef).iPrec;
				break;
			}
			case DATA_CONST:			// Константа
			{
				strMsg = GetErrMsg(ER_DEF_REFTYPE, sRef);
				return FALSE;
			}
			case DATA_TABSRC:			// Осн.таблица источника
			{
				if (!SQLConvertType(*pODBC, 
					arSrcTabFld.GetAt(iRef).iSQLType, iType))
				{
					strMsg = GetErrMsg(ER_CONV_FLDTYPE, 
									arSrcTabFld.GetAt(iRef).sName);
					return FALSE;
				}
				iSize = arSrcTabFld.GetAt(iRef).iSize;
				iPrec = arSrcTabFld.GetAt(iRef).iPrec;
				break;
			}
			case DATA_SUBSRC:			// Под.таблица источника
			{
				if (!SQLConvertType(*pODBC, 
					arSrcSubFld.GetAt(iRef).iSQLType, iType))
				{
					strMsg = GetErrMsg(ER_CONV_FLDTYPE, 
									arSrcSubFld.GetAt(iRef).sName);
					return FALSE;
				}
				iSize = arSrcSubFld.GetAt(iRef).iSize;
				iPrec = arSrcSubFld.GetAt(iRef).iPrec;
				break;
			}
			case DATA_TABDST:			// Осн.таблица приемника
			{
				if (!SQLConvertType(*pODBC, 
					arDstTabFld.GetAt(iRef).iSQLType, iType))
				{
					strMsg = GetErrMsg(ER_CONV_FLDTYPE, 
									arDstTabFld.GetAt(iRef).sName);
					return FALSE;
				}
				iSize = arDstTabFld.GetAt(iRef).iSize;
				iPrec = arDstTabFld.GetAt(iRef).iPrec;
				break;
			}
			case DATA_SUBDST:			// Под.таблица приемника
			{
				if (!SQLConvertType(*pODBC, 
					arDstSubFld.GetAt(iRef).iSQLType, iType))
				{
					strMsg = GetErrMsg(ER_CONV_FLDTYPE, 
									arDstSubFld.GetAt(iRef).sName);
					return FALSE;
				}
				iSize = arDstSubFld.GetAt(iRef).iSize;
				iPrec = arDstSubFld.GetAt(iRef).iPrec;
				break;
			}
			}
		}
		// Добавление параметра
		if (pArPrm != NULL)
		{
			pArPrm->SetSize(i + 1);
			pArPrm->ElementAt(i).sName    = sRef;
			pArPrm->ElementAt(i).sValue   = sRef;
			pArPrm->ElementAt(i).iSrc     = iSrc;
			pArPrm->ElementAt(i).iRef     = iRef;
			pArPrm->ElementAt(i).iSQLType = iType;
			pArPrm->ElementAt(i).iSize    = iSize;
			pArPrm->ElementAt(i).iPrec    = iPrec;
		}
	}
	// Передача имени процедуры
	if (bProc)	strMsg = sProcName;
	else		strMsg.Empty();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка SQL-оператора к выполнению

BOOL CTransfer::PrepareSQLParms(ODBC_INFO *pODBC, CFldArray *pArPrm, 
								CString strOwner, CString &strSQL,   
								CString &strMsg)
{
	// Разбор параметров SQL-выражения
	if (!ParseSQLParams(pODBC, pArPrm, strSQL, strMsg)) return FALSE;

	// Установка имени процедуры
	CString	sProcName = strMsg;

	// Установка признака процедуры
	BOOL bProc = (sProcName.IsEmpty())?FALSE:TRUE;

	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strMsg = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// Подготовка SQL оператора
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strSQL), 
		SQL_NTS) != SQL_SUCCESS)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strMsg);
		goto err;
	}
	SQLSMALLINT	nDataType, nColDec, fNullable;
	SQLUINTEGER	nColSize;
	SQLINTEGER	cbDataType,cbColDec,cbNullable,cbColSize;

	if (bProc && pODBC->bProcColumns) // Считывание параметров процедуры
	{
		// Если поддерживается Owner для процедур
		if (pODBC->bOuProcInv && !strOwner.IsEmpty())
		{
			// Чтение списка параметров процедуры
			// в схеме владельца strOwner
			if (::SQLProcedureColumns(hstmt, NULL, 0, 
				(SQLTCHAR*)LPCTSTR(strOwner), SQL_NTS, 
				(SQLTCHAR*)LPCTSTR(sProcName), SQL_NTS, 
				NULL, 0) != SQL_SUCCESS)
			{
				SQLGetError(hstmt, strMsg);
				strMsg = GetErrMsg(ER_DESC_PARAMS, strMsg);
				goto err;
			}
		}
		else
		{
			// Чтение списка параметров процедуры
			if (::SQLProcedureColumns(hstmt, NULL, 0, NULL, 0, 
				(SQLTCHAR*)LPCTSTR(sProcName), SQL_NTS, 
				NULL, 0) != SQL_SUCCESS)
			{
				SQLGetError(hstmt, strMsg);
				strMsg = GetErrMsg(ER_DESC_PARAMS, strMsg);
				goto err;
			}
		}
		// Связывание полей запроса
		::SQLBindCol(hstmt, 6, SQL_C_SSHORT, &nDataType, 0, &cbDataType);
		::SQLBindCol(hstmt, 8, SQL_C_SLONG,  &nColSize, 0, &cbColSize);
		::SQLBindCol(hstmt, 10, SQL_C_SSHORT, &nColDec, 0, &cbColDec);
		::SQLBindCol(hstmt, 12, SQL_C_SSHORT, &fNullable, 0, &cbNullable);
		// Установка описания параметров в массиве
		for (int j = 0;; j++)
		{
			// Считывание описания параметра
			if (SQLFetch(hstmt) != SQL_SUCCESS) break;
			if (j >= pArPrm->GetSize())
			{
				::SQLFreeStmt(hstmt, SQL_CLOSE);
				::SQLFreeStmt(hstmt, SQL_UNBIND);
				strMsg = GetErrMsg(ER_UNS_PARAMS);
				goto err;
			}
			// Установка описания параметра
			pArPrm->ElementAt(j).iSQLType = nDataType;
			pArPrm->ElementAt(j).iSize    = nColSize;
			pArPrm->ElementAt(j).iPrec    = nColDec;
			pArPrm->ElementAt(j).fNull    = fNullable;
			// Наименование типа данных
			pArPrm->ElementAt(j).sType = SQLGetTypeDef(*pODBC, 
											nDataType).sTypeName;
			// Инициализация буфера данных
			pArPrm->ElementAt(j).vData.GetBuffer(nDataType,nColSize,TRUE);
		}
		::SQLFreeStmt(hstmt, SQL_CLOSE);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
	}
	else			// Считывание описания параметров SQL
	{
		// Определение числа параметров
		short k, nCols;
		if (::SQLNumParams(hstmt, &nCols) != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strMsg);
			strMsg = GetErrMsg(ER_DESC_PARAMS, strMsg);
			goto err;
		}
		if (nCols > pArPrm->GetSize())
		{
			strMsg = GetErrMsg(ER_UNS_PARAMS);
			goto err;
		}
		for (k = 0; k < nCols && k < pArPrm->GetSize(); k++)
		{
			// Если поддерживается функция описания параметров
			if (!bProc && pODBC->bDescParam)
			{
				// Получение данных через функцию описания параметра
				if (::SQLDescribeParam(hstmt, k + 1, &nDataType, 
					&nColSize, &nColDec, &fNullable) != SQL_SUCCESS)
				{
					SQLGetError(hstmt, strMsg);
					strMsg = GetErrMsg(ER_DESC_PARAMS, strMsg);
					goto err;
				}
				// Установка описания параметра
				pArPrm->ElementAt(k).iSQLType = nDataType;
				pArPrm->ElementAt(k).iSize    = nColSize;
				pArPrm->ElementAt(k).iPrec    = nColDec;
				pArPrm->ElementAt(k).fNull    = fNullable;
			}
			else
			{
				nDataType = (SQLSMALLINT)pArPrm->GetAt(k).iSQLType;
				nColSize  = (SQLUINTEGER)pArPrm->GetAt(k).iSize;
			}
			// Наименование типа данных
			pArPrm->ElementAt(k).sType = SQLGetTypeDef(*pODBC, 
											nDataType).sTypeName;
			// Инициализация буфера данных
			pArPrm->ElementAt(k).vData.GetBuffer(nDataType,nColSize,TRUE);
		}
	}
	// Уничтожение дексриптора SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return TRUE;

err:
	// Уничтожение дексриптора SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Создание таблицы приемника и уникального индекса

BOOL CTransfer::CreateTable(ODBC_INFO *pODBC, UINT iSchema, 
							CFldArray &arFld, CFldArray &arMap, 
							CFldArray &arSrc, CString strTable, 
							CString &strErr) 
{
	arFld.RemoveAll();
	strTable.MakeLower();
	if (strTable.IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_TABLE);
		return FALSE;
	}

	// Формирование массива полей таблицы
	BOOL bKeyFound = FALSE;
	BOOL bUseSrc = (iSchema == TRN_COPY || 
					iSchema == TRN_COMPLEX);
	int	 i, j, n;
	if (bUseSrc) n = arSrc.GetSize();
	else		 n = arMap.GetSize();
	for (i = 0; i < n; i++)
	{
		arFld.SetSize(i + 1);
		if (bUseSrc)
		{
			// Формирование полей из источника
			arFld[i].sName	= arSrc[i].sName;
			arFld[i].iSize	= arSrc[i].iSize;
			arFld[i].iPrec	= arSrc[i].iPrec;
			arFld[i].fNull	= arSrc[i].fNull;
			arFld[i].iFlag	= arSrc[i].iFlag;
			arFld[i].iSrc	= DATA_REF;
			arFld[i].sValue	= arSrc[i].sName;
			// Установка типа данных поля
			if (!SQLConvertType(*pODBC, arSrc[i].iSQLType, 
								arFld[i].iSQLType))
			{
				strErr = GetErrMsg(ER_CONV_FLDTYPE, arSrc[i].sName);
				return FALSE;
			}
		}
		else
		{
			// Формирование полей из карты
			arFld[i].sName    = arMap[i].sName;
			arFld[i].iSize    = arMap[i].iSize;
			arFld[i].iPrec    = arMap[i].iPrec;
			arFld[i].fNull    = arMap[i].fNull;
			arFld[i].iFlag    = arMap[i].iFlag;
			arFld[i].iSrc     = arMap[i].iSrc;
			arFld[i].sValue   = arMap[i].sValue;
			// Установка типа данных поля
			if (!SQLConvertType(*pODBC, arMap[i].iSQLType, 
								arFld[i].iSQLType))
			{
				strErr = GetErrMsg(ER_CONV_FLDTYPE, arMap[i].sName);
				return FALSE;
			}
		}
		// Признак ключевого поля
		if (arFld[i].iFlag == FLD_KEY) bKeyFound = TRUE;
		// Название типа данных из источника ODBC
		arFld[i].sType = SQLGetTypeDef(*pODBC, 
						arFld[i].iSQLType).sTypeName;
		// Инициализация буфера данных
		arFld[i].vData.GetBuffer(arFld[i].iSQLType, 
								 arFld[i].iSize, TRUE);
	}
	if (!bKeyFound)
	{
		strErr = GetErrMsg(ER_NO_KEYS);
		return FALSE;
	}
	// При копировании схемы источника
	// Обработка массива карты полей
	if (bUseSrc)
	{
		n = arMap.GetSize();
		for (i = 0; i < n; i++)
		{
			// Если переопределение поля
			if (arMap[i].iSrc == DATA_REF)
			{
				// Поиск поля источника (в приемнике) по ссылке
				if ((j = FindField(arFld, arMap[i].sValue)) < 0)
				{
					strErr = GetErrMsg(ER_FIND_FLDSRCTAB, arMap[i].sValue);
					return FALSE;
				}
				// Если ключевое поле
				if (arFld[j].iFlag == FLD_KEY)
				{
					strErr = GetErrMsg(ER_REPL_FLDKEY, arFld[j].sName);
					return FALSE;
				}
				// Поиск замещаемого имени в приемнике
				if (FindField(arFld, arMap[i].sName) >= 0)
				{
					strErr = GetErrMsg(ER_REPL_FLDDEF, arMap[i].sName);
					return FALSE;
				}
				// Замещение имени поля приемника
				arFld[j].sName	= arMap[i].sName;
			}
			else
			{
				// Поиск поля приемника по наименованию
				if ((j = FindField(arFld, arMap[i].sName)) < 0)
				{
					strErr = GetErrMsg(ER_FIND_FLDDSTTAB, arMap[i].sValue);
					return FALSE;
				}
				// Если ключевое поле
				if (arFld[j].iFlag == FLD_KEY)
				{
					strErr = GetErrMsg(ER_MOD_FLDKEY, arFld[j].sName);
					return FALSE;
				}
				// Если пустая ссылка на значение
				if (arMap[i].iSrc == DATA_LOCAL &&
					arMap[i].sValue.IsEmpty())
				{
					// Установка только флага поля
					// Оставляем ссылку без изменения
					arMap[i].iSrc	= arFld[j].iSrc;
					arMap[i].sValue	= arFld[j].sValue;
				}
			}
			// Установка флага поля приемника
			arFld[j].iFlag	= arMap[i].iFlag;
			// Установка ссылки
			arFld[j].iSrc	= arMap[i].iSrc;
			arFld[j].sValue	= arMap[i].sValue;
		}
	}

	// Формирование команд для создания
	// таблицы и уникального индекса
	if ((n = arFld.GetSize()) == 0)
	{
		strErr = GetErrMsg(ER_DEF_FIELDS);
		return FALSE;
	}
	CString sPar;
	CString sDrop = _T("drop table ") + strTable;
	CString sTab  = _T("create table ") + strTable + _T(" (");
	CString sIdx  = _T("create unique index ") + strTable + 
					_T("_pk on ") + strTable + _T(" (");

	for (i = 0; i < n; i++)
	{
		// Формирование команды создания таблицы
		sTab += arFld[i].sName + _T(" ") + arFld[i].sType;
		switch (arFld[i].iSQLType)
		{
		case SQL_DECIMAL:
		case SQL_NUMERIC:
			if (!SQLGetTypeDef(*pODBC, arFld[i].iSQLType).bFxPrcScale)
				sPar.Format("(%i,%i)", arFld[i].iSize, arFld[i].iPrec);
			break;
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_BINARY:
		case SQL_VARBINARY:
			sPar.Format("(%i)", arFld[i].iSize);
			break;
		default:
			sPar = _T("");
			break;
		}
		sTab += sPar + _T(",");

		// Формирование команды создания индекса
		if (arFld[i].iFlag == FLD_KEY)
			sIdx += arFld[i].sName + _T(",");
	}
	sIdx = sIdx.Left(sIdx.GetLength()-1) + _T(")");
	sTab = sTab.Left(sTab.GetLength()-1) + _T(")");

	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// Уничтожение таблицы
	::SQLExecDirect(hstmt, (SQLTCHAR *)LPCTSTR(sDrop), SQL_NTS);
	// Создание таблицы
	if (::SQLExecDirect(hstmt, (SQLTCHAR *)LPCTSTR(sTab), 
		SQL_NTS) != SQL_SUCCESS)
	{
		SQLGetError(hstmt, strErr);
		::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		strErr = GetErrMsg(ER_CRT_TABLE, strErr);
		return FALSE;
	}
	// Создание уникального индекса
	if (::SQLExecDirect(hstmt, (SQLTCHAR *)LPCTSTR(sIdx), 
		SQL_NTS) != SQL_SUCCESS)
	{
		SQLGetError(hstmt, strErr);
		::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		strErr = GetErrMsg(ER_CRT_INDEX, strErr);
		return FALSE;
	}
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка таблицы приемника

BOOL CTransfer::ParseTable(ODBC_INFO *pODBC, UINT iSchema, 
						   CFldArray &arFld, CFldArray &arMap, 
						   CString strOwner, CString strTable, 
						   CString &strErr) 
{
	arFld.RemoveAll();
	strTable.MakeLower();
	if (strTable.IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_TABLE);
		return FALSE;
	}
	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// Если поддерживается Owner для таблиц
	if (pODBC->bOuTableDef && !strOwner.IsEmpty())
	{
		// Проверка существования таблицы
		// в схеме владельца Owner
		if (::SQLTables(hstmt, NULL, 0, 
			(SQLTCHAR*)LPCTSTR(strOwner), SQL_NTS, 
			(SQLTCHAR*)LPCTSTR(strTable), SQL_NTS, 
			(SQLTCHAR*)"TABLE", SQL_NTS) != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			strErr = GetErrMsg(ER_DESC_CATTAB, strErr);
			return FALSE;
		}
	}
	else
	{
		// Проверка существования таблицы
		if (::SQLTables(hstmt, NULL, 0, NULL, 0, 
			(SQLTCHAR*)LPCTSTR(strTable), SQL_NTS, 
			(SQLTCHAR*)"TABLE", SQL_NTS) != SQL_SUCCESS)
		{
			::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			SQLGetError(hstmt, strErr);
			strErr = GetErrMsg(ER_DESC_CATTAB, strErr);
			return FALSE;
		}
	}
	if (::SQLFetch(hstmt) != SQL_SUCCESS)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		strErr = GetErrMsg(ER_DEF_TABLE, strErr);
		return FALSE;
	}
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	// Если поддерживается Owner для таблиц
	if (pODBC->bOuTableDef && !strOwner.IsEmpty())
	{
		// Чтение описания полей таблицы
		// в схеме владельца Owner
		if (::SQLColumns(hstmt, NULL, 0, 
			(SQLTCHAR*)LPCTSTR(strOwner), SQL_NTS, 
			(SQLTCHAR*)LPCTSTR(strTable), SQL_NTS, 
			NULL, 0) != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			strErr = GetErrMsg(ER_DESC_FIELDS, strErr);
			return FALSE;
		}
	}
	else
	{
		// Чтение описания полей таблицы
		if (::SQLColumns(hstmt, NULL, 0, NULL, 0, 
			(SQLTCHAR*)LPCTSTR(strTable), SQL_NTS, 
			NULL, 0) != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			strErr = GetErrMsg(ER_DESC_FIELDS, strErr);
			return FALSE;
		}
	}
	TCHAR*	szColName;
	TCHAR	szTypeName[100];
	long	nmColName, nColSize, nColDec;
	short	nDataType, fNullable;
	long	cbDataType,cbColDec,cbNullable;
	long	cbColName,cbTypeName,cbColSize;
	// Определение максимальной длины наименования поля
	nmColName = pODBC->iMaxColNameLen + 1;
	// Выделение памяти для наименования поля
	szColName = (TCHAR*)malloc((nmColName)*sizeof(TCHAR));
	// Связывание полей запроса
	::SQLBindCol(hstmt, 4, SQL_C_CHAR, szColName, nmColName, &cbColName);
	::SQLBindCol(hstmt, 5, SQL_C_SSHORT, &nDataType, 0, &cbDataType);
	::SQLBindCol(hstmt, 6, SQL_C_CHAR, szTypeName, 100, &cbTypeName);
	::SQLBindCol(hstmt, 7, SQL_C_SLONG, &nColSize, 0, &cbColSize);
	::SQLBindCol(hstmt, 8, SQL_C_SLONG, &nColDec, 0, &cbColDec);
	::SQLBindCol(hstmt, 11, SQL_C_SSHORT, &fNullable, 0, &cbNullable);
	// Установка описания полей в массиве
	int	 i, j, n;
	for(i = 0; SQLFetch(hstmt) == SQL_SUCCESS; i++)
	{
		arFld.SetSize(i + 1);
		arFld[i].sName    = CString(szColName);
		arFld[i].sType    = CString(szTypeName);
		arFld[i].iSQLType = nDataType;
		arFld[i].iSize    = nColSize;
		arFld[i].iPrec    = nColDec;
		arFld[i].fNull    = fNullable;
		// Инициализация буфера данных
		arFld[i].vData.GetBuffer(arFld[i].iSQLType, 
								 arFld[i].iSize, TRUE);
	}
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	free(szColName);

	// Установка ключевых полей в массиве
	if (!SetFldKeys(pODBC, arFld, strOwner, strTable, strErr))
	{
		strErr = GetErrMsg(ER_DEF_KEYS, strErr);
		return FALSE;
	}
	// Обработка массива полей приемника
	if ((n = arFld.GetSize()) == 0)
	{
		strErr = GetErrMsg(ER_DEF_FIELDS);
		return FALSE;
	}
	// Установка ссылок на источник данных
	// Цикл по полям таблицы приемника
	for (i = 0; i < n; i++)
	{
		// Поиск поля в описании карты полей
		// Если отсутствует описание в карте полей
		if ((j = FindField(arMap, arFld[i].sName)) < 0)
		{
			// При копировании схемы источника
			if (iSchema == TRN_COPY || 
				iSchema == TRN_COMPLEX)
			{
				// Установка ссылки на таблицу источника
				arFld[i].iSrc	= DATA_REF;
				arFld[i].sValue = arFld[i].sName;
			}
			else
			{
				strErr = GetErrMsg(ER_FIND_FLDMAP, arFld[i].sName);
				return FALSE;
			}
		}
		// Если найдено описание в карте полей
		else
		{
			// При копировании схемы источника
			if (iSchema == TRN_COPY || 
				iSchema == TRN_COMPLEX)
			{
				// Если ключевое поле
				if (arFld[i].iFlag == FLD_KEY)
				{
					strErr = GetErrMsg(ER_MOD_FLDKEY, arFld[i].sName);
					return FALSE;
				}
				// Если пустая ссылка на значение
				if (arMap[j].iSrc == DATA_LOCAL && 
					arMap[j].sValue.IsEmpty())
				{
					// Установка только флага поля
					// Оставляем ссылку на источник
					arMap[j].iSrc	= DATA_REF;
					arMap[j].sValue	= arFld[i].sName;
				}
			}
			// Установка флага поля
			arFld[i].iFlag	= arMap[j].iFlag;
			// Установка ссылки
			arFld[i].iSrc	= arMap[j].iSrc;
			arFld[i].sValue	= arMap[j].sValue;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка признака команд CHANGE в массиве полей

BOOL CTransfer::ParseFldCmd(CFldArray &arFld, CCmdArray &arCmd, 
							CString &strErr) 
{
	for (int i = 0; i < arFld.GetSize(); i++)
	{
		if (FindSQLCmd(arCmd, EV_ON_CHANGE, arFld[i].sName) >= 0)
		{
			// Проверка флага поля
			if (arFld[i].iFlag == FLD_DATA)	// Привязка команды
			{
				arFld[i].iFlag = FLD_EVCHANGE;
			}
			else							// Невозможна привязка
			{
				strErr = GetErrMsg(ER_BAD_FLDCMD, arFld[i].sName);
				return FALSE;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка массива команд

BOOL CTransfer::ParseCmdArr(ODBC_INFO *pODBC, REF_TYPE iRefType, 
							CCmdArray &arCmd, BOOL bMakeParams, 
							CString &strErr) 
{
	CString strCmd, strEvt;
	for (int i = 0, nLen = 0; i < arCmd.GetSize(); i++)
	{
		// Установка наименования команды
		strEvt = arCmd[i].sEvent;
		if (arCmd[i].iEvent == EV_ON_CHANGE &&
			!arCmd[i].sField.IsEmpty())
		{
			strEvt += _T(".") + arCmd[i].sField;
		}
		// Создание параметров
		if (bMakeParams)
		{
			arCmd[i].nCount = 0;
			// Замена всех символов '\r\n' на пробел
			arCmd[i].sCmd.Replace(_T("\r\n"), _T(" "));
			// Создание массива подкоманд
			if (arCmd[i].pArCmd == NULL)
				arCmd[i].pArCmd = new CStringArray;
			else
				arCmd[i].pArCmd->RemoveAll();
			// Создание массива параметров
			if (arCmd[i].pArPrm == NULL)
				arCmd[i].pArPrm = new CArFldArray;
			else
			{
				// Удаление всех массивов параметров
				for (int k = arCmd[i].pArPrm->GetSize() - 1; k >= 0; k--)
				{
					if (arCmd[i].pArPrm->GetAt(k) != NULL)
					{
						ClearFldArr(*arCmd[i].pArPrm->ElementAt(k));
						delete arCmd[i].pArPrm->ElementAt(k);
						arCmd[i].pArPrm->ElementAt(k) = NULL;
					}
				}
				arCmd[i].pArPrm->RemoveAll();
				arCmd[i].pArPrm->FreeExtra();
			}
		}
		nLen = arCmd[i].sCmd.GetLength();
		// Цикл по списку подкоманд одной команды
		for (int n = 0, m = 0, j = 0; m >= 0 && n < nLen; n = m + 1, j++)
		{
			// Выделение строки подкоманды
			if ((m = arCmd[i].sCmd.Find(';',n)) >= 0)
				strCmd = arCmd[i].sCmd.Mid(n, m - n);
			else
				strCmd = arCmd[i].sCmd.Mid(n);
			strCmd.TrimLeft();
			strCmd.TrimRight();
			// Создание параметров
			if (bMakeParams)
			{
				// Добавление элемента
				arCmd[i].nCount = j + 1;
				arCmd[i].pArCmd->SetSize(j + 1);
				arCmd[i].pArPrm->SetSize(j + 1);
				// Создание массива параметров подкоманды
				CFldArray *pArFld = new CFldArray;
				arCmd[i].pArPrm->SetAt(j, pArFld);
				// Подготовка массива параметров подкоманды
				CString sOwner = (iRefType == REF_TAB) ? pTrn->sTabOwn : pTrn->sSubOwn;
				if (!PrepareSQLParms(pODBC, pArFld, 
					(sOwner.IsEmpty()) ? pODBC->sUserName : sOwner, strCmd, strErr))
				{
					strErr = GetErrMsg(ER_CMD_PARAMS, strErr);
					strErr = strEvt + _T(" - ") + strErr;
					return FALSE;
				}
				// Установка ссылок на источник данных
				// в массиве параметров подкоманды
				if (!SetFldRefs(*pArFld, iRefType, strErr))
				{
					strErr = GetErrMsg(ER_CMD_PARAMS, strErr);
					strErr = strEvt + _T(" - ") + strErr;
					return FALSE;
				}
				// Установка строки подкоманды
				arCmd[i].pArCmd->SetAt(j, strCmd);
			}
			// Проверка параметров
			else
			{
				// Разбор параметров SQL подкоманды
				if (!ParseSQLParams(pODBC, NULL, strCmd, strErr))
				{
					strErr = GetErrMsg(ER_CMD_PARAMS, strErr);
					strErr = strEvt + _T(" - ") + strErr;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка строки сообщения

BOOL CTransfer::ParseStrMsg(CString &strMsg, CString &strErr) 
{
	arStrMsgPrm.RemoveAll();
	if (strMsg.IsEmpty()) return TRUE;
	// Удаление всех символов <?>
	strMsg.Remove('?');
	// Обработка строки сообщения и
	// формирование массива параметров
	CString	s, tmp;
	for (int i = 0, j = 0, n = 0; tmp = strMsg.Mid(j); i++, j++)
	{
		// Поиск символа форматирования
		if ((n = tmp.FindOneOf(_T("^:&$~#!%@"))) < 0)
			break;
		// Вычисление смещения в строке сообщения
		j += n;
		// Выделение символа форматирования
		// Если двойной символ, то это не параметр
		if ((s = tmp.Mid(n,1)) == tmp.Mid(n+1,1))
		{
			// Удаление одного символа и
			// уменьшение индекса параметров
			strMsg.Delete(j, 1); i--; continue;
		}
		// Добавление параметра
		arStrMsgPrm.SetSize(i + 1);
		// Установка источника
		if (s == _T(":"))		arStrMsgPrm[i].iSrc = DATA_PARAM;
		else if (s == _T("&"))
		{
			arStrMsgPrm[i].iSrc = DATA_CONST;
			// Ссылка на константу пока недоступна!
			strErr = GetErrMsg(ER_BAD_REF, s);
			return FALSE;
		}
		else if (s == _T("$"))	arStrMsgPrm[i].iSrc = DATA_TABSRC;
		else if (s == _T("~"))	arStrMsgPrm[i].iSrc = DATA_SUBSRC;
		else if (s == _T("^") ||
				 s == _T("#") ||
				 s == _T("!") ||
				 s == _T("%") ||
				 s == _T("@"))
		{
			strErr = GetErrMsg(ER_BAD_REF, s);
			return FALSE;
		}
		// Установка значении ссылки
		GetWord(tmp, n+1, s);
		arStrMsgPrm[i].sName  = s;
		arStrMsgPrm[i].sValue = s;
		// Инициализация типа данных
		arStrMsgPrm[i].vData.InitType(SQL_C_CHAR, 255);
		// Замена ссылки на знак ? в строке сообщения
		strMsg.Delete(j, s.GetLength()+1);
		strMsg.Insert(j, '?');
	}
	// Установка ссылок
	if (!SetFldRefs(arStrMsgPrm, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_DEF_REFS, strErr);
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Конструирование оператора Select по описанию полей

BOOL CTransfer::MakeSelect(CFldArray &arFld, CString strTable, 
						   CString strWhere, BOOL bUseKeys, 
						   CString &strSQL,  CString &strErr) 
{
	if (strTable.IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_TABLE);
		return FALSE;
	}
	int	i, n = arFld.GetSize();
	if (n == 0)
	{
		strErr = GetErrMsg(ER_DEF_FIELDS);
		return FALSE;
	}
	strSQL = _T("select ");
	CString	strKeys, strSize, strPrec;
	for (i = 0; i < n; i++)
	{
		// Формирование списка колонок Select
		strSQL += arFld[i].sName + _T(",");
		// Формирование условия по уникальным ключам
		if (bUseKeys && arFld[i].iFlag == FLD_KEY)
		{
			strKeys += arFld[i].sName + _T("=");
			// Установка ссылки на источник
			switch (arFld[i].iSrc)
			{
			case DATA_LOCAL:  strKeys += _T("^"); break;
			case DATA_AUTO:   strKeys += _T("+"); break;
			case DATA_PARAM:  strKeys += _T(":"); break;
			case DATA_CONST:  strKeys += _T("&"); break;
			case DATA_REF:    strKeys += _T("$"); break;
			case DATA_TABSRC: strKeys += _T("$"); break;
			case DATA_SUBSRC: strKeys += _T("~"); break;
			case DATA_TABDST: strKeys += _T("#"); break;
			case DATA_SUBDST: strKeys += _T("!"); break;
			case DATA_FUNC:   strKeys += _T("%"); break;
			case DATA_PROC:   strKeys += _T("@"); break;
			}
			// Установка значения ссылки
			if (arFld[i].iSrc == DATA_LOCAL ||
				arFld[i].iSrc == DATA_AUTO  ||
				arFld[i].iSrc == DATA_FUNC  ||
				arFld[i].iSrc == DATA_PROC)
				strKeys += arFld[i].sName;
			else
				strKeys += arFld[i].sValue;
			// Установка типа данных
			strKeys += _T("(") + SQLGetTypeStr(arFld[i].iSQLType);
			// Установка длины и точности
			strSize.Format("%i", arFld[i].iSize);
			strPrec.Format("%i", arFld[i].iPrec);
			switch (arFld[i].iSQLType)
			{
			case SQL_NUMERIC:
			case SQL_DECIMAL:
				strKeys += _T(",") + strSize;
				strKeys += _T(",") + strPrec + _T(")");
				break;
			case SQL_FLOAT:
			case SQL_CHAR:
			case SQL_VARCHAR:
			case SQL_BINARY:
			case SQL_VARBINARY:
				strKeys += _T(",") + strSize + _T(")");
				break;
			default:
				strKeys += _T(")");
				break;
			}
			strKeys += _T(" and ");
		}
	}
	strSQL  = strSQL.Left(strSQL.GetLength() - 1);
	strSQL += _T(" from ") + strTable;
	// Установка условия по уникальным ключам
	if (bUseKeys)
	{
		if (!strKeys.IsEmpty())
		{
			strKeys = strKeys.Left(strKeys.GetLength() - 5);
			strSQL += _T(" where ") + strKeys;
		}
		else
		{
			strErr = GetErrMsg(ER_NO_KEYS);
			return FALSE;
		}
	}
	// Установка заданного условия
	else if (!strWhere.IsEmpty())
	{
		strSQL += _T(" where ") + strWhere;
	}
	strSQL.MakeLower();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка оператора Insert

BOOL CTransfer::MakeInsert(CFldArray &arFld, CFldArray &arPrm, 
						   CString strTable, CString &strSQL, 
						   REF_TYPE iRefType, CString &strErr) 
{
	strSQL.Empty();
	arPrm.RemoveAll();
	if (strTable.IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_TABLE);
		return FALSE;
	}
	// Формирование оператора INSERT
	CString strPrm = _T(" values (");
	// Формирование массива параметров INSERT
	int	 i, n = arFld.GetSize();
	for (i = 0; i < n; i++)
	{
		arPrm.SetSize(i + 1);
		arPrm[i].sName    = arFld[i].sName;
		arPrm[i].iSQLType = arFld[i].iSQLType;
		arPrm[i].iSize    = arFld[i].iSize;
		arPrm[i].iPrec    = arFld[i].iPrec;
		arPrm[i].fNull    = arFld[i].fNull;
		arPrm[i].iFlag    = arFld[i].iFlag;
		// Установка ссылки на источник
		arPrm[i].iSrc     = arFld[i].iSrc;
		arPrm[i].iRef     = arFld[i].iRef;
		arPrm[i].sValue   = arFld[i].sValue;
		// Установка ссылки на приемник
		switch (iRefType)
		{
		case REF_TAB:	arPrm[i].iDst = DATA_TABDST; break;
		case REF_SUB:	arPrm[i].iDst = DATA_SUBDST; break;
		default:		arPrm[i].iDst = DATA_TABDST; break;
		}
		arPrm[i].iIdx     = i;
		// Инициализация буфера данных
		arPrm[i].vData.GetBuffer(arPrm[i].iSQLType, 
								 arPrm[i].iSize, TRUE);
		// Формирование строк оператора
		strSQL += arPrm[i].sName + _T(",");
		strPrm += _T("?,");
	}
	strSQL = strSQL.Left(strSQL.GetLength()-1) + _T(")");
	strPrm = strPrm.Left(strPrm.GetLength()-1) + _T(")");
	strSQL = _T("insert into ") + strTable + _T(" (") + strSQL;
	strSQL+= strPrm;
	strSQL.MakeLower();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка оператора Update

BOOL CTransfer::MakeUpdate(CFldArray &arFld, CFldArray &arPrm, 
						   CString strTable, CString &strSQL, 
						   REF_TYPE iRefType, CString &strErr) 
{
	strSQL.Empty();
	arPrm.RemoveAll();
	if (strTable.IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_TABLE);
		return FALSE;
	}
	// Формирование оператора UPDATE
	BOOL bKeyFound = FALSE;
	CString strPrm;
	// Формирование массива параметров UPDATE
	int	 i, j, n = arFld.GetSize();
	// Формирование параметров SET
	for (i = 0, j = 0; i < n; i++)
	{
		// Установка обновления неключевых полей
		if (arFld[i].iFlag != FLD_KEY &&
			arFld[i].iFlag != FLD_NOCHANGE)
		{
			arPrm.SetSize(j + 1);
			arPrm[j].sName    = arFld[i].sName;
			arPrm[j].iSQLType = arFld[i].iSQLType;
			arPrm[j].iSize    = arFld[i].iSize;
			arPrm[j].iPrec    = arFld[i].iPrec;
			arPrm[j].fNull    = arFld[i].fNull;
			arPrm[j].iFlag    = arFld[i].iFlag;
			// Установка ссылки на источник
			arPrm[j].iSrc     = arFld[i].iSrc;
			arPrm[j].iRef     = arFld[i].iRef;
			arPrm[j].sValue   = arFld[i].sValue;
			// Установка ссылки на приемник
			switch (iRefType)
			{
			case REF_TAB:	arPrm[j].iDst = DATA_TABDST; break;
			case REF_SUB:	arPrm[j].iDst = DATA_SUBDST; break;
			default:		arPrm[j].iDst = DATA_TABDST; break;
			}
			arPrm[j].iIdx     = i;
			// Инициализация буфера данных
			arPrm[j].vData.GetBuffer(arPrm[j].iSQLType, 
									 arPrm[j].iSize, TRUE);
			// Формирование строки SET
			strSQL += arPrm[j].sName + _T("=?,");
			j++;
		}
	}
	if (j == 0)
	{
		strErr = GetErrMsg(ER_NO_MODFLD);
		return FALSE;
	}
	// Формирование параметров WHERE
	// в конце массива параметров
	for (i = 0; i < n; i++)
	{
		// Использование ключевых полей
		if (arFld[i].iFlag == FLD_KEY)
		{
			bKeyFound = TRUE;
			arPrm.SetSize(j + 1);
			arPrm[j].sName    = arFld[i].sName;
			arPrm[j].iSQLType = arFld[i].iSQLType;
			arPrm[j].iSize    = arFld[i].iSize;
			arPrm[j].iPrec    = arFld[i].iPrec;
			arPrm[j].fNull    = arFld[i].fNull;
			arPrm[j].iFlag    = arFld[i].iFlag;
			// Установка ссылки на источник
			arPrm[j].iSrc     = arFld[i].iSrc;
			arPrm[j].iRef     = arFld[i].iRef;
			arPrm[j].sValue   = arFld[i].sValue;
			// Установка ссылки на приемник
			switch (iRefType)
			{
			case REF_TAB:	arPrm[j].iDst = DATA_TABDST; break;
			case REF_SUB:	arPrm[j].iDst = DATA_SUBDST; break;
			default:		arPrm[j].iDst = DATA_TABDST; break;
			}
			arPrm[j].iIdx     = i;
			// Инициализация буфера данных
			arPrm[j].vData.GetBuffer(arPrm[j].iSQLType, 
									 arPrm[j].iSize, TRUE);
			// Формирование строки WHERE
			strPrm += arPrm[j].sName + _T("=? and ");
			j++;
		}
	}
	if (bKeyFound)
	{
		strPrm  = strPrm.Left(strPrm.GetLength()-5);
		strSQL  = strSQL.Left(strSQL.GetLength()-1);
		strSQL  = _T("update ") + strTable + _T(" set ") + strSQL;
		strSQL += _T(" where ") + strPrm;
	}
	else
	{
		strErr = GetErrMsg(ER_NO_KEYS);
		return FALSE;
	}
	strSQL.MakeLower();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Подготовка оператора Delete

BOOL CTransfer::MakeDelete(CFldArray &arFld, CFldArray &arPrm, 
						   CString strTable, CString &strSQL, 
						   REF_TYPE iRefType, CString &strErr) 
{
	strSQL.Empty();
	arPrm.RemoveAll();
	if (strTable.IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_TABLE);
		return FALSE;
	}
	// Формирование оператора DELETE
	BOOL bKeyFound = FALSE;
	CString strPrm;
	// Формирование массива параметров DELETE
	int	 i, j, n = arFld.GetSize();
	for (i = 0, j = 0; i < n; i++)
	{
		if (arFld[i].iFlag == FLD_KEY)
		{
			bKeyFound = TRUE;
			arPrm.SetSize(j + 1);
			arPrm[j].sName    = arFld[i].sName;
			arPrm[j].iSQLType = arFld[i].iSQLType;
			arPrm[j].iSize    = arFld[i].iSize;
			arPrm[j].iPrec    = arFld[i].iPrec;
			arPrm[j].fNull    = arFld[i].fNull;
			arPrm[j].iFlag    = arFld[i].iFlag;
			// Установка ссылки на источник
			arPrm[j].iSrc     = arFld[i].iSrc;
			arPrm[j].iRef     = arFld[i].iRef;
			arPrm[j].sValue   = arFld[i].sName;
			// Установка ссылки на приемник
			switch (iRefType)
			{
			case REF_TAB:	arPrm[j].iDst = DATA_TABDST; break;
			case REF_SUB:	arPrm[j].iDst = DATA_SUBDST; break;
			default:		arPrm[j].iDst = DATA_TABDST; break;
			}
			arPrm[j].iIdx     = i;
			// Инициализация буфера данных
			arPrm[j].vData.GetBuffer(arPrm[j].iSQLType, 
									 arPrm[j].iSize, TRUE);
			// Формирование строки параметров
			strPrm += arPrm[j].sName + _T("=? and ");
			j++;
		}
	}
	if (bKeyFound)
	{
		strPrm  = strPrm.Left(strPrm.GetLength()-5);
		strSQL  = _T("delete from ") + strTable;
		strSQL += _T(" where ") + strPrm;
	}
	else
	{
		strErr = GetErrMsg(ER_NO_KEYS);
		return FALSE;
	}
	strSQL.MakeLower();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Связывание параметров

BOOL CTransfer::BindParams(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
						   CFldArray &arPrm, CString &strErr) 
{
	RETCODE nRet;
	void* pValue;
	int i, nCount = arPrm.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// Получение буфера данных (без инициализации)
		pValue  = arPrm[i].vData.GetBuffer(arPrm[i].iSQLType, 
									arPrm[i].iSize, FALSE);
		switch (arPrm[i].iSQLType)
		{
		// Связывание параметра по ссылке
		case SQL_LONGVARCHAR:
		case SQL_LONGVARBINARY:
		{
			// Установка максимального размера длинных полей
			arPrm[i].iSize = SQLGetTypeDef(*pODBC, arPrm[i].iSQLType).lPrec;
			// Подстановка данных во время выполнения
			arPrm[i].vData.m_lIndex = SQL_LEN_DATA_AT_EXEC(0);
			nRet = ::SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, 
					arPrm[i].vData.m_iType, arPrm[i].iSQLType, 
					arPrm[i].iSize, arPrm[i].iPrec, (SQLPOINTER)i, 
					arPrm[i].vData.m_length, &arPrm[i].vData.m_lIndex);
			break;
		}
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_NUMERIC:
		{
			// Данные устанавливаются перед выполнением
			arPrm[i].vData.m_lIndex = SQL_NTS;
			nRet = ::SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, 
					arPrm[i].vData.m_iType, arPrm[i].iSQLType, 
					arPrm[i].iSize, arPrm[i].iPrec, pValue, 
					arPrm[i].vData.m_length, &arPrm[i].vData.m_lIndex);
			break;
		}
		// Связывание параметра по значению
		case SQL_DATE:
		case SQL_TIME:
		case SQL_TIMESTAMP:
		case SQL_TYPE_DATE:
		case SQL_TYPE_TIME:
		case SQL_TYPE_TIMESTAMP:
		{
			// Данные устанавливаются перед выполнением
			arPrm[i].vData.m_lIndex = arPrm[i].vData.m_length;
			// Использование C-типа вместо SQL-типа
			nRet = ::SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, 
					arPrm[i].vData.m_iType, arPrm[i].vData.m_iType, 
					arPrm[i].iSize, arPrm[i].iPrec, pValue, 
					arPrm[i].vData.m_length, &arPrm[i].vData.m_lIndex);
			break;
		}
		// Связывание параметра по значению
		default:
		{
			// Данные устанавливаются перед выполнением
			arPrm[i].vData.m_lIndex = arPrm[i].vData.m_length;
			nRet = ::SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, 
					arPrm[i].vData.m_iType, arPrm[i].iSQLType, 
					arPrm[i].iSize, arPrm[i].iPrec, pValue, 
					arPrm[i].vData.m_length, &arPrm[i].vData.m_lIndex);
			break;
		}
		}
		if (nRet != SQL_SUCCESS)
		{
			// Чтение текста ошибки
			SQLGetError(hstmt, strErr);
			strErr = arPrm[i].sName + _T("\n")+strErr;
			strErr = GetErrMsg(ER_BIND_PARAM, strErr);
			return FALSE;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Связывание полей данных

BOOL CTransfer::BindFields(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
						   CFldArray &arFld, CString &strErr) 
{
	void* pValue;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// Поля длинных данных не связываются
		// Данные будут считываться дополнительно
		if (arFld[i].iSQLType == SQL_LONGVARCHAR ||
			arFld[i].iSQLType == SQL_LONGVARBINARY)
		{
			// Установка максимального размера длинных полей
			arFld[i].iSize = SQLGetTypeDef(*pODBC, 
							arFld[i].iSQLType).lPrec;
		}
		// Связывание поля по значению
		else
		{
			// Получение буфера данных (без инициализации)
			pValue  = arFld[i].vData.GetBuffer(arFld[i].iSQLType, 
										arFld[i].iSize, FALSE);
			// Связывание поля данных
			if (::SQLBindCol(hstmt, i + 1, arFld[i].vData.m_iType, 
				pValue, arFld[i].vData.m_length, 
				&arFld[i].vData.m_lIndex) != SQL_SUCCESS)
			{
				// Чтение текста ошибки
				SQLGetError(hstmt, strErr);
				strErr = arFld[i].sName + _T("\n")+strErr;
				strErr = GetErrMsg(ER_BIND_FIELD, strErr);
				return FALSE;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Выполнение SQL Select Max

long CTransfer::ExecSQLMax(ODBC_INFO *pODBC, FLD_INFO &fldInfo, 
						   CString &strErr)
{
	void*	pValue;
	CString	strCmd;
	CString strMsg;
	CTime	curTime;
	RETCODE	nRet = SQL_ERROR;

	// Формирование строки SQL оператора для запроса из таблицы приемника
	strCmd = _T("select max(") + fldInfo.sName + _T(") from ") + pTrn->sTabDst;
	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return SQL_ERROR;
	}
	// Подготовка SQL оператора
	AfxGetApp()->BeginWaitCursor();
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strCmd), 
		SQL_NTS) == SQL_ERROR)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		goto out;
	}
	// Получение буфера данных (без инициализации)
	pValue = fldInfo.vData.GetBuffer(fldInfo.iSQLType, 
									 fldInfo.iSize, FALSE);
	// Связывание результирующего поля
	if (::SQLBindCol(hstmt, 1, fldInfo.vData.m_iType, 
		pValue, fldInfo.vData.m_length, 
		&fldInfo.vData.m_lIndex) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_BIND_FIELD, fldInfo.sName);
		goto out;
	}
	// Если установлена отладка SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strCmd + _T("\n\n"));
		}
		// Формирование строки для показа
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strCmd + _T("\n\n") + strMsg;
		// Показ SQL Select
		if (TrnParm.iShowErr)
		{
			if (AfxGetApp()->m_pMainWnd->MessageBox(strMsg, pTrn->sName,
				MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
			{
				bBreaked = TRUE;
				goto out;
			}
		}
		AfxGetApp()->m_pMainWnd->UpdateWindow();
		if (pDlg != NULL) pDlg->UpdateWindow();
	}
	// Выполнение оператора SQL
	if ((nRet = ::SQLExecute(hstmt)) != SQL_SUCCESS)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		goto out;
	}
	// Получение результата
	::SQLFetch(hstmt);

out:
	// Освобождение переменных
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	// Уничтожение дексриптора SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	AfxGetApp()->EndWaitCursor();
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// Выполнение SQL Select Count

long CTransfer::ExecSQLCount(ODBC_INFO *pODBC, CString strCmd, 
							 CFldArray &arPrm, CString &strErr) 
{
	int  i;
	CTime	curTime;
	CString	strMsg;
	long cbCount, nCount = -1;

	// Формирование строки SQL оператора
	strCmd.MakeLower();
	// Удаление выражения <order by>
	if ((i = strCmd.Find("order by")) > 0)
		strCmd = strCmd.Left(i);
	// Замена выражения <select>
	if ((i = strCmd.Find("from")) > 0)
	{
		strCmd.Delete(0, i);
		strCmd.Insert(0, _T("select count(*) "));
	}
	// Заполнение данных для параметров 
	if (!SetFldData(arPrm, OP_COMMAND, strErr))
	{
		strErr = GetErrMsg(ER_SET_PARAMS, strErr);
		return -1;
	}
	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return -1;
	}
	// Подготовка SQL оператора
	AfxGetApp()->BeginWaitCursor();
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strCmd), 
		SQL_NTS) == SQL_ERROR)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		goto out;
	}
	// Связывание параметров SQL Select
	if (!BindParams(pODBC, hstmt, arPrm, strErr))
	{
		strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
		goto out;
	}
	// Связывание результирующего поля
	if (::SQLBindCol(hstmt, 1, SQL_C_SLONG, 
		&nCount, 0, &cbCount) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_BIND_FIELD, _T("count(*)"));
		goto out;
	}
	// Если установлена отладка SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// Формирование строки оператора
		CString strSQL = FormatSQL(strCmd, arPrm);
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strSQL + _T("\n\n"));
		}
		// Формирование строки для показа
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// Показ SQL Select
		if (TrnParm.iShowErr)
		{
			if (AfxGetApp()->m_pMainWnd->MessageBox(strMsg, pTrn->sName,
				MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
			{
				bBreaked = TRUE;
				goto out;
			}
		}
		AfxGetApp()->m_pMainWnd->UpdateWindow();
		if (pDlg != NULL) pDlg->UpdateWindow();
	}
	// Выполнение оператора SQL
	if (::SQLExecute(hstmt) != SQL_SUCCESS)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		goto out;
	}
	// Получение результата
	::SQLFetch(hstmt);

out:
	// Освобождение переменных
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	// Уничтожение дексриптора SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	AfxGetApp()->EndWaitCursor();
	return nCount;
}

/////////////////////////////////////////////////////////////////////
// Выполнение встроенной SQL функции

RETCODE CTransfer::ExecSQLFunc(ODBC_INFO *pODBC, FNC_TYPE func, 
								CString strParms, CString &strVal, 
								CString &strErr)
{
	long i;
	CTime	curTime;
	CFldArray arPrm;
	CDataValue dataValue;
	CString sFnc,sCmd,sFld,sTab,sExp;
	CString strMsg;
	RETCODE nRet = SQL_ERROR;

	// Разбор параметров функции
	if (func != FNC_SQLCOUNT)
	{
		// Наименование поля запроса
		if ((i = strParms.Find(',')) >= 0)
		{
			sFld = strParms.Left(i);
			sFld.TrimLeft();
			sFld.TrimRight();
			strParms = strParms.Mid(i + 1);
			strParms.TrimLeft();
		}
	}
	// Таблица запроса
	if ((i = strParms.Find(',')) >= 0)
	{
		sTab = strParms.Left(i);
		sTab.TrimLeft();
		sTab.TrimRight();
		strParms = strParms.Mid(i + 1);
		strParms.TrimLeft();
	}
	strParms.TrimRight();

	// Выражение SQL Where
	if (!strParms.IsEmpty())
	{
		sExp = _T(" where ") + strParms;
	}
	// Формирование строки SQL оператора
	switch (func)
	{
		case FNC_SQLGETVAL:
		{
			sFnc = _T("SQLGetVal");
			sCmd = _T("select ") + sFld + _T(" from ") + sTab + sExp;
			break;
		}
		case FNC_SQLMAXVAL:
		{
			sFnc = _T("SQLMaxVal");
			sCmd = _T("select max(") + sFld + _T(") from ") + sTab + sExp;
			break;
		}
		case FNC_SQLMINVAL:
		{
			sFnc = _T("SQLMinVal");
			sCmd = _T("select min(") + sFld + _T(") from ") + sTab + sExp;
			break;
		}
		case FNC_SQLAVGVAL:
		{
			sFnc = _T("SQLAvgVal");
			sCmd = _T("select avg(") + sFld + _T(") from ") + sTab + sExp;
			break;
		}
		case FNC_SQLSUMVAL:
		{
			sFnc = _T("SQLSumVal");
			sCmd = _T("select sum(") + sFld + _T(") from ") + sTab + sExp;
			break;
		}
		case FNC_SQLVARVAL:
		{
			sFnc = _T("SQLVarVal");
			sCmd = _T("select var(") + sFld + _T(") from ") + sTab + sExp;
			break;
		}
		case FNC_SQLCOUNT:
		{
			sFnc = _T("SQLCount");
			sCmd = _T("select count(*) from ") + sTab + sExp;
			break;
		}
		default:
		{
			strErr = GetErrMsg(ER_FIND_FUNC, _T(""));
			return SQL_ERROR;
		}
	}
	// Подготовка параметров SQL Where (всегда в схеме pDst->sUserName)
	if (!PrepareSQLParms(pDst, &arPrm, pDst->sUserName, sCmd, strErr))
	{
		strErr = GetErrMsg(ER_PARSE_PARAMS, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + sCmd;
		return SQL_ERROR;
	}
	// Заполнение данных для параметров 
	if (!SetFldData(arPrm, OP_COMMAND, strErr))
	{
		strErr = GetErrMsg(ER_SET_PARAMS, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + sCmd;
		return SQL_ERROR;
	}
	// Создание дескриптора SQL оператора
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		return SQL_ERROR;
	}
	// Подготовка SQL оператора
	AfxGetApp()->BeginWaitCursor();
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(sCmd), 
		SQL_NTS) == SQL_ERROR)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		goto out;
	}
	if (arPrm.GetSize() > 0)
	{
		// Связывание параметров SQL Select
		if (!BindParams(pODBC, hstmt, arPrm, strErr))
		{
			strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
			strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
			goto out;
		}
	}
	// Получение информации о результирующем поле
	SQLCHAR*	szColName;
	SQLSMALLINT	nmColName, nmColLen;
	SQLSMALLINT	nDataType, nColDec, fNullable;
	SQLUINTEGER	nColSize;
	SQLINTEGER	cbColSize;
	void*		pValue;
	// Определение максимальной длины наименования поля
	nmColName = pODBC->iMaxColNameLen + 1;
	// Выделение памяти для наименования поля
	szColName = (SQLCHAR*)malloc((nmColName)*sizeof(SQLCHAR));
	// Считывание описания результирующего поля
	if (::SQLDescribeCol(hstmt, 1, szColName, 
		nmColName, &nmColLen, &nDataType, 
		&nColSize, &nColDec, &fNullable) != SQL_SUCCESS)
	{
		SQLGetError(hstmt, strErr);
		strErr = GetErrMsg(ER_DESC_FIELDS, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		free(szColName);
		goto out;
	}
	// Освобождение памяти
	free(szColName);
	// Инициализация буфера данных
	pValue = dataValue.GetBuffer(nDataType, nColSize, TRUE);
	// Связывание результирующего поля
	if (::SQLBindCol(hstmt, 1, nDataType, 
		pValue, nColSize, &cbColSize) != SQL_SUCCESS)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		strErr = GetErrMsg(ER_BIND_FIELD, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		goto out;
	}

	// Если установлена отладка SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// Формирование строки оператора
		CString strSQL = FormatSQL(sCmd, arPrm);
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strSQL + _T("\n\n"));
		}
		// Формирование строки для показа
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// Показ SQL Select
		if (TrnParm.iShowErr)
		{
			if (AfxGetApp()->m_pMainWnd->MessageBox(strMsg, pTrn->sName,
				MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
			{
				bBreaked = TRUE;
				goto out;
			}
		}
		AfxGetApp()->m_pMainWnd->UpdateWindow();
		if (pDlg != NULL) pDlg->UpdateWindow();
	}
	// Выполнение оператора SQL
	if ((nRet = ::SQLExecute(hstmt)) != SQL_SUCCESS)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		goto out;
	}
	// Получение результата
	::SQLFetch(hstmt);

	// Перевод результата в строку
	strVal = dataValue.GetString();

out:
	// Освобождение переменных
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	// Уничтожение дексриптора SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	AfxGetApp()->EndWaitCursor();
	return nRet;
}
/////////////////////////////////////////////////////////////////////
// Выполнение подготовленного SQL Select (и чтение строки данных)

RETCODE CTransfer::ExecSelect(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
							  CFldArray &arFld, CFldArray &arPrm, 
							  CString &strCmd,  BOOL bFetchData, 
							  CString &strErr) 
{
	CTime	curTime;
	CString strMsg;

	// Создание дескриптора SQL
	if (hstmt == SQL_NULL_HSTMT)
	{
		if (::SQLAllocHandle(SQL_HANDLE_STMT, 
			pODBC->hdbc, &hstmt) != SQL_SUCCESS)
		{
			strErr = GetErrMsg(ER_CRT_SQL);
			hstmt = SQL_NULL_HSTMT;
			return SQL_ERROR;
		}
	}
	else
	{
		// Освобождение дескриптора SQL
		::SQLFreeStmt(hstmt, SQL_CLOSE);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	}
	// Установка данных в массиве параметров
	if (!SetFldData(arPrm, OP_COMMAND, strErr))
	{
		strErr = GetErrMsg(ER_SET_PARAMS, strErr);
		return SQL_ERROR;
	}
	// Связывание параметров
	if (!BindParams(pODBC, hstmt, arPrm, strErr))
	{
		strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
		::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = SQL_NULL_HSTMT;
		return SQL_ERROR;
	}
	// Если установлена отладка SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// Формирование строки оператора
		CString strSQL = FormatSQL(strCmd, arPrm);
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strSQL + _T("\n\n"));
		}
		// Формирование строки для показа
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// Показ SQL Select
		if (TrnParm.iShowErr)
		{
			if (AfxGetApp()->m_pMainWnd->MessageBox(strMsg, pTrn->sName,
				MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
			{
				bBreaked = TRUE;
				return SQL_ERROR;
			}
		}
		AfxGetApp()->m_pMainWnd->UpdateWindow();
		if (pDlg != NULL) pDlg->UpdateWindow();
	}
	// Выполнение SQL оператора
	RETCODE nRet = ::SQLExecDirect(hstmt, (SQLTCHAR*)LPCTSTR(strCmd), SQL_NTS);
	// Передача данных для несвязанных параметров
	if (nRet == SQL_NEED_DATA)
	{
		SQLPOINTER nIdx;
		// Определение индекса элемента массива
		while ((nRet = ::SQLParamData(hstmt, &nIdx)) == SQL_NEED_DATA)
		{
			PutLongData(hstmt, arPrm[(int)nIdx].vData);
		}
	}
	// Если ошибка выполнения
	if (nRet == SQL_ERROR)
	{
		// Чтение текста ошибки
		SQLGetError(hstmt, strErr);
		return nRet;
	}
	// Связывание полей данных
	if (!BindFields(pODBC, hstmt, arFld, strErr))
	{
		strErr = GetErrMsg(ER_BIND_FIELDS, strErr);
		return SQL_ERROR;
	}
	// Считывание строки SQL запроса
	if (bFetchData)
	{
		nRet = FetchData(hstmt, arFld, strErr);
		if (nRet == SQL_ERROR)
		{
			// Чтение текста ошибки
			SQLGetError(hstmt, strErr);
			return nRet;
		}
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// Выполнение подготовленной команды SQL DML

RETCODE CTransfer::ExecUpdate(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
							  OP_TYPE iOpType,  CFldArray &arPrm, 
							  CString &strCmd,  BOOL bUseSrcData, 
							  CString &strErr) 
{
	CTime curTime;
	CString	strMsg;
	if (hstmt == SQL_NULL_HSTMT)
	{
		// Создание дескриптора SQL оператора
		if (::SQLAllocHandle(SQL_HANDLE_STMT, 
			pODBC->hdbc, &hstmt) != SQL_SUCCESS)
		{
			strErr = GetErrMsg(ER_CRT_SQL);
			hstmt = SQL_NULL_HSTMT;
			return SQL_ERROR;
		}
	}
	else
	{
		// Освобождение параметров SQL оператора
		::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	}
	// Установка данных в массиве параметров
	if (bUseSrcData)
	{
		// по ссылке на источник
		if (!SetFldData(arPrm, iOpType, strErr))
		{
			strErr = GetErrMsg(ER_SET_PARAMS, strErr);
			return SQL_ERROR;
		}
		// Конвертация текстовых данных
		if (pTrn->iConvert != TRN_UNCHANGED)
			ConvertTxt(arPrm, iOpType, pTrn->iConvert);
	}
	else
	{
		// по ссылке на приемник
		if (!SetDstData(arPrm, strErr))
		{
			strErr = GetErrMsg(ER_SET_PARAMS, strErr);
			return SQL_ERROR;
		}
	}
	// Связывание параметров
	if (!BindParams(pODBC, hstmt, arPrm, strErr))
	{
		strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
		return SQL_ERROR;
	}
	// Если установлена отладка SQL операторов
	if (TrnParm.iTraceOpr > 0)
	{
		// Формирование строки оператора
		CString strSQL = FormatSQL(strCmd, arPrm);
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			switch (iOpType)
			{
			case OP_INSERT: strMsg = _T("SQL INSERT:\n"); break;
			case OP_UPDATE: strMsg = _T("SQL UPDATE:\n"); break;
			case OP_DELETE: strMsg = _T("SQL DELETE:\n"); break;
			default:		strMsg = _T("");
			}
			curTime = CTime::GetCurrentTime();
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strSQL + _T("\n\n"));
		}
		// Формирование строки для показа
		CString strMsg;
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// Показ SQL оператора
		if (TrnParm.iShowErr)
		{
			if (AfxGetApp()->m_pMainWnd->MessageBox(strMsg, pTrn->sName,
				MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
			{
				bBreaked = TRUE;
				return SQL_ERROR;
			}
		}
		AfxGetApp()->m_pMainWnd->UpdateWindow();
		if (pDlg != NULL) pDlg->UpdateWindow();
	}
	// Выполнение SQL оператора
	RETCODE nRet = ::SQLExecDirect(hstmt, (SQLTCHAR*)LPCTSTR(strCmd), SQL_NTS);
	// Передача данных для несвязанных параметров
	if (nRet == SQL_NEED_DATA)
	{
		SQLPOINTER nIdx;
		// Определение индекса элемента массива
		while ((nRet = ::SQLParamData(hstmt, &nIdx)) == SQL_NEED_DATA)
		{
			PutLongData(hstmt, arPrm[(int)nIdx].vData);
		}
	}
	// Если ошибка выполнения
	if (nRet == SQL_ERROR)
	{
		SQLGetError(hstmt, strErr);
		// Откат транзакции
		if (bTxnCommit)
		{
			::SQLEndTran(SQL_HANDLE_DBC, pODBC->hdbc, SQL_ROLLBACK);
		}
		return nRet;
	}
	// Успешно с замечанием
	else if (nRet == SQL_SUCCESS_WITH_INFO)
	{
		SQLGetError(hstmt, strErr);
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// Выполнение SQL-команды приемника

RETCODE CTransfer::ExecSQLCmd(ODBC_INFO *pODBC, CCmdArray &arCmd, 
							  EV_TYPE iEventID, LPCTSTR szField, 
							  CString &strErr) 
{
	// Поиск команды по идентификатору
	int i = FindSQLCmd(arCmd, iEventID, szField);
	if (i < 0) return SQL_SUCCESS;
	// Выполнение команды
	RETCODE nRet = SQL_SUCCESS;
	CFldArray *pArPrm;
	CTime	curTime;
	CString strEvt,strCmd,strMsg, strSQL;
	// Цикл по списку подкоманд одной команды
	for (int j = 0; j < arCmd[i].nCount; j++)
	{
		if (arCmd[i].hstmt == SQL_NULL_HSTMT)
		{
			// Создание дескриптора SQL команды
			if (::SQLAllocHandle(SQL_HANDLE_STMT, 
				pODBC->hdbc, &arCmd[i].hstmt) != SQL_SUCCESS)
			{
				strErr = GetErrMsg(ER_CRT_SQL);
				arCmd[i].hstmt = SQL_NULL_HSTMT;
				return SQL_ERROR;
			}
		}
		else
		{
			// Освобождение параметров SQL команды
			::SQLFreeStmt(arCmd[i].hstmt, SQL_RESET_PARAMS);
		}
		// Получение строки подкоманды
		strCmd = arCmd[i].pArCmd->ElementAt(j);
		// Получение параметров подкоманды
		pArPrm = arCmd[i].pArPrm->ElementAt(j);
		// Установка параметров подкоманды
		if (!SetFldData(*pArPrm, OP_COMMAND, strErr))
		{
			strErr = GetErrMsg(ER_SET_PARAMS, strErr);
			strSQL = strCmd; nRet = SQL_ERROR; goto err;
		}
		// Конвертация текстовых данных
		if (pTrn->iConvert != TRN_UNCHANGED)
			ConvertTxt(*pArPrm, OP_COMMAND, pTrn->iConvert);
		// Связывание параметров
		if (!BindParams(pODBC, arCmd[i].hstmt, *pArPrm, strErr))
		{
			strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
			nRet = SQL_ERROR; goto err;
		}
		// Форматирование строки SQL
		strSQL = FormatSQL(strCmd, *pArPrm);
		// Если установлена отладка SQL команд
		if (TrnParm.iTraceCmd > 0)
		{
			// Запись протокола операции в файл
			if (bFileLog && TrnParm.iWriteOpr)
			{
				curTime = CTime::GetCurrentTime();
				strMsg = _T("SQL COMMAND:\n");
				strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
				fileLog.WriteString(strMsg);
				fileLog.WriteString(strSQL + _T("\n\n"));
			}
			// Формирование строки для показа
			CString strMsg;
			strMsg.LoadString(IDS_MS_BREAK);
			strMsg = strSQL + _T("\n\n") + strMsg;
			// Показ SQL команды
			if (TrnParm.iShowErr)
			{
				if (AfxGetApp()->m_pMainWnd->MessageBox(strMsg, pTrn->sName,
					MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
				{
					bBreaked = TRUE;
					nRet = SQL_ERROR;
					goto err;
				}
			}
			AfxGetApp()->m_pMainWnd->UpdateWindow();
			if (pDlg != NULL) pDlg->UpdateWindow();
		}
		// Выполнение одной подкоманды
		nRet = ::SQLExecDirect(arCmd[i].hstmt,(SQLTCHAR*)LPCTSTR(strCmd),SQL_NTS);
		// Передача данных для несвязанных полей
		if (nRet == SQL_NEED_DATA)
		{
			SQLPOINTER nIdx;
			nRet = ::SQLParamData(arCmd[i].hstmt, &nIdx);
			// Определение индекса элемента массива
			while (nRet == SQL_NEED_DATA)
			{
				// Установка длинных данных
				PutLongData(arCmd[i].hstmt, 
							pArPrm->GetAt((int)nIdx).vData);
				nRet = ::SQLParamData(arCmd[i].hstmt, &nIdx);
			}
		}
		// Если нет данных
		if (nRet == SQL_NO_DATA) return nRet;
		// Если ошибка выполнения
		else if (nRet == SQL_ERROR)
		{
			SQLGetError(arCmd[i].hstmt, strErr);
			// Откат транзакции
			if (bTxnCommit)
			{
				::SQLEndTran(SQL_HANDLE_DBC, pODBC->hdbc, SQL_ROLLBACK);
			}
			goto err;
		}
		// Успешно с замечанием
		else if (nRet == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetError(arCmd[i].hstmt, strErr);
			goto err;
		}
	}
	return nRet;

err:
	strEvt = arCmd[i].sEvent;
	if (arCmd[i].iEvent == EV_ON_CHANGE)
		strEvt += _T(".")+arCmd[i].sField;
	strErr = strEvt + _T(":\n") + strSQL + _T("\n") + strErr;
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// Выполнение SQL-команд в полях таблицы приемника

RETCODE CTransfer::ExecFldCmd(ODBC_INFO *pODBC, CFldArray &arFld, 
							  CCmdArray &arCmd, EV_TYPE iEventID, 
							  CString &strErr) 
{
	RETCODE nRet = SQL_SUCCESS;
	CString strRet;
	int i, k, n = arFld.GetSize();
	for (i = 0, k = 0; i < n; i++)
	{
		if (arFld[i].iFlag == FLD_EVCHANGE && arFld[i].bMod)
		{
			// Выполнение SQL-команды для поля <arFld[i].sName>
			nRet = ExecSQLCmd(pODBC,arCmd,iEventID,arFld[i].sName,strErr);
			if (nRet == SQL_ERROR)	goto err;	// Обработка ошибки
			else					k = i;		// Запомним номер поля
		}
	}
	return nRet;

err:
	if (!bTxnCommit)
	{
		// Реверсирование идентификатора команды
		switch (iEventID)
		{
		case EV_UNKNOWN:	iEventID = EV_UNKNOWN;   break;
		case EV_ON_START:	iEventID = EV_UNKNOWN;   break;
		case EV_ON_END:		iEventID = EV_UNKNOWN;   break;
		case EV_CALL_EXEC:	iEventID = EV_CALL_BACK; break;
		case EV_CALL_BACK:	iEventID = EV_CALL_EXEC; break;
		case EV_ON_INSERT:	iEventID = EV_RE_INSERT; break;
		case EV_ON_UPDATE:	iEventID = EV_RE_UPDATE; break;
		case EV_ON_DELETE:	iEventID = EV_RE_DELETE; break;
		case EV_RE_INSERT:	iEventID = EV_ON_INSERT; break;
		case EV_RE_UPDATE:	iEventID = EV_ON_UPDATE; break;
		case EV_RE_DELETE:	iEventID = EV_ON_DELETE; break;
		case EV_ON_CHANGE:	iEventID = EV_RE_CHANGE; break;
		case EV_RE_CHANGE:	iEventID = EV_ON_CHANGE; break;
		}
		// Обработка в обратном порядке (откат команд)
		for (i = k; i >= 0; i--)
		{
			if (arFld[i].iFlag == FLD_EVCHANGE && arFld[i].bMod)
			{
				// Откат SQL-команды для поля <arFld[i].sName>
				nRet = ExecSQLCmd(pODBC,arCmd,iEventID,arFld[i].sName,strRet);
				if (nRet == SQL_ERROR)			// Ошибка отката
				{
					// Показ сообщения об ошибке
					if (bShowErr) ShowErr(pTrn->sName, strRet);
				}
			}
		}
	}
	return SQL_ERROR;
}

/////////////////////////////////////////////////////////////////////
// Чтение строки данных

RETCODE CTransfer::FetchData(SQLHSTMT &hstmt, CFldArray &arFld, 
							 CString &strErr) 
{
	RETCODE nRet = ::SQLFetch(hstmt);
	
	switch (nRet)
	{
	case SQL_SUCCESS_WITH_INFO:
	{
		// Прочитать сообщение и продолжить
		SQLGetError(hstmt, strErr);
	}
	case SQL_SUCCESS:
	{
		CString subErr;
		int i, nCols = arFld.GetSize();
		// Завершение чтения данных
		for (i = 0; i < nCols; i++)
		{
			if (arFld[i].iSQLType == SQL_LONGVARCHAR ||
				arFld[i].iSQLType == SQL_LONGVARBINARY)
			{
				arFld[i].vData.SetNull();
				// Чтение длинных данных
				nRet = GetLongData(hstmt, i + 1, arFld[i].vData, strErr);
				if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
					return nRet;
			}
			// Установка пустого значения
			if (arFld[i].vData.m_lIndex == SQL_NULL_DATA)
			{
				arFld[i].vData.SetNull();
			}
		}
		break;
	}
	case SQL_ERROR:		SQLGetError(hstmt, strErr);	break;
	default:			strErr.Empty();				break;
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// Позиционное чтение данных

RETCODE CTransfer::FetchScroll(SQLHSTMT &hstmt, CFldArray &arFld, 
							   USHORT iDirection, UINT iOffset, 
							   CString &strErr) 
{
	RETCODE nRet = ::SQLFetchScroll(hstmt, 
					(SQLSMALLINT)iDirection, 
					(SQLINTEGER)iOffset);
	
	switch (nRet)
	{
	case SQL_SUCCESS_WITH_INFO:
	{
		// Прочитать сообщение и продолжить
		SQLGetError(hstmt, strErr);
	}
	case SQL_SUCCESS:
	{
		int i, nCols = arFld.GetSize();
		for (i = 0; i < nCols; i++)
		{
			if (arFld[i].iSQLType == SQL_LONGVARCHAR ||
				arFld[i].iSQLType == SQL_LONGVARBINARY)
			{
				// Чтение длинных данных
				nRet = GetLongData(hstmt, i + 1, arFld[i].vData, strErr);
				if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
					return nRet;
			}
			// Установка пустого значения
			if (arFld[i].vData.m_lIndex == SQL_NULL_DATA)
				arFld[i].vData.SetNull();
		}
		break;
	}
	case SQL_ERROR:		SQLGetError(hstmt, strErr);	break;
	default:			strErr.Empty();				break;
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// Получение длинных данных для SQL оператора

RETCODE CTransfer::GetLongData(SQLHSTMT &hstmt, UINT iCol, 
							   CDataValue &vData, CString &strErr) 
{
	switch (vData.m_iType)
	{
	case SQL_C_CHAR:		// SQL_LONGVARCHAR
	{
		// Установка минимальной длины буфера (1)
		TCHAR* pstr = (TCHAR*)vData.GetBuffer(SQL_LONGVARCHAR, 1, TRUE);
		// Пробное чтение для определения общего размера данных
		RETCODE nRet = ::SQLGetData(hstmt, iCol, vData.m_iType, 
						pstr, vData.m_length, &vData.m_lIndex);
		// Если считывание выполнено успешно 
		// с замечанием о нехватке длины буфера
		if (nRet == SQL_SUCCESS_WITH_INFO && 
			(long)vData.m_length <= vData.m_lIndex)
		{
			// Инициализация буфера общего размера данных
			pstr = (TCHAR*)vData.InitType(SQL_C_CHAR, vData.m_lIndex + 1);
			vData.m_bLong = TRUE;
			// Повторное чтение данные
			return ::SQLGetData(hstmt, iCol, vData.m_iType, 
					pstr, vData.m_length, &vData.m_lIndex);
		}
		else if (nRet != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			return nRet;
		}
		if (vData.m_lIndex == SQL_NULL_DATA )
		{
			vData.m_length = 0;
		}
		break;
	}
	case SQL_C_BINARY:		// SQL_LONGVARBINARY
	{
		BYTE data;
		// Установка минимальной длины буфера (1)
		BYTE* pbin = (BYTE*)vData.GetBuffer(SQL_LONGVARBINARY, 1, TRUE);
		// Пробное чтение для определения общего размера данных
		RETCODE nRet = ::SQLGetData(hstmt, iCol, vData.m_iType, 
						&data, vData.m_length, &vData.m_lIndex);
		// Если считывание выполнено успешно 
		// с замечанием о нехватке длины буфера
		if (nRet == SQL_SUCCESS_WITH_INFO && 
			(long)vData.m_length <= vData.m_lIndex)
		{
			// Инициализация буфера общего размера данных
			pbin = (BYTE*)vData.InitType(SQL_C_BINARY, vData.m_lIndex + 1);
			vData.m_bLong = TRUE;
			// Запись первого байта
			*pbin = data; pbin++;
			// Повторное чтение данные
			return ::SQLGetData(hstmt, iCol, vData.m_iType, 
					pbin, vData.m_length, &vData.m_lIndex);
		}
		else if (nRet != SQL_SUCCESS)
		{
			SQLGetError(hstmt, strErr);
			return nRet;
		}
		if (vData.m_lIndex == SQL_NULL_DATA )
		{
			vData.m_length = 0;
		}
		break;
	}
	}
	return SQL_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// Установка длинных данных для SQL оператора

RETCODE CTransfer::PutLongData(SQLHSTMT &hstmt, CDataValue &vData) 
{
	switch (vData.m_iType)
	{
	case SQL_C_CHAR:		// SQL_LONGVARCHAR
	{
		if (vData.m_length > 0)
		{
			void* pstr = vData.GetBuffer(SQL_LONGVARCHAR, 0, FALSE);
			return ::SQLPutData(hstmt, (SQLPOINTER)pstr, SQL_NTS);
		}
		else
		{
			return ::SQLPutData(hstmt, (SQLPOINTER)0, SQL_NULL_DATA);
		}
	}
	case SQL_C_BINARY:		// SQL_LONGVARBINARY
	{
		if (vData.m_length > 0)
		{
			void* pbin = vData.GetBuffer(SQL_LONGVARBINARY, 0, FALSE);
			return ::SQLPutData(hstmt, (SQLPOINTER)pbin, vData.m_length);
		}
		else
		{
			return ::SQLPutData(hstmt, (SQLPOINTER)0, SQL_NULL_DATA);
		}
	}
	}
	return SQL_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// Формирование строки сообщения

CString CTransfer::FormatMsg(LPCTSTR szMsg) 
{
	CString s,strErr,strMsg(szMsg);
	// Заполнение данных для параметров
	if (!SetFldData(arStrMsgPrm, OP_COMMAND, strErr))
	{
		return strMsg;
	}
	// Форматирование строки
	int m = arStrMsgPrm.GetSize();
	for (int i=0, n=0; i<m && (n=strMsg.Find(_T("?"),n))>=0; i++)
	{
		// Замена знака '?' на значение ссылки
		s = arStrMsgPrm.GetAt(i).vData.GetString();
		strMsg.Delete(n, 1);
		strMsg.Insert(n, s);
		n += s.GetLength();
	}
	return strMsg;
}

/////////////////////////////////////////////////////////////////////
// Формирование строки SQL оператора (кроме Select)

CString CTransfer::FormatSQL(LPCTSTR szSQL, CFldArray &arPrm) 
{
	// Форматирование строки
	CString s,strSQL(szSQL);
	int m = arPrm.GetSize();
	for (int i = 0, n = 0; i < m && (n = strSQL.Find(_T("?"),n))>=0; i++)
	{
		// Замена знака '?' на значение ссылки
		s = arPrm.GetAt(i).vData.GetString();
		// Заключение значения в кавычки
		// для текста или даты-времени
		switch (arPrm.ElementAt(i).vData.m_iType)
		{
		case SQL_C_CHAR:
		case SQL_C_TIME:
		case SQL_C_DATE:
		case SQL_C_TIMESTAMP:
		case SQL_C_BINARY:
			s = _T("'") + s + _T("'");
			break;
		}
		strSQL.Delete(n, 1);
		strSQL.Insert(n, s);
		n += s.GetLength();
	}
	return strSQL;
}

/////////////////////////////////////////////////////////////////////
// Формирование строки SQL оператора по идентификатору ошибки

CString CTransfer::FormatUpd(ER_TYPE iErrType) 
{
	CString strOut("\n");
	switch (iErrType)		// Тип ошибки
	{
    case ER_INSTAB: strOut+= FormatSQL(sDstTabIns, arTabInsPrm); break;
    case ER_UPDTAB: strOut+= FormatSQL(sDstTabUpd, arTabUpdPrm); break;
	case ER_DELTAB: strOut+= FormatSQL(sDstTabDel, arTabDelPrm); break;
    case ER_INSSUB: strOut+= FormatSQL(sDstSubIns, arSubInsPrm); break;
    case ER_UPDSUB: strOut+= FormatSQL(sDstSubUpd, arSubUpdPrm); break;
	case ER_DELSUB: strOut+= FormatSQL(sDstSubDel, arSubDelPrm); break;
	default:		strOut = _T("");							 break;
	}
	return strOut;
}

/////////////////////////////////////////////////////////////////////
// Формирование строки ошибки с заголовком сообщения

CString CTransfer::FormatErr(LPCTSTR szErr) 
{
	CString strErr(szErr);
	if (bMessage && pDlg != NULL)
	{
		strErr = pDlg->m_sText + _T(":\n") + strErr;
	}
	return strErr;
}

/////////////////////////////////////////////////////////////////////
// Проверка статуса трансфера

BOOL CTransfer::IsValid()
{
	return bValid;
}

/////////////////////////////////////////////////////////////////////
// Подготовка трансферта к выполнению

BOOL CTransfer::Parse(CString &strErr) 
{
	strErr.Empty();

	if (pSrc == NULL || pDst == NULL || pTrn == NULL)
		return FALSE;
	if (pSrc->hdbc == SQL_NULL_HDBC) return FALSE;
	if (pDst->hdbc == SQL_NULL_HDBC) return FALSE;

	//////////////////////////////////////////////////////////////////
	//                УСТАНОВКА ОБЩИХ ПАРАМЕТРОВ
	//////////////////////////////////////////////////////////////////
	// Установка признака COMPLEX
	if (pTrn->iSchema == TRN_COMPLEX)
	{
#ifdef _TRIAL_VER
		strErr.LoadString(IDS_ER_TRIAL_COMPLEX);
		return FALSE;
#else
		bComplex = TRUE;
#endif
	}
	// Установка наличия строки сообщения
	if (!pTrn->sMsg.IsEmpty())
		bMessage = TRUE;
	// Установка типа Commit
	if (pDst->iTxnCapable == SQL_TC_DDL_COMMIT)
		bTxnCommit = TRUE;
	else
		bTxnCommit = FALSE;
	// Установка необходимости поиска данных
	// по ключевым полям в таблице приемника
	if (pTrn->iTabPrc == DST_CREATE ||
		pTrn->iTabPrc == DST_ADD)
		bTabSearch = FALSE;
	else
		bTabSearch = TRUE;
	// Установка необходимости поиска данных
	// по ключевым полям в субтаблице приемника
	if (pTrn->iSubPrc == DST_CREATE ||
		pTrn->iSubPrc == DST_ADD)
		bSubSearch = FALSE;
	else
		bSubSearch = TRUE;

	//////////////////////////////////////////////////////////////////
	//                ОБРАБОТКА ОСНОВНОЙ ТАБЛИЦЫ
	//////////////////////////////////////////////////////////////////
	// Подготовка колонок SQL Select таблицы источника
	if (!ParseColumns(pSrc, arSrcTabFld, pTrn->sTabSQL, strErr))
	{
		strErr = GetErrMsg(ER_PREP_SELSRCTAB, strErr);
		return FALSE;
	}
	// При создании таблицы приемника из источника или
	// при полном сравнении источника и приемника: 
	// Установка ключевых полей в таблице источника
	if (pTrn->iTabPrc == DST_CREATE && pTrn->iSchema == TRN_COPY    ||
		pTrn->iTabPrc == DST_CREATE && pTrn->iSchema == TRN_COMPLEX ||
		pTrn->iTabPrc == DST_FULL)
	{
		// Определение таблицы в SQL запросе источника
		if (!TableInSQL(pTrn->sTabSQL, sSrcMTable, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SRCTAB, strErr);
			return FALSE;
		}
		// Установка ключевых полей таблицы источника
		if (!SetFldKeys(pSrc, arSrcTabFld, pSrc->sUserName, 
						sSrcMTable, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SRCTAB, strErr);
			return FALSE;
		}
	}
	// Заполнение карты полей таблицы приемника
	if (!FillFieldArray(pTrn->sTabMap, arMapTabFld))
	{
		strErr = GetErrMsg(ER_SNTX_MAPTAB, strErr);
		return FALSE;
	}
	// Подготовка таблицы приемника
	if (pTrn->iTabPrc == DST_CREATE)
	{
		// Создание таблицы приемника
		if (!CreateTable(pDst, pTrn->iSchema, arDstTabFld, 
						 arMapTabFld, arSrcTabFld, 
						 pTrn->sTabDst, strErr))
		{
			strErr = GetErrMsg(ER_CRT_TAB, strErr);
			return FALSE;
		}
	}
	else
	{
		// Подготовка массива полей таблицы приемника
		if (!ParseTable(pDst, pTrn->iSchema, arDstTabFld, arMapTabFld, 
						(pTrn->sTabOwn.IsEmpty()) ? pDst->sUserName : pTrn->sTabOwn, 
						pTrn->sTabDst, strErr))
		{
			strErr = GetErrMsg(ER_PREP_DSTTAB, strErr);
			return FALSE;
		}
	}
	// Установка ссылок на источник данных
	// в массиве полей таблицы приемника
	if (!SetFldRefs(arDstTabFld, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_DSTTAB, strErr);
		return FALSE;
	}
	// Если необходим поиск в таблице приемника
	if (bTabSearch)
	{
		// Конструирование SQL запроса поиска в таблице приемника
		// по условию значения первичных ключей таблицы приемника
		if (!MakeSelect(arDstTabFld, pTrn->sTabDst, _T(""), 
						TRUE, sDstTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDDSTTAB, strErr);
			return FALSE;
		}
	}
	// При полном сравнении источника и приемника
	if (pTrn->iTabPrc == DST_FULL)
	{
		// Установка ссылок на приемник
		// для ключевых полей источника
		if (!SetDstRefs(arSrcTabFld, REF_TAB, TRUE, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SRCTAB, strErr);
			return FALSE;
		}
		// Конструирование SQL запроса поиска в осн.источнике
		// по условию значения первичных ключей таблицы
		if (!MakeSelect(arSrcTabFld, sSrcMTable, _T(""), 
						TRUE, sSrcTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDSRCTAB, strErr);
			return FALSE;
		}
		// Конструирование SQL Select таблицы приемника
		// по заданному условию
		if (!MakeSelect(arDstTabFld, pTrn->sTabDst, pTrn->sTabCond, 
						FALSE, sDstTabSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SELDSTTAB, strErr);
			return FALSE;
		}
	}
	// Заполнение массива команд таблицы приемника
	if (!FillCmdArray(pTrn->sTabCmd, arDstTabCmd))
	{
		strErr = GetErrMsg(ER_SNTX_CMDTAB, strErr);
		return FALSE;
	}
	// Подготовка массива команд таблицы приемника
	if (!ParseCmdArr(pDst, REF_TAB, arDstTabCmd, FALSE, strErr))
	{
		strErr = GetErrMsg(ER_PREP_CMDTAB, strErr);
		return FALSE;
	}
	// Подготовка признака команд CHANGE в массиве полей приемника
	if (!ParseFldCmd(arDstTabFld, arDstTabCmd, strErr))
	{
		return FALSE;
	}
	// Установка необходимости выполнения команд
	// при обработке каждой строки таблицы приемника
	if (FindEvent(arDstTabCmd, EV_ON_INSERT) >= 0 ||
		FindEvent(arDstTabCmd, EV_ON_UPDATE) >= 0 ||
		FindEvent(arDstTabCmd, EV_ON_DELETE) >= 0 ||
		pTrn->iSchema == TRN_COMPLEX)
		bTabRowCmd = TRUE;
	// Установка необходимости выполнения команд OnChange
	// при изменении данных в полях таблицы приемника
	if (FindEvent(arDstTabCmd, EV_ON_CHANGE) >= 0)
		bTabFldCmd = TRUE;

#ifndef _TRIAL_VER
	//////////////////////////////////////////////////////////////////
	//            ОБРАБОТКА ПОДЧИНЕННОЙ ТАБЛИЦЫ
	//////////////////////////////////////////////////////////////////
	if (pTrn->iSchema == TRN_COMPLEX)
	{
		// Подготовка колонок SQL Select субтаблицы источника
		if (!ParseColumns(pSrc, arSrcSubFld, pTrn->sSubSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SELSRCSUB, strErr);
			return FALSE;
		}
		// При создании субтаблицы приемника из источника или
		// при полном сравнении источника и приемника: 
		// Установка ключевых полей в субтаблице источника
		if (pTrn->iSubPrc == DST_CREATE && pTrn->iSchema == TRN_COPY    ||
			pTrn->iSubPrc == DST_CREATE && pTrn->iSchema == TRN_COMPLEX ||
			pTrn->iSubPrc == DST_FULL)
		{
			// Определение таблицы в SQL запросе источника
			if (!TableInSQL(pTrn->sSubSQL, sSrcSTable, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SRCSUB, strErr);
				return FALSE;
			}
			// Установка ключевых полей субтаблицы источника
			if (!SetFldKeys(pSrc, arSrcSubFld, pSrc->sUserName, 
							sSrcSTable, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SRCSUB, strErr);
				return FALSE;
			}
		}
		// Заполнение карты полей субтаблицы приемника
		if (!FillFieldArray(pTrn->sSubMap, arMapSubFld))
		{
			strErr = GetErrMsg(ER_SNTX_MAPSUB, strErr);
			return FALSE;
		}
		// Подготовка субтаблицы приемника
		if (pTrn->iSubPrc == DST_CREATE)
		{
			// Создание субтаблицы приемника
			if (!CreateTable(pDst, pTrn->iSchema, arDstSubFld, 
							 arMapSubFld, arSrcSubFld, 
							 pTrn->sSubDst, strErr))
			{
				strErr = GetErrMsg(ER_CRT_SUB, strErr);
				return FALSE;
			}
		}
		else
		{
			// Подготовка массива полей субтаблицы приемника
			if (!ParseTable(pDst, pTrn->iSchema, arDstSubFld, arMapSubFld, 
							(pTrn->sSubOwn.IsEmpty()) ? pDst->sUserName : pTrn->sSubOwn, 
							pTrn->sSubDst, strErr))
			{
				strErr = GetErrMsg(ER_PREP_DSTSUB, strErr);
				return FALSE;
			}
		}
		// Установка ссылок на источник данных
		// в массиве полей субтаблицы приемника
		if (!SetFldRefs(arDstSubFld, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_DSTSUB, strErr);
			return FALSE;
		}
		// Если необходим поиск в субтаблице приемника
		if (bSubSearch)
		{
			// Конструирование SQL запроса поиска в субтаблице приемника
			// по условию значения первичных ключей субтаблицы приемника
			if (!MakeSelect(arDstSubFld, pTrn->sSubDst, _T(""), 
							TRUE, sDstSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDDSTSUB, strErr);
				return FALSE;
			}
		}
		// При полном сравнении источника и приемника
		if (pTrn->iSubPrc == DST_FULL)
		{
			// Установка ссылок на приемник
			// для ключевых полей под.источника
			if (!SetDstRefs(arSrcSubFld, REF_SUB, TRUE, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SRCSUB, strErr);
				return FALSE;
			}
			// Конструирование SQL запроса поиска в под.источнике
			// по условию значения первичных ключей субтаблицы
			if (!MakeSelect(arSrcSubFld, sSrcSTable, _T(""), 
							TRUE, sSrcSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDSRCSUB, strErr);
				return FALSE;
			}
			// Конструирование SQL Select субтаблицы приемника
			// по заданному условию
			if (!MakeSelect(arDstSubFld, pTrn->sSubDst, pTrn->sSubCond, 
							FALSE, sDstSubSQL, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SELDSTSUB, strErr);
				return FALSE;
			}
		}
		// Заполнение массива команд субтаблицы приемника
		if (!FillCmdArray(pTrn->sSubCmd, arDstSubCmd))
		{
			strErr = GetErrMsg(ER_SNTX_CMDSUB, strErr);
			return FALSE;
		}
		// Подготовка массива команд субтаблицы приемника
		if (!ParseCmdArr(pDst, REF_SUB, arDstSubCmd, FALSE, strErr))
		{
			strErr = GetErrMsg(ER_PREP_CMDSUB, strErr);
			return FALSE;
		}
		// Подготовка признака команд CHANGE в массиве полей приемника
		if (!ParseFldCmd(arDstSubFld, arDstSubCmd, strErr))
		{
			return FALSE;
		}
		// Установка необходимости выполнения команд
		// при обработке каждой строки субтаблицы приемника
		if (FindEvent(arDstSubCmd, EV_ON_INSERT) >= 0 ||
			FindEvent(arDstSubCmd, EV_ON_UPDATE) >= 0 ||
			FindEvent(arDstSubCmd, EV_ON_DELETE) >= 0 ||
			pTrn->iSchema == TRN_COMPLEX)
			bSubRowCmd = TRUE;
		// Установка необходимости выполнения команд OnChange
		// при изменении данных в полях субтаблицы приемника
		if (FindEvent(arDstSubCmd, EV_ON_CHANGE) >= 0)
			bSubFldCmd = TRUE;
	}
#endif
	// Подготовка строки сообщения
	if (bMessage)
	{
		if (!ParseStrMsg(pTrn->sMsg, strErr))
		{
			strErr = GetErrMsg(ER_PREP_MSG, strErr);
			return FALSE;
		}
	}
	// Признак инициализации
	bValid = TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Выполнение трансферта

BOOL CTransfer::Run(CString &strErr, UINT &nMsg, UINT &nErr) 
{
	strErr.Empty();
	if (!bValid)
	{
		strErr = GetErrMsg(ER_NOTVALID);
		return FALSE;
	}
#ifdef _TRIAL_VER
	if (pTrn->iSchema == TRN_COMPLEX)
	{
		// Ограничение по схеме в Trial версии
		strErr.FormatMessage(IDS_ER_TRIAL_COMPLEX);
		return FALSE;
	}
#endif

	// Переменные состояния
	BOOL bRetCode	= TRUE;		// Возвращаемое значение
	BOOL bRetTabRow = TRUE;		// Код завершения строки
	BOOL bSubError	= FALSE;	// Признак ошибки в Detail
	RETCODE nRet;				// Результат SQL оператора
	MOD_FLAG iMFlag;			// Флаг модификации данных
	OP_TYPE iOpType;			// Тип оператора SQL
	CString strCapt;			// Строка заголовка
	CString strStat;			// Строка статуса
	CString strSav;				// Строка сохранения
	CString strMsg;				// Строка сообщения
	CString strRet;				// Строка результата
	CTime	curTime;			// Текущее время
	int		mSize;				// Размер массива таблицы приемника
	int		i,k;				// Счетчики циклов
	int		iPos;				// Номер позиции
	long	n,nCount;			// Счетчики строк

	bBreaked = FALSE;			// Признак прерывания
	bShowErr = TrnParm.iShowErr;// Признак показа ошибки

	if (bCancel) goto end;

	AfxGetApp()->BeginWaitCursor();
	// Создание окна сообщения
	pDlg = new CProgressDlg;
	pDlg->Create(IDD_PROGRESSDLG, AfxGetApp()->m_pMainWnd);
	pDlg->SetWindowText(pTrn->sName);
	pDlg->SetBreakEnable(TRUE);
	pDlg->SetRange(0, 100);
	pDlg->SetPos(0);

	////////////////////////////////////////////////////////////////
	//              ПОДГОТОВКА К ВЫПОЛНЕНИЮ
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_PREPARE);
	pDlg->SetMessageText(strMsg);
	// Установка режима COMMIT для приемника
	if (bTxnCommit)
	{
		::SQLSetConnectAttr(pDst->hdbc, SQL_ATTR_AUTOCOMMIT, 
			(SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_IS_INTEGER);
	}
	// Если полное сравнение
	if (pTrn->iTabPrc == DST_FULL)
	{
		// Подготовка параметров SQL Select основного приемника
		if (!PrepareSQLParms(pDst, &arDstTabPrm, 
			(pTrn->sTabOwn.IsEmpty()) ? pDst->sUserName : pTrn->sTabOwn, sDstTabSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_PARDSTTAB, strErr);
			bRetCode = FALSE; goto end;
		}
		// Подготовка параметров SQL запроса к основному источнику
		if (!PrepareSQLParms(pSrc, &arSrcTabPrm, pSrc->sUserName, sSrcTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDSRCTAB, strErr);
			bRetCode = FALSE; goto end;
		}
	}
	if (bCancel) goto end;
	// Подготовка SQL-операторов таблицы приемника
	// Подготовка оператора SQL Insert таблицы приемника
	if (!MakeInsert(arDstTabFld, arTabInsPrm, pTrn->sTabDst, 
					sDstTabIns, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_INSTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// Подготовка оператора SQL Update таблицы приемника
	if (!MakeUpdate(arDstTabFld, arTabUpdPrm, pTrn->sTabDst, 
					sDstTabUpd, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_UPDTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// Подготовка оператора SQL Delete таблицы приемника
	if (!MakeDelete(arDstTabFld, arTabDelPrm, pTrn->sTabDst, 
					sDstTabDel, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_DELTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// Подготовка массива команд таблицы приемника
	if (!ParseCmdArr(pDst, REF_TAB, arDstTabCmd, TRUE, strErr))
	{
		strErr = GetErrMsg(ER_PREP_CMDTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	if (bCancel) goto end;

#ifndef _TRIAL_VER
	// Подготовка SQL-операторов субтаблицы приемника
	if (pTrn->iSchema == TRN_COMPLEX)
	{
		// Если полное сравнение
		if (pTrn->iSubPrc == DST_FULL)
		{
			// Подготовка параметров SQL Select подчиненного приемника
			if (!PrepareSQLParms(pDst, &arDstSubPrm, 
				(pTrn->sSubOwn.IsEmpty()) ? pDst->sUserName : pTrn->sSubOwn, sDstSubSQL, strErr))
			{
				strErr = GetErrMsg(ER_PREP_PARDSTSUB, strErr);
				bRetCode = FALSE; goto end;
			}
			// Подготовка параметров SQL запроса к подчиненному источнику
			if (!PrepareSQLParms(pSrc, &arSrcSubQry, pSrc->sUserName, sSrcSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDSRCSUB, strErr);
				bRetCode = FALSE; goto end;
			}
		}
		// Подготовка оператора SQL Insert субтаблицы приемника
		if (!MakeInsert(arDstSubFld, arSubInsPrm, pTrn->sSubDst, 
						sDstSubIns, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_INSSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// Подготовка оператора SQL Update субтаблицы приемника
		if (!MakeUpdate(arDstSubFld, arSubUpdPrm, pTrn->sSubDst, 
						sDstSubUpd, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_UPDSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// Подготовка оператора SQL Delete субтаблицы приемника
		if (!MakeDelete(arDstSubFld, arSubDelPrm, pTrn->sSubDst, 
						sDstSubDel, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_DELSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// Подготовка массива команд субтаблицы приемника
		if (!ParseCmdArr(pDst, REF_SUB, arDstSubCmd, TRUE, strErr))
		{
			strErr = GetErrMsg(ER_PREP_CMDSUB, strErr);
			bRetCode = FALSE; goto end;
		}
	}
#endif

	if (bCancel) goto end;
	////////////////////////////////////////////////////////////////
	// Выполнение команды OnStart
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_TABSTART);
	pDlg->SetMessageText(strMsg);
	nRet = ExecSQLCmd(pDst, arDstTabCmd, EV_ON_START, NULL, strErr);
	if (nRet == SQL_ERROR)
	{
		nErr++;
		// Запись текста ошибки в файл
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// Показ сообщения об ошибке
		nRet = ShowMsg(pTrn->sName, strErr);
		if (nRet == IDCANCEL)	goto end;
		else if (nRet == IDNO)	bShowErr = FALSE;
	}
	else if (nRet == SQL_SUCCESS_WITH_INFO)
	{
		WarningHandle(strErr, nMsg);
	}
	if (bTxnCommit)					// Завершение транзакции
	{
		::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
	}
	// Если вид обработки - не полное сравнение,
	// то переход на начало передачи данных
	if (pTrn->iTabPrc != DST_FULL) goto trn;
	////////////////////////////////////////////////////////////////
	//              ОБРАБОТКА ПРИЕМНИКА (для полного сравнения)
	////////////////////////////////////////////////////////////////
	if (bCancel) goto end;
	strMsg.LoadString(IDS_MS_READ_DST);
	pDlg->SetMessageText(strMsg);
	// Подсчет числа строк SQL Select основного приемника
	nCount = ExecSQLCount(pDst, sDstTabSQL, arDstTabPrm, strErr);
	if (nCount == 0) goto trn;
	if (nCount <  0)
	{
		strErr = GetErrMsg(ER_COUNT_DSTTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	AfxGetApp()->BeginWaitCursor();
	////////////////////////////////////////////////////////////////
	// Выполнение SQL Select основного приемника (без Fetch)
	////////////////////////////////////////////////////////////////
	if (bCancel) goto end;
	if (ExecSelect(pDst, hDstTabSQL, arDstTabFld, arDstTabPrm, 
					sDstTabSQL, FALSE, strErr) == SQL_ERROR)
	{
		strErr = GetErrMsg(ER_READ_DSTTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	AfxGetApp()->EndWaitCursor();
	////////////////////////////////////////////////////////////////
	//              ЦИКЛ ПОДГОТОВКИ ДАННЫХ
	////////////////////////////////////////////////////////////////
	if (bCancel) goto end;
	// Цикл по основной таблице приемника
	for (n = 1, iPos = 0;; n++)
	{
		if (bCancel) goto end;
		// Проверка статуса
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE;	goto end;
		}
		// Установка строки статуса
		strStat.FormatMessage(IDS_MS_FMT_ROWS, n, nCount);
		pDlg->SetStatusText(strStat);
		// Установка позиции
		if ((int)(n*100/nCount) > iPos)
		{
			iPos = (int)(n*100/nCount);
			pDlg->SetPos(iPos);
		}
		// Считывание строки данных из приемника
		nRet = FetchData(hDstTabSQL, arDstTabFld, strErr);
		// Проверка результата чтения
		if (nRet == SQL_NO_DATA)	break;		// Нет данных
		else if (nRet == SQL_ERROR)				// Ошибка чтения
		{
			strErr = GetErrMsg(ER_READ_DSTTAB, strErr);
			goto msg;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_READ_DSTTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// Выполнение SQL запроса к таблице источника
		// по значению первичных ключей таблицы
		nRet = ExecSelect(pSrc, hSrcTabSQL, arSrcTabFld, arSrcTabPrm, 
							sSrcTabQry, TRUE, strErr);
		switch (nRet)
		{
		case SQL_ERROR:					// Ошибка чтения
		{
			strErr = GetErrMsg(ER_FIND_SRCTAB, strErr);
			goto msg;
		}
		case SQL_SUCCESS_WITH_INFO:		// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_FIND_SRCTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		case SQL_SUCCESS:				// Данные найдены
		{
			// Сравнение данных в приемнике по ссылке
			if (!CompareRef(arDstTabFld, OP_UPDATE, iMFlag, strErr))
			{
				// Запись текста ошибки в файл
				// и продолжить выполнение
				strErr = GetErrMsg(ER_COMP_DSTTAB, strErr);
				WarningHandle(strErr, nMsg);
			}
			if (iMFlag == MOD_TRG)	// Если сработал триггер
			{
				// Добавление строки в буфер для удаления
				AddBufRow(arTabRowBuf,arSrcTabFld,arDstTabFld,n,
							ROW_UNKNOWN,FALSE,FALSE);
			}
			else						// Триггер не сработал
			{
				continue;
			}
			break;
		}
		case SQL_NO_DATA:				// Не найдено данных
		{
			// Добавление строки в буфер для удаления
			AddBufRow(arTabRowBuf,arSrcTabFld,arDstTabFld,n,
						ROW_UNKNOWN,FALSE,FALSE);
			break;
		}
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (n > 99) break;
#endif
		continue;

msg:	nErr++;
		// Форматирование текста сообщения
		strErr = FormatErr(strErr);
		// Запись текста ошибки в файл
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// Если было прерывание в отладке
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
		// Вызов диалога сообщения
		if (bShowErr)
		{
			nRet = ShowMsg(pTrn->sName, strErr);
			if (nRet == IDCANCEL)	goto end;
			else if (nRet == IDNO)	bShowErr = FALSE;
		}
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (n > 99) break;
#endif
	}
	// Освобождение дескрипторов SQL
	FreeSQLStmt(hSrcTabSQL, TRUE);
	FreeSQLStmt(hDstTabSQL, TRUE);
	// Очистка данных в источнике
	ResetFldArr(arSrcTabFld);
	////////////////////////////////////////////////////////////////
	//              ЦИКЛ ОБРАБОТКИ ДАННЫХ
	////////////////////////////////////////////////////////////////
	if (bCancel) goto end;
	// Обработка данных в приемнике
	strMsg.LoadString(IDS_MS_PROCESS_DST);
	pDlg->SetMessageText(strMsg);
	mSize = arDstTabFld.GetSize();
	// Обработка буфера строк (удаление строк из таблицы)
	for (i = 0; i < arTabRowBuf.GetSize(); i++)
	{
		if (bCancel) goto end;
		// Проверка статуса
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// Установка строки подтекста
		strStat.FormatMessage(IDS_MS_FMT_ROWS, arTabRowBuf[i].nRow, nCount);
		pDlg->SetSubText(strStat);
		// Установка позиции
		if ((int)(arTabRowBuf[i].nRow*100/nCount) > iPos)
		{
			iPos = (int)(arTabRowBuf[i].nRow*100/nCount);
			pDlg->SetPos(iPos);
		}
		// Установка строки сообщения
		if (bMessage)
		{
			strMsg = FormatMsg(pTrn->sMsg);
			pDlg->SetMessageText(strMsg);
		}
		// Перенос данных в приемник
		for (k = 0; k < mSize; k++)
		{
			arDstTabFld[k].bMod  = FALSE;
			arDstTabFld[k].vData = arTabRowBuf[i].pArDstFld->GetAt(k).vData;
		}
		// Обработка строки
		if (bComplex)				// Удаление данных из субтаблицы
		{
			if (!RunDetail(OP_DELETE,TRUE,fileLog,nMsg,nErr,strErr))
				goto err;
		}
		if (bTabRowCmd)				// Выполнение SQL-команды
		{
			// Выполнение команды OnDelete
			nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_DELETE,NULL,strErr);
	
			if (nRet == SQL_ERROR)	// Ошибка SQL-команды
			{
				if (bComplex)		// Восстановление субтаблицы
				{
					BackDetail(TRUE, fileLog, nMsg, nErr);
				}
				goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
		}
		// Выполнение операции удаления (по приемнику)
		nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
								sDstTabDel, FALSE, strErr);
		// Проверка результата операции
		if (nRet == SQL_ERROR)			// Ошибка удаления
		{
			strErr  = GetErrMsg(ER_DELTAB, strErr);
			strErr += FormatUpd(ER_DELTAB);
			if (!bTxnCommit)			// Выполнение обратной команды
			{
				nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_DELETE,NULL,strRet);
				if (nRet == SQL_ERROR)	// Ошибка обратной команды
				{
					ErrorHandle(ER_BACK_CMD, strRet, nErr);
				}
			}
			if (bComplex)				// Восстановление субтаблицы
			{
				BackDetail(TRUE, fileLog, nMsg, nErr);
			}
			goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_DELTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		if (bTxnCommit)					// Завершение транзакции
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
		// Установка статуса строки в буфере
		arTabRowBuf[i].nStatus = ROW_DELETED;
		// Запись сообщения в файл сообщений
		if (bMessage && bFileMsg)
		{
			strRet.FormatMessage(IDS_MS_DELETED, strMsg);
			fileMsg.WriteString(strRet + _T("\n"));
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (i > 99) break;
#endif
		continue;

err:	nErr++;
		// Форматирование текста сообщения
		strErr = FormatErr(strErr);
		// Запись текста ошибки в файл
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// Если было прерывание в отладке
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
		// Вызов диалога сообщения
		if (bShowErr)
		{
			nRet = ShowMsg(pTrn->sName, strErr);
			if (nRet == IDCANCEL)	goto end;
			else if (nRet == IDNO)	bShowErr = FALSE;
		}
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (i > 99) break;
#endif
	}
	// Очистка буфера строк
	ClearBufArr(arTabRowBuf);

	////////////////////////////////////////////////////////////////
	//              ПЕРЕДАЧА ДАННЫХ
	////////////////////////////////////////////////////////////////
trn:if (bCancel) goto end;
	strMsg.LoadString(IDS_MS_READ_SRC);
	pDlg->SetMessageText(strMsg);
	pDlg->SetStatusText(_T(""));
	pDlg->SetPos(0);
	AfxGetApp()->BeginWaitCursor();
	// Подготовка параметров SQL Select основного источника
	if (bCancel) goto end;
	if (!PrepareSQLParms(pSrc, &arSrcTabPrm, pSrc->sUserName, pTrn->sTabSQL, strErr))
	{
		strErr = GetErrMsg(ER_PREP_PARSRCTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// Если требуется поиск данных в таблице приемника
	if (bTabSearch)
	{
		// Подготовка параметров SQL запроса к основному приемнику
		if (!PrepareSQLParms(pDst, &arDstTabPrm, 
			(pTrn->sTabOwn.IsEmpty()) ? pDst->sUserName : pTrn->sTabOwn, sDstTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDDSTTAB, strErr);
			bRetCode = FALSE; goto end;
		}
	}
	// Для схемы COMPLEX
	if (bCancel) goto end;

#ifndef _TRIAL_VER
	if (pTrn->iSchema == TRN_COMPLEX)
	{
		// Подготовка параметров SQL Select подчиненного источника
		if (!PrepareSQLParms(pSrc, &arSrcSubPrm, pSrc->sUserName, pTrn->sSubSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_PARSRCSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// Если требуется поиск данных в субтаблице приемника
		if (bSubSearch)
		{
			// Подготовка параметров SQL подзапроса поиска
			// в подчиненном приемнике
			if (!PrepareSQLParms(pDst, &arDstSubQry, 
				(pTrn->sSubOwn.IsEmpty()) ? pDst->sUserName : pTrn->sSubOwn, sDstSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDDSTSUB, strErr);
				bRetCode = FALSE; goto end;
			}
		}
	}
#endif
	// Подсчет числа строк SQL Select основного источника
	if (bCancel) goto end;
	nCount = ExecSQLCount(pSrc, pTrn->sTabSQL, arSrcTabPrm, strErr);
	if (nCount == 0) goto end;
	if (nCount <  0)
	{
		strErr = GetErrMsg(ER_COUNT_SRCTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	AfxGetApp()->BeginWaitCursor();
	////////////////////////////////////////////////////////////////
	// Выполнение SQL Select основного источника (без Fetch)
	if (bCancel) goto end;
	////////////////////////////////////////////////////////////////
	if (ExecSelect(pSrc, hSrcTabSQL, arSrcTabFld, arSrcTabPrm, 
					pTrn->sTabSQL, FALSE, strErr) == SQL_ERROR)
	{
		strErr = GetErrMsg(ER_READ_SRCTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	AfxGetApp()->EndWaitCursor();
	////////////////////////////////////////////////////////////////
	//              ЦИКЛ ПЕРЕДАЧИ ДАННЫХ
	////////////////////////////////////////////////////////////////
	// Перенос данных из источника в приемник
	if (bCancel) goto end;
	strMsg.LoadString(IDS_MS_SEND);
	pDlg->SetMessageText(strMsg);
	// Цикл по основной таблице источника
	for (n = 1, iPos = 0;; n++)
	{
		if (bCancel) goto end;
		// Проверка статуса
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE;	goto end;
		}
		// Установка строки статуса
		strStat.FormatMessage(IDS_MS_FMT_ROWS, n, nCount);
		pDlg->SetStatusText(strStat);
		// Установка позиции
		if ((int)(n*100/nCount) > iPos)
		{
			iPos = (int)(n*100/nCount);
			pDlg->SetPos(iPos);
		}
		// Считывание строки данных из источника
		nRet = FetchData(hSrcTabSQL, arSrcTabFld, strErr);
		// Проверка результата чтения
		if (nRet == SQL_NO_DATA)	break;		// Нет данных
		else if (nRet == SQL_ERROR)				// Ошибка чтения
		{
			strErr = GetErrMsg(ER_READ_SRCTAB, strErr);
			goto brk;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_READ_SRCTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// Установка строки сообщения
		if (bMessage)
		{
			strMsg = FormatMsg(pTrn->sMsg);
			pDlg->SetMessageText(strMsg);
		}
		// Обработка данных
		if (bTabSearch)					// Если нужен поиск данных..
		{
			// Выполнение SQL Select из таблицы приемника
			// по значению первичных ключей таблицы
			nRet = ExecSelect(pDst, hDstTabSQL, arDstTabFld, arDstTabPrm,
								sDstTabQry, TRUE, strErr);
			switch (nRet)
			{
			case SQL_ERROR:				// Ошибка чтения
			{
				strErr = GetErrMsg(ER_FIND_DSTTAB, strErr);
				goto brk;
			}
			case SQL_NO_DATA:			// Не найдено данных
			{
				iOpType = OP_INSERT;	// Добавление строки
				nRet = ExecUpdate(pDst, hDstTabIns, iOpType, arTabInsPrm, 
										sDstTabIns, TRUE, strErr);
				break;
			}
			case SQL_SUCCESS_WITH_INFO:	// Предупреждение
			{
				// Запись текста предупреждения в файл
				// и продолжить выполнение
				strErr = GetErrMsg(ER_FIND_DSTTAB, strErr);
				WarningHandle(strErr, nMsg);
			}
			case SQL_SUCCESS:			// Данные найдены
			{
				// Сравнение данных в приемнике по ссылке
				if (!CompareRef(arDstTabFld, OP_UPDATE, iMFlag, strErr))
				{
					// Запись текста ошибки в файл
					// и продолжить выполнение
					strErr = GetErrMsg(ER_COMP_DSTTAB, strErr);
					WarningHandle(strErr, nMsg);
				}
				switch (iMFlag)			// Проверка флага модификации
				{
				case MOD_NO:			// Данные не изменились
				{
					iOpType = OP_UPDATE;// Тип операции - Update
					goto cont;			// Переход к обработке субтаблицы
					break;
				}
				case MOD_YES:			// Данные изменились
				case MOD_CMD:			// Команда-событие
				case MOD_EXEC:			// Флаг выполнения
				{
					iOpType = OP_UPDATE;	// Обновление строки
					nRet = ExecUpdate(pDst, hDstTabUpd, iOpType, arTabUpdPrm, 
										sDstTabUpd, TRUE, strErr);
					break;
				}
				case MOD_BACK:			// Флаг отката
				{
					if (bComplex)		// Откат данных в субтаблице
					{
						if (!RunCmdDetail(OP_DELETE,fileLog,nMsg,nErr,strErr))
							goto brk;
					}
					if (bTabRowCmd)		// Выполнение SQL-команды
					{
						// Выполнение команды OnDelete
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_DELETE,NULL,strErr);
	
						if (nRet == SQL_ERROR)	// Ошибка SQL-команды
						{
							if (bComplex)	// Восстановление субтаблицы
							{
								BackDetail(TRUE, fileLog, nMsg, nErr);
							}
							goto brk;
						}
						else if (nRet == SQL_SUCCESS_WITH_INFO)
						{
							WarningHandle(strErr, nMsg);
						}
						if (bTxnCommit)		// Завершение транзакции
						{
							::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
						}
					}
					// Очистка буфера строк
					ClearBufArr(arSubRowBuf);

					// Запись сообщения в файл сообщений
					if (bMessage && bFileMsg)
					{
						strRet.FormatMessage(IDS_MS_REVERSED, strMsg);
						fileMsg.WriteString(strRet + _T("\n"));
					}
					iOpType = OP_UPDATE;	// Обновление строки
					nRet = ExecUpdate(pDst, hDstTabUpd, iOpType, arTabUpdPrm, 
										sDstTabUpd, TRUE, strErr);
					break;
				}
				case MOD_TRG:			// Триггер строки
				{
					if (bComplex)		// Удаление данных из субтаблицы
					{
						if (!RunDetail(OP_DELETE,TRUE,fileLog,nMsg,nErr,strErr))
							goto brk;
					}
					if (bTabRowCmd)		// Выполнение SQL-команды
					{
						// Выполнение команды OnDelete
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_DELETE,NULL,strErr);
	
						if (nRet == SQL_ERROR)	// Ошибка SQL-команды
						{
							if (bComplex)		// Восстановление субтаблицы
							{
								BackDetail(TRUE, fileLog, nMsg, nErr);
							}
							goto brk;
						}
						else if (nRet == SQL_SUCCESS_WITH_INFO)
						{
							WarningHandle(strErr, nMsg);
						}
					}
					// Выполнение операции удаления (по приемнику)
					nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
											sDstTabDel, FALSE, strErr);
					// Проверка результата операции
					if (nRet == SQL_ERROR)		// Ошибка удаления
					{
						strErr  = GetErrMsg(ER_DELTAB, strErr);
						strErr += FormatUpd(ER_DELTAB);
						if (!bTxnCommit)		// Выполнение обратной команды
						{
							nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_DELETE,NULL,strRet);
							if (nRet == SQL_ERROR)	// Ошибка обратной команды
							{
								ErrorHandle(ER_BACK_CMD, strRet, nErr);
							}
						}
						if (bComplex)			// Восстановление субтаблицы
						{
							BackDetail(TRUE, fileLog, nMsg, nErr);
						}
						goto brk;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						// Запись текста предупреждения в файл
						// и продолжить выполнение
						strErr = GetErrMsg(ER_DELTAB, strErr);
						WarningHandle(strErr, nMsg);
					}
					if (bTxnCommit)				// Завершение транзакции
					{
						::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
					}
					iOpType = OP_INSERT;		// Добавление строки
					nRet = ExecUpdate(pDst, hDstTabIns, iOpType, arTabInsPrm, 
											sDstTabIns, TRUE, strErr);
					break;
				}
				default:
				{
					iOpType = OP_UPDATE;	// Обновление строки
					nRet = ExecUpdate(pDst, hDstTabUpd, iOpType, arTabUpdPrm, 
										sDstTabUpd, TRUE, strErr);
					break;
				}
				}
				break;
			}
			}
		}
		else							// Если безусловное добавление..
		{
			iOpType = OP_INSERT;		// Добавление строки
			nRet = ExecUpdate(pDst, hDstTabIns, iOpType, arTabInsPrm, 
									sDstTabIns, TRUE, strErr);
		}
		// Проверка результата операции
		if (nRet == SQL_ERROR)			// Ошибка записи
		{
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSTAB, strErr);
							strErr+= FormatUpd(ER_INSTAB); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDTAB, strErr);
							strErr+= FormatUpd(ER_UPDTAB); break;
			}
			goto brk;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSTAB, strErr); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDTAB, strErr); break;
			}
			WarningHandle(strErr, nMsg);
		}
		if (bTabRowCmd)					// Выполнение SQL-команды
		{
			switch (iOpType)			// Тип операции
			{
			case OP_INSERT:				// Команда OnInsert
			{
				nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_INSERT,NULL,strErr);
				break;
			}
			case OP_UPDATE:				// Команда OnUpdate
			{
				switch (iMFlag)			// Флаг модификации
				{
				case MOD_EXEC:			// Не выполнять при 
				case MOD_BACK:			// выполнении/откате
				{
					nRet = SQL_SUCCESS;
					break;
				}
				default:				// Команда OnUpdate
				{
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_UPDATE,NULL,strErr);
					break;
				}
				}
				break;
			}
			}
			if (nRet == SQL_ERROR)		// Ошибка SQL-команды
			{
				// Откат операции
				if (!bTxnCommit && (iOpType == OP_INSERT) && bComplex)
				{
					// Сохранение текста ошибки
					strSav = strErr;
					// Выполнение команды CallBack
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_CALL_BACK,NULL,strRet);
					if (nRet == SQL_ERROR)
					{
						ErrorHandle(ER_BACK_OP, strRet, nErr);
					}
					else
					{
						nRet = SQL_ERROR; goto cont;
					}
				}
				goto errCmd;			// Переход на обработку ошибки
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
		}
		if (bTabFldCmd)					// Обработка триггера OnChange
		{
			if ((iOpType == OP_UPDATE) && (iMFlag == MOD_CMD))
			{
				// Выполнение команды OnChange для измененных полей
				nRet = ExecFldCmd(pDst, arDstTabFld, arDstTabCmd,
								  EV_ON_CHANGE, strErr);
				if (nRet == SQL_ERROR)
				{
					goto errEvt;
				}
				else if (nRet == SQL_SUCCESS_WITH_INFO)
				{
					WarningHandle(strErr, nMsg);
				}
			}
		}
		if (bTxnCommit)					// Завершение транзакции
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}


		/////////////////////////////////////////////////////////////
		// Обработка субтаблицы
		/////////////////////////////////////////////////////////////
cont:	if (bComplex)					// Обработка субтаблицы
		{
			if (iOpType == OP_INSERT)
			{
				// Заполнение данных в таблице приемника
				// для получения данных по ссылке #
				// в командах субтаблицы приемника
				SetFldData(arDstTabFld, iOpType, strErr);
				// Установка флага выполнения команд
				if (nRet == SQL_ERROR)	bRetTabRow = FALSE;
				else					bRetTabRow = TRUE;
			}
			else
			{
				// Установка флага выполнения команд
				switch (iMFlag)	// Флаг модификации
				{
				case MOD_EXEC:	bRetTabRow = FALSE;	break;
				case MOD_BACK:	bRetTabRow = FALSE;	break;
				default:		bRetTabRow = TRUE;	break;
				}
			}
			// Выполнение трансферта субтаблицы
			if (!RunDetail(iOpType,bRetTabRow,fileLog,nMsg,nErr,strErr))
			{
				if (bTabRowCmd)			// Выполнение обратной команды
				{
					if ((iOpType == OP_INSERT) && bRetTabRow)
					{
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_INSERT,
											NULL,strRet);
					}
					else if ((iOpType == OP_UPDATE) && iMFlag)
					{
						switch (iMFlag)			// Флаг модификации
						{
						case MOD_EXEC:			// Не выполнять при 
						case MOD_BACK:			// выполнении/откате
						{
							nRet = SQL_SUCCESS;
							break;
						}
						default:				// Команда ReUpdate
						{
							nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_UPDATE,
												NULL,strRet);
							break;
						}
						}
					}
					else
					{
						nRet = SQL_SUCCESS;
					}
					if (nRet == SQL_ERROR)	// Ошибка обратной команды
					{
						ErrorHandle(ER_BACK_CMD, strRet, nErr);
						// Не завершать отката
						goto brk;
					}
				}
				if (iOpType == OP_INSERT) // Выполнение обратной операции
				{
					// Проверка буфера строк субтаблицы:
					// если в буфере остались строки, 
					// значит была ошибка SQL-команды субтаблицы, 
					// то есть откатаны только команды субтаблицы
					if (arSubRowBuf.GetSize() > 0)	// Команда CallBack
					{
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_CALL_BACK,
											NULL,strRet);
					}
					else				// Удаление строки (по источнику)
					{
						nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
												sDstTabDel, TRUE, strRet);
					}
				}
				else if ((iOpType == OP_UPDATE) && iMFlag)
				{
					if (bTabFldCmd && (iMFlag == MOD_CMD)) // Откат OnChange
					{
						// Выполнение ReChange для измененных полей
						nRet = ExecFldCmd(pDst, arDstTabFld, arDstTabCmd,
										  EV_RE_CHANGE, strRet);
						if (nRet == SQL_ERROR)
						{
							ErrorHandle(ER_BACK_CMD, strRet, nErr);
							// Не завершать отката
							goto brk;
						}
						else if (nRet == SQL_SUCCESS_WITH_INFO)
						{
							WarningHandle(strRet, nMsg);
						}
					}
					// Восстановление строки (по приемнику)
					nRet = ExecUpdate(pDst, hDstTabUpd, OP_UPDATE, arTabUpdPrm, 
											sDstTabUpd, FALSE, strRet);
				}
				else
				{
					nRet = SQL_SUCCESS;
				}
				if (nRet == SQL_ERROR)	// Ошибка обратной операции
				{
					ErrorHandle(ER_BACK_OP, strRet, nErr);
					// Не завершать отката
					goto brk;
				}
				else if (bTxnCommit)	// Завершение транзакции
				{
					::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
				}
				goto brk;
			}
			if ((iOpType == OP_INSERT) && !bRetTabRow)
			{
				strErr = strSav;		// Восстановление текста ошибки
				goto brk;				// Переход на обработку ошибки
			}
		}
		// Запись сообщения в файл сообщений
		if (bMessage && bFileMsg)
		{
			if (iOpType == OP_INSERT)
			{
				strRet.FormatMessage(IDS_MS_INSERTED, strMsg);
				fileMsg.WriteString(strRet + _T("\n"));
			}
			else if ((iOpType == OP_UPDATE) && iMFlag)
			{
				strRet.FormatMessage(IDS_MS_UPDATED, strMsg);
				fileMsg.WriteString(strRet + _T("\n"));
			}
		}
		/////////////////////////////////////////////////////////////
		// Обработка флага выполнения
		/////////////////////////////////////////////////////////////
		if ((iOpType == OP_UPDATE) && (iMFlag == MOD_EXEC))
		{
			// Заполнение данных в таблице приемника
			// для получения данных по ссылке #
			// в командах субтаблицы приемника
			SetFldData(arDstTabFld, iOpType, strErr);

			if (bTabRowCmd)		// Выполнение SQL-команды
			{
				// Выполнение команды OnInsert
				nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_INSERT,NULL,strErr);

				if (nRet == SQL_ERROR)	// Ошибка SQL-команды
				{
					// Выполнение команды CallBack
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_CALL_BACK, NULL,strRet);

					if (nRet == SQL_ERROR)	// Ошибка обратной операции
					{
						ErrorHandle(ER_BACK_OP, strRet, nErr);
						// Не завершать отката
						goto brk;
					}
					else if (bTxnCommit)	// Завершение транзакции
					{
						::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
					}
					goto brk;
				}
				else if (nRet == SQL_SUCCESS_WITH_INFO)
				{
					nMsg++;
					if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
				}
				if (bTxnCommit)			// Завершение транзакции
				{
					::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
				}
			}
			if (bComplex)		// Проведение данных в субтаблице
			{
				if (!RunCmdDetail(OP_INSERT,fileLog,nMsg,nErr,strErr))
				{
					// Выполнение команды ReInsert
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_INSERT,NULL,strRet);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						ErrorHandle(ER_BACK_OP, strRet, nErr);
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strRet, nMsg);
					}
					// Выполнение команды CallBack
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_CALL_BACK, NULL,strRet);

					if (nRet == SQL_ERROR)	// Ошибка обратной операции
					{
						ErrorHandle(ER_BACK_OP, strRet, nErr);
						// Не завершать отката
						goto brk;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strRet, nMsg);
					}
					if (bTxnCommit)	// Завершение транзакции
					{
						::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
					}
					goto brk;
				}
				// Очистка буфера строк
				ClearBufArr(arSubRowBuf);
			}
			// Запись сообщения в файл сообщений
			if (bMessage && bFileMsg)
			{
				strRet.FormatMessage(IDS_MS_EXECUTED, strMsg);
				fileMsg.WriteString(strRet + _T("\n"));
			}
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (n > 99) break;
#endif
		continue;

		/////////////////////////////////////////////////////////////
		// Обработка ошибок
		/////////////////////////////////////////////////////////////
		// Ошибка триггера OnChange
errEvt:	if (bTabRowCmd && !bTxnCommit)	// Выполнение обратной команды
		{
			nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_UPDATE,NULL,strRet);
			if (nRet == SQL_ERROR)		// Ошибка обратной команды
			{
				ErrorHandle(ER_BACK_CMD, strRet, nErr);
			}
		}
		// Ошибка SQL-команды
errCmd:	if (!bTxnCommit)				// Выполнение обратной операции
		{
			switch (iOpType)			// Тип исходной операции
			{
			case OP_INSERT:				// Удаление строки (по источнику)
				nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
										sDstTabDel, TRUE, strRet);
				break;
			case OP_UPDATE:				// Восстановление (по приемнику)
				nRet = ExecUpdate(pDst, hDstTabUpd, OP_UPDATE, arTabUpdPrm, 
										sDstTabUpd, FALSE, strRet);
				break;
			}
			if (nRet == SQL_ERROR)		// Ошибка обратной операции
			{
				ErrorHandle(ER_BACK_OP, strRet, nErr);
			}
		}

brk:	nErr++;
		// Добавление к тексту ошибки текста сообщения
		strErr = FormatErr(strErr);
		// Запись текста ошибки в файл
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// Если было прерывание в отладке
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
		// Вызов диалога сообщения
		if (bShowErr)
		{
			nRet = ShowMsg(pTrn->sName, strErr);
			if (nRet == IDCANCEL)	goto end;
			else if (nRet == IDNO)	bShowErr = FALSE;
		}
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (n > 99) break;
#endif
		continue;
	}
	////////////////////////////////////////////////////////////////
	// Выполнение команды OnEnd
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_TABEND);
	pDlg->SetMessageText(strMsg);
	nRet = ExecSQLCmd(pDst, arDstTabCmd, EV_ON_END, NULL, strErr);
	if (nRet == SQL_ERROR)
	{
		nErr++;
		// Запись текста ошибки в файл
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// Показ сообщения об ошибке
		if (bShowErr) ShowErr(pTrn->sName, strErr);
	}
	else if (nRet == SQL_SUCCESS_WITH_INFO)
	{
		WarningHandle(strErr, nMsg);
	}
	if (bTxnCommit)					// Завершение транзакции
	{
		::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
	}

end:AfxGetApp()->EndWaitCursor();
	// Освобождение дескрипторов SQL
	FreeAllStmt();
	// Очистка всех массивов
	ClearAllArr();
	// Удаление окна сообщения
	if (pDlg != NULL)
	{
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
	return bRetCode;
}

/////////////////////////////////////////////////////////////////////
// Выполнение трансферта в субтаблице

BOOL CTransfer::RunDetail(OP_TYPE iMainType, BOOL bRunCmd, 
						  CStdioFile &fileLog, UINT &nMsg, UINT &nErr, 
						  CString &strErr) 
{
	// Параметры:
	// iMainType - тип операции основной таблицы
	// bRunCmd   - признак выполнения SQL-команды
	// &fileLog  - ссылка на файл протокола
	// &nErr     - ссылка на счетчик ошибок
	// &nMsg     - ссылка на счетчик предупреждений
	// &strErr   - ссылка на строку ошибки
	// Переменные состояния
	BOOL bRetCode = TRUE;		// Возвращаемое значение
	RETCODE nRet;				// Результат SQL оператора
	MOD_FLAG iMFlag;			// Флаг модификации данных
	OP_TYPE iOpType;			// Тип оператора SQL
	CString strSav;				// Строка сохранения
	CString strMsg;				// Строка сообщения
	CString strRet;				// Строка результата
	int		mSize;				// Размер массива субтаблицы приемника
	int		i,j;				// Счетчики циклов
	long	n;					// Счетчик строк

	// Очистка буфера строк
	ClearBufArr(arSubRowBuf);

	////////////////////////////////////////////////////////////////
	// Выполнение команды OnStart
	////////////////////////////////////////////////////////////////
	if (bRunCmd && iMainType == OP_INSERT)
	{
		strMsg.LoadString(IDS_MS_SUBSTART);
		pDlg->SetSubText(strMsg);
		nRet = ExecSQLCmd(pDst, arDstSubCmd, EV_ON_START, NULL, strErr);
		if (nRet == SQL_ERROR)
		{
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)
		{
			WarningHandle(strErr, nMsg);
		}
		if (bTxnCommit)					// Завершение транзакции
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
	}
	// Если не удаление данных и не полное сравнение, 
	// то переход на начало передачи данных
	if (iMainType != OP_DELETE && pTrn->iSubPrc != DST_FULL)
		goto trn;

	////////////////////////////////////////////////////////////////
	//              ОБРАБОТКА ПРИЕМНИКА
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_READ_SUBDST);
	pDlg->SetSubText(strMsg);
	////////////////////////////////////////////////////////////////
	// Выполнение SQL Select подчиненного приемника (без Fetch)
	////////////////////////////////////////////////////////////////
	if (ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubPrm, 
					sDstSubSQL, FALSE, strErr) == SQL_ERROR)
	{
		strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
		bRetCode = FALSE; goto err;
	}
	////////////////////////////////////////////////////////////////
	//              ЦИКЛ ПОДГОТОВКИ ДАННЫХ
	////////////////////////////////////////////////////////////////
	// Цикл по субтаблице приемника
	for (n = 1;; n++)
	{
		if (bCancel) goto end;
		// Проверка статуса
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			// Очистка буфера строк
			ClearBufArr(arSubRowBuf);
			// Выход без восстановления
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// Установка строки подтекста
		strMsg.FormatMessage(IDS_MS_FMT_SUBPRC, n);
		pDlg->SetSubText(strMsg);
		// Считывание строки данных из субтаблицы приемника
		nRet = FetchData(hDstSubSQL, arDstSubFld, strErr);
		// Проверка результата чтения
		if (nRet == SQL_NO_DATA)	break;		// Нет данных
		else if (nRet == SQL_ERROR)				// Ошибка чтения
		{
			// Очистка буфера строк
			ClearBufArr(arSubRowBuf);
			// Выход без восстановления
			strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// Если операция удаления, не искать в источнике
		if (iMainType == OP_DELETE) nRet = SQL_NO_DATA;
		else
		{
			// Выполнение SQL запроса к субтаблице источника
			// по значению первичных ключей субтаблицы
			nRet = ExecSelect(pSrc, hSrcSubSQL, arSrcSubFld, arSrcSubQry, 
								sSrcSubQry, TRUE, strErr);
		}
		switch (nRet)
		{
		case SQL_ERROR:					// Ошибка чтения
		{
			// Очистка буфера строк
			ClearBufArr(arSubRowBuf);
			// Выход без восстановления
			strErr = GetErrMsg(ER_FIND_SRCSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		case SQL_SUCCESS_WITH_INFO:		// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_FIND_SRCSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		case SQL_SUCCESS:				// Данные найдены
		{
			// Сравнение данных в приемнике по ссылке
			if (!CompareRef(arDstSubFld, OP_UPDATE, iMFlag, strErr))
			{
				// Запись текста ошибки в файл
				// и продолжить выполнение
				strErr = GetErrMsg(ER_COMP_DSTSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			if (iMFlag == MOD_TRG)	// Если сработал триггер
			{
				// Добавление строки в буфер для удаления
				AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
							ROW_UNKNOWN,FALSE,FALSE);
			}
			else						// Триггер не сработал
			{
				continue;
			}
			break;
		}
		case SQL_NO_DATA:				// Не найдено данных
		{
			// Добавление строки в буфер для удаления
			AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
						ROW_UNKNOWN,FALSE,FALSE);
			break;
		}
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (n > 99) break;
#endif
	}
	// Освобождение дескрипторов SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// Очистка данных в источнике
	ResetFldArr(arSrcSubFld);
	////////////////////////////////////////////////////////////////
	//              ЦИКЛ ОБРАБОТКИ ДАННЫХ
	////////////////////////////////////////////////////////////////
	mSize = arDstSubFld.GetSize();
	// Обработка буфера строк (удаление строк из субтаблицы)
	for (i = 0; i < arSubRowBuf.GetSize(); i++)
	{
		if (bCancel) goto end;
		// Проверка статуса
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// Установка строки подтекста
		strMsg.FormatMessage(IDS_MS_FMT_SUBDEL, arSubRowBuf[i].nRow);
		pDlg->SetSubText(strMsg);
		// Перенос данных в приемник
		for (j = 0; j < mSize; j++)
		{
			arDstSubFld[j].bMod  = FALSE;
			arDstSubFld[j].vData = arSubRowBuf[i].pArDstFld->GetAt(j).vData;
		}
		// Обработка строки
		if (bRunCmd && bSubRowCmd)	// Выполнение SQL-команды
		{
			nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_DELETE,NULL,strErr);
			if (nRet == SQL_ERROR)	// Ошибка SQL-команды
			{
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
			arSubRowBuf[i].bRunCmd = TRUE;
		}
		// Выполнение операции удаления (по приемнику)
		nRet = ExecUpdate(pDst, hDstSubDel, OP_DELETE, arSubDelPrm, 
								sDstSubDel, FALSE, strErr);
		// Проверка результата операции
		if (nRet == SQL_ERROR)			// Ошибка удаления
		{
			strErr  = GetErrMsg(ER_DELSUB, strErr);
			strErr += FormatUpd(ER_DELSUB);
			if (bRunCmd && !bTxnCommit)	// Выполнение обратной команды
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,NULL,strRet);
				if (nRet == SQL_ERROR)	// Ошибка обратной команды
				{
					ErrorHandle(ER_BACK_CMD, strRet, nErr);
				}
				else
				{
					arSubRowBuf[i].bRunCmd = FALSE;
				}
			}
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_DELSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		if (bTxnCommit)					// Завершение транзакции
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
		// Установка статуса строки в буфере
		arSubRowBuf[i].nStatus = ROW_DELETED;

#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (i > 99) break;
#endif
	}
	// Если операция удаления, закончить обработку
	if (iMainType == OP_DELETE) goto end;


	////////////////////////////////////////////////////////////////
	//              ПЕРЕДАЧА ДАННЫХ
	////////////////////////////////////////////////////////////////
trn:strMsg.LoadString(IDS_MS_READ_SUBSRC);
	pDlg->SetSubText(strMsg);
	////////////////////////////////////////////////////////////////
	// Выполнение SQL Select подчиненного источника (без Fetch)
	////////////////////////////////////////////////////////////////
	if (ExecSelect(pSrc, hSrcSubSQL, arSrcSubFld, arSrcSubPrm, 
					pTrn->sSubSQL, FALSE, strErr) == SQL_ERROR)
	{
		strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
		bRetCode = FALSE; goto err;
	}
	////////////////////////////////////////////////////////////////
	//              ЦИКЛ ПЕРЕДАЧИ ДАННЫХ
	////////////////////////////////////////////////////////////////
	for (n = 1;; n++)
	{
		if (bCancel) goto end;
		// Проверка статуса
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// Установка строки подтекста
		strMsg.FormatMessage(IDS_MS_FMT_SUBUPD, n);
		pDlg->SetSubText(strMsg);
		// Считывание строки данных из источника
		nRet = FetchData(hSrcSubSQL, arSrcSubFld, strErr);
		// Проверка результата чтения
		if (nRet == SQL_NO_DATA)	break;		// Нет данных
		else if (nRet == SQL_ERROR)				// Ошибка чтения
		{
			strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// Обработка данных
		if (bSubSearch && (iMainType == OP_UPDATE))	// Если нужен поиск
		{
			// Выполнение SQL запроса к субтаблице приемника
			// по значению первичных ключей субтаблицы
			nRet = ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubQry, 
								sDstSubQry, TRUE, strErr);
			switch (nRet)
			{
			case SQL_ERROR:				// Ошибка чтения
			{
				strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
				bRetCode = FALSE; goto err;
			}
			case SQL_NO_DATA:			// Не найдено данных
			{
				iOpType = OP_INSERT;
				// Заполнение данных в субтаблице приемника
				SetFldData(arDstSubFld, iOpType, strErr);
				// Добавление строки
				nRet = ExecUpdate(pDst, hDstSubIns, iOpType, arSubInsPrm, 
										sDstSubIns, TRUE, strErr);
				break;
			}
			case SQL_SUCCESS_WITH_INFO:	// Предупреждение
			{
				// Запись текста предупреждения в файл
				// и продолжить выполнение
				strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			case SQL_SUCCESS:			// Данные найдены
			{
				// Сравнение данных в приемнике по ссылке
				if (!CompareRef(arDstSubFld, OP_UPDATE, iMFlag, strErr))
				{
					// Запись текста ошибки в файл
					// и продолжить выполнение
					strErr = GetErrMsg(ER_COMP_DSTSUB, strErr);
					WarningHandle(strErr, nMsg);
				}
				switch (iMFlag)			// Проверка флага модификации
				{
				case MOD_NO:			// Данные не изменились
				{
					iOpType = OP_UNDEF;
					continue;
				}
				case MOD_YES:			// Данные изменились
				case MOD_CMD:			// Команда-событие
				case MOD_EXEC:			// Флаг выполнения
				case MOD_BACK:			// Флаг отката
				{
					iOpType = OP_UPDATE;	// Модификация строки
					nRet = ExecUpdate(pDst, hDstSubUpd, iOpType, arSubUpdPrm, 
											sDstSubUpd, TRUE, strErr);
					break;
				}
				case MOD_TRG:			// Если сработал триггер
				{
					if (bRunCmd && bSubRowCmd)	// Выполнение SQL-команды
					{
						nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_DELETE,NULL,strErr);
						if (nRet == SQL_ERROR)	// Ошибка SQL-команды
						{
							bRetCode = FALSE; goto err;
						}
						else if (nRet == SQL_SUCCESS_WITH_INFO)
						{
							WarningHandle(strErr, nMsg);
						}
					}
					// Выполнение операции удаления (по приемнику)
					nRet = ExecUpdate(pDst, hDstSubDel, OP_DELETE, arSubDelPrm, 
											sDstSubDel, FALSE, strErr);
					// Проверка результата операции
					if (nRet == SQL_ERROR)			// Ошибка удаления
					{
						strErr  = GetErrMsg(ER_DELSUB, strErr);
						strErr += FormatUpd(ER_DELSUB);
						if (bRunCmd && !bTxnCommit)	// Выполнение обратной команды
						{
							nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,NULL,strRet);
							if (nRet == SQL_ERROR)	// Ошибка обратной команды
							{
								ErrorHandle(ER_BACK_CMD, strRet, nErr);
							}
						}
						bRetCode = FALSE; goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
					{
						// Запись текста предупреждения в файл
						// и продолжить выполнение
						strErr = GetErrMsg(ER_DELSUB, strErr);
						WarningHandle(strErr, nMsg);
					}
					if (bTxnCommit)					// Завершение транзакции
					{
						::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
					}
					// Добавление строки в буфер
					AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
								ROW_DELETED,bRunCmd,FALSE);

					iOpType = OP_INSERT;			// Добавление строки
					// Заполнение данных в субтаблице приемника
					SetFldData(arDstSubFld, iOpType, strErr);
					nRet = ExecUpdate(pDst, hDstSubIns, iOpType, arSubInsPrm, 
											sDstSubIns, TRUE, strErr);
					break;
				}
				default:
				{
					iOpType = OP_UPDATE;	// Модификация строки
					nRet = ExecUpdate(pDst, hDstSubUpd, iOpType, arSubUpdPrm, 
											sDstSubUpd, TRUE, strErr);
					break;
				}
				}
				break;
			}
			}
		}
		else							// Если безусловное добавление..
		{
			iOpType = OP_INSERT;		// Добавление строки
			// Заполнение данных в субтаблице приемника
			SetFldData(arDstSubFld, iOpType, strErr);
			nRet = ExecUpdate(pDst, hDstSubIns, iOpType, arSubInsPrm, 
									sDstSubIns, TRUE, strErr);
		}
		// Проверка результата операции
		if (nRet == SQL_ERROR)			// Ошибка записи
		{
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSSUB, strErr);
							strErr+= FormatUpd(ER_INSSUB); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDSUB, strErr);
							strErr+= FormatUpd(ER_UPDSUB); break;
			}
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
		{
			// Запись текста предупреждения в файл
			// и продолжить выполнение
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSSUB, strErr); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDSUB, strErr); break;
			}
			WarningHandle(strErr, nMsg);
		}
		if ((iMainType == OP_INSERT) && (iOpType == OP_INSERT))
		{
			// Завершение транзакции
			if (bTxnCommit)
				::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}

		/////////////////////////////////////////////////////////////
		// Выполнение SQL-команды
		/////////////////////////////////////////////////////////////
		if (bRunCmd && bSubRowCmd)		// Выполнение SQL-команды
		{
			switch (iOpType)			// Тип операции
			{
			case OP_INSERT:				// Команда OnInsert
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_INSERT,NULL,strErr);
				break;
			}
			case OP_UPDATE:				// Команда OnUpdate
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_UPDATE,NULL,strErr);
				break;
			}
			}
			if (nRet == SQL_ERROR)		// Ошибка SQL-команды
			{
				if ((iMainType == OP_INSERT) && (iOpType == OP_INSERT))
				{
					// Установить код возврата и запомнить ошибку
					bRetCode = FALSE; strSav = strErr;

					// Откат только команд предыдущих строк субтаблицы!
					BackDetail(FALSE, fileLog, nMsg, nErr);

					// Добавление строки в буфер (без признака SQL-команды)
					AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
								ROW_INSERTED,FALSE,FALSE);

					// Продолжить без выполнения команд
					bRunCmd = FALSE; continue;
				}
				if (!bTxnCommit)		// Выполнение обратной операции
				{
					switch (iOpType)	// Тип исходной операции
					{
					case OP_INSERT:		// Удаление строки (по источнику)
						nRet = ExecUpdate(pDst,hDstSubDel,OP_DELETE,arSubDelPrm,
											   sDstSubDel,TRUE,strRet);
						break;
					case OP_UPDATE:		// Восстановление (по приемнику)
						nRet = ExecUpdate(pDst,hDstSubUpd,OP_UPDATE,arSubUpdPrm,
											   sDstSubUpd,FALSE,strRet);
						break;
					}
					if (nRet == SQL_ERROR)	// Ошибка обратной операции
					{
						ErrorHandle(ER_BACK_CMD, strRet, nErr);
					}
				}
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
		}
		// Обработка триггера OnChange
		if (bSubFldCmd && (iOpType == OP_UPDATE) && (iMFlag == MOD_CMD))
		{
			// Выполнение команды OnChange для измененных полей
			nRet = ExecFldCmd(pDst, arDstSubFld, arDstSubCmd,
							  EV_ON_CHANGE, strErr);
			if (nRet == SQL_ERROR)
			{
				if (!bTxnCommit)
				{
					if (bRunCmd && bSubRowCmd)	// Выполнение обратной команды
					{
						nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_UPDATE,NULL,strRet);
						if (nRet == SQL_ERROR)	// Ошибка обратной команды
						{
							ErrorHandle(ER_BACK_CMD, strRet, nErr);
							// Не завершать отката
							bRetCode = FALSE; goto err;
						}
					}
					// Восстановление строки (по приемнику)
					nRet = ExecUpdate(pDst, hDstSubUpd,OP_UPDATE,arSubUpdPrm, 
											sDstSubUpd,FALSE,strRet);
					if (nRet == SQL_ERROR) // Ошибка обратной операции
					{
						ErrorHandle(ER_BACK_CMD, strRet, nErr);
					}
				}
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
		}
		if (bTxnCommit)					// Завершение транзакции
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
		switch (iOpType)				// Добавление строки в буфер
		{
		case OP_INSERT:
			AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
						ROW_INSERTED,bRunCmd,FALSE);
			break;
		case OP_UPDATE:
		{
			if (iMFlag == MOD_CMD)
				AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
							ROW_MODIFIED,bRunCmd,TRUE);
			else
				AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
							ROW_MODIFIED,bRunCmd,FALSE);
			break;
		}
		}
#ifdef _TRIAL_VER
		// Ограничение на количество записей в Trial версии
		if (n > 99) break;
#endif
	}

	////////////////////////////////////////////////////////////////
	// Выполнение команды OnEnd
	////////////////////////////////////////////////////////////////
	if (bRunCmd && iMainType == OP_INSERT)
	{
		strMsg.LoadString(IDS_MS_SUBEND);
		pDlg->SetSubText(strMsg);
		nRet = ExecSQLCmd(pDst, arDstSubCmd, EV_ON_END, NULL, strErr);
		if (nRet == SQL_ERROR)
		{
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)
		{
			WarningHandle(strErr, nMsg);
		}
		if (bTxnCommit)					// Завершение транзакции
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
	}

end:// Освобождение дескрипторов SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// Очистка строки подтекста
	pDlg->SetSubText("");
	// Восстановить текст ошибки
	if (!bRetCode) strErr = strSav;
	return bRetCode;

err:
	// Освобождение дескрипторов SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// Откат назад (обработка буфера строк с его очисткой)
	BackDetail(TRUE, fileLog, nMsg, nErr);
	// Очистка строки подтекста
	pDlg->SetSubText("");
	return bRetCode;
}

/////////////////////////////////////////////////////////////////////
// Выполнение команд в субтаблице

BOOL CTransfer::RunCmdDetail(OP_TYPE iMainType, CStdioFile &fileLog, 
							 UINT &nMsg, UINT &nErr, CString &strErr) 
{
	// Параметры:
	// iMainType - тип операции основной таблицы
	// &fileLog  - ссылка на файл протокола
	// &nErr     - ссылка на счетчик ошибок
	// &nMsg     - ссылка на счетчик предупреждений
	// &strErr   - ссылка на строку ошибки
	// Переменные состояния
	BOOL bRetCode = TRUE;		// Возвращаемое значение
	RETCODE nRet;				// Результат SQL оператора
	MOD_FLAG iMFlag;			// Флаг модификации данных
	OP_TYPE iOpType;			// Тип оператора SQL
	CString strSav;				// Строка сохранения
	CString strMsg;				// Строка сообщения
	CString strRet;				// Строка результата
	long	n;					// Счетчик строк

	if (!bSubRowCmd) return TRUE;

	// Очистка буфера строк
	ClearBufArr(arSubRowBuf);

	////////////////////////////////////////////////////////////////
	//              ОБРАБОТКА ПРИЕМНИКА
	////////////////////////////////////////////////////////////////
	if (iMainType == OP_DELETE)
	{
		strMsg.LoadString(IDS_MS_READ_SUBDST);
		pDlg->SetSubText(strMsg);
		////////////////////////////////////////////////////////////////
		// Выполнение SQL Select подчиненного приемника (без Fetch)
		////////////////////////////////////////////////////////////////
		if (ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubPrm, 
						sDstSubSQL, FALSE, strErr) == SQL_ERROR)
		{
			strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		// Цикл по субтаблице приемника
		for (n = 1;; n++)
		{
			if (bCancel) goto end;
			// Проверка статуса
			pDlg->CheckState();
			if (pDlg->IsBreaked())
			{
				bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
				goto err;
			}
			// Установка строки подтекста
			strMsg.FormatMessage(IDS_MS_FMT_SUBPRC, n);
			pDlg->SetSubText(strMsg);
			// Считывание строки данных из субтаблицы приемника
			nRet = FetchData(hDstSubSQL, arDstSubFld, strErr);
			// Проверка результата чтения
			if (nRet == SQL_NO_DATA)	break;		// Нет данных
			else if (nRet == SQL_ERROR)				// Ошибка чтения
			{
				strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
			{
				// Запись текста предупреждения в файл
				// и продолжить выполнение
				strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			// Выполнение SQL-команды
			nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_DELETE,NULL,strErr);
			if (nRet == SQL_ERROR)	// Ошибка SQL-команды
			{
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
			// Добавление строки в буфер
			AddBufRow(arSubRowBuf,arDstSubFld,arDstSubFld,n,
					ROW_CMDONDEL,TRUE,FALSE);

#ifdef _TRIAL_VER
			// Ограничение на количество записей в Trial версии
			if (n > 99) break;
#endif
		}
		// Освобождение дескриптора SQL
		FreeSQLStmt(hDstSubSQL, TRUE);
	}

	////////////////////////////////////////////////////////////////
	//              ОБРАБОТКА ИСТОЧНИКА
	////////////////////////////////////////////////////////////////
	else
	{
		strMsg.LoadString(IDS_MS_READ_SUBSRC);
		pDlg->SetSubText(strMsg);
		////////////////////////////////////////////////////////////////
		// Выполнение SQL Select подчиненного источника (без Fetch)
		////////////////////////////////////////////////////////////////
		if (ExecSelect(pSrc, hSrcSubSQL, arSrcSubFld, arSrcSubPrm, 
						pTrn->sSubSQL, FALSE, strErr) == SQL_ERROR)
		{
			strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		// Цикл по субтаблице источника
		for (n = 1;; n++)
		{
			if (bCancel) goto end;
			// Проверка статуса
			pDlg->CheckState();
			if (pDlg->IsBreaked())
			{
				bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
				goto err;
			}
			// Установка строки подтекста
			strMsg.FormatMessage(IDS_MS_FMT_SUBUPD, n);
			pDlg->SetSubText(strMsg);
			// Считывание строки данных из источника
			nRet = FetchData(hSrcSubSQL, arSrcSubFld, strErr);
			// Проверка результата чтения
			if (nRet == SQL_NO_DATA)	break;		// Нет данных
			else if (nRet == SQL_ERROR)				// Ошибка чтения
			{
				strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)	// Предупреждение
			{
				// Запись текста предупреждения в файл
				// и продолжить выполнение
				strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			// Обработка данных
			if (iMainType == OP_UPDATE)				// Модификация
			{
				// Выполнение SQL запроса к субтаблице приемника
				// по значению первичных ключей субтаблицы
				nRet = ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubQry, 
									sDstSubQry, TRUE, strErr);
				switch (nRet)
				{
				case SQL_ERROR:				// Ошибка чтения
				{
					strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
					bRetCode = FALSE;
					goto err;
				}
				case SQL_NO_DATA:			// Не найдено данных
				{
					iOpType = OP_INSERT;	// Команда ON_INSERT
					break;
				}
				case SQL_SUCCESS_WITH_INFO:	// Предупреждение
				{
					// Запись текста предупреждения в файл
					// и продолжить выполнение
					strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
					WarningHandle(strErr, nMsg);
				}
				case SQL_SUCCESS:			// Данные найдены
				{
					// Сравнение данных в приемнике по ссылке
					if (!CompareRef(arDstSubFld, OP_UPDATE, iMFlag, strErr))
					{
						// Запись текста ошибки в файл
						// и продолжить выполнение
						strErr = GetErrMsg(ER_COMP_DSTSUB, strErr);
						WarningHandle(strErr, nMsg);
					}
					if (iMFlag)				// Проверка флага модификации
					{
						iOpType = OP_UPDATE;// Команда ON_UPDATE
					}
					else					// Данные не изменились
					{
						iOpType = OP_UNDEF;	// Не выполнять команду
						continue;
					}
					break;
				}
				}
			}
			else							// Выполнение
			{
				iOpType = OP_INSERT;		// Команда ON_INSERT
			}
			// Выполнение команды SQL
			switch (iOpType)			// Тип операции
			{
			case OP_INSERT:				// Команда OnInsert
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_INSERT,NULL,strErr);
				break;
			}
			case OP_UPDATE:				// Команда OnUpdate
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_UPDATE,NULL,strErr);
				break;
			}
			}
			if (nRet == SQL_ERROR)		// Ошибка SQL-команды
			{
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
			if (bTxnCommit)				// Завершение транзакции
			{
				::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
			}
			switch (iOpType)			// Добавление строки в буфер
			{
			case OP_INSERT:
				AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
							ROW_CMDONINS,TRUE,FALSE);
				break;
			case OP_UPDATE:
			{
				AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
							ROW_CMDONUPD,TRUE,FALSE);
				break;
			}
			}

#ifdef _TRIAL_VER
			// Ограничение на количество записей в Trial версии
			if (n > 99) break;
#endif
		}
	}

end:// Освобождение дескрипторов SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// Очистка строки подтекста
	pDlg->SetSubText("");
	return bRetCode;

err:
	// Освобождение дескрипторов SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// Откат назад (обработка буфера строк с его очисткой)
	BackDetail(TRUE, fileLog, nMsg, nErr);
	// Очистка строки подтекста
	pDlg->SetSubText("");
	return bRetCode;
}


/////////////////////////////////////////////////////////////////////
// Откат трансферта субтаблицы (обработка буфера строк)

BOOL CTransfer::BackDetail(BOOL bDelBuf, CStdioFile &fileLog, 
						   UINT &nMsg, UINT &nErr) 
{
	// Параметры:
	// bDelBuf   - признак удаления данных буфера
	// &fileLog  - ссылка на файл протокола
	// &nErr     - ссылка на счетчик ошибок
	// &nMsg     - ссылка на счетчик предупреждений

	// Переменные состояния
	RETCODE nRet;
	CString strErr,strMsg;
	// Размер массива субтаблицы источника
	int nSize = arSrcSubFld.GetSize();
	// Размер массива субтаблицы приемника
	int mSize = arDstSubFld.GetSize();
	// Обработка буфера строк с конца
	for (int i = arSubRowBuf.GetSize() - 1; i >= 0; i--)
	{
		if (arSubRowBuf[i].pArSrcFld != NULL && 
			arSubRowBuf[i].pArDstFld != NULL)
		{
			// Перенос данных в источник
			for (int j = 0; j < nSize; j++)
			{
				arSrcSubFld[j].bMod  = arSubRowBuf[i].pArSrcFld->GetAt(j).bMod;
				arSrcSubFld[j].vData = arSubRowBuf[i].pArSrcFld->GetAt(j).vData;
			}
			// Перенос данных в приемник
			for (int k = 0; k < mSize; k++)
			{
				arDstSubFld[k].bMod  = arSubRowBuf[i].pArDstFld->GetAt(k).bMod;
				arDstSubFld[k].vData = arSubRowBuf[i].pArDstFld->GetAt(k).vData;
			}
			// Обработка данных
			switch (arSubRowBuf[i].nStatus)	// Статус строки
			{
			case ROW_INSERTED:				// Строка вставлена
			{
				// Установка подтекста
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// Откат SQL-команды строки
				if (arSubRowBuf[i].bRunCmd && bSubRowCmd)
				{
					// Выполнение команды ReInsert
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_INSERT,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				if (bDelBuf)				// Удаление(по источнику)
				{
					nRet = ExecUpdate(pDst, hDstSubDel,OP_DELETE,arSubDelPrm,
											sDstSubDel,TRUE,strErr);

					if (nRet == SQL_ERROR)	// Ошибка удаления
					{
						strErr = GetErrMsg(ER_DELSUB, strErr);
						strErr+= FormatUpd(ER_DELSUB);
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						strErr=GetErrMsg(ER_DELSUB,strErr);
						WarningHandle(strErr, nMsg);
					}
				}
				break;
			}
			case ROW_MODIFIED:				// Строка изменена
			{
				// Установка подтекста
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// Откат команд OnChange
				if (arSubRowBuf[i].bEvChanged && bSubFldCmd)
				{
					// Выполнение команды ReChange для измененных полей
					nRet = ExecFldCmd(pDst, arDstSubFld, arDstSubCmd,
									  EV_RE_CHANGE, strErr);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bEvChanged = FALSE;
				}
				// Откат SQL-команды строки
				if (arSubRowBuf[i].bRunCmd && bSubRowCmd)
				{
					// Выполнение команды ReUpdate
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_UPDATE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				if (bDelBuf)				// Восстановление(по приемнику)
				{
					nRet = ExecUpdate(pDst, hDstSubUpd,OP_UPDATE,arSubUpdPrm,
											sDstSubUpd,FALSE,strErr);

					if (nRet == SQL_ERROR)	// Ошибка записи
					{
						strErr = GetErrMsg(ER_UPDSUB, strErr);
						strErr+= FormatUpd(ER_UPDSUB);
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						strErr=GetErrMsg(ER_UPDSUB,strErr);
						WarningHandle(strErr, nMsg);
					}
				}
				break;
			}
			case ROW_DELETED:				// Строка удалена
			{
				// Установка подтекста
				strMsg.FormatMessage(IDS_MS_FMT_SUBRES,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				if (bDelBuf)				// Вставка(по приемнику)
				{
					nRet = ExecUpdate(pDst, hDstSubIns,OP_INSERT,arSubInsPrm,
											sDstSubIns,FALSE,strErr);

					if (nRet == SQL_ERROR)	// Ошибка записи
					{
						strErr = GetErrMsg(ER_INSSUB, strErr);
						strErr+= FormatUpd(ER_INSSUB);
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						strErr=GetErrMsg(ER_INSSUB,strErr);
						WarningHandle(strErr, nMsg);
					}
				}
				// Откат SQL-команды строки
				if (arSubRowBuf[i].bRunCmd && bSubRowCmd)
				{
					// Выполнение команды ReDelete
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				break;
			}
			case ROW_CMDONINS:				// Команда ON_INSERT
			{
				// Установка подтекста
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// Откат SQL-команды строки
				if (arSubRowBuf[i].bRunCmd)
				{
					// Выполнение команды ReInsert
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_INSERT,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				break;
			}
			case ROW_CMDONUPD:				// Команда ON_UPDATE
			{
				// Установка подтекста
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// Откат SQL-команды строки
				if (arSubRowBuf[i].bRunCmd)
				{
					// Выполнение команды ReUpdate
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_UPDATE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				break;
			}
			case ROW_CMDONDEL:				// Команда ON_DELETE
			{
				// Установка подтекста
				strMsg.FormatMessage(IDS_MS_FMT_SUBRES,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// Откат SQL-команды строки
				if (arSubRowBuf[i].bRunCmd)
				{
					// Выполнение команды ReDelete
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// Ошибка SQL-команды
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				break;
			}
			default:	goto cont;
			}
			if (bTxnCommit)					// Завершение транзакции
			{
				::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
			}
cont:		if (bDelBuf)					// Удаление элемента
			{
				if (arSubRowBuf[i].pArSrcFld != NULL)
				{
					ClearFldArr(*arSubRowBuf[i].pArSrcFld);
					delete arSubRowBuf[i].pArSrcFld;
					arSubRowBuf[i].pArSrcFld = NULL;
				}
				if (arSubRowBuf[i].pArDstFld != NULL)
				{
					ClearFldArr(*arSubRowBuf[i].pArDstFld);
					delete arSubRowBuf[i].pArDstFld;
					arSubRowBuf[i].pArDstFld = NULL;
				}
				arSubRowBuf.RemoveAt(i);
			}
		}
		continue;

err:	ErrorHandle(ER_BACK_CMD, strErr, nErr);
	}

	if (bDelBuf)
	{
		arSubRowBuf.FreeExtra();
		arSubRowBuf.SetSize(0);
	}
	return TRUE;
}
