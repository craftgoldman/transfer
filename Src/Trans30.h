// Trans30.h : main header file for the TRANS30 application
//

#if !defined(_TRANS30_H)
#define _TRANS30_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "DataVal.h"

#define SECS_TO_FT_MULT 10000000

/////////////////////////////////////////////////////////////////////
// ���������� ���������
//

#define MDI_UPDATE_BAR      (WM_USER + 1)
#define FORM_SET_STATE      (WM_USER + 2)
#define FORM_GET_STATE      (WM_USER + 3)
#define FORM_UPDATE_DATA    (WM_USER + 4)
#define FORM_UPDATE_MENU    (WM_USER + 5)
#define FORM_QUERY_SAVE     (WM_USER + 6)
#define TRAY_NOTIFYICON		(WM_APP + 100)

// ��������� �����
enum FORM_STATE {
	FORM_EDIT,			// ��������������
	FORM_NEW			// ����
};

// ����� ����������
enum TRN_SCHEMA {
	TRN_COLUMNS,		// �������
	TRN_COPY,			// �����
	TRN_COMPLEX			// ��������
};

// ����� ����������
enum TRN_CONVERT {
	TRN_UNCHANGED,		// �������������
	TRN_OEMTOANSI,		// OEM � ANSI
	TRN_ANSITOOEM		// ANSI � OEM
};

// ��� ��������� �������
enum DST_PROCESS {
    DST_CREATE,			// ��������
    DST_ADD,			// ����������
    DST_REPLACE,		// ���������
	DST_FULL			// ������ ���������
};

// ���� ���������� ������
enum SRC_TYPE {
	DATA_LOCAL,			// ��������
	DATA_AUTO,			// �������������
	DATA_PARAM,			// ��������
	DATA_CONST,			// ���������
	DATA_TABSRC,		// �������� ��������
	DATA_SUBSRC,		// ����������� ��������
	DATA_TABDST,		// �������� ��������
	DATA_SUBDST,		// ����������� ��������
	DATA_REF,			// ���������������
	DATA_FUNC,			// �������
	DATA_PROC			// ���������
};

// ����� �����
enum FLD_FLAG {
	FLD_DATA,			// ������ ������
	FLD_KEY,			// ���������� ����
	FLD_NOCHANGE,		// ������������ ����
	FLD_EVCHANGE,		// ������� ��� ���������
	FLD_FRUNBACK,		// ���� ����������/������
	FLD_TRIGGER			// ������� ��� ���������
};

// ���� �������
enum EV_TYPE {
	EV_UNKNOWN,			// ��������������
	EV_ON_START,		// �� ������
	EV_ON_END,			// ��� ���������
	EV_CALL_EXEC,		// ��� ����������
	EV_CALL_BACK,		// ��� ������
	EV_ON_INSERT,		// ��� ������� ������
	EV_ON_UPDATE,		// ��� ��������� ������
	EV_ON_DELETE,		// ��� �������� ������
	EV_RE_INSERT,		// ��� ������ �������
	EV_RE_UPDATE,		// ��� ������ ���������
	EV_RE_DELETE,		// ��� ������ ��������
	EV_ON_CHANGE,		// ��� ��������� ����
	EV_RE_CHANGE		// ��� ������ ��������� ����
};

// ������ ������
enum ROW_STATUS {
	ROW_UNKNOWN,		// ��������������
	ROW_DELETED,		// ������ �������
	ROW_MODIFIED,		// ������ ��������
	ROW_INSERTED,		// ������ ���������
	ROW_CMDONDEL,		// ������� ON_DELETE
	ROW_CMDONUPD,		// ������� ON_UPDATE
	ROW_CMDONINS		// ������� ON_INSERT
};

// ���� ���������� �������
enum FNC_TYPE {
	FNC_SQLGETVAL,		// ������ �������� ����
	FNC_SQLMAXVAL,		// ������ ������������� �������� ����
	FNC_SQLMINVAL,		// ������ ������������� �������� ����
	FNC_SQLAVGVAL,		// ������ �������� �������� ����
	FNC_SQLSUMVAL,		// ������ ����� �������� ����
	FNC_SQLVARVAL,		// ������ �������� �������� ����
	FNC_SQLCOUNT		// ������ ���������� ������� 
};

