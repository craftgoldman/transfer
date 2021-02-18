// Transfer.h: interface for the CTransfer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TRANSFER_H)
#define _TRANSFER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "trans30.h"
#include "Progress.h"

// Тип ссылки
enum REF_TYPE {
    REF_TAB,	// Таблица
    REF_SUB		// Субтаблица
};

// Тип операции
enum OP_TYPE {
	OP_UNDEF,	// Не определено
    OP_INSERT,	// SQL Insert
    OP_UPDATE,	// SQL Update
	OP_DELETE,	// SQL Delete
	OP_COMMAND	// SQL-команда
};

// Флаг изменения (по приоритету)
enum MOD_FLAG {
	MOD_NO,		// 0 Not modified
	MOD_YES,	// 1 Modified
    MOD_CMD,	// 2 OnChange
	MOD_EXEC,	// 3 Execute
	MOD_BACK,	// 4 RollBack
    MOD_TRG		// 5 Trigger
};

class CTransfer : public CObject  
{
    DECLARE_DYNCREATE(CTransfer)

// Construction
public:
	CTransfer();
	CTransfer(int nKindTrans,
			  ODBC_INFO *pSrcInfo,
			  ODBC_INFO *pDstInfo,
			  TRN_INFO  *pTrnInfo,
			  CFldArray *pArrParm);
	virtual ~CTransfer();

// Implementation
public:
	BOOL IsValid();
	BOOL Parse(CString &strErr);
	BOOL Run(CString &strErr, UINT &nMsg, UINT &nErr);

// Operations
protected:
	void ErrorHandle(ER_TYPE errType, CString &strErr, UINT &nErr);
	void WarningHandle(CString &strErr, UINT &nMsg);
	void FreeAllStmt();
	void ClearAllArr();
	void ClearFldArr(CFldArray &arFld);
	void ClearBufArr(CBufArray &arBuf);
	void ClearCmdArr(CCmdArray &arCmd);
	void ResetFldArr(CFldArray &arFld);
	void AddBufRow(CBufArray &arBuf, CFldArray &arSrcFld, 
					CFldArray &arDstFld, long nRow, 
					ROW_STATUS nStatus, BOOL bRunCmd, 
					BOOL bEvChanged);
	void FreeSQLStmt(SQLHSTMT &hstmt, BOOL bDrop=TRUE);
	void ShowErr(LPCTSTR szTitle, LPCTSTR szError);
	int  ShowMsg(LPCTSTR szTitle, LPCTSTR szMessage);
	int  FindParam(CFldArray &arPrm, LPCTSTR szID);
	int  FindField(CFldArray &arFld, LPCTSTR szName);
	int  FindSrcRef(CFldArray &arFld, REF_TYPE iRefType, 
					LPCTSTR szRef);
	int  FindEvent(CCmdArray &arCmd, EV_TYPE iEventID);
	int  FindSQLCmd(CCmdArray &arCmd, EV_TYPE iEventID, 
					LPCTSTR szField);
	BOOL TableInSQL(CString strSQL,   CString &strTable, 
					CString &strErr);
	BOOL CompareRef(CFldArray &arFld, OP_TYPE iOpType, 
					MOD_FLAG &iMFlag, CString &strErr);
	BOOL SetDstRefs(CFldArray &arFld, REF_TYPE iRefType, 
					BOOL bKeysOnly,   CString &strErr);
	BOOL SetFldRefs(CFldArray &arFld, REF_TYPE iRefType, 
					CString &strErr);
	BOOL SetFldKeys(ODBC_INFO *pODBC, CFldArray &arFld, 
					CString strOwner, CString strTable, 
					CString &strErr);
	BOOL SetFldData(CFldArray &arFld, OP_TYPE iOpType, 
					CString &strErr);
	BOOL SetDstData(CFldArray &arFld, CString &strErr);
	BOOL ConvertTxt(CFldArray &arFld, OP_TYPE iOpType,
					UINT iConvert);
	BOOL ParseColumns(ODBC_INFO *pODBC, CFldArray &arFld, 
					CString strSQL,    CString &strErr);
	BOOL ParseSQLParams(ODBC_INFO *pODBC, CFldArray *pArPrm, 
					CString &strSQL,   CString &strMsg);
	BOOL PrepareSQLParms(ODBC_INFO *pODBC, CFldArray *pArPrm, 
					CString strOwner, CString &strSQL,   
					CString &strMsg);
	BOOL CreateTable(ODBC_INFO *pODBC, UINT iSchema, 
					CFldArray &arFld, CFldArray &arMap, 
					CFldArray &arSrc, CString strTable, 
					CString &strErr);
	BOOL ParseTable(ODBC_INFO *pODBC, UINT iSchema, 
					CFldArray &arFld, CFldArray &arMap, 
					CString strOwner, CString strTable, 
					CString &strErr);
	BOOL ParseFldCmd(CFldArray &arFld, CCmdArray &arCmd, 
					CString &strErr);
	BOOL ParseCmdArr(ODBC_INFO *pODBC, REF_TYPE iRefType, 
					CCmdArray &arCmd, BOOL bMakeParams, 
					CString &strErr);
	BOOL ParseStrMsg(CString &strMsg, CString &strErr);
	BOOL MakeSelect(CFldArray &arFld, CString strTable, 
					CString strWhere, BOOL bUseKeys, 
					CString &strSQL,  CString &strErr);
	BOOL MakeInsert(CFldArray &arFld, CFldArray &arPrm, 
					CString strTable, CString &strSQL, 
					REF_TYPE iRefType, CString &strErr);
	BOOL MakeUpdate(CFldArray &arFld, CFldArray &arPrm, 
					CString strTable, CString &strSQL, 
					REF_TYPE iRefType, CString &strErr);
	BOOL MakeDelete(CFldArray &arFld, CFldArray &arPrm, 
					CString strTable, CString &strSQL, 
					REF_TYPE iRefType, CString &strErr);
	BOOL BindParams(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
					CFldArray &arPrm, CString &strErr);
	BOOL BindFields(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
					CFldArray &arFld, CString &strErr);
	long ExecSQLMax(ODBC_INFO *pODBC, FLD_INFO &fldInfo, 
					CString &strErr);
	long ExecSQLCount(ODBC_INFO *pODBC, CString strCmd, 
					  CFldArray &arPrm, CString &strErr);
	RETCODE ExecSQLFunc(ODBC_INFO *pODBC, FNC_TYPE func, 
						CString strParms, CString &strVal, 
						CString &strErr);
	RETCODE ExecSelect(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
					   CFldArray &arFld, CFldArray &arPrm, 
					   CString &strCmd,  BOOL bFetchData, 
					   CString &strErr);
	RETCODE ExecUpdate(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
					   OP_TYPE iOpType,  CFldArray &arPrm, 
					   CString &strCmd,  BOOL bUseSrcData, 
					   CString &strErr);
	RETCODE ExecSQLCmd(ODBC_INFO *pODBC, CCmdArray &arCmd, 
					   EV_TYPE iEventID, LPCTSTR szField, 
					   CString &strErr);
	RETCODE ExecFldCmd(ODBC_INFO *pODBC, CFldArray &arFld, 
					   CCmdArray &arCmd, EV_TYPE iEventID, 
					   CString &strErr);
	RETCODE FetchData(SQLHSTMT &hstmt, CFldArray &arFld, 
					  CString &strErr);
	RETCODE FetchScroll(SQLHSTMT &hstmt, CFldArray &arFld, 
						USHORT iDirection, UINT iOffset, 
						CString &strErr);
	RETCODE GetLongData(SQLHSTMT &hstmt, UINT iCol, 
						CDataValue &vData, CString &strErr);
	RETCODE PutLongData(SQLHSTMT &hstmt, CDataValue &vData);
	CString FormatMsg(LPCTSTR szMsg);
	CString FormatSQL(LPCTSTR szSQL, CFldArray &arPrm);
	CString FormatUpd(ER_TYPE iErrType);
	CString FormatErr(LPCTSTR szErr);
	BOOL RunDetail(OP_TYPE iMainType, BOOL bRunCmd, CStdioFile &fileErr, 
					UINT &nMsg, UINT &nErr, CString &strErr);
	BOOL RunCmdDetail(OP_TYPE iMainType, CStdioFile &fileErr, 
					UINT &nMsg, UINT &nErr, CString &strErr);
	BOOL BackDetail(BOOL bDelBuf, CStdioFile &fileErr, 
					UINT &nMsg, UINT &nErr);

// Properties
protected:
	BOOL bValid;			// Признак инициализации
	BOOL bShowErr;			// Признак показа ошибки
	BOOL bBreaked;			// Признак прерывания
	BOOL bComplex;			// Признак COMPLEX
	BOOL bMessage;			// Признак строки сообщения
	BOOL bTxnCommit;		// Признак Commit в DBMS приемника
	BOOL bTabSearch;		// Признак поиска в таблице приемника
	BOOL bSubSearch;		// Признак поиска в субтаблице приемника
	BOOL bTabRowCmd;		// Признак построчных команд таблицы
	BOOL bSubRowCmd;		// Признак построчных команд субтаблицы
	BOOL bTabFldCmd;		// Признак команд полей таблицы
	BOOL bSubFldCmd;		// Признак команд полей субтаблицы
	CProgressDlg* pDlg;		// Диалог выполнения
	int  nKind;				// Вид трансферта
	ODBC_INFO*    pSrc;		// Описание источника ODBC
	ODBC_INFO*    pDst;		// Описание приемника ODBC
	TRN_INFO*     pTrn;		// Описание трансферта
	CFldArray*    pPrm;		// Массив параметров трансферта
	CBufArray arTabRowBuf;	// Буфер строк таблицы
	CBufArray arSubRowBuf;	// Буфер строк субтаблицы
	CFldArray arStrMsgPrm;	// Массив параметров строки сообщения
	CFldArray arMapTabFld;	// Карта полей таблицы приемника
	CFldArray arMapSubFld;	// Карта полей субтаблицы приемника
	CFldArray arSrcTabPrm;	// Параметры Select источника
	CFldArray arSrcTabFld;	// Поля      Select источника
	CFldArray arSrcSubQry;	// Параметры Query  субисточника
	CFldArray arSrcSubPrm;	// Параметры Select субисточника
	CFldArray arSrcSubFld;	// Поля      Select субисточника
	CFldArray arDstTabPrm;	// Параметры Select приемника
	CFldArray arDstTabFld;	// Поля      Select приемника
	CFldArray arDstSubQry;	// Параметры Query  субприемника
	CFldArray arDstSubPrm;	// Параметры Select субприемника
	CFldArray arDstSubFld;	// Поля      Select субприемника
	CFldArray arTabInsPrm;	// Параметры Insert таблицы приемника
	CFldArray arTabUpdPrm;	// Параметры Update таблицы приемника
	CFldArray arTabDelPrm;	// Параметры Delete таблицы приемника
	CFldArray arSubInsPrm;	// Параметры Insert субтаблицы приемника
	CFldArray arSubUpdPrm;	// Параметры Update субтаблицы приемника
	CFldArray arSubDelPrm;	// Параметры Delete субтаблицы приемника
	CCmdArray arDstTabCmd;	// Команды таблицы приемника
	CCmdArray arDstSubCmd;	// Команды субталицы приемника
	CString   sSrcMTable;	// Основная таблица источника
	CString   sSrcTabQry;	// SQL Query  таблицы источника
	CString   sDstTabSQL;	// SQL Select таблицы приемника
	CString   sDstTabQry;	// SQL Query  таблицы приемника
	CString   sDstTabIns;	// SQL Insert таблицы приемника
	CString   sDstTabUpd;	// SQL Update таблицы приемника
	CString   sDstTabDel;	// SQL Delete таблицы приемника
	CString   sSrcSTable;	// Подчиненная таблица источника
	CString   sSrcSubQry;	// SQL Query  субтаблицы источника
	CString   sDstSubSQL;	// SQL Select субтаблицы приемника
	CString   sDstSubQry;	// SQL Query  субтаблицы приемника
	CString   sDstSubIns;	// SQL Insert субтаблицы приемника
	CString   sDstSubUpd;	// SQL Update субтаблицы приемника
	CString   sDstSubDel;	// SQL Delete субтаблицы приемника
	SQLHSTMT  hSrcTabSQL;	// hstmt Select таблицы источника
	SQLHSTMT  hDstTabSQL;	// hstmt Select таблицы приемника
	SQLHSTMT  hDstTabIns;	// hstmt Insert таблицы приемника
	SQLHSTMT  hDstTabUpd;	// hstmt Update таблицы приемника
	SQLHSTMT  hDstTabDel;	// hstmt Delete таблицы приемника
	SQLHSTMT  hSrcSubSQL;	// hstmt Select субтаблицы источника
	SQLHSTMT  hDstSubSQL;	// hstmt Select субтаблицы приемника
	SQLHSTMT  hDstSubIns;	// hstmt Insert субтаблицы приемника
	SQLHSTMT  hDstSubUpd;	// hstmt Update субтаблицы приемника
	SQLHSTMT  hDstSubDel;	// hstmt Delete субтаблицы приемника
};

#endif // !defined(_TRANSFER_H)