// ���� ������
enum ER_TYPE {
    ER_UNKNOWN,			// ��������������
    ER_NOTVALID,		// ��� ����������
	ER_NO_PPL,			// �� ������ ���� �������
	ER_NO_EXPR,			// ������ ���������
	ER_NO_TABLE,		// �� ������� ��� �������
	ER_NO_KEYS,			// �� ����������� �������� �����
	ER_NO_FIELDS,		// �������������� ����� �� �������� �����
	ER_NO_MODFLD,		// �� ���������� ���������� �����
	ER_NEED_CODE,		// ������� ���!
	ER_NEED_NAME,		// ������� ������������!
	ER_NEED_FILE,		// ������� ��� �����!
	ER_NEED_VALUE,		// ������� ��������!
	ER_NEED_FIELD,		// ������� ��� ����!
	ER_NEED_COMMAND,	// ������� �������!
	ER_NEED_SQLTAB,		// ������� SQL ������ ������� ���������!
	ER_NEED_SQLSUB,		// ������� SQL ������ ���������� ���������!
	ER_IGNORE_SQLSUB,	// SQL ������ ���������� �� ����� ��������������!
	ER_DOUBLE_FIELD,	// ��� ���� ��� ������������
	ER_DOUBLE_COND,		// ������� ��� ������������
	ER_DOUBLE_CODE,		// ��� ��� ������������
	ER_FIND_FILE,		// ���� �� ������
	ER_READ_FILE,		// ������ ������ �����
	ER_OPEN_FILE,		// ���������� ������� ����
	ER_WRITE_FILE,		// ������ ������ � ����
	ER_CONN_SRC,		// ������ ���������� � �� ���������
	ER_CONN_DST,		// ������ ���������� � �� ���������
    ER_INSTAB,			// ������ SQL Insert �������
    ER_UPDTAB,			// ������ SQL Update �������
	ER_DELTAB,			// ������ SQL Delete �������
    ER_INSSUB,			// ������ SQL Insert ����������
    ER_UPDSUB,			// ������ SQL Update ����������
	ER_DELSUB,			// ������ SQL Delete ����������
	ER_SNTX_PARAM,		// ������ ���������� ���������
	ER_SNTX_PARAMS,		// ������ ���������� ����������
	ER_SNTX_MAPTAB,		// ������ ���������� ����� ����� �������
	ER_SNTX_MAPSUB,		// ������ ���������� ����� ����� ����������
	ER_SNTX_CMDTAB,		// ������ ���������� SQL-������ �������
	ER_SNTX_CMDSUB,		// ������ ���������� SQL-������ ����������
	ER_MAIN_TABLE,		// ������ ����������� �������� �������
	ER_CRT_SQL,			// ������ �������� ����������� SQL
	ER_CRT_TABLE,		// ������ �������� �������
	ER_CRT_INDEX,		// ������ �������� ����������� �������
	ER_CRT_TAB,			// �������� ������� ���������
	ER_CRT_SUB,			// �������� ���������� ���������
	ER_PREP_SRCTAB,		// ���������� ������� ���������
	ER_PREP_SRCSUB,		// ���������� ���������� ���������
	ER_PREP_DSTTAB,		// ���������� ������� ���������
	ER_PREP_DSTSUB,		// ���������� ���������� ���������
	ER_PREP_MSG,		// ���������� ������ ���������
	ER_PREP_INSTAB,		// ���������� SQL Insert �������
	ER_PREP_UPDTAB,		// ���������� SQL Update �������
	ER_PREP_DELTAB,		// ���������� SQL Delete �������
	ER_PREP_INSSUB,		// ���������� SQL Insert ����������
	ER_PREP_UPDSUB,		// ���������� SQL Update ����������
	ER_PREP_DELSUB,		// ���������� SQL Delete ����������
	ER_PREP_CMDTAB,		// ���������� SQL-������ �������
	ER_PREP_CMDSUB,		// ���������� SQL-������ ����������
	ER_PREP_PARSRCTAB,	// ���������� ���������� ������� ���������
	ER_PREP_SELSRCTAB,	// ���������� SQL Select ������� ���������
	ER_PREP_FNDSRCTAB,	// ���������� SQL ������� � ������� ���������
	ER_PREP_PARSRCSUB,	// ���������� ���������� ���������� ���������
	ER_PREP_SELSRCSUB,	// ���������� SQL Select ���������� ���������
	ER_PREP_FNDSRCSUB,	// ���������� SQL ������� � ���������� ���������
	ER_PREP_PARDSTTAB,	// ���������� ���������� ������� ���������
	ER_PREP_SELDSTTAB,	// ���������� SQL Select ������� ���������
	ER_PREP_FNDDSTTAB,	// ���������� SQL ������� � ������� ���������
	ER_PREP_PARDSTSUB,	// ���������� ���������� ���������� ���������
	ER_PREP_SELDSTSUB,	// ���������� SQL Select ���������� ���������
	ER_PREP_FNDDSTSUB,	// ���������� SQL ������� � ���������� ���������
	ER_COUNT_SRCTAB,	// ������� ����� � ������� ���������
	ER_COUNT_DSTTAB,	// ������� ����� � ������� ���������
	ER_READ_SRCTAB,		// ������ ������ �� ������� ���������
	ER_READ_DSTTAB,		// ������ ������ �� ������� ���������
	ER_READ_SRCSUB,		// ������ ������ �� ���������� ���������
	ER_READ_DSTSUB,		// ������ ������ �� ���������� ���������
	ER_FIND_SRCTAB,		// ����� ������ � ������� ���������
	ER_FIND_DSTTAB,		// ����� ������ � ������� ���������
	ER_FIND_SRCSUB,		// ����� ������ � ���������� ���������
	ER_FIND_DSTSUB,		// ����� ������ � ���������� ���������
	ER_FIND_PARAM,		// �� ������ ��������
	ER_FIND_CONST,		// �� ������� ���������
	ER_FIND_FLDKEY,		// �� ������� �������� ����
	ER_FIND_FLDMAP,		// �� ������� ���� � ����� �����
	ER_FIND_FLDSRCTAB,	// �� ������� ���� ������� ���������
	ER_FIND_FLDSRCSUB,	// �� ������� ���� ���������� ���������
	ER_FIND_FLDDSTREF,	// �� ������� ���� �� ������� �� ��������
	ER_FIND_FLDDSTTAB,	// �� ������� ���� ������� ���������
	ER_FIND_FLDDSTSUB,	// �� ������� ���� ���������� ���������
	ER_FIND_FUNC,		// �� ������� �������
	ER_FIND_PROC,		// �� ������� ���������
	ER_BAD_TYPE,		// ����������� ���
	ER_BAD_DATA,		// �������� ��������
	ER_BAD_CHAR,		// ������������ ������
	ER_BAD_VAL,			// ������������ ��������
	ER_BAD_REF,			// ������������ ������
	ER_BAD_FLDCMD,		// ������������ ���� ��� ������� OnChange
	ER_COMP_DSTTAB,		// ��������� ������ � ������� ���������
	ER_COMP_DSTSUB,		// ��������� ������ � ���������� ���������
	ER_CONV_PARTYPE,	// ������ ����������� ���� ���������
	ER_CONV_FLDTYPE,	// ������ ����������� ���� ����
	ER_CONV_PARAM,		// ������ ����������� �������� ���������
	ER_CONV_CONST,		// ������ ����������� �������� ���������
	ER_CONV_FIELD,		// ������ ����������� �������� ����
	ER_CONV_FUNC,		// ������ ����������� �������� �������
	ER_CONV_PROC,		// ������ ����������� �������� ���������
	ER_DEFINE,			// ������ � ��������
	ER_DEF_INDEX,		// ������ ����������� ����������� �������
	ER_DEF_REFTYPE,		// ���������� ���������� ��� �� ������
	ER_DEF_FIELDS,		// �� ��������� ������ �����
	ER_DEF_TABLE,		// ������� �� ����������
	ER_DEF_KEYS,		// ����������� �������� �����
	ER_DEF_REFS,		// ��������� ������
	ER_MOD_FLDKEY,		// ���������� �������� �������� ����
	ER_REPL_FLDKEY,		// ���������� �������� �������� ����
	ER_REPL_FLDDEF,		// ���������� �������� ��� ������������ ����
	ER_PARSE_PARAMS,	// ������ ������� ����������
	ER_CMD_PARAMS,		// ������ � ���������� �������
	ER_DESC_FIELDS,		// ������ ��������� �������� �����
	ER_DESC_PARAMS,		// ������ ��������� �������� ����������
	ER_DESC_CATTAB,		// ������ ��������� ������ ������
	ER_UNS_PARAMS,		// ������������ ����������
	ER_BIND_PARAM,		// ������ ���������� ���������
	ER_BIND_FIELD,		// ������ ���������� ����
	ER_BIND_PARAMS,		// ������ ���������� ����������
	ER_BIND_FIELDS,		// ������ ���������� ����� ������
	ER_SET_PARAMS,		// ������ ��������� ����������
	ER_BACK_DETAIL,		// ������ �������������� ������������
	ER_BACK_CMD,		// ������ ���������� �������� �������
	ER_BACK_OP			// ������ ���������� �������� ��������
};

// ��������� ���������
typedef struct _TRN_PARM {
	// ���������� � ��
	CString	sDSN;		// �������� ODBC
	CString	sUID;		// ������������� ������������
	CString	sPWD;		// ������ ������������
	// ����������
	CString	sNameApp;	// ������������ ����������
	CString	sDirApp;	// ���������� �������
	// �����
	CString sDirPpl;	// ���������� ������ ����������
	CString	sDirLog;	// ������� ����� ���������
	CString	sDirMsg;	// ������� ������ ���������
	CString	sFileLog;	// ���� ���������
	CString	sFileMsg;	// ���� ���������
	CString	sPplImp;	// ���� ������� �� ���������
	CString sPplExp;	// ���� �������� �� ���������
	UINT	iUseImp;	// ������ �� ���������
	UINT	iUseExp;	// ������� �� ���������
	// ���������
	CString	sLangID;	// ������������� �����
	UINT	iSplashW;	// ����� ���� "� ���������" ��� �������
	UINT	iMinimize;	// ������� ����������� ��� �������
	UINT	iSysTray;	// ������� ����������� � SysTray
	UINT	iSavePos;	// ������� ���������� ������� �������� ����
	UINT	iSaveMDI;	// ������� ���������� ������� MDI ����
	UINT	iMaxiMDI;	// ������� ������������ MDI ����
	// ���������
	UINT	iDefType;	// ���� ������ �� ���������
	UINT	iAutoExc;	// ���������� ������ �������
	UINT	iDelFile;	// �������� ��������� ��� �������
	UINT	iWriteOpr;	// ������ ��������� �������� � ����
	UINT	iWriteMsg;	// ������ ��������� � ���� ��� �������
	UINT	iShowErr;	// �������� ��������� �� ������� �� �����
	UINT	iTraceOpr;	// ������������ SQL-���������
	UINT	iTraceCmd;	// ������������ SQL-�������
	UINT	iTraceSel;	// ������������ SQL-Select
	// ����� �������
	UINT	iDepart;	// �������������
	UINT	iExcTime;	// �������� ������
	UINT	iIntTime;	// �������� ������
	CString	sFileImp;	// ���� �������
	CString	sFileExp;	// ���� ��������
	CString	sFileEmp;	// ���� ������
	CString	sDirPut;	// ������� ��������
	CString	sDirGet;	// ������� ������
	CString	sNameArc;	// ��� ������
} TRN_PARM;

// �������� �������
typedef struct _PPL {
	int		iNum;		// �����
	int		iKind;		// ��� �������
	CString	sFile;		// ���� ��������
	CString	sName;		// ������������
} PPL;

// �������� ����� ����������
typedef struct _TRN_INFO {
	BOOL	bValid;		// ������� ��������
	BOOL	bSelect;	// ������� ������
	UINT	iSchema;	// ����� ����������
	UINT	iConvert;	// ����������� ������
	CString	sName;		// ������������
	CString	sMsg;		// ������ ������ ���������
	UINT	iTabPrc;	// ��� ��������� ���������
	CString	sTabSQL;	// SQL Select ������� ���������
	CString	sTabOwn;	// ����� (��������) ���������
	CString	sTabDst;	// ������� ���������
	CString	sTabCond;	// ������� ������� ���������
	CString	sTabMap;	// ����� ����� ���������
	CString	sTabCmd;	// ������� ���������
	UINT	iSubPrc;	// ��� ��������� ����������
	CString	sSubSQL;	// SQL Select ����������
	CString	sSubOwn;	// ����� (��������) ����������
	CString	sSubDst;	// ������� ����������
	CString	sSubCond;	// ������� ������� ����������
	CString	sSubMap;	// ����� ����� ����������
	CString	sSubCmd;	// ������� ����������
} TRN_INFO;

// ����������� ������� �����������
typedef CArray<TRN_INFO,TRN_INFO&> CTrnArray;

// �������� ����
typedef struct _FLD_INFO {
	UINT		iSQLType;	// SQL-���
	int			iSize;		// ������
	int			iPrec;		// ��������
	int			fNull;		// Nullable
	int			iFlag;		// ����
	int			iSrc;		// ��������
	int			iRef;		// ������
	int			iDst;		// ��������
	int			iIdx;		// ������
	BOOL		bMod;		// ������� ���������
	CString		sID;		// �������������
	CString		sName;		// ������������
	CString		sType;		// �������� ����
	CString		sValue;		// ��������
	CDataValue	vData;		// ������
} FLD_INFO;

// ����������� ������� �����
typedef CArray<FLD_INFO,FLD_INFO&> CFldArray;

// ����������� 2-������� ������� �����
typedef CTypedPtrArray<CPtrArray, CFldArray*> CArFldArray;

// �������� ������ ������
typedef struct _ROW_INFO {
	long		nRow;		// ����� ������
	ROW_STATUS	nStatus;	// ������ ������
	BOOL		bRunCmd;	// ������� SQL-�������
	BOOL		bEvChanged;	// ������� OnChange
	CFldArray*	pArSrcFld;	// ������ ���������
	CFldArray*	pArDstFld;	// ������ ���������
} ROW_INFO;

// ����������� ������ ����� ������
typedef CArray<ROW_INFO,ROW_INFO&> CBufArray;

// �������� �������
typedef struct _CMD_INFO {
	EV_TYPE			iEvent;	// ID �������
	CString			sEvent;	// ������������
	CString			sField;	// ���� ��
	CString			sCmd;	// ������ ������
	int				nCount;	// ����� ���������
	CStringArray*	pArCmd;	// ������ ���������
	CArFldArray*	pArPrm;	// ������ ����������
	SQLHSTMT		hstmt;	// SQL HSTMT
} CMD_INFO;

// ����������� ������� ������
typedef CArray<CMD_INFO,CMD_INFO&> CCmdArray;

// ���������� � �������
typedef struct _PPL_INFO {
	CString		sParm;	// ���������
	CString		sSrc;	// ��������
	CString		sDst;	// ��������
	CTrnArray	arTrn;	// ������ �����������
} PPL_INFO;

// ���������� � ���� ������ ODBC
typedef struct _TYPE_INFO {
	CString	sTypeName;	// SQL Type Name
	CString	sLTypeName;	// Local Type Name
	CString	sLitPfx;	// Literal Prefix
	CString	sLitSfx;	// Literal Suffix
	CString	sCrtParams;	// Create Params
	UINT	fSearchable;// Searchable Flag
	UINT	iType;		// SQL Data Type
	UINT	iMinScale;	// Minimum Scale
	UINT	iMaxScale;	// Maximim Scale
	ULONG	lPrec;		// Precision
	BOOL	bNullable;	// Is Nullable
	BOOL	bCaseSens;	// Is Case Sensitive
	BOOL	bUnsigned;	// Is Unsigned
	BOOL	bFxPrcScale;// Is Fixed Prec Scale
	BOOL	bAutoInc;	// Is Auto Increment
} TYPE_INFO;

// ����������� ������� ����� ������ ODBC
typedef CArray<TYPE_INFO,TYPE_INFO&> CTypeArray;

// ���������� �� ��������� ODBC
typedef struct _ODBC_INFO {
	// Connection Handle
	SQLHDBC	hdbc;			// Connection Handle
	CString	sUserName;		// User Name
	// Driver Information
	CString	sDbmsName;		// DBMS Name
	CString	sDbmsVer;		// DBMS Verision
	CString	sDrvName;		// Driver Name
	CString	sDrvVer;		// Driver Version
	CString	sOdbcVer;		// ODBC Version
	CString	sDrvOdbcVer;	// Driver ODBC Version
	UWORD	iApiConf;		// ODBC API Conformance
	UWORD	iSqlConf;		// ODBC SQL Conformance
	BOOL	bSqlOptIef;		// ODBC SQL Opt IEF
	UWORD	iSagCliConf;	// ODBC Sag Cli Conformance
	UWORD	iActConns;		// Active Connections
	UWORD	iActStmts;		// Active Statements
	UWORD	iFileUsage;		// File Usage
	BOOL	bMultSets;		// Mult Result Sets
	BOOL	bReadOnly;		// Data Source Read Only
	BOOL	bAcsProc;		// Accessible Procedures
	BOOL	bAcsTabl;		// Accessible Tables
	// Limits
	UDWORD	lMaxBinLitLen;	// Max Binary Literal Length
	UDWORD	lMaxChrLitLen;	// Max Char Literal Length
	UWORD	iMaxColNameLen;	// Max Column Name Length
	UWORD	iMaxColInGrpBy;	// Max Columns In Group By
	UWORD	iMaxColInIndex;	// Max Columns In Index
	UWORD	iMaxColInOrdBy;	// Max Columns In Order By
	UWORD	iMaxColInSelect;// Max Columns In Select
	UWORD	iMaxColInTable;	// Max Columns In Table
	UWORD	iMaxCurNameLen;	// Max Cursor Name Length
	UDWORD	lMaxIndexSize;	// Max Index Size
	UWORD	iMaxOwnNameLen;	// Max Owner Name Length
	UWORD	iMaxPrcNameLen;	// Max Procedure Name Length
	UWORD	iMaxQlfNameLen;	// Max Qualifier Name Length
	UDWORD	lMaxRowSize;	// Max Row Size
	BOOL	bMaxRowSzIncL;	// Max Row Size Includes Long
	UDWORD	lMaxStmtLen;	// Max Statement Length
	UWORD	iMaxTabNameLen;	// Max Table Name Length
	UWORD	iMaxTabInSelect;// Max Tables In Select
	UWORD	iMaxUsrNameLen;	// Max User Name Length
	// Supported SQL
	BOOL	bAlterAddCol;	// Alter Table Add Column
	BOOL	bAlterDropCol;	// Alter Table Drop Column
	BOOL	bColAlias;		// Column Alias
	UWORD	iCorrName;		// Correlation Name
	BOOL	bDescParam;		// Describe Parameter
	BOOL	bProcColumns;	// Procedure Columns
	BOOL	bExpInOrdBy;	// Expressions In Order By
	UWORD	iGroupBy;		// Group By
	BOOL	bLikeEsc;		// Like Escape Clause
	BOOL	bOrdByInSel;	// Order By Columns In Select
	CString	sOuterJoins;	// Outer Joins
	BOOL	bPsPosDelete;	// Positioned Delete
	BOOL	bPsPosUpdate;	// Positioned Update
	BOOL	bPsSelForUpdate;// Positioned Select For Update
	BOOL	bSqCorrSubQry;	// SQ Correlated Subqueries
	BOOL	bSqComparison;	// SQ Comparison
	BOOL	bSqExists;		// SQ Exists
	BOOL	bSqIn;			// SQ In
	BOOL	blSqQuantif;	// SQ Quantified
	BOOL	bUUnion;		// Union
	BOOL	bUUnionAll;		// Union All
	// Miscellaneous
	UWORD	iTxnCapable;	// Txn Capable
	UWORD	iCurComBhvr;	// Cursor Commit Behavior
	UWORD	iCurRlbBhvr;	// Cursor Rollback Behavior
	BOOL	bTxnReadComm;	// Txn Read Commited
	BOOL	bTxnReadUncom;	// Txn Read Uncommited
	BOOL	bTxnRepRead;	// Txn Repeatable Read
	BOOL	bTxnSerial;		// Txn Serializable
	UDWORD	lDefTxnIsol;	// Default Txn Isolation
	BOOL	bMultActTxn;	// Multiple Active Txn
	BOOL	bBpClose;		// Bookmark Persistence Close
	BOOL	bBpDelete;		// Bookmark Persistence Delete
	BOOL	bBpDrop;		// Bookmark Persistence Drop
	BOOL	bBpScroll;		// Bookmark Persistence Scroll
	BOOL	bBpTrans;		// Bookmark Persistence Transaction
	BOOL	bBpUpdate;		// Bookmark Persistence Update
	BOOL	bBpOtherHstmt;	// Bookmark Persistence Other HSTMT
	BOOL	bLckNoChange;	// Lock No Change
	BOOL	bLckExclusive;	// Lock Exclusive
	BOOL	bLckUnlock;		// Lock Unlock
	BOOL	bSsAdditions;	// Static Sensitivity Additions
	BOOL	bSsDeletions;	// Static Sensitivity Deletions
	BOOL	bSsUpdates;		// Static Sensitivity Updates
	// Others
	BOOL	bNeedLDataLen;	// Need Long Data Length
	UWORD	iNonNullCols;	// Non Nullable Columns
	UWORD	iConcNullBhvr;	// Concat Null Behavior
	UWORD	iNullCollat;	// Null Collation
	BOOL	bProcedures;	// Procedured
	BOOL	bRowUpdates;	// Row Updates
	BOOL	bFdFetchNext;	// Fetch Direction Next
	BOOL	bFdFetchFirst;	// Fetch Direction First
	BOOL	bFdFetchLast;	// Fetch Direction Last
	BOOL	bFdFetchPrior;	// Fetch Direction Prior
	BOOL	bFdFetchAbs;	// Fetch Direction Absolute
	BOOL	bFdFetchRltv;	// Fetch Direction Relative
	BOOL	bFdFetchBmrk;	// Fetch Direction Bookmark
	BOOL	bGdAnyColumn;	// Get Data Any Column
	BOOL	bGdAnyOrder;	// Get Data Any Order
	BOOL	bGdBlock;		// Get Data Block
	BOOL	bGdBound;		// Get Data Bound
	BOOL	bPosPosition;	// Positioned Position
	BOOL	bPosRefresh;	// Positioned Refresh
	BOOL	bPosUpdate;		// Positioned Update
	BOOL	bPosDelete;		// Positioned Delete
	BOOL	bPosAdd;		// Positioned Add
	BOOL	bSccoReadOnly;	// Scroll Concurrency Read Only
	BOOL	bSccoLock;		// Scroll Concurrency Lock
	BOOL	bSccoOptRow;	// Scroll Concurrency Opt Rowver
	BOOL	bSccoOptVal;	// Scroll Concurrency Opt Values
	BOOL	bSoForwOnly;	// Scroll Options Forward Only
	BOOL	bSoStatic;		// Scroll Options Static
	BOOL	bSoKeyDriven;	// Scroll Options Keyset Driven
	BOOL	bSoDynamic;		// Scroll Options Dynamic
	BOOL	bSoMixed;		// Scroll Options Mixed
	BOOL	bOuDmlStmts;	// Owner Usage DML Statements
	BOOL	bOuProcInv;		// Owner Usage Procedure Invocation
	BOOL	bOuTableDef;	// Owner Usage Table Definition
	BOOL	bOuIndexDef;	// Owner Usage Index Definition
	BOOL	bOuPrivDef;		// Owner Usage Privilege Definition
	BOOL	bQuDmlStmts;	// Qualifier Usage DML Statements
	BOOL	bQuProcInv;		// Qualifier Usage Procedure Invocation
	BOOL	bQuTableDef;	// Qualifier Usage Table Definition
	BOOL	bQuIndexDef;	// Qualifier Usage Index Definition
	BOOL	bQuPrivDef;		// Qualifier Usage Privilege Definition
	// Type Array
	CTypeArray	arType;		// Array of Data Types
} ODBC_INFO;


/////////////////////////////////////////////////////////////////////
// ���������� ���������� ����������
//

extern HWND				hwndPopup;	// HWND Popup Window
extern HINSTANCE        hLangDLL;	// HINSTANCE Language DLL
extern CStdioFile       fileLog;	// ���� ���������
extern CStdioFile       fileMsg;	// ���� ���������
extern CString          sFileLog;	// ������������ ����� ���������
extern CString          sFileMsg;	// ������������ ����� ���������
extern CString          sLicense;	// ���������� ����� ��������
extern BOOL             bFileLog;	// ������� ����� ���������
extern BOOL             bFileMsg;	// ������� ����� ���������
extern BOOL             bExchange;	// ������� ������� ������
extern BOOL             bImport;	// ������� �������
extern BOOL             bExport;	// ������� ��������
extern BOOL             bCancel;	// ������� ��������
extern CMenu            menuMDI;	// ���� ��� ���� MDI ����
extern TRN_PARM         TrnParm;	// ��������� ����������
extern CArray<PPL,PPL&> arPPL;		// ������ �������� ������

////////////////////////////////////////////////////////////////////
// �������� � ��������� Tray

BOOL TrayMessage(HWND hWnd, DWORD dwMessage, UINT uID, HICON hIcon, LPCTSTR szTip);

////////////////////////////////////////////////////////////////////
// �������� ����� ���������

BOOL OpenFileLog();

////////////////////////////////////////////////////////////////////
// �������� ����� ���������

BOOL CloseFileLog();

/////////////////////////////////////////////////////////////////////
// ��������� ������� ������������� ������ (Timer #1)

void CALLBACK TimerExchange(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

/////////////////////////////////////////////////////////////////////
// ��������� ������� ������ ����� ������ (Timer #2)

void CALLBACK TimerInterrogate(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

/////////////////////////////////////////////////////////////////////
// ������ ������ �������

void StartExchange();

/////////////////////////////////////////////////////////////////////
// ������� ������ �������

void StopExchange();

/////////////////////////////////////////////////////////////////////
// ������ ������ �� �������� ������

BOOL ImportDataExchange(HWND hWnd);

/////////////////////////////////////////////////////////////////////
// ������� ������ � ������� ������

BOOL ExportDataExchange(HWND hWnd);

/////////////////////////////////////////////////////////////////////
// ������������� ��������� ����������

BOOL InitOptions(void);

/////////////////////////////////////////////////////////////////////
// ������ ��������� ����������

BOOL WriteOptions(void);

/////////////////////////////////////////////////////////////////////
// ������ �� �����

BOOL FileGet(LPCTSTR szName, CString &strData);

/////////////////////////////////////////////////////////////////////
// ������ � ����

BOOL FilePut(LPCTSTR szName, CString strData);

/////////////////////////////////////////////////////////////////////
// ����� MDI ���� �� ���������

CMDIChildWnd* FindMDIChildWnd(CMDIFrameWnd *pFrameWnd, LPCTSTR szTitle);

/////////////////////////////////////////////////////////////////////
// ����� ������� ������� �� ����� �����

UINT FindPPL(LPCTSTR szFile);

/////////////////////////////////////////////////////////////////////
// �������� ����� ������ �� ��������������

BSTR GetErrMsg(ER_TYPE iErrType, LPCTSTR szErr = NULL);

/////////////////////////////////////////////////////////////////////
// �������� �� ��������� �����

BOOL GetKeyVal(LPCTSTR szText, LPCTSTR szKey, CString &strData);

/////////////////////////////////////////////////////////////////////
// �������� �� ��������� �����: Key=Value
// �����: GetKeyValue(input_string, "Key")

CString GetKeyValue(LPCTSTR szData, LPCTSTR szKey);

/////////////////////////////////////////////////////////////////////
// �������� ������ �� ��������� �����

BOOL GetKeyStr(LPCTSTR szText, LPCTSTR szKey, CString &strData);

/////////////////////////////////////////////////////////////////////
// �������� ������� � ��������� �������

int GetLex(LPCTSTR szText, int nStart, CString &strLex);

/////////////////////////////////////////////////////////////////////
// �������� ����� � ��������� �������

int GetWord(LPCTSTR szText, int nStart, CString &strWord);

/////////////////////////////////////////////////////////////////////
// ���������� ������� ����������

BOOL FillParmArray(LPCTSTR szText, CFldArray &arPrm);

/////////////////////////////////////////////////////////////////////
// ���������� ������� �����

BOOL FillFieldArray(LPCTSTR szText, CFldArray &arFld);

/////////////////////////////////////////////////////////////////////
// ���������� ������� ������

BOOL FillCmdArray(LPCTSTR szText, CCmdArray &arCmd);

/////////////////////////////////////////////////////////////////////
// ���������� ��������� �������� ����������

BOOL FillTrnInfo(LPCTSTR szText, TRN_INFO &trnInfo, CString &strErr);

/////////////////////////////////////////////////////////////////////
// ������ ��������� � ������ ������ �� '?'

BOOL ParseRefExp(CString &strExp, CString &strErr);

/////////////////////////////////////////////////////////////////////
// ������ ����������� �������

BOOL ReadPPL(LPCTSTR szFile, PPL_INFO &info);

/////////////////////////////////////////////////////////////////////
// ������ ����������� �������

BOOL WritePPL(LPCTSTR szFile, CString strName, PPL_INFO &info);

/////////////////////////////////////////////////////////////////////
// ������ �������� ������ � ������� �������

BOOL RunPPLSel(int nKind = -1);

/////////////////////////////////////////////////////////////////////
// ������ �������� ������

BOOL RunPipe(LPCTSTR szFile, LPCTSTR szTitle = NULL, 
			 int nKind = 0, BOOL bUseDlg = TRUE, 
			 LPCTSTR szParm = NULL);

/////////////////////////////////////////////////////////////////////
// SQL ������� - ���������

UDWORD SQLGetError(SQLHSTMT hstmt, CString &strErr);

CString SQLGetInfoString(SQLHDBC hdbc, UWORD fInfoType);

UWORD SQLGetInfoShort(SQLHDBC hdbc, UWORD fInfoType);

UDWORD SQLGetInfoLong(SQLHDBC hdbc, UWORD fInfoType);

BOOL SQLGetInfoBool(SQLHDBC hdbc, UWORD fInfoType);

int SQLGetTypeID(CString strType);

int SQLGetTypeIndex(ODBC_INFO &info, UINT iSQLType);

TYPE_INFO SQLGetTypeDef(ODBC_INFO &info, UINT iSQLType);

BOOL SQLConvertType(ODBC_INFO &dstInfo, UINT iSrcType, UINT &iDstType);

CString SQLGetDefStrVal(UINT iSQLType);

CString SQLGetTypeStr(UINT iSQLType);

/////////////////////////////////////////////////////////////////////
// ���������� ���������� �� ��������� ODBC

void SQLGetODBCInfo(SQLHDBC hdbc, ODBC_INFO& info);

/////////////////////////////////////////////////////////////////////
// ������ ���������� �������� ����� �� �������

BOOL QueryRegSzValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, CString &sData, LPCTSTR szDefault = NULL);

/////////////////////////////////////////////////////////////////////
// ������ ��������� �������� ����� �� �������

BOOL QueryRegDwValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, UINT &nData, UINT nDefault = 0);

/////////////////////////////////////////////////////////////////////
// ������ ���������� �������� ����� � ������

BOOL WriteRegSzValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, LPCTSTR szData);

/////////////////////////////////////////////////////////////////////
// ������ ��������� �������� ����� � ������

BOOL WriteRegDwValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, UINT nData);

/////////////////////////////////////////////////////////////////////
//
// �������-��������� �� ������ ���������
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// ��������� ������� �������� �����

BOOL _getFileCreateDT(LPCTSTR lpFileName, FILETIME &time);

/////////////////////////////////////////////////////////////////////
// ��������� ���������� ������� ������� � �����

BOOL _getFileAccessDT(LPCTSTR lpFileName, FILETIME &time);

/////////////////////////////////////////////////////////////////////
// ��������� �������� ���������� ������� "������" �� ����� �������

BOOL _getCurrentSysDT(FILETIME &time);

/////////////////////////////////////////////////////////////////////
// CTrans30App:
// See Trans30.cpp for the implementation of this class
//

class CTrans30App : public CWinApp
{
public:
	CTrans30App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrans30App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	//{{AFX_MSG(CTrans30App)
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// CPadDoc document

class CPadDoc : public CRichEditDoc
{
protected:
	CPadDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPadDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPadDoc)
	public:
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPadDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPadDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////
// CPadView view

class CPadView : public CRichEditView
{
protected:
	CPadView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPadView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPadView)
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPadView();

	// Generated message map functions
protected:
	CRect m_rectMargin;
	int   m_nUnits;
	BOOL SetDefaultMargins();
	BOOL SetDefaultFont();
	virtual void DeleteContents();
	//{{AFX_MSG(CPadView)
	afx_msg void OnFilePrint();
	afx_msg void OnPageSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	inline int roundleast(int n)
	{
		int mod = n%10;
		n -= mod;
		if (mod >= 5)
			n += 10;
		else if (mod <= -5)
			n -= 10;
		return n;
	}
	void RoundRect(LPRECT r1);
	void MulDivRect(LPRECT r1, LPRECT r2, int num, int div);
};

/////////////////////////////////////////////////////////////////////
// CPadFrame frame

class CPadFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPadFrame)
protected:
	CPadFrame();           // protected constructor used by dynamic creation

protected:
	static CMenu menu;

	// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPadFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPadFrame();

	// Generated message map functions
	//{{AFX_MSG(CPadFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDataClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_TRANS30_H)
