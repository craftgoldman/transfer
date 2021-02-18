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
	// ����������� ������������ SQL
	FreeAllStmt();
	// ������� ������ � ��������
	ClearAllArr();
	// �������� �������
	if (pDlg != NULL) delete pDlg;
}

/////////////////////////////////////////////////////////////////////
// ��������� ������ ����������

void CTransfer::ErrorHandle(ER_TYPE errType, CString &strErr, UINT &nErr)
{
	// ���������:
	// errType - ��� ������
	// strErr  - ������ ������
	// nErr    - ������� ������

	// ��������� ������� ������
	nErr++;
	// �������� ������ ����� ������
	strErr = GetErrMsg(errType, strErr);
	// �������������� ������ ������
	strErr = FormatErr(strErr);
	// ������ ������ ������ � ����
	if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
	// ����� ��������� �� ������
	if (bShowErr) ShowErr(pTrn->sName, strErr);
}

/////////////////////////////////////////////////////////////////////
// ��������� �������������� ����������

void CTransfer::WarningHandle(CString &strErr, UINT &nMsg)
{
	// ���������:
	// strErr  - ������ ��������������
	// nErr    - ������� ��������������

	// ��������� ������� ��������������
	nMsg++;
	if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
}

/////////////////////////////////////////////////////////////////////
// ������������ ���� ������������ SQL

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
// ������� ������ �� ���� ��������

void CTransfer::ClearAllArr() 
{
	// ������� �������� ����� � ����������
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
	// ������� ������� �����
	ClearBufArr(arTabRowBuf);
	ClearBufArr(arSubRowBuf);
	// ������� �������� ������
	ClearCmdArr(arDstTabCmd);
	ClearCmdArr(arDstSubCmd);
}

/////////////////////////////////////////////////////////////////////
// ������� ������� ����� (����������)

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
// ������� ������ �����

void CTransfer::ClearBufArr(CBufArray &arBuf) 
{
	if (arBuf.GetSize() == 0) return;

	for (int i = arBuf.GetSize() - 1; i >= 0; i--)
	{
		if (arBuf[i].pArSrcFld != NULL)
		{
			// �������� ������ ���������
			ClearFldArr(*arBuf[i].pArSrcFld);
			delete arBuf[i].pArSrcFld;
			arBuf[i].pArSrcFld = NULL;
		}
		if (arBuf[i].pArDstFld != NULL)
		{
			// �������� ������ ���������
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
// ������� ������� ������ (� �� ����������)

void CTransfer::ClearCmdArr(CCmdArray &arCmd) 
{
	if (arCmd.GetSize() == 0) return;

	for (int i = arCmd.GetSize() - 1; i >= 0; i--)
	{
		// ����������� ���������� SQL
		if (arCmd[i].hstmt != SQL_NULL_HSTMT)
		{
			::SQLFreeStmt(arCmd[i].hstmt, SQL_CLOSE);
			::SQLFreeStmt(arCmd[i].hstmt, SQL_UNBIND);
			::SQLFreeStmt(arCmd[i].hstmt, SQL_RESET_PARAMS);
			::SQLFreeStmt(arCmd[i].hstmt, SQL_DROP);
			arCmd[i].hstmt = SQL_NULL_HSTMT;
		}
		// ����������� ������� ���������
		if (arCmd[i].pArCmd != NULL)
		{
			arCmd[i].pArCmd->RemoveAll();
			arCmd[i].pArCmd->FreeExtra();
			delete arCmd[i].pArCmd;
			arCmd[i].pArCmd = NULL;
		}
		// ����������� ������� ����������
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
// ���������� �������� �� ��������� ��� ��������� �������

void CTransfer::ResetFldArr(CFldArray &arFld) 
{
	for (int i = arFld.GetSize() - 1; i >= 0; i--)
	{
		arFld[i].vData.SetDefault();
	}
}

/////////////////////////////////////////////////////////////////////
// �������� ������ ������ � �����

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

	// ������� ������ ���������
	int n = arSrcFld.GetSize();
	rowInfo.pArSrcFld->SetSize(n);
	for (int i = 0; i < n; i++)
	{
		rowInfo.pArSrcFld->ElementAt(i).bMod  = arSrcFld[i].bMod;
		rowInfo.pArSrcFld->ElementAt(i).vData = arSrcFld[i].vData;
	}
	// ������� ������ ���������
	int m = arDstFld.GetSize();
	rowInfo.pArDstFld->SetSize(m);
	for (int j = 0; j < m; j++)
	{
		rowInfo.pArDstFld->ElementAt(j).bMod  = arDstFld[j].bMod;
		rowInfo.pArDstFld->ElementAt(j).vData = arDstFld[j].vData;
	}
	// ���������� ��������
	arBuf.Add(rowInfo);
}

/////////////////////////////////////////////////////////////////////
// ������������ ����������� SQL ���������

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
// ����� ��������� �� ������

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
// ����� ��������� (� ������������ ����������)

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
// ����� ��������� � ������� �� ID

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
// ����� ���� � ������� �� �����

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
// ����� ���� � ������� �� ������ �� ��������

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
// ����� ������� � ������� ������ �� ��������������

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
// ����� ������� � ������� �� �������������� � ����� ����

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
// ����������� �������� ������� SQL �������

BOOL CTransfer::TableInSQL(CString strSQL, CString &strTable, 
						   CString &strErr) 
{
	CString	tmp;
	// ��������� ������ �������
	int	i = GetLex(strSQL, 0, tmp);
	if (tmp == _T("select"))
	{
		// �������� ������� ����� SELECT
		i = GetLex(strSQL, i, strTable);
		// �������� �� ��������� <table.*>
		if (strSQL.Mid(i,  1) == _T(".") &&
			strSQL.Mid(i+1,1) == _T("*"))
		{
			// �������� ��������� �������
			GetLex(strSQL, i+2, tmp);
			if (tmp == _T(",")) goto err;
		}
		else
		{
			// �������� ��� ������� �� FROM
			if ((i = strSQL.Find(_T("from"))) >= 0)
			{
				// �������������� �������
				i = GetLex(strSQL, i+4, strTable);
				// �������� ��������� �������
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
// ��������� ������ � ������� ����� (����������) �� ������

BOOL CTransfer::CompareRef(CFldArray &arFld, OP_TYPE iOpType, 
						   MOD_FLAG &iMFlag, CString &strErr) 
{
	iMFlag = MOD_NO; // Not modified
	CDataValue dv;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// ������� ������������ �����
		if ((arFld[i].iFlag == FLD_NOCHANGE) &&
			(iOpType == OP_UPDATE)) continue;

		// ������������� ������ �� ���� ����
		dv.GetBuffer(arFld[i].iSQLType, arFld[i].iSize, TRUE);
		// ��������� ���������� ��������
		switch (arFld[i].iSrc)		// ��� ���������
		{
		case DATA_LOCAL:			// �������� ����
		{
			// ��������� �������� �� ������������
			break;
		}
		case DATA_AUTO:				// �������������
		{
			// ���������������� �������� �� ������������
			break;
		}
		case DATA_PARAM:			// ��������
		{
			if (!dv.ConvertInto((LPCTSTR)pPrm->GetAt(arFld[i].iRef).sValue))
			{
				strErr = GetErrMsg(ER_CONV_PARAM, 
									pPrm->GetAt(arFld[i].iRef).sID);
				return FALSE;
			}
			break;
		}
		case DATA_CONST:			// ���������
		{
			strErr = GetErrMsg(ER_CONV_CONST, arFld[i].sValue);
			return FALSE;
		}
		case DATA_TABSRC:			// �������� ������� ���������
		{
			if (!dv.ConvertInto(arSrcTabFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arSrcTabFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_SUBSRC:			// ����������� ������� ���������
		{
			if (!dv.ConvertInto(arSrcSubFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arSrcSubFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_TABDST:			// �������� ������� ���������
		{
			if (!dv.ConvertInto(arDstTabFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstTabFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_SUBDST:			// ����������� ������� ���������
		{
			if (!dv.ConvertInto(arDstSubFld.GetAt(arFld[i].iRef).vData))
			{
				strErr = GetErrMsg(ER_CONV_FIELD, 
								arDstSubFld.GetAt(arFld[i].iRef).sName);
				return FALSE;
			}
			break;
		}
		case DATA_FUNC:				// ���������� �������
		{
			strErr = GetErrMsg(ER_CONV_FUNC, arFld[i].sValue);
			return FALSE;
		}
		case DATA_PROC:				// ��������� ���������
		{
			strErr = GetErrMsg(ER_CONV_PROC, arFld[i].sValue);
			return FALSE;
		}
		}
		// ��������� �������� �����
		if (dv != arFld[i].vData)
		{
			// ��������� ����� ����������� ����
			arFld[i].bMod = TRUE;
			// ��������� ����� ����������� ������
			// ������ �� ����������� ����������
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
				// ���� ����������/������
				// ���� �������� ������������� � �����,
				// �� ��������� ����� �������� �� ����
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
// ��������� ������ �� �������� ��� �������� ����� �����

BOOL CTransfer::SetDstRefs(CFldArray &arFld, REF_TYPE iRefType, 
						   BOOL bKeysOnly,   CString &strErr) 
{
	// iRefType - ��� ������: 0 - �������, 1 - ����������
	int	i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		if (!bKeysOnly || arFld[i].iFlag == FLD_KEY)
		{
			switch (iRefType)
			{
			case REF_TAB:				// ���.������� ���������
				arFld[i].iSrc = DATA_TABDST;
				arFld[i].iRef = FindSrcRef(arDstTabFld, iRefType, 
											arFld[i].sName);
				if (arFld[i].iRef >= 0)	// ��������� ����� ����
					arFld[i].sValue = arDstTabFld.GetAt(
										arFld[i].iRef).sName;
				break;
			case REF_SUB:				// ���.������� ���������
				arFld[i].iSrc = DATA_SUBDST;
				arFld[i].iRef = FindSrcRef(arDstSubFld, iRefType, 
											arFld[i].sName);
				if (arFld[i].iRef >= 0)	// ��������� ����� ����
					arFld[i].sValue = arDstSubFld.GetAt(
										arFld[i].iRef).sName;
				break;
			default:					// ���.������� ���������
				arFld[i].iSrc = DATA_TABDST;
				arFld[i].iRef = FindSrcRef(arDstTabFld, iRefType, 
											arFld[i].sName);
				if (arFld[i].iRef >= 0)	// ��������� ����� ����
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
// ��������� ������ �� �������� ������ � ������� �����

BOOL CTransfer::SetFldRefs(CFldArray &arFld, REF_TYPE iRefType, 
						   CString &strErr) 
{
	// iRefType - ��� ������: 0 - �������, 1 - ����������
	int	i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		switch (arFld[i].iSrc)		// ��� ���������
		{
		case DATA_LOCAL:			// �������� ����
		{
			if (arFld[i].sValue.IsEmpty())
			{
				// ��������� �������� �� ���������
				arFld[i].vData.SetDefault();
			}
			else
			{
				// ��������� ���������� ��������
				if (!arFld[i].vData.ConvertInto((LPCTSTR)arFld[i].sValue))
				{
					strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
					return FALSE;
				}
			}
			break;
		}
		case DATA_AUTO:				// �������������
		{
			// ��������� �������� �� ���������
			arFld[i].vData.SetDefault();

			break;
		}
		case DATA_PARAM:			// ��������
		{
			arFld[i].iRef = FindParam(*pPrm, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_PARAM, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_CONST:			// ���������
		{
			strErr = GetErrMsg(ER_FIND_CONST, arFld[i].sValue);
			return FALSE;
		}
		case DATA_REF:				// ������
		{
			switch (iRefType)
			{
			case REF_TAB:			// ���.������� ���������
				arFld[i].iSrc = DATA_TABSRC;
				arFld[i].iRef = FindField(arSrcTabFld, arFld[i].sValue);
				break;
			case REF_SUB:			// ���.������� ���������
				arFld[i].iSrc = DATA_SUBSRC;
				arFld[i].iRef = FindField(arSrcSubFld, arFld[i].sValue);
				break;
			default:				// ���.������� ���������
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
		case DATA_TABSRC:			// ���.������� ���������
		{
			arFld[i].iRef = FindField(arSrcTabFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDSRCTAB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_SUBSRC:			// ���.������� ���������
		{
			arFld[i].iRef = FindField(arSrcSubFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDSRCSUB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_TABDST:			// ���.������� ���������
		{
			arFld[i].iRef = FindField(arDstTabFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDDSTTAB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_SUBDST:			// ���.������� ���������
		{
			arFld[i].iRef = FindField(arDstSubFld, arFld[i].sValue);
			if (arFld[i].iRef < 0)
			{
				strErr = GetErrMsg(ER_FIND_FLDDSTSUB, arFld[i].sValue);
				return FALSE;
			}
			break;
		}
		case DATA_FUNC:				// ���������� �������
		{
			// ����� ���������� �������
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
			// ��������� ������ ���������� �������
			if (n >= 0 && m >= 0)
				arFld[i].sValue = arFld[i].sValue.Mid(n + 1, m - n - 1);

			break;
		}
		case DATA_PROC:				// ��������� ���������
		{
			// ����� ��������� ���������
			strErr = GetErrMsg(ER_FIND_PROC, arFld[i].sValue);
			return FALSE;
		}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ��������� �������� ����� � �������
// �� ����������� ������� �������

BOOL CTransfer::SetFldKeys(ODBC_INFO *pODBC, CFldArray &arFld, 
						   CString strOwner, CString strTable, 
						   CString &strErr) 
{
	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// ���� �������������� Owner ��� ������
	if (pODBC->bOuTableDef && !strOwner.IsEmpty())
	{
		// ����������� ����������� �������
		// � ����� ��������� Owner
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
		// ����������� ����������� �������
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
	// ����������� ������������ ����� ������������ ����
	nmColName = pODBC->iMaxColNameLen + 1;
	// ��������� ������ ��� ������������ ����
	szColName = (TCHAR*)malloc((nmColName)*sizeof(TCHAR));
	// ���������� ����� �������
	::SQLBindCol(hstmt, 7, SQL_C_SHORT, &nDataType, 0, &cbDataType);
	::SQLBindCol(hstmt, 9, SQL_C_CHAR, szColName, nmColName, &cbColName);
	// ������ ��������� �����
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
// ��������� ������ � ������� ����� (����������) �� ������

BOOL CTransfer::SetFldData(CFldArray &arFld, OP_TYPE iOpType, 
						   CString &strErr) 
{
	BOOL bError = FALSE;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// ��� ������ ����� �������� ��������� �����
		if (bError)
		{
			arFld[i].vData.SetDefault();
			continue;
		}

		if (iOpType == OP_UPDATE)
		{
			// ������� ������������ � 
			// ���������������� �����
			if (arFld[i].iFlag == FLD_NOCHANGE || 
				arFld[i].iSrc  == DATA_AUTO)
				continue;
		}

		// ��������� ��������
		switch (arFld[i].iSrc)		// ��� ���������
		{
		case DATA_LOCAL:			// �������� ����
		{
			if (!arFld[i].vData.ConvertInto((LPCTSTR)arFld[i].sValue))
			{
				strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
				strErr = arFld[i].sName + _T(" - ") + strErr;
				bError = TRUE;
			}
			break;
		}
		case DATA_AUTO:				// �������������
		{
			CString sValue;
			
			// ��������� ������������� �������� ���� � ������� ��������� ���������
			if (ExecSQLMax(pDst, arFld[i], strErr) == SQL_ERROR)
			{
				bError = TRUE;
				continue;
			}

			// ���������� ��������
			double nMaxV, nAuto;
			nAuto = atof((LPCTSTR)arFld[i].sValue);
			nMaxV = atof((LPCTSTR)arFld[i].vData.GetString());
			nMaxV+= nAuto;
			sValue.Format("%f", nMaxV);
			// ��������� ��������
			if (!arFld[i].vData.ConvertInto((LPCTSTR)sValue))
			{
				strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
				strErr = arFld[i].sName + _T(" - ") + strErr;
				bError = TRUE;
			}
			break;
		}
		case DATA_PARAM:			// ��������
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
		case DATA_CONST:			// ���������
		{
			strErr = GetErrMsg(ER_CONV_CONST, arFld[i].sValue);
			bError = TRUE;
			break;
		}
		case DATA_TABSRC:			// �������� ������� ���������
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
		case DATA_SUBSRC:			// ����������� ������� ���������
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
		case DATA_TABDST:			// �������� ������� ���������
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
		case DATA_SUBDST:			// ����������� ������� ���������
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
		case DATA_FUNC:				// ���������� �������
		{
			CString sValue;
			
			// ���������� SQL ������� � ���������� ���������� � sValue
			if (ExecSQLFunc(pDst, (FNC_TYPE)arFld[i].iRef, arFld[i].sValue, sValue, strErr) == SQL_ERROR)
			{
				bError = TRUE;
				continue;
			}

			// ��������� ��������
			if (!arFld[i].vData.ConvertInto((LPCTSTR)sValue))
			{
				strErr = GetErrMsg(ER_BAD_DATA, arFld[i].sValue);
				strErr = arFld[i].sName + _T(" - ") + strErr;
				bError = TRUE;
			}
			break;
		}
		case DATA_PROC:				// ��������� ���������
		{
			strErr = GetErrMsg(ER_CONV_PROC, arFld[i].sValue);
			bError = TRUE;
		}
		}
	}
	return bError ? FALSE : TRUE;
}

/////////////////////////////////////////////////////////////////////
// ��������� ������ � ������� ����� (����������) �� ���������

BOOL CTransfer::SetDstData(CFldArray &arFld, CString &strErr) 
{
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// ��������� ��������
		switch (arFld[i].iDst)		// ��� ���������
		{
		case DATA_TABDST:			// �������� ������� ���������
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
		case DATA_SUBDST:			// ����������� ������� ���������
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
// ����������� ��������� �����

BOOL CTransfer::ConvertTxt(CFldArray &arFld, OP_TYPE iOpType,
						   UINT iConvert) 
{
	CString s;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// ������� ������������ �����
		if ((arFld[i].iFlag == FLD_NOCHANGE) &&
			(iOpType == OP_UPDATE)) continue;

		// ����������� ��������
		switch (arFld[i].iSrc)	// ��� ���������
		{
		case DATA_TABSRC:		// �������� ������� ���������
		case DATA_SUBSRC:		// ����������� ������� ���������
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
// ���������� ������� ������� SQL Select (��� ��������� ������ SQL)

BOOL CTransfer::ParseColumns(ODBC_INFO *pODBC, CFldArray &arFld, 
							 CString strSQL,   CString &strErr) 
{
	arFld.RemoveAll();
	// ������ ��������� � ������ ������ �� '?'
	// ��� �������� ������� ����������
	if (!ParseSQLParams(pODBC, NULL, strSQL, strErr))
	{
		strErr = GetErrMsg(ER_PARSE_PARAMS, strErr);
		return FALSE;
	}
	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// ���������� SQL ���������
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strSQL), 
		SQL_NTS) != SQL_SUCCESS)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		goto err;
	}
	// ����������� ����� �����
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
	// ����������� ������������ ����� ������������ ����
	nmColName = pODBC->iMaxColNameLen + 1;
	// ��������� ������ ��� ������������ ����
	szColName = (SQLCHAR*)malloc((nmColName)*sizeof(SQLCHAR));
	// ���������� �������� �������������� �����
	for (j = 0; j < nCols; j++)
	{
		// ���������� �������� ����
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
		// ������������� ������ ������
		arFld[j].vData.GetBuffer(arFld[j].iSQLType, 
								 arFld[j].iSize, TRUE);
	}
	// ����������� ����������� SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	// ������������ ������
	free(szColName);
	return TRUE;

err:
	// ����������� ����������� SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// ������ ���������� SQL-���������

BOOL CTransfer::ParseSQLParams(ODBC_INFO *pODBC, CFldArray *pArPrm, 
							   CString &strSQL,  CString &strMsg) 
{
	if (pODBC == NULL) return FALSE;
	if (strSQL.IsEmpty())
	{
		strMsg = GetErrMsg(ER_NO_EXPR);
		return FALSE;
	}
	// ������� ������� ����������
	if (pArPrm != NULL) pArPrm->RemoveAll();
	// �������� ���� �������� '?'
	strSQL.Remove('?');

	CString	s,sProcName,sRef,sType,sLen,sDec;
	int		i,n,l,iSrc,iRef,iSize,iPrec;
	UINT	iType;
	BOOL	bProc;

	// �������� �������
	i = GetLex(strSQL, 0, s);
	// �������� �������� {}
	if (s.Compare(_T("{")) == 0)
	{
		strSQL.TrimLeft('{');
		strSQL.TrimRight('}');
		// �������� ������� �����
		i = GetLex(strSQL, 0, s);
	}
	// ��������� �������� ���������
	if (s.CompareNoCase(_T("call")) == 0)
	{
		bProc = TRUE;
		// ��������� ����� ���������
		GetLex(strSQL, i, sProcName);
		// ��������� Escape-��������� ODBC
		strSQL = _T("{") + strSQL + _T("}");
	}
	else
	{
		bProc = FALSE;
	}
	// ������������� SQL-��������� �
	// ������������ ������� ����������
	for (i = 0, n = 0, l = 0;; i++)
	{
		// ����� �������� ������
		if ((n = strSQL.FindOneOf(_T("^:&$~#!@"))) < 0)
			break;
		// ��������� ��������
		s = strSQL.Mid(n, 1);
		// ����������� �������� ������
		GetWord(strSQL, n+1, sRef);
		// ������ ������ �� ���� ? � SQL ���������
		strSQL.Delete(n, sRef.GetLength()+1);
		strSQL.Insert(n, '?');
		// ��������� ���������
		if (s == _T("^"))		iSrc = DATA_LOCAL;
		else if (s == _T("+"))	iSrc = DATA_AUTO;
		else if (s == _T(":"))	iSrc = DATA_PARAM;
		else if (s == _T("&"))
		{
			iSrc = DATA_CONST;
			// ������ �� ��������� ���� ����������!
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
		// ��������� ������
		switch (iSrc)				// ��� ���������
		{
		case DATA_LOCAL:			// ��������
		{
			break;
		}
		case DATA_AUTO:				// �������������
		{
			break;
		}
		case DATA_PARAM:			// ��������
		{
			if ((iRef = FindParam(*pPrm, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_PARAM, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_TABSRC:			// ���.������� ���������
		{
			if ((iRef = FindField(arSrcTabFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDSRCTAB, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_SUBSRC:			// ���.������� ���������
		{
			if ((iRef = FindField(arSrcSubFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDSRCSUB, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_TABDST:			// ���.������� ���������
		{
			if ((iRef = FindField(arDstTabFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDDSTTAB, sRef);
				return FALSE;
			}
			break;
		}
		case DATA_SUBDST:			// ���.������� ���������
		{
			if ((iRef = FindField(arDstSubFld, sRef)) < 0)
			{
				strMsg = GetErrMsg(ER_FIND_FLDDSTSUB, sRef);
				return FALSE;
			}
			break;
		}
		}
		// ���������, �� ������ �� ���
		l = GetLex(strSQL, n + 1, s);
		if (s == _T("("))
		{
			// �������� ���������� ����
			l = GetLex(strSQL, l, sType);
			sType.MakeUpper();
			// �������� ��������� �������
			l = GetLex(strSQL, l, s);
			// ���� ������ ������
			if (s == _T(","))
			{
				// �������� ������ ����
				l = GetLex(strSQL, l, sLen);
				// �������� ��������� �������
				l = GetLex(strSQL, l, s);
				// ���� ������� ��������
				if (s == _T(","))
				{
					// �������� �������� ����
					l = GetLex(strSQL, l, sDec);
					// �������� ��������� �������
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
			// ��������� ���� ���������
			if ((iType = SQLGetTypeID(sType)) == 0)
			{
				strMsg = GetErrMsg(ER_BAD_TYPE, sType);
				return FALSE;
			}
			// ��������������� ���� ���������
			if (!SQLConvertType(*pODBC, iType, iType))
			{
				strMsg = GetErrMsg(ER_CONV_PARTYPE, sRef);
				return FALSE;
			}
			// ��������� ������� � ��������
			iSize = atoi(sLen);
			iPrec = atoi(sDec);
			// �������� ���������� � ����
			strSQL.Delete(n + 1, l - n - 1);
		}
		// ��� ���� �� ������, � �������
		// �������� ���� ��������� ����������
		else if (bProc && !pODBC->bProcColumns ||
				!bProc && !pODBC->bDescParam)
		{
			// ���� �� ��������� �������������
			// ���� �� ���������
			if (TrnParm.iDefType <= 0)
			{
				strMsg = GetErrMsg(ER_DEF_REFTYPE, sRef);
				return FALSE;
			}
			// ������� ��������� ���� �� ������
			switch (iSrc)				// ��� ���������
			{
			case DATA_PARAM:			// ��������
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
			case DATA_CONST:			// ���������
			{
				strMsg = GetErrMsg(ER_DEF_REFTYPE, sRef);
				return FALSE;
			}
			case DATA_TABSRC:			// ���.������� ���������
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
			case DATA_SUBSRC:			// ���.������� ���������
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
			case DATA_TABDST:			// ���.������� ���������
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
			case DATA_SUBDST:			// ���.������� ���������
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
		// ���������� ���������
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
	// �������� ����� ���������
	if (bProc)	strMsg = sProcName;
	else		strMsg.Empty();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ���������� SQL-��������� � ����������

BOOL CTransfer::PrepareSQLParms(ODBC_INFO *pODBC, CFldArray *pArPrm, 
								CString strOwner, CString &strSQL,   
								CString &strMsg)
{
	// ������ ���������� SQL-���������
	if (!ParseSQLParams(pODBC, pArPrm, strSQL, strMsg)) return FALSE;

	// ��������� ����� ���������
	CString	sProcName = strMsg;

	// ��������� �������� ���������
	BOOL bProc = (sProcName.IsEmpty())?FALSE:TRUE;

	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strMsg = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// ���������� SQL ���������
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strSQL), 
		SQL_NTS) != SQL_SUCCESS)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strMsg);
		goto err;
	}
	SQLSMALLINT	nDataType, nColDec, fNullable;
	SQLUINTEGER	nColSize;
	SQLINTEGER	cbDataType,cbColDec,cbNullable,cbColSize;

	if (bProc && pODBC->bProcColumns) // ���������� ���������� ���������
	{
		// ���� �������������� Owner ��� ��������
		if (pODBC->bOuProcInv && !strOwner.IsEmpty())
		{
			// ������ ������ ���������� ���������
			// � ����� ��������� strOwner
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
			// ������ ������ ���������� ���������
			if (::SQLProcedureColumns(hstmt, NULL, 0, NULL, 0, 
				(SQLTCHAR*)LPCTSTR(sProcName), SQL_NTS, 
				NULL, 0) != SQL_SUCCESS)
			{
				SQLGetError(hstmt, strMsg);
				strMsg = GetErrMsg(ER_DESC_PARAMS, strMsg);
				goto err;
			}
		}
		// ���������� ����� �������
		::SQLBindCol(hstmt, 6, SQL_C_SSHORT, &nDataType, 0, &cbDataType);
		::SQLBindCol(hstmt, 8, SQL_C_SLONG,  &nColSize, 0, &cbColSize);
		::SQLBindCol(hstmt, 10, SQL_C_SSHORT, &nColDec, 0, &cbColDec);
		::SQLBindCol(hstmt, 12, SQL_C_SSHORT, &fNullable, 0, &cbNullable);
		// ��������� �������� ���������� � �������
		for (int j = 0;; j++)
		{
			// ���������� �������� ���������
			if (SQLFetch(hstmt) != SQL_SUCCESS) break;
			if (j >= pArPrm->GetSize())
			{
				::SQLFreeStmt(hstmt, SQL_CLOSE);
				::SQLFreeStmt(hstmt, SQL_UNBIND);
				strMsg = GetErrMsg(ER_UNS_PARAMS);
				goto err;
			}
			// ��������� �������� ���������
			pArPrm->ElementAt(j).iSQLType = nDataType;
			pArPrm->ElementAt(j).iSize    = nColSize;
			pArPrm->ElementAt(j).iPrec    = nColDec;
			pArPrm->ElementAt(j).fNull    = fNullable;
			// ������������ ���� ������
			pArPrm->ElementAt(j).sType = SQLGetTypeDef(*pODBC, 
											nDataType).sTypeName;
			// ������������� ������ ������
			pArPrm->ElementAt(j).vData.GetBuffer(nDataType,nColSize,TRUE);
		}
		::SQLFreeStmt(hstmt, SQL_CLOSE);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
	}
	else			// ���������� �������� ���������� SQL
	{
		// ����������� ����� ����������
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
			// ���� �������������� ������� �������� ����������
			if (!bProc && pODBC->bDescParam)
			{
				// ��������� ������ ����� ������� �������� ���������
				if (::SQLDescribeParam(hstmt, k + 1, &nDataType, 
					&nColSize, &nColDec, &fNullable) != SQL_SUCCESS)
				{
					SQLGetError(hstmt, strMsg);
					strMsg = GetErrMsg(ER_DESC_PARAMS, strMsg);
					goto err;
				}
				// ��������� �������� ���������
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
			// ������������ ���� ������
			pArPrm->ElementAt(k).sType = SQLGetTypeDef(*pODBC, 
											nDataType).sTypeName;
			// ������������� ������ ������
			pArPrm->ElementAt(k).vData.GetBuffer(nDataType,nColSize,TRUE);
		}
	}
	// ����������� ����������� SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return TRUE;

err:
	// ����������� ����������� SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// �������� ������� ��������� � ����������� �������

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

	// ������������ ������� ����� �������
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
			// ������������ ����� �� ���������
			arFld[i].sName	= arSrc[i].sName;
			arFld[i].iSize	= arSrc[i].iSize;
			arFld[i].iPrec	= arSrc[i].iPrec;
			arFld[i].fNull	= arSrc[i].fNull;
			arFld[i].iFlag	= arSrc[i].iFlag;
			arFld[i].iSrc	= DATA_REF;
			arFld[i].sValue	= arSrc[i].sName;
			// ��������� ���� ������ ����
			if (!SQLConvertType(*pODBC, arSrc[i].iSQLType, 
								arFld[i].iSQLType))
			{
				strErr = GetErrMsg(ER_CONV_FLDTYPE, arSrc[i].sName);
				return FALSE;
			}
		}
		else
		{
			// ������������ ����� �� �����
			arFld[i].sName    = arMap[i].sName;
			arFld[i].iSize    = arMap[i].iSize;
			arFld[i].iPrec    = arMap[i].iPrec;
			arFld[i].fNull    = arMap[i].fNull;
			arFld[i].iFlag    = arMap[i].iFlag;
			arFld[i].iSrc     = arMap[i].iSrc;
			arFld[i].sValue   = arMap[i].sValue;
			// ��������� ���� ������ ����
			if (!SQLConvertType(*pODBC, arMap[i].iSQLType, 
								arFld[i].iSQLType))
			{
				strErr = GetErrMsg(ER_CONV_FLDTYPE, arMap[i].sName);
				return FALSE;
			}
		}
		// ������� ��������� ����
		if (arFld[i].iFlag == FLD_KEY) bKeyFound = TRUE;
		// �������� ���� ������ �� ��������� ODBC
		arFld[i].sType = SQLGetTypeDef(*pODBC, 
						arFld[i].iSQLType).sTypeName;
		// ������������� ������ ������
		arFld[i].vData.GetBuffer(arFld[i].iSQLType, 
								 arFld[i].iSize, TRUE);
	}
	if (!bKeyFound)
	{
		strErr = GetErrMsg(ER_NO_KEYS);
		return FALSE;
	}
	// ��� ����������� ����� ���������
	// ��������� ������� ����� �����
	if (bUseSrc)
	{
		n = arMap.GetSize();
		for (i = 0; i < n; i++)
		{
			// ���� ��������������� ����
			if (arMap[i].iSrc == DATA_REF)
			{
				// ����� ���� ��������� (� ���������) �� ������
				if ((j = FindField(arFld, arMap[i].sValue)) < 0)
				{
					strErr = GetErrMsg(ER_FIND_FLDSRCTAB, arMap[i].sValue);
					return FALSE;
				}
				// ���� �������� ����
				if (arFld[j].iFlag == FLD_KEY)
				{
					strErr = GetErrMsg(ER_REPL_FLDKEY, arFld[j].sName);
					return FALSE;
				}
				// ����� ����������� ����� � ���������
				if (FindField(arFld, arMap[i].sName) >= 0)
				{
					strErr = GetErrMsg(ER_REPL_FLDDEF, arMap[i].sName);
					return FALSE;
				}
				// ��������� ����� ���� ���������
				arFld[j].sName	= arMap[i].sName;
			}
			else
			{
				// ����� ���� ��������� �� ������������
				if ((j = FindField(arFld, arMap[i].sName)) < 0)
				{
					strErr = GetErrMsg(ER_FIND_FLDDSTTAB, arMap[i].sValue);
					return FALSE;
				}
				// ���� �������� ����
				if (arFld[j].iFlag == FLD_KEY)
				{
					strErr = GetErrMsg(ER_MOD_FLDKEY, arFld[j].sName);
					return FALSE;
				}
				// ���� ������ ������ �� ��������
				if (arMap[i].iSrc == DATA_LOCAL &&
					arMap[i].sValue.IsEmpty())
				{
					// ��������� ������ ����� ����
					// ��������� ������ ��� ���������
					arMap[i].iSrc	= arFld[j].iSrc;
					arMap[i].sValue	= arFld[j].sValue;
				}
			}
			// ��������� ����� ���� ���������
			arFld[j].iFlag	= arMap[i].iFlag;
			// ��������� ������
			arFld[j].iSrc	= arMap[i].iSrc;
			arFld[j].sValue	= arMap[i].sValue;
		}
	}

	// ������������ ������ ��� ��������
	// ������� � ����������� �������
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
		// ������������ ������� �������� �������
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

		// ������������ ������� �������� �������
		if (arFld[i].iFlag == FLD_KEY)
			sIdx += arFld[i].sName + _T(",");
	}
	sIdx = sIdx.Left(sIdx.GetLength()-1) + _T(")");
	sTab = sTab.Left(sTab.GetLength()-1) + _T(")");

	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// ����������� �������
	::SQLExecDirect(hstmt, (SQLTCHAR *)LPCTSTR(sDrop), SQL_NTS);
	// �������� �������
	if (::SQLExecDirect(hstmt, (SQLTCHAR *)LPCTSTR(sTab), 
		SQL_NTS) != SQL_SUCCESS)
	{
		SQLGetError(hstmt, strErr);
		::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		strErr = GetErrMsg(ER_CRT_TABLE, strErr);
		return FALSE;
	}
	// �������� ����������� �������
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
// ���������� ������� ���������

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
	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return FALSE;
	}
	// ���� �������������� Owner ��� ������
	if (pODBC->bOuTableDef && !strOwner.IsEmpty())
	{
		// �������� ������������� �������
		// � ����� ��������� Owner
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
		// �������� ������������� �������
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
	// ���� �������������� Owner ��� ������
	if (pODBC->bOuTableDef && !strOwner.IsEmpty())
	{
		// ������ �������� ����� �������
		// � ����� ��������� Owner
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
		// ������ �������� ����� �������
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
	// ����������� ������������ ����� ������������ ����
	nmColName = pODBC->iMaxColNameLen + 1;
	// ��������� ������ ��� ������������ ����
	szColName = (TCHAR*)malloc((nmColName)*sizeof(TCHAR));
	// ���������� ����� �������
	::SQLBindCol(hstmt, 4, SQL_C_CHAR, szColName, nmColName, &cbColName);
	::SQLBindCol(hstmt, 5, SQL_C_SSHORT, &nDataType, 0, &cbDataType);
	::SQLBindCol(hstmt, 6, SQL_C_CHAR, szTypeName, 100, &cbTypeName);
	::SQLBindCol(hstmt, 7, SQL_C_SLONG, &nColSize, 0, &cbColSize);
	::SQLBindCol(hstmt, 8, SQL_C_SLONG, &nColDec, 0, &cbColDec);
	::SQLBindCol(hstmt, 11, SQL_C_SSHORT, &fNullable, 0, &cbNullable);
	// ��������� �������� ����� � �������
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
		// ������������� ������ ������
		arFld[i].vData.GetBuffer(arFld[i].iSQLType, 
								 arFld[i].iSize, TRUE);
	}
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	free(szColName);

	// ��������� �������� ����� � �������
	if (!SetFldKeys(pODBC, arFld, strOwner, strTable, strErr))
	{
		strErr = GetErrMsg(ER_DEF_KEYS, strErr);
		return FALSE;
	}
	// ��������� ������� ����� ���������
	if ((n = arFld.GetSize()) == 0)
	{
		strErr = GetErrMsg(ER_DEF_FIELDS);
		return FALSE;
	}
	// ��������� ������ �� �������� ������
	// ���� �� ����� ������� ���������
	for (i = 0; i < n; i++)
	{
		// ����� ���� � �������� ����� �����
		// ���� ����������� �������� � ����� �����
		if ((j = FindField(arMap, arFld[i].sName)) < 0)
		{
			// ��� ����������� ����� ���������
			if (iSchema == TRN_COPY || 
				iSchema == TRN_COMPLEX)
			{
				// ��������� ������ �� ������� ���������
				arFld[i].iSrc	= DATA_REF;
				arFld[i].sValue = arFld[i].sName;
			}
			else
			{
				strErr = GetErrMsg(ER_FIND_FLDMAP, arFld[i].sName);
				return FALSE;
			}
		}
		// ���� ������� �������� � ����� �����
		else
		{
			// ��� ����������� ����� ���������
			if (iSchema == TRN_COPY || 
				iSchema == TRN_COMPLEX)
			{
				// ���� �������� ����
				if (arFld[i].iFlag == FLD_KEY)
				{
					strErr = GetErrMsg(ER_MOD_FLDKEY, arFld[i].sName);
					return FALSE;
				}
				// ���� ������ ������ �� ��������
				if (arMap[j].iSrc == DATA_LOCAL && 
					arMap[j].sValue.IsEmpty())
				{
					// ��������� ������ ����� ����
					// ��������� ������ �� ��������
					arMap[j].iSrc	= DATA_REF;
					arMap[j].sValue	= arFld[i].sName;
				}
			}
			// ��������� ����� ����
			arFld[i].iFlag	= arMap[j].iFlag;
			// ��������� ������
			arFld[i].iSrc	= arMap[j].iSrc;
			arFld[i].sValue	= arMap[j].sValue;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ���������� �������� ������ CHANGE � ������� �����

BOOL CTransfer::ParseFldCmd(CFldArray &arFld, CCmdArray &arCmd, 
							CString &strErr) 
{
	for (int i = 0; i < arFld.GetSize(); i++)
	{
		if (FindSQLCmd(arCmd, EV_ON_CHANGE, arFld[i].sName) >= 0)
		{
			// �������� ����� ����
			if (arFld[i].iFlag == FLD_DATA)	// �������� �������
			{
				arFld[i].iFlag = FLD_EVCHANGE;
			}
			else							// ���������� ��������
			{
				strErr = GetErrMsg(ER_BAD_FLDCMD, arFld[i].sName);
				return FALSE;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ���������� ������� ������

BOOL CTransfer::ParseCmdArr(ODBC_INFO *pODBC, REF_TYPE iRefType, 
							CCmdArray &arCmd, BOOL bMakeParams, 
							CString &strErr) 
{
	CString strCmd, strEvt;
	for (int i = 0, nLen = 0; i < arCmd.GetSize(); i++)
	{
		// ��������� ������������ �������
		strEvt = arCmd[i].sEvent;
		if (arCmd[i].iEvent == EV_ON_CHANGE &&
			!arCmd[i].sField.IsEmpty())
		{
			strEvt += _T(".") + arCmd[i].sField;
		}
		// �������� ����������
		if (bMakeParams)
		{
			arCmd[i].nCount = 0;
			// ������ ���� �������� '\r\n' �� ������
			arCmd[i].sCmd.Replace(_T("\r\n"), _T(" "));
			// �������� ������� ���������
			if (arCmd[i].pArCmd == NULL)
				arCmd[i].pArCmd = new CStringArray;
			else
				arCmd[i].pArCmd->RemoveAll();
			// �������� ������� ����������
			if (arCmd[i].pArPrm == NULL)
				arCmd[i].pArPrm = new CArFldArray;
			else
			{
				// �������� ���� �������� ����������
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
		// ���� �� ������ ��������� ����� �������
		for (int n = 0, m = 0, j = 0; m >= 0 && n < nLen; n = m + 1, j++)
		{
			// ��������� ������ ����������
			if ((m = arCmd[i].sCmd.Find(';',n)) >= 0)
				strCmd = arCmd[i].sCmd.Mid(n, m - n);
			else
				strCmd = arCmd[i].sCmd.Mid(n);
			strCmd.TrimLeft();
			strCmd.TrimRight();
			// �������� ����������
			if (bMakeParams)
			{
				// ���������� ��������
				arCmd[i].nCount = j + 1;
				arCmd[i].pArCmd->SetSize(j + 1);
				arCmd[i].pArPrm->SetSize(j + 1);
				// �������� ������� ���������� ����������
				CFldArray *pArFld = new CFldArray;
				arCmd[i].pArPrm->SetAt(j, pArFld);
				// ���������� ������� ���������� ����������
				CString sOwner = (iRefType == REF_TAB) ? pTrn->sTabOwn : pTrn->sSubOwn;
				if (!PrepareSQLParms(pODBC, pArFld, 
					(sOwner.IsEmpty()) ? pODBC->sUserName : sOwner, strCmd, strErr))
				{
					strErr = GetErrMsg(ER_CMD_PARAMS, strErr);
					strErr = strEvt + _T(" - ") + strErr;
					return FALSE;
				}
				// ��������� ������ �� �������� ������
				// � ������� ���������� ����������
				if (!SetFldRefs(*pArFld, iRefType, strErr))
				{
					strErr = GetErrMsg(ER_CMD_PARAMS, strErr);
					strErr = strEvt + _T(" - ") + strErr;
					return FALSE;
				}
				// ��������� ������ ����������
				arCmd[i].pArCmd->SetAt(j, strCmd);
			}
			// �������� ����������
			else
			{
				// ������ ���������� SQL ����������
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
// ���������� ������ ���������

BOOL CTransfer::ParseStrMsg(CString &strMsg, CString &strErr) 
{
	arStrMsgPrm.RemoveAll();
	if (strMsg.IsEmpty()) return TRUE;
	// �������� ���� �������� <?>
	strMsg.Remove('?');
	// ��������� ������ ��������� �
	// ������������ ������� ����������
	CString	s, tmp;
	for (int i = 0, j = 0, n = 0; tmp = strMsg.Mid(j); i++, j++)
	{
		// ����� ������� ��������������
		if ((n = tmp.FindOneOf(_T("^:&$~#!%@"))) < 0)
			break;
		// ���������� �������� � ������ ���������
		j += n;
		// ��������� ������� ��������������
		// ���� ������� ������, �� ��� �� ��������
		if ((s = tmp.Mid(n,1)) == tmp.Mid(n+1,1))
		{
			// �������� ������ ������� �
			// ���������� ������� ����������
			strMsg.Delete(j, 1); i--; continue;
		}
		// ���������� ���������
		arStrMsgPrm.SetSize(i + 1);
		// ��������� ���������
		if (s == _T(":"))		arStrMsgPrm[i].iSrc = DATA_PARAM;
		else if (s == _T("&"))
		{
			arStrMsgPrm[i].iSrc = DATA_CONST;
			// ������ �� ��������� ���� ����������!
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
		// ��������� �������� ������
		GetWord(tmp, n+1, s);
		arStrMsgPrm[i].sName  = s;
		arStrMsgPrm[i].sValue = s;
		// ������������� ���� ������
		arStrMsgPrm[i].vData.InitType(SQL_C_CHAR, 255);
		// ������ ������ �� ���� ? � ������ ���������
		strMsg.Delete(j, s.GetLength()+1);
		strMsg.Insert(j, '?');
	}
	// ��������� ������
	if (!SetFldRefs(arStrMsgPrm, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_DEF_REFS, strErr);
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ��������������� ��������� Select �� �������� �����

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
		// ������������ ������ ������� Select
		strSQL += arFld[i].sName + _T(",");
		// ������������ ������� �� ���������� ������
		if (bUseKeys && arFld[i].iFlag == FLD_KEY)
		{
			strKeys += arFld[i].sName + _T("=");
			// ��������� ������ �� ��������
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
			// ��������� �������� ������
			if (arFld[i].iSrc == DATA_LOCAL ||
				arFld[i].iSrc == DATA_AUTO  ||
				arFld[i].iSrc == DATA_FUNC  ||
				arFld[i].iSrc == DATA_PROC)
				strKeys += arFld[i].sName;
			else
				strKeys += arFld[i].sValue;
			// ��������� ���� ������
			strKeys += _T("(") + SQLGetTypeStr(arFld[i].iSQLType);
			// ��������� ����� � ��������
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
	// ��������� ������� �� ���������� ������
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
	// ��������� ��������� �������
	else if (!strWhere.IsEmpty())
	{
		strSQL += _T(" where ") + strWhere;
	}
	strSQL.MakeLower();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ���������� ��������� Insert

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
	// ������������ ��������� INSERT
	CString strPrm = _T(" values (");
	// ������������ ������� ���������� INSERT
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
		// ��������� ������ �� ��������
		arPrm[i].iSrc     = arFld[i].iSrc;
		arPrm[i].iRef     = arFld[i].iRef;
		arPrm[i].sValue   = arFld[i].sValue;
		// ��������� ������ �� ��������
		switch (iRefType)
		{
		case REF_TAB:	arPrm[i].iDst = DATA_TABDST; break;
		case REF_SUB:	arPrm[i].iDst = DATA_SUBDST; break;
		default:		arPrm[i].iDst = DATA_TABDST; break;
		}
		arPrm[i].iIdx     = i;
		// ������������� ������ ������
		arPrm[i].vData.GetBuffer(arPrm[i].iSQLType, 
								 arPrm[i].iSize, TRUE);
		// ������������ ����� ���������
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
// ���������� ��������� Update

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
	// ������������ ��������� UPDATE
	BOOL bKeyFound = FALSE;
	CString strPrm;
	// ������������ ������� ���������� UPDATE
	int	 i, j, n = arFld.GetSize();
	// ������������ ���������� SET
	for (i = 0, j = 0; i < n; i++)
	{
		// ��������� ���������� ���������� �����
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
			// ��������� ������ �� ��������
			arPrm[j].iSrc     = arFld[i].iSrc;
			arPrm[j].iRef     = arFld[i].iRef;
			arPrm[j].sValue   = arFld[i].sValue;
			// ��������� ������ �� ��������
			switch (iRefType)
			{
			case REF_TAB:	arPrm[j].iDst = DATA_TABDST; break;
			case REF_SUB:	arPrm[j].iDst = DATA_SUBDST; break;
			default:		arPrm[j].iDst = DATA_TABDST; break;
			}
			arPrm[j].iIdx     = i;
			// ������������� ������ ������
			arPrm[j].vData.GetBuffer(arPrm[j].iSQLType, 
									 arPrm[j].iSize, TRUE);
			// ������������ ������ SET
			strSQL += arPrm[j].sName + _T("=?,");
			j++;
		}
	}
	if (j == 0)
	{
		strErr = GetErrMsg(ER_NO_MODFLD);
		return FALSE;
	}
	// ������������ ���������� WHERE
	// � ����� ������� ����������
	for (i = 0; i < n; i++)
	{
		// ������������� �������� �����
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
			// ��������� ������ �� ��������
			arPrm[j].iSrc     = arFld[i].iSrc;
			arPrm[j].iRef     = arFld[i].iRef;
			arPrm[j].sValue   = arFld[i].sValue;
			// ��������� ������ �� ��������
			switch (iRefType)
			{
			case REF_TAB:	arPrm[j].iDst = DATA_TABDST; break;
			case REF_SUB:	arPrm[j].iDst = DATA_SUBDST; break;
			default:		arPrm[j].iDst = DATA_TABDST; break;
			}
			arPrm[j].iIdx     = i;
			// ������������� ������ ������
			arPrm[j].vData.GetBuffer(arPrm[j].iSQLType, 
									 arPrm[j].iSize, TRUE);
			// ������������ ������ WHERE
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
// ���������� ��������� Delete

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
	// ������������ ��������� DELETE
	BOOL bKeyFound = FALSE;
	CString strPrm;
	// ������������ ������� ���������� DELETE
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
			// ��������� ������ �� ��������
			arPrm[j].iSrc     = arFld[i].iSrc;
			arPrm[j].iRef     = arFld[i].iRef;
			arPrm[j].sValue   = arFld[i].sName;
			// ��������� ������ �� ��������
			switch (iRefType)
			{
			case REF_TAB:	arPrm[j].iDst = DATA_TABDST; break;
			case REF_SUB:	arPrm[j].iDst = DATA_SUBDST; break;
			default:		arPrm[j].iDst = DATA_TABDST; break;
			}
			arPrm[j].iIdx     = i;
			// ������������� ������ ������
			arPrm[j].vData.GetBuffer(arPrm[j].iSQLType, 
									 arPrm[j].iSize, TRUE);
			// ������������ ������ ����������
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
// ���������� ����������

BOOL CTransfer::BindParams(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
						   CFldArray &arPrm, CString &strErr) 
{
	RETCODE nRet;
	void* pValue;
	int i, nCount = arPrm.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// ��������� ������ ������ (��� �������������)
		pValue  = arPrm[i].vData.GetBuffer(arPrm[i].iSQLType, 
									arPrm[i].iSize, FALSE);
		switch (arPrm[i].iSQLType)
		{
		// ���������� ��������� �� ������
		case SQL_LONGVARCHAR:
		case SQL_LONGVARBINARY:
		{
			// ��������� ������������� ������� ������� �����
			arPrm[i].iSize = SQLGetTypeDef(*pODBC, arPrm[i].iSQLType).lPrec;
			// ����������� ������ �� ����� ����������
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
			// ������ ��������������� ����� �����������
			arPrm[i].vData.m_lIndex = SQL_NTS;
			nRet = ::SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, 
					arPrm[i].vData.m_iType, arPrm[i].iSQLType, 
					arPrm[i].iSize, arPrm[i].iPrec, pValue, 
					arPrm[i].vData.m_length, &arPrm[i].vData.m_lIndex);
			break;
		}
		// ���������� ��������� �� ��������
		case SQL_DATE:
		case SQL_TIME:
		case SQL_TIMESTAMP:
		case SQL_TYPE_DATE:
		case SQL_TYPE_TIME:
		case SQL_TYPE_TIMESTAMP:
		{
			// ������ ��������������� ����� �����������
			arPrm[i].vData.m_lIndex = arPrm[i].vData.m_length;
			// ������������� C-���� ������ SQL-����
			nRet = ::SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, 
					arPrm[i].vData.m_iType, arPrm[i].vData.m_iType, 
					arPrm[i].iSize, arPrm[i].iPrec, pValue, 
					arPrm[i].vData.m_length, &arPrm[i].vData.m_lIndex);
			break;
		}
		// ���������� ��������� �� ��������
		default:
		{
			// ������ ��������������� ����� �����������
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
			// ������ ������ ������
			SQLGetError(hstmt, strErr);
			strErr = arPrm[i].sName + _T("\n")+strErr;
			strErr = GetErrMsg(ER_BIND_PARAM, strErr);
			return FALSE;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ���������� ����� ������

BOOL CTransfer::BindFields(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
						   CFldArray &arFld, CString &strErr) 
{
	void* pValue;
	int i, nCount = arFld.GetSize();
	for (i = 0; i < nCount; i++)
	{
		// ���� ������� ������ �� �����������
		// ������ ����� ����������� �������������
		if (arFld[i].iSQLType == SQL_LONGVARCHAR ||
			arFld[i].iSQLType == SQL_LONGVARBINARY)
		{
			// ��������� ������������� ������� ������� �����
			arFld[i].iSize = SQLGetTypeDef(*pODBC, 
							arFld[i].iSQLType).lPrec;
		}
		// ���������� ���� �� ��������
		else
		{
			// ��������� ������ ������ (��� �������������)
			pValue  = arFld[i].vData.GetBuffer(arFld[i].iSQLType, 
										arFld[i].iSize, FALSE);
			// ���������� ���� ������
			if (::SQLBindCol(hstmt, i + 1, arFld[i].vData.m_iType, 
				pValue, arFld[i].vData.m_length, 
				&arFld[i].vData.m_lIndex) != SQL_SUCCESS)
			{
				// ������ ������ ������
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
// ���������� SQL Select Max

long CTransfer::ExecSQLMax(ODBC_INFO *pODBC, FLD_INFO &fldInfo, 
						   CString &strErr)
{
	void*	pValue;
	CString	strCmd;
	CString strMsg;
	CTime	curTime;
	RETCODE	nRet = SQL_ERROR;

	// ������������ ������ SQL ��������� ��� ������� �� ������� ���������
	strCmd = _T("select max(") + fldInfo.sName + _T(") from ") + pTrn->sTabDst;
	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return SQL_ERROR;
	}
	// ���������� SQL ���������
	AfxGetApp()->BeginWaitCursor();
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strCmd), 
		SQL_NTS) == SQL_ERROR)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		goto out;
	}
	// ��������� ������ ������ (��� �������������)
	pValue = fldInfo.vData.GetBuffer(fldInfo.iSQLType, 
									 fldInfo.iSize, FALSE);
	// ���������� ��������������� ����
	if (::SQLBindCol(hstmt, 1, fldInfo.vData.m_iType, 
		pValue, fldInfo.vData.m_length, 
		&fldInfo.vData.m_lIndex) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_BIND_FIELD, fldInfo.sName);
		goto out;
	}
	// ���� ����������� ������� SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// ������ ��������� �������� � ����
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strCmd + _T("\n\n"));
		}
		// ������������ ������ ��� ������
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strCmd + _T("\n\n") + strMsg;
		// ����� SQL Select
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
	// ���������� ��������� SQL
	if ((nRet = ::SQLExecute(hstmt)) != SQL_SUCCESS)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		goto out;
	}
	// ��������� ����������
	::SQLFetch(hstmt);

out:
	// ������������ ����������
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	// ����������� ����������� SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	AfxGetApp()->EndWaitCursor();
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// ���������� SQL Select Count

long CTransfer::ExecSQLCount(ODBC_INFO *pODBC, CString strCmd, 
							 CFldArray &arPrm, CString &strErr) 
{
	int  i;
	CTime	curTime;
	CString	strMsg;
	long cbCount, nCount = -1;

	// ������������ ������ SQL ���������
	strCmd.MakeLower();
	// �������� ��������� <order by>
	if ((i = strCmd.Find("order by")) > 0)
		strCmd = strCmd.Left(i);
	// ������ ��������� <select>
	if ((i = strCmd.Find("from")) > 0)
	{
		strCmd.Delete(0, i);
		strCmd.Insert(0, _T("select count(*) "));
	}
	// ���������� ������ ��� ���������� 
	if (!SetFldData(arPrm, OP_COMMAND, strErr))
	{
		strErr = GetErrMsg(ER_SET_PARAMS, strErr);
		return -1;
	}
	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		return -1;
	}
	// ���������� SQL ���������
	AfxGetApp()->BeginWaitCursor();
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(strCmd), 
		SQL_NTS) == SQL_ERROR)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		goto out;
	}
	// ���������� ���������� SQL Select
	if (!BindParams(pODBC, hstmt, arPrm, strErr))
	{
		strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
		goto out;
	}
	// ���������� ��������������� ����
	if (::SQLBindCol(hstmt, 1, SQL_C_SLONG, 
		&nCount, 0, &cbCount) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_BIND_FIELD, _T("count(*)"));
		goto out;
	}
	// ���� ����������� ������� SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// ������������ ������ ���������
		CString strSQL = FormatSQL(strCmd, arPrm);
		// ������ ��������� �������� � ����
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strSQL + _T("\n\n"));
		}
		// ������������ ������ ��� ������
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// ����� SQL Select
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
	// ���������� ��������� SQL
	if (::SQLExecute(hstmt) != SQL_SUCCESS)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		goto out;
	}
	// ��������� ����������
	::SQLFetch(hstmt);

out:
	// ������������ ����������
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	// ����������� ����������� SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	AfxGetApp()->EndWaitCursor();
	return nCount;
}

/////////////////////////////////////////////////////////////////////
// ���������� ���������� SQL �������

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

	// ������ ���������� �������
	if (func != FNC_SQLCOUNT)
	{
		// ������������ ���� �������
		if ((i = strParms.Find(',')) >= 0)
		{
			sFld = strParms.Left(i);
			sFld.TrimLeft();
			sFld.TrimRight();
			strParms = strParms.Mid(i + 1);
			strParms.TrimLeft();
		}
	}
	// ������� �������
	if ((i = strParms.Find(',')) >= 0)
	{
		sTab = strParms.Left(i);
		sTab.TrimLeft();
		sTab.TrimRight();
		strParms = strParms.Mid(i + 1);
		strParms.TrimLeft();
	}
	strParms.TrimRight();

	// ��������� SQL Where
	if (!strParms.IsEmpty())
	{
		sExp = _T(" where ") + strParms;
	}
	// ������������ ������ SQL ���������
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
	// ���������� ���������� SQL Where (������ � ����� pDst->sUserName)
	if (!PrepareSQLParms(pDst, &arPrm, pDst->sUserName, sCmd, strErr))
	{
		strErr = GetErrMsg(ER_PARSE_PARAMS, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + sCmd;
		return SQL_ERROR;
	}
	// ���������� ������ ��� ���������� 
	if (!SetFldData(arPrm, OP_COMMAND, strErr))
	{
		strErr = GetErrMsg(ER_SET_PARAMS, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + sCmd;
		return SQL_ERROR;
	}
	// �������� ����������� SQL ���������
	SQLHSTMT hstmt;
	if (::SQLAllocHandle(SQL_HANDLE_STMT, 
		pODBC->hdbc, &hstmt) != SQL_SUCCESS)
	{
		strErr = GetErrMsg(ER_CRT_SQL);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		return SQL_ERROR;
	}
	// ���������� SQL ���������
	AfxGetApp()->BeginWaitCursor();
	if (::SQLPrepare(hstmt, (SQLCHAR*)LPCTSTR(sCmd), 
		SQL_NTS) == SQL_ERROR)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		goto out;
	}
	if (arPrm.GetSize() > 0)
	{
		// ���������� ���������� SQL Select
		if (!BindParams(pODBC, hstmt, arPrm, strErr))
		{
			strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
			strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
			goto out;
		}
	}
	// ��������� ���������� � �������������� ����
	SQLCHAR*	szColName;
	SQLSMALLINT	nmColName, nmColLen;
	SQLSMALLINT	nDataType, nColDec, fNullable;
	SQLUINTEGER	nColSize;
	SQLINTEGER	cbColSize;
	void*		pValue;
	// ����������� ������������ ����� ������������ ����
	nmColName = pODBC->iMaxColNameLen + 1;
	// ��������� ������ ��� ������������ ����
	szColName = (SQLCHAR*)malloc((nmColName)*sizeof(SQLCHAR));
	// ���������� �������� ��������������� ����
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
	// ������������ ������
	free(szColName);
	// ������������� ������ ������
	pValue = dataValue.GetBuffer(nDataType, nColSize, TRUE);
	// ���������� ��������������� ����
	if (::SQLBindCol(hstmt, 1, nDataType, 
		pValue, nColSize, &cbColSize) != SQL_SUCCESS)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		strErr = GetErrMsg(ER_BIND_FIELD, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		goto out;
	}

	// ���� ����������� ������� SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// ������������ ������ ���������
		CString strSQL = FormatSQL(sCmd, arPrm);
		// ������ ��������� �������� � ����
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strSQL + _T("\n\n"));
		}
		// ������������ ������ ��� ������
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// ����� SQL Select
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
	// ���������� ��������� SQL
	if ((nRet = ::SQLExecute(hstmt)) != SQL_SUCCESS)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		strErr += _T("\n") + sFnc + _T(": ") + FormatSQL(sCmd, arPrm);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		goto out;
	}
	// ��������� ����������
	::SQLFetch(hstmt);

	// ������� ���������� � ������
	strVal = dataValue.GetString();

out:
	// ������������ ����������
	::SQLFreeStmt(hstmt, SQL_CLOSE);
	::SQLFreeStmt(hstmt, SQL_UNBIND);
	::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	// ����������� ����������� SQL
	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	AfxGetApp()->EndWaitCursor();
	return nRet;
}
/////////////////////////////////////////////////////////////////////
// ���������� ��������������� SQL Select (� ������ ������ ������)

RETCODE CTransfer::ExecSelect(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
							  CFldArray &arFld, CFldArray &arPrm, 
							  CString &strCmd,  BOOL bFetchData, 
							  CString &strErr) 
{
	CTime	curTime;
	CString strMsg;

	// �������� ����������� SQL
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
		// ������������ ����������� SQL
		::SQLFreeStmt(hstmt, SQL_CLOSE);
		::SQLFreeStmt(hstmt, SQL_UNBIND);
		::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	}
	// ��������� ������ � ������� ����������
	if (!SetFldData(arPrm, OP_COMMAND, strErr))
	{
		strErr = GetErrMsg(ER_SET_PARAMS, strErr);
		return SQL_ERROR;
	}
	// ���������� ����������
	if (!BindParams(pODBC, hstmt, arPrm, strErr))
	{
		strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
		::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = SQL_NULL_HSTMT;
		return SQL_ERROR;
	}
	// ���� ����������� ������� SQL Select
	if (TrnParm.iTraceSel > 0)
	{
		// ������������ ������ ���������
		CString strSQL = FormatSQL(strCmd, arPrm);
		// ������ ��������� �������� � ����
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMsg = _T("SQL SELECT:\n");
			strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
			fileLog.WriteString(strMsg);
			fileLog.WriteString(strSQL + _T("\n\n"));
		}
		// ������������ ������ ��� ������
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// ����� SQL Select
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
	// ���������� SQL ���������
	RETCODE nRet = ::SQLExecDirect(hstmt, (SQLTCHAR*)LPCTSTR(strCmd), SQL_NTS);
	// �������� ������ ��� ����������� ����������
	if (nRet == SQL_NEED_DATA)
	{
		SQLPOINTER nIdx;
		// ����������� ������� �������� �������
		while ((nRet = ::SQLParamData(hstmt, &nIdx)) == SQL_NEED_DATA)
		{
			PutLongData(hstmt, arPrm[(int)nIdx].vData);
		}
	}
	// ���� ������ ����������
	if (nRet == SQL_ERROR)
	{
		// ������ ������ ������
		SQLGetError(hstmt, strErr);
		return nRet;
	}
	// ���������� ����� ������
	if (!BindFields(pODBC, hstmt, arFld, strErr))
	{
		strErr = GetErrMsg(ER_BIND_FIELDS, strErr);
		return SQL_ERROR;
	}
	// ���������� ������ SQL �������
	if (bFetchData)
	{
		nRet = FetchData(hstmt, arFld, strErr);
		if (nRet == SQL_ERROR)
		{
			// ������ ������ ������
			SQLGetError(hstmt, strErr);
			return nRet;
		}
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// ���������� �������������� ������� SQL DML

RETCODE CTransfer::ExecUpdate(ODBC_INFO *pODBC, SQLHSTMT &hstmt, 
							  OP_TYPE iOpType,  CFldArray &arPrm, 
							  CString &strCmd,  BOOL bUseSrcData, 
							  CString &strErr) 
{
	CTime curTime;
	CString	strMsg;
	if (hstmt == SQL_NULL_HSTMT)
	{
		// �������� ����������� SQL ���������
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
		// ������������ ���������� SQL ���������
		::SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
	}
	// ��������� ������ � ������� ����������
	if (bUseSrcData)
	{
		// �� ������ �� ��������
		if (!SetFldData(arPrm, iOpType, strErr))
		{
			strErr = GetErrMsg(ER_SET_PARAMS, strErr);
			return SQL_ERROR;
		}
		// ����������� ��������� ������
		if (pTrn->iConvert != TRN_UNCHANGED)
			ConvertTxt(arPrm, iOpType, pTrn->iConvert);
	}
	else
	{
		// �� ������ �� ��������
		if (!SetDstData(arPrm, strErr))
		{
			strErr = GetErrMsg(ER_SET_PARAMS, strErr);
			return SQL_ERROR;
		}
	}
	// ���������� ����������
	if (!BindParams(pODBC, hstmt, arPrm, strErr))
	{
		strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
		return SQL_ERROR;
	}
	// ���� ����������� ������� SQL ����������
	if (TrnParm.iTraceOpr > 0)
	{
		// ������������ ������ ���������
		CString strSQL = FormatSQL(strCmd, arPrm);
		// ������ ��������� �������� � ����
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
		// ������������ ������ ��� ������
		CString strMsg;
		strMsg.LoadString(IDS_MS_BREAK);
		strMsg = strSQL + _T("\n\n") + strMsg;
		// ����� SQL ���������
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
	// ���������� SQL ���������
	RETCODE nRet = ::SQLExecDirect(hstmt, (SQLTCHAR*)LPCTSTR(strCmd), SQL_NTS);
	// �������� ������ ��� ����������� ����������
	if (nRet == SQL_NEED_DATA)
	{
		SQLPOINTER nIdx;
		// ����������� ������� �������� �������
		while ((nRet = ::SQLParamData(hstmt, &nIdx)) == SQL_NEED_DATA)
		{
			PutLongData(hstmt, arPrm[(int)nIdx].vData);
		}
	}
	// ���� ������ ����������
	if (nRet == SQL_ERROR)
	{
		SQLGetError(hstmt, strErr);
		// ����� ����������
		if (bTxnCommit)
		{
			::SQLEndTran(SQL_HANDLE_DBC, pODBC->hdbc, SQL_ROLLBACK);
		}
		return nRet;
	}
	// ������� � ����������
	else if (nRet == SQL_SUCCESS_WITH_INFO)
	{
		SQLGetError(hstmt, strErr);
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////
// ���������� SQL-������� ���������

RETCODE CTransfer::ExecSQLCmd(ODBC_INFO *pODBC, CCmdArray &arCmd, 
							  EV_TYPE iEventID, LPCTSTR szField, 
							  CString &strErr) 
{
	// ����� ������� �� ��������������
	int i = FindSQLCmd(arCmd, iEventID, szField);
	if (i < 0) return SQL_SUCCESS;
	// ���������� �������
	RETCODE nRet = SQL_SUCCESS;
	CFldArray *pArPrm;
	CTime	curTime;
	CString strEvt,strCmd,strMsg, strSQL;
	// ���� �� ������ ��������� ����� �������
	for (int j = 0; j < arCmd[i].nCount; j++)
	{
		if (arCmd[i].hstmt == SQL_NULL_HSTMT)
		{
			// �������� ����������� SQL �������
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
			// ������������ ���������� SQL �������
			::SQLFreeStmt(arCmd[i].hstmt, SQL_RESET_PARAMS);
		}
		// ��������� ������ ����������
		strCmd = arCmd[i].pArCmd->ElementAt(j);
		// ��������� ���������� ����������
		pArPrm = arCmd[i].pArPrm->ElementAt(j);
		// ��������� ���������� ����������
		if (!SetFldData(*pArPrm, OP_COMMAND, strErr))
		{
			strErr = GetErrMsg(ER_SET_PARAMS, strErr);
			strSQL = strCmd; nRet = SQL_ERROR; goto err;
		}
		// ����������� ��������� ������
		if (pTrn->iConvert != TRN_UNCHANGED)
			ConvertTxt(*pArPrm, OP_COMMAND, pTrn->iConvert);
		// ���������� ����������
		if (!BindParams(pODBC, arCmd[i].hstmt, *pArPrm, strErr))
		{
			strErr = GetErrMsg(ER_BIND_PARAMS, strErr);
			nRet = SQL_ERROR; goto err;
		}
		// �������������� ������ SQL
		strSQL = FormatSQL(strCmd, *pArPrm);
		// ���� ����������� ������� SQL ������
		if (TrnParm.iTraceCmd > 0)
		{
			// ������ ��������� �������� � ����
			if (bFileLog && TrnParm.iWriteOpr)
			{
				curTime = CTime::GetCurrentTime();
				strMsg = _T("SQL COMMAND:\n");
				strMsg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMsg;
				fileLog.WriteString(strMsg);
				fileLog.WriteString(strSQL + _T("\n\n"));
			}
			// ������������ ������ ��� ������
			CString strMsg;
			strMsg.LoadString(IDS_MS_BREAK);
			strMsg = strSQL + _T("\n\n") + strMsg;
			// ����� SQL �������
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
		// ���������� ����� ����������
		nRet = ::SQLExecDirect(arCmd[i].hstmt,(SQLTCHAR*)LPCTSTR(strCmd),SQL_NTS);
		// �������� ������ ��� ����������� �����
		if (nRet == SQL_NEED_DATA)
		{
			SQLPOINTER nIdx;
			nRet = ::SQLParamData(arCmd[i].hstmt, &nIdx);
			// ����������� ������� �������� �������
			while (nRet == SQL_NEED_DATA)
			{
				// ��������� ������� ������
				PutLongData(arCmd[i].hstmt, 
							pArPrm->GetAt((int)nIdx).vData);
				nRet = ::SQLParamData(arCmd[i].hstmt, &nIdx);
			}
		}
		// ���� ��� ������
		if (nRet == SQL_NO_DATA) return nRet;
		// ���� ������ ����������
		else if (nRet == SQL_ERROR)
		{
			SQLGetError(arCmd[i].hstmt, strErr);
			// ����� ����������
			if (bTxnCommit)
			{
				::SQLEndTran(SQL_HANDLE_DBC, pODBC->hdbc, SQL_ROLLBACK);
			}
			goto err;
		}
		// ������� � ����������
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
// ���������� SQL-������ � ����� ������� ���������

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
			// ���������� SQL-������� ��� ���� <arFld[i].sName>
			nRet = ExecSQLCmd(pODBC,arCmd,iEventID,arFld[i].sName,strErr);
			if (nRet == SQL_ERROR)	goto err;	// ��������� ������
			else					k = i;		// �������� ����� ����
		}
	}
	return nRet;

err:
	if (!bTxnCommit)
	{
		// �������������� �������������� �������
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
		// ��������� � �������� ������� (����� ������)
		for (i = k; i >= 0; i--)
		{
			if (arFld[i].iFlag == FLD_EVCHANGE && arFld[i].bMod)
			{
				// ����� SQL-������� ��� ���� <arFld[i].sName>
				nRet = ExecSQLCmd(pODBC,arCmd,iEventID,arFld[i].sName,strRet);
				if (nRet == SQL_ERROR)			// ������ ������
				{
					// ����� ��������� �� ������
					if (bShowErr) ShowErr(pTrn->sName, strRet);
				}
			}
		}
	}
	return SQL_ERROR;
}

/////////////////////////////////////////////////////////////////////
// ������ ������ ������

RETCODE CTransfer::FetchData(SQLHSTMT &hstmt, CFldArray &arFld, 
							 CString &strErr) 
{
	RETCODE nRet = ::SQLFetch(hstmt);
	
	switch (nRet)
	{
	case SQL_SUCCESS_WITH_INFO:
	{
		// ��������� ��������� � ����������
		SQLGetError(hstmt, strErr);
	}
	case SQL_SUCCESS:
	{
		CString subErr;
		int i, nCols = arFld.GetSize();
		// ���������� ������ ������
		for (i = 0; i < nCols; i++)
		{
			if (arFld[i].iSQLType == SQL_LONGVARCHAR ||
				arFld[i].iSQLType == SQL_LONGVARBINARY)
			{
				arFld[i].vData.SetNull();
				// ������ ������� ������
				nRet = GetLongData(hstmt, i + 1, arFld[i].vData, strErr);
				if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
					return nRet;
			}
			// ��������� ������� ��������
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
// ����������� ������ ������

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
		// ��������� ��������� � ����������
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
				// ������ ������� ������
				nRet = GetLongData(hstmt, i + 1, arFld[i].vData, strErr);
				if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
					return nRet;
			}
			// ��������� ������� ��������
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
// ��������� ������� ������ ��� SQL ���������

RETCODE CTransfer::GetLongData(SQLHSTMT &hstmt, UINT iCol, 
							   CDataValue &vData, CString &strErr) 
{
	switch (vData.m_iType)
	{
	case SQL_C_CHAR:		// SQL_LONGVARCHAR
	{
		// ��������� ����������� ����� ������ (1)
		TCHAR* pstr = (TCHAR*)vData.GetBuffer(SQL_LONGVARCHAR, 1, TRUE);
		// ������� ������ ��� ����������� ������ ������� ������
		RETCODE nRet = ::SQLGetData(hstmt, iCol, vData.m_iType, 
						pstr, vData.m_length, &vData.m_lIndex);
		// ���� ���������� ��������� ������� 
		// � ���������� � �������� ����� ������
		if (nRet == SQL_SUCCESS_WITH_INFO && 
			(long)vData.m_length <= vData.m_lIndex)
		{
			// ������������� ������ ������ ������� ������
			pstr = (TCHAR*)vData.InitType(SQL_C_CHAR, vData.m_lIndex + 1);
			vData.m_bLong = TRUE;
			// ��������� ������ ������
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
		// ��������� ����������� ����� ������ (1)
		BYTE* pbin = (BYTE*)vData.GetBuffer(SQL_LONGVARBINARY, 1, TRUE);
		// ������� ������ ��� ����������� ������ ������� ������
		RETCODE nRet = ::SQLGetData(hstmt, iCol, vData.m_iType, 
						&data, vData.m_length, &vData.m_lIndex);
		// ���� ���������� ��������� ������� 
		// � ���������� � �������� ����� ������
		if (nRet == SQL_SUCCESS_WITH_INFO && 
			(long)vData.m_length <= vData.m_lIndex)
		{
			// ������������� ������ ������ ������� ������
			pbin = (BYTE*)vData.InitType(SQL_C_BINARY, vData.m_lIndex + 1);
			vData.m_bLong = TRUE;
			// ������ ������� �����
			*pbin = data; pbin++;
			// ��������� ������ ������
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
// ��������� ������� ������ ��� SQL ���������

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
// ������������ ������ ���������

CString CTransfer::FormatMsg(LPCTSTR szMsg) 
{
	CString s,strErr,strMsg(szMsg);
	// ���������� ������ ��� ����������
	if (!SetFldData(arStrMsgPrm, OP_COMMAND, strErr))
	{
		return strMsg;
	}
	// �������������� ������
	int m = arStrMsgPrm.GetSize();
	for (int i=0, n=0; i<m && (n=strMsg.Find(_T("?"),n))>=0; i++)
	{
		// ������ ����� '?' �� �������� ������
		s = arStrMsgPrm.GetAt(i).vData.GetString();
		strMsg.Delete(n, 1);
		strMsg.Insert(n, s);
		n += s.GetLength();
	}
	return strMsg;
}

/////////////////////////////////////////////////////////////////////
// ������������ ������ SQL ��������� (����� Select)

CString CTransfer::FormatSQL(LPCTSTR szSQL, CFldArray &arPrm) 
{
	// �������������� ������
	CString s,strSQL(szSQL);
	int m = arPrm.GetSize();
	for (int i = 0, n = 0; i < m && (n = strSQL.Find(_T("?"),n))>=0; i++)
	{
		// ������ ����� '?' �� �������� ������
		s = arPrm.GetAt(i).vData.GetString();
		// ���������� �������� � �������
		// ��� ������ ��� ����-�������
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
// ������������ ������ SQL ��������� �� �������������� ������

CString CTransfer::FormatUpd(ER_TYPE iErrType) 
{
	CString strOut("\n");
	switch (iErrType)		// ��� ������
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
// ������������ ������ ������ � ���������� ���������

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
// �������� ������� ���������

BOOL CTransfer::IsValid()
{
	return bValid;
}

/////////////////////////////////////////////////////////////////////
// ���������� ���������� � ����������

BOOL CTransfer::Parse(CString &strErr) 
{
	strErr.Empty();

	if (pSrc == NULL || pDst == NULL || pTrn == NULL)
		return FALSE;
	if (pSrc->hdbc == SQL_NULL_HDBC) return FALSE;
	if (pDst->hdbc == SQL_NULL_HDBC) return FALSE;

	//////////////////////////////////////////////////////////////////
	//                ��������� ����� ����������
	//////////////////////////////////////////////////////////////////
	// ��������� �������� COMPLEX
	if (pTrn->iSchema == TRN_COMPLEX)
	{
#ifdef _TRIAL_VER
		strErr.LoadString(IDS_ER_TRIAL_COMPLEX);
		return FALSE;
#else
		bComplex = TRUE;
#endif
	}
	// ��������� ������� ������ ���������
	if (!pTrn->sMsg.IsEmpty())
		bMessage = TRUE;
	// ��������� ���� Commit
	if (pDst->iTxnCapable == SQL_TC_DDL_COMMIT)
		bTxnCommit = TRUE;
	else
		bTxnCommit = FALSE;
	// ��������� ������������� ������ ������
	// �� �������� ����� � ������� ���������
	if (pTrn->iTabPrc == DST_CREATE ||
		pTrn->iTabPrc == DST_ADD)
		bTabSearch = FALSE;
	else
		bTabSearch = TRUE;
	// ��������� ������������� ������ ������
	// �� �������� ����� � ���������� ���������
	if (pTrn->iSubPrc == DST_CREATE ||
		pTrn->iSubPrc == DST_ADD)
		bSubSearch = FALSE;
	else
		bSubSearch = TRUE;

	//////////////////////////////////////////////////////////////////
	//                ��������� �������� �������
	//////////////////////////////////////////////////////////////////
	// ���������� ������� SQL Select ������� ���������
	if (!ParseColumns(pSrc, arSrcTabFld, pTrn->sTabSQL, strErr))
	{
		strErr = GetErrMsg(ER_PREP_SELSRCTAB, strErr);
		return FALSE;
	}
	// ��� �������� ������� ��������� �� ��������� ���
	// ��� ������ ��������� ��������� � ���������: 
	// ��������� �������� ����� � ������� ���������
	if (pTrn->iTabPrc == DST_CREATE && pTrn->iSchema == TRN_COPY    ||
		pTrn->iTabPrc == DST_CREATE && pTrn->iSchema == TRN_COMPLEX ||
		pTrn->iTabPrc == DST_FULL)
	{
		// ����������� ������� � SQL ������� ���������
		if (!TableInSQL(pTrn->sTabSQL, sSrcMTable, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SRCTAB, strErr);
			return FALSE;
		}
		// ��������� �������� ����� ������� ���������
		if (!SetFldKeys(pSrc, arSrcTabFld, pSrc->sUserName, 
						sSrcMTable, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SRCTAB, strErr);
			return FALSE;
		}
	}
	// ���������� ����� ����� ������� ���������
	if (!FillFieldArray(pTrn->sTabMap, arMapTabFld))
	{
		strErr = GetErrMsg(ER_SNTX_MAPTAB, strErr);
		return FALSE;
	}
	// ���������� ������� ���������
	if (pTrn->iTabPrc == DST_CREATE)
	{
		// �������� ������� ���������
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
		// ���������� ������� ����� ������� ���������
		if (!ParseTable(pDst, pTrn->iSchema, arDstTabFld, arMapTabFld, 
						(pTrn->sTabOwn.IsEmpty()) ? pDst->sUserName : pTrn->sTabOwn, 
						pTrn->sTabDst, strErr))
		{
			strErr = GetErrMsg(ER_PREP_DSTTAB, strErr);
			return FALSE;
		}
	}
	// ��������� ������ �� �������� ������
	// � ������� ����� ������� ���������
	if (!SetFldRefs(arDstTabFld, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_DSTTAB, strErr);
		return FALSE;
	}
	// ���� ��������� ����� � ������� ���������
	if (bTabSearch)
	{
		// ��������������� SQL ������� ������ � ������� ���������
		// �� ������� �������� ��������� ������ ������� ���������
		if (!MakeSelect(arDstTabFld, pTrn->sTabDst, _T(""), 
						TRUE, sDstTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDDSTTAB, strErr);
			return FALSE;
		}
	}
	// ��� ������ ��������� ��������� � ���������
	if (pTrn->iTabPrc == DST_FULL)
	{
		// ��������� ������ �� ��������
		// ��� �������� ����� ���������
		if (!SetDstRefs(arSrcTabFld, REF_TAB, TRUE, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SRCTAB, strErr);
			return FALSE;
		}
		// ��������������� SQL ������� ������ � ���.���������
		// �� ������� �������� ��������� ������ �������
		if (!MakeSelect(arSrcTabFld, sSrcMTable, _T(""), 
						TRUE, sSrcTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDSRCTAB, strErr);
			return FALSE;
		}
		// ��������������� SQL Select ������� ���������
		// �� ��������� �������
		if (!MakeSelect(arDstTabFld, pTrn->sTabDst, pTrn->sTabCond, 
						FALSE, sDstTabSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SELDSTTAB, strErr);
			return FALSE;
		}
	}
	// ���������� ������� ������ ������� ���������
	if (!FillCmdArray(pTrn->sTabCmd, arDstTabCmd))
	{
		strErr = GetErrMsg(ER_SNTX_CMDTAB, strErr);
		return FALSE;
	}
	// ���������� ������� ������ ������� ���������
	if (!ParseCmdArr(pDst, REF_TAB, arDstTabCmd, FALSE, strErr))
	{
		strErr = GetErrMsg(ER_PREP_CMDTAB, strErr);
		return FALSE;
	}
	// ���������� �������� ������ CHANGE � ������� ����� ���������
	if (!ParseFldCmd(arDstTabFld, arDstTabCmd, strErr))
	{
		return FALSE;
	}
	// ��������� ������������� ���������� ������
	// ��� ��������� ������ ������ ������� ���������
	if (FindEvent(arDstTabCmd, EV_ON_INSERT) >= 0 ||
		FindEvent(arDstTabCmd, EV_ON_UPDATE) >= 0 ||
		FindEvent(arDstTabCmd, EV_ON_DELETE) >= 0 ||
		pTrn->iSchema == TRN_COMPLEX)
		bTabRowCmd = TRUE;
	// ��������� ������������� ���������� ������ OnChange
	// ��� ��������� ������ � ����� ������� ���������
	if (FindEvent(arDstTabCmd, EV_ON_CHANGE) >= 0)
		bTabFldCmd = TRUE;

#ifndef _TRIAL_VER
	//////////////////////////////////////////////////////////////////
	//            ��������� ����������� �������
	//////////////////////////////////////////////////////////////////
	if (pTrn->iSchema == TRN_COMPLEX)
	{
		// ���������� ������� SQL Select ���������� ���������
		if (!ParseColumns(pSrc, arSrcSubFld, pTrn->sSubSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_SELSRCSUB, strErr);
			return FALSE;
		}
		// ��� �������� ���������� ��������� �� ��������� ���
		// ��� ������ ��������� ��������� � ���������: 
		// ��������� �������� ����� � ���������� ���������
		if (pTrn->iSubPrc == DST_CREATE && pTrn->iSchema == TRN_COPY    ||
			pTrn->iSubPrc == DST_CREATE && pTrn->iSchema == TRN_COMPLEX ||
			pTrn->iSubPrc == DST_FULL)
		{
			// ����������� ������� � SQL ������� ���������
			if (!TableInSQL(pTrn->sSubSQL, sSrcSTable, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SRCSUB, strErr);
				return FALSE;
			}
			// ��������� �������� ����� ���������� ���������
			if (!SetFldKeys(pSrc, arSrcSubFld, pSrc->sUserName, 
							sSrcSTable, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SRCSUB, strErr);
				return FALSE;
			}
		}
		// ���������� ����� ����� ���������� ���������
		if (!FillFieldArray(pTrn->sSubMap, arMapSubFld))
		{
			strErr = GetErrMsg(ER_SNTX_MAPSUB, strErr);
			return FALSE;
		}
		// ���������� ���������� ���������
		if (pTrn->iSubPrc == DST_CREATE)
		{
			// �������� ���������� ���������
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
			// ���������� ������� ����� ���������� ���������
			if (!ParseTable(pDst, pTrn->iSchema, arDstSubFld, arMapSubFld, 
							(pTrn->sSubOwn.IsEmpty()) ? pDst->sUserName : pTrn->sSubOwn, 
							pTrn->sSubDst, strErr))
			{
				strErr = GetErrMsg(ER_PREP_DSTSUB, strErr);
				return FALSE;
			}
		}
		// ��������� ������ �� �������� ������
		// � ������� ����� ���������� ���������
		if (!SetFldRefs(arDstSubFld, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_DSTSUB, strErr);
			return FALSE;
		}
		// ���� ��������� ����� � ���������� ���������
		if (bSubSearch)
		{
			// ��������������� SQL ������� ������ � ���������� ���������
			// �� ������� �������� ��������� ������ ���������� ���������
			if (!MakeSelect(arDstSubFld, pTrn->sSubDst, _T(""), 
							TRUE, sDstSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDDSTSUB, strErr);
				return FALSE;
			}
		}
		// ��� ������ ��������� ��������� � ���������
		if (pTrn->iSubPrc == DST_FULL)
		{
			// ��������� ������ �� ��������
			// ��� �������� ����� ���.���������
			if (!SetDstRefs(arSrcSubFld, REF_SUB, TRUE, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SRCSUB, strErr);
				return FALSE;
			}
			// ��������������� SQL ������� ������ � ���.���������
			// �� ������� �������� ��������� ������ ����������
			if (!MakeSelect(arSrcSubFld, sSrcSTable, _T(""), 
							TRUE, sSrcSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDSRCSUB, strErr);
				return FALSE;
			}
			// ��������������� SQL Select ���������� ���������
			// �� ��������� �������
			if (!MakeSelect(arDstSubFld, pTrn->sSubDst, pTrn->sSubCond, 
							FALSE, sDstSubSQL, strErr))
			{
				strErr = GetErrMsg(ER_PREP_SELDSTSUB, strErr);
				return FALSE;
			}
		}
		// ���������� ������� ������ ���������� ���������
		if (!FillCmdArray(pTrn->sSubCmd, arDstSubCmd))
		{
			strErr = GetErrMsg(ER_SNTX_CMDSUB, strErr);
			return FALSE;
		}
		// ���������� ������� ������ ���������� ���������
		if (!ParseCmdArr(pDst, REF_SUB, arDstSubCmd, FALSE, strErr))
		{
			strErr = GetErrMsg(ER_PREP_CMDSUB, strErr);
			return FALSE;
		}
		// ���������� �������� ������ CHANGE � ������� ����� ���������
		if (!ParseFldCmd(arDstSubFld, arDstSubCmd, strErr))
		{
			return FALSE;
		}
		// ��������� ������������� ���������� ������
		// ��� ��������� ������ ������ ���������� ���������
		if (FindEvent(arDstSubCmd, EV_ON_INSERT) >= 0 ||
			FindEvent(arDstSubCmd, EV_ON_UPDATE) >= 0 ||
			FindEvent(arDstSubCmd, EV_ON_DELETE) >= 0 ||
			pTrn->iSchema == TRN_COMPLEX)
			bSubRowCmd = TRUE;
		// ��������� ������������� ���������� ������ OnChange
		// ��� ��������� ������ � ����� ���������� ���������
		if (FindEvent(arDstSubCmd, EV_ON_CHANGE) >= 0)
			bSubFldCmd = TRUE;
	}
#endif
	// ���������� ������ ���������
	if (bMessage)
	{
		if (!ParseStrMsg(pTrn->sMsg, strErr))
		{
			strErr = GetErrMsg(ER_PREP_MSG, strErr);
			return FALSE;
		}
	}
	// ������� �������������
	bValid = TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// ���������� ����������

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
		// ����������� �� ����� � Trial ������
		strErr.FormatMessage(IDS_ER_TRIAL_COMPLEX);
		return FALSE;
	}
#endif

	// ���������� ���������
	BOOL bRetCode	= TRUE;		// ������������ ��������
	BOOL bRetTabRow = TRUE;		// ��� ���������� ������
	BOOL bSubError	= FALSE;	// ������� ������ � Detail
	RETCODE nRet;				// ��������� SQL ���������
	MOD_FLAG iMFlag;			// ���� ����������� ������
	OP_TYPE iOpType;			// ��� ��������� SQL
	CString strCapt;			// ������ ���������
	CString strStat;			// ������ �������
	CString strSav;				// ������ ����������
	CString strMsg;				// ������ ���������
	CString strRet;				// ������ ����������
	CTime	curTime;			// ������� �����
	int		mSize;				// ������ ������� ������� ���������
	int		i,k;				// �������� ������
	int		iPos;				// ����� �������
	long	n,nCount;			// �������� �����

	bBreaked = FALSE;			// ������� ����������
	bShowErr = TrnParm.iShowErr;// ������� ������ ������

	if (bCancel) goto end;

	AfxGetApp()->BeginWaitCursor();
	// �������� ���� ���������
	pDlg = new CProgressDlg;
	pDlg->Create(IDD_PROGRESSDLG, AfxGetApp()->m_pMainWnd);
	pDlg->SetWindowText(pTrn->sName);
	pDlg->SetBreakEnable(TRUE);
	pDlg->SetRange(0, 100);
	pDlg->SetPos(0);

	////////////////////////////////////////////////////////////////
	//              ���������� � ����������
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_PREPARE);
	pDlg->SetMessageText(strMsg);
	// ��������� ������ COMMIT ��� ���������
	if (bTxnCommit)
	{
		::SQLSetConnectAttr(pDst->hdbc, SQL_ATTR_AUTOCOMMIT, 
			(SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_IS_INTEGER);
	}
	// ���� ������ ���������
	if (pTrn->iTabPrc == DST_FULL)
	{
		// ���������� ���������� SQL Select ��������� ���������
		if (!PrepareSQLParms(pDst, &arDstTabPrm, 
			(pTrn->sTabOwn.IsEmpty()) ? pDst->sUserName : pTrn->sTabOwn, sDstTabSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_PARDSTTAB, strErr);
			bRetCode = FALSE; goto end;
		}
		// ���������� ���������� SQL ������� � ��������� ���������
		if (!PrepareSQLParms(pSrc, &arSrcTabPrm, pSrc->sUserName, sSrcTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDSRCTAB, strErr);
			bRetCode = FALSE; goto end;
		}
	}
	if (bCancel) goto end;
	// ���������� SQL-���������� ������� ���������
	// ���������� ��������� SQL Insert ������� ���������
	if (!MakeInsert(arDstTabFld, arTabInsPrm, pTrn->sTabDst, 
					sDstTabIns, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_INSTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// ���������� ��������� SQL Update ������� ���������
	if (!MakeUpdate(arDstTabFld, arTabUpdPrm, pTrn->sTabDst, 
					sDstTabUpd, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_UPDTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// ���������� ��������� SQL Delete ������� ���������
	if (!MakeDelete(arDstTabFld, arTabDelPrm, pTrn->sTabDst, 
					sDstTabDel, REF_TAB, strErr))
	{
		strErr = GetErrMsg(ER_PREP_DELTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// ���������� ������� ������ ������� ���������
	if (!ParseCmdArr(pDst, REF_TAB, arDstTabCmd, TRUE, strErr))
	{
		strErr = GetErrMsg(ER_PREP_CMDTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	if (bCancel) goto end;

#ifndef _TRIAL_VER
	// ���������� SQL-���������� ���������� ���������
	if (pTrn->iSchema == TRN_COMPLEX)
	{
		// ���� ������ ���������
		if (pTrn->iSubPrc == DST_FULL)
		{
			// ���������� ���������� SQL Select ������������ ���������
			if (!PrepareSQLParms(pDst, &arDstSubPrm, 
				(pTrn->sSubOwn.IsEmpty()) ? pDst->sUserName : pTrn->sSubOwn, sDstSubSQL, strErr))
			{
				strErr = GetErrMsg(ER_PREP_PARDSTSUB, strErr);
				bRetCode = FALSE; goto end;
			}
			// ���������� ���������� SQL ������� � ������������ ���������
			if (!PrepareSQLParms(pSrc, &arSrcSubQry, pSrc->sUserName, sSrcSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDSRCSUB, strErr);
				bRetCode = FALSE; goto end;
			}
		}
		// ���������� ��������� SQL Insert ���������� ���������
		if (!MakeInsert(arDstSubFld, arSubInsPrm, pTrn->sSubDst, 
						sDstSubIns, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_INSSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// ���������� ��������� SQL Update ���������� ���������
		if (!MakeUpdate(arDstSubFld, arSubUpdPrm, pTrn->sSubDst, 
						sDstSubUpd, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_UPDSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// ���������� ��������� SQL Delete ���������� ���������
		if (!MakeDelete(arDstSubFld, arSubDelPrm, pTrn->sSubDst, 
						sDstSubDel, REF_SUB, strErr))
		{
			strErr = GetErrMsg(ER_PREP_DELSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// ���������� ������� ������ ���������� ���������
		if (!ParseCmdArr(pDst, REF_SUB, arDstSubCmd, TRUE, strErr))
		{
			strErr = GetErrMsg(ER_PREP_CMDSUB, strErr);
			bRetCode = FALSE; goto end;
		}
	}
#endif

	if (bCancel) goto end;
	////////////////////////////////////////////////////////////////
	// ���������� ������� OnStart
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_TABSTART);
	pDlg->SetMessageText(strMsg);
	nRet = ExecSQLCmd(pDst, arDstTabCmd, EV_ON_START, NULL, strErr);
	if (nRet == SQL_ERROR)
	{
		nErr++;
		// ������ ������ ������ � ����
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// ����� ��������� �� ������
		nRet = ShowMsg(pTrn->sName, strErr);
		if (nRet == IDCANCEL)	goto end;
		else if (nRet == IDNO)	bShowErr = FALSE;
	}
	else if (nRet == SQL_SUCCESS_WITH_INFO)
	{
		WarningHandle(strErr, nMsg);
	}
	if (bTxnCommit)					// ���������� ����������
	{
		::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
	}
	// ���� ��� ��������� - �� ������ ���������,
	// �� ������� �� ������ �������� ������
	if (pTrn->iTabPrc != DST_FULL) goto trn;
	////////////////////////////////////////////////////////////////
	//              ��������� ��������� (��� ������� ���������)
	////////////////////////////////////////////////////////////////
	if (bCancel) goto end;
	strMsg.LoadString(IDS_MS_READ_DST);
	pDlg->SetMessageText(strMsg);
	// ������� ����� ����� SQL Select ��������� ���������
	nCount = ExecSQLCount(pDst, sDstTabSQL, arDstTabPrm, strErr);
	if (nCount == 0) goto trn;
	if (nCount <  0)
	{
		strErr = GetErrMsg(ER_COUNT_DSTTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	AfxGetApp()->BeginWaitCursor();
	////////////////////////////////////////////////////////////////
	// ���������� SQL Select ��������� ��������� (��� Fetch)
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
	//              ���� ���������� ������
	////////////////////////////////////////////////////////////////
	if (bCancel) goto end;
	// ���� �� �������� ������� ���������
	for (n = 1, iPos = 0;; n++)
	{
		if (bCancel) goto end;
		// �������� �������
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE;	goto end;
		}
		// ��������� ������ �������
		strStat.FormatMessage(IDS_MS_FMT_ROWS, n, nCount);
		pDlg->SetStatusText(strStat);
		// ��������� �������
		if ((int)(n*100/nCount) > iPos)
		{
			iPos = (int)(n*100/nCount);
			pDlg->SetPos(iPos);
		}
		// ���������� ������ ������ �� ���������
		nRet = FetchData(hDstTabSQL, arDstTabFld, strErr);
		// �������� ���������� ������
		if (nRet == SQL_NO_DATA)	break;		// ��� ������
		else if (nRet == SQL_ERROR)				// ������ ������
		{
			strErr = GetErrMsg(ER_READ_DSTTAB, strErr);
			goto msg;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_READ_DSTTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// ���������� SQL ������� � ������� ���������
		// �� �������� ��������� ������ �������
		nRet = ExecSelect(pSrc, hSrcTabSQL, arSrcTabFld, arSrcTabPrm, 
							sSrcTabQry, TRUE, strErr);
		switch (nRet)
		{
		case SQL_ERROR:					// ������ ������
		{
			strErr = GetErrMsg(ER_FIND_SRCTAB, strErr);
			goto msg;
		}
		case SQL_SUCCESS_WITH_INFO:		// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_FIND_SRCTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		case SQL_SUCCESS:				// ������ �������
		{
			// ��������� ������ � ��������� �� ������
			if (!CompareRef(arDstTabFld, OP_UPDATE, iMFlag, strErr))
			{
				// ������ ������ ������ � ����
				// � ���������� ����������
				strErr = GetErrMsg(ER_COMP_DSTTAB, strErr);
				WarningHandle(strErr, nMsg);
			}
			if (iMFlag == MOD_TRG)	// ���� �������� �������
			{
				// ���������� ������ � ����� ��� ��������
				AddBufRow(arTabRowBuf,arSrcTabFld,arDstTabFld,n,
							ROW_UNKNOWN,FALSE,FALSE);
			}
			else						// ������� �� ��������
			{
				continue;
			}
			break;
		}
		case SQL_NO_DATA:				// �� ������� ������
		{
			// ���������� ������ � ����� ��� ��������
			AddBufRow(arTabRowBuf,arSrcTabFld,arDstTabFld,n,
						ROW_UNKNOWN,FALSE,FALSE);
			break;
		}
		}
#ifdef _TRIAL_VER
		// ����������� �� ���������� ������� � Trial ������
		if (n > 99) break;
#endif
		continue;

msg:	nErr++;
		// �������������� ������ ���������
		strErr = FormatErr(strErr);
		// ������ ������ ������ � ����
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// ���� ���� ���������� � �������
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
		// ����� ������� ���������
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
		// ����������� �� ���������� ������� � Trial ������
		if (n > 99) break;
#endif
	}
	// ������������ ������������ SQL
	FreeSQLStmt(hSrcTabSQL, TRUE);
	FreeSQLStmt(hDstTabSQL, TRUE);
	// ������� ������ � ���������
	ResetFldArr(arSrcTabFld);
	////////////////////////////////////////////////////////////////
	//              ���� ��������� ������
	////////////////////////////////////////////////////////////////
	if (bCancel) goto end;
	// ��������� ������ � ���������
	strMsg.LoadString(IDS_MS_PROCESS_DST);
	pDlg->SetMessageText(strMsg);
	mSize = arDstTabFld.GetSize();
	// ��������� ������ ����� (�������� ����� �� �������)
	for (i = 0; i < arTabRowBuf.GetSize(); i++)
	{
		if (bCancel) goto end;
		// �������� �������
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// ��������� ������ ���������
		strStat.FormatMessage(IDS_MS_FMT_ROWS, arTabRowBuf[i].nRow, nCount);
		pDlg->SetSubText(strStat);
		// ��������� �������
		if ((int)(arTabRowBuf[i].nRow*100/nCount) > iPos)
		{
			iPos = (int)(arTabRowBuf[i].nRow*100/nCount);
			pDlg->SetPos(iPos);
		}
		// ��������� ������ ���������
		if (bMessage)
		{
			strMsg = FormatMsg(pTrn->sMsg);
			pDlg->SetMessageText(strMsg);
		}
		// ������� ������ � ��������
		for (k = 0; k < mSize; k++)
		{
			arDstTabFld[k].bMod  = FALSE;
			arDstTabFld[k].vData = arTabRowBuf[i].pArDstFld->GetAt(k).vData;
		}
		// ��������� ������
		if (bComplex)				// �������� ������ �� ����������
		{
			if (!RunDetail(OP_DELETE,TRUE,fileLog,nMsg,nErr,strErr))
				goto err;
		}
		if (bTabRowCmd)				// ���������� SQL-�������
		{
			// ���������� ������� OnDelete
			nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_DELETE,NULL,strErr);
	
			if (nRet == SQL_ERROR)	// ������ SQL-�������
			{
				if (bComplex)		// �������������� ����������
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
		// ���������� �������� �������� (�� ���������)
		nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
								sDstTabDel, FALSE, strErr);
		// �������� ���������� ��������
		if (nRet == SQL_ERROR)			// ������ ��������
		{
			strErr  = GetErrMsg(ER_DELTAB, strErr);
			strErr += FormatUpd(ER_DELTAB);
			if (!bTxnCommit)			// ���������� �������� �������
			{
				nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_DELETE,NULL,strRet);
				if (nRet == SQL_ERROR)	// ������ �������� �������
				{
					ErrorHandle(ER_BACK_CMD, strRet, nErr);
				}
			}
			if (bComplex)				// �������������� ����������
			{
				BackDetail(TRUE, fileLog, nMsg, nErr);
			}
			goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_DELTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		if (bTxnCommit)					// ���������� ����������
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
		// ��������� ������� ������ � ������
		arTabRowBuf[i].nStatus = ROW_DELETED;
		// ������ ��������� � ���� ���������
		if (bMessage && bFileMsg)
		{
			strRet.FormatMessage(IDS_MS_DELETED, strMsg);
			fileMsg.WriteString(strRet + _T("\n"));
		}
#ifdef _TRIAL_VER
		// ����������� �� ���������� ������� � Trial ������
		if (i > 99) break;
#endif
		continue;

err:	nErr++;
		// �������������� ������ ���������
		strErr = FormatErr(strErr);
		// ������ ������ ������ � ����
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// ���� ���� ���������� � �������
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
		// ����� ������� ���������
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
		// ����������� �� ���������� ������� � Trial ������
		if (i > 99) break;
#endif
	}
	// ������� ������ �����
	ClearBufArr(arTabRowBuf);

	////////////////////////////////////////////////////////////////
	//              �������� ������
	////////////////////////////////////////////////////////////////
trn:if (bCancel) goto end;
	strMsg.LoadString(IDS_MS_READ_SRC);
	pDlg->SetMessageText(strMsg);
	pDlg->SetStatusText(_T(""));
	pDlg->SetPos(0);
	AfxGetApp()->BeginWaitCursor();
	// ���������� ���������� SQL Select ��������� ���������
	if (bCancel) goto end;
	if (!PrepareSQLParms(pSrc, &arSrcTabPrm, pSrc->sUserName, pTrn->sTabSQL, strErr))
	{
		strErr = GetErrMsg(ER_PREP_PARSRCTAB, strErr);
		bRetCode = FALSE; goto end;
	}
	// ���� ��������� ����� ������ � ������� ���������
	if (bTabSearch)
	{
		// ���������� ���������� SQL ������� � ��������� ���������
		if (!PrepareSQLParms(pDst, &arDstTabPrm, 
			(pTrn->sTabOwn.IsEmpty()) ? pDst->sUserName : pTrn->sTabOwn, sDstTabQry, strErr))
		{
			strErr = GetErrMsg(ER_PREP_FNDDSTTAB, strErr);
			bRetCode = FALSE; goto end;
		}
	}
	// ��� ����� COMPLEX
	if (bCancel) goto end;

#ifndef _TRIAL_VER
	if (pTrn->iSchema == TRN_COMPLEX)
	{
		// ���������� ���������� SQL Select ������������ ���������
		if (!PrepareSQLParms(pSrc, &arSrcSubPrm, pSrc->sUserName, pTrn->sSubSQL, strErr))
		{
			strErr = GetErrMsg(ER_PREP_PARSRCSUB, strErr);
			bRetCode = FALSE; goto end;
		}
		// ���� ��������� ����� ������ � ���������� ���������
		if (bSubSearch)
		{
			// ���������� ���������� SQL ���������� ������
			// � ����������� ���������
			if (!PrepareSQLParms(pDst, &arDstSubQry, 
				(pTrn->sSubOwn.IsEmpty()) ? pDst->sUserName : pTrn->sSubOwn, sDstSubQry, strErr))
			{
				strErr = GetErrMsg(ER_PREP_FNDDSTSUB, strErr);
				bRetCode = FALSE; goto end;
			}
		}
	}
#endif
	// ������� ����� ����� SQL Select ��������� ���������
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
	// ���������� SQL Select ��������� ��������� (��� Fetch)
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
	//              ���� �������� ������
	////////////////////////////////////////////////////////////////
	// ������� ������ �� ��������� � ��������
	if (bCancel) goto end;
	strMsg.LoadString(IDS_MS_SEND);
	pDlg->SetMessageText(strMsg);
	// ���� �� �������� ������� ���������
	for (n = 1, iPos = 0;; n++)
	{
		if (bCancel) goto end;
		// �������� �������
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE;	goto end;
		}
		// ��������� ������ �������
		strStat.FormatMessage(IDS_MS_FMT_ROWS, n, nCount);
		pDlg->SetStatusText(strStat);
		// ��������� �������
		if ((int)(n*100/nCount) > iPos)
		{
			iPos = (int)(n*100/nCount);
			pDlg->SetPos(iPos);
		}
		// ���������� ������ ������ �� ���������
		nRet = FetchData(hSrcTabSQL, arSrcTabFld, strErr);
		// �������� ���������� ������
		if (nRet == SQL_NO_DATA)	break;		// ��� ������
		else if (nRet == SQL_ERROR)				// ������ ������
		{
			strErr = GetErrMsg(ER_READ_SRCTAB, strErr);
			goto brk;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_READ_SRCTAB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// ��������� ������ ���������
		if (bMessage)
		{
			strMsg = FormatMsg(pTrn->sMsg);
			pDlg->SetMessageText(strMsg);
		}
		// ��������� ������
		if (bTabSearch)					// ���� ����� ����� ������..
		{
			// ���������� SQL Select �� ������� ���������
			// �� �������� ��������� ������ �������
			nRet = ExecSelect(pDst, hDstTabSQL, arDstTabFld, arDstTabPrm,
								sDstTabQry, TRUE, strErr);
			switch (nRet)
			{
			case SQL_ERROR:				// ������ ������
			{
				strErr = GetErrMsg(ER_FIND_DSTTAB, strErr);
				goto brk;
			}
			case SQL_NO_DATA:			// �� ������� ������
			{
				iOpType = OP_INSERT;	// ���������� ������
				nRet = ExecUpdate(pDst, hDstTabIns, iOpType, arTabInsPrm, 
										sDstTabIns, TRUE, strErr);
				break;
			}
			case SQL_SUCCESS_WITH_INFO:	// ��������������
			{
				// ������ ������ �������������� � ����
				// � ���������� ����������
				strErr = GetErrMsg(ER_FIND_DSTTAB, strErr);
				WarningHandle(strErr, nMsg);
			}
			case SQL_SUCCESS:			// ������ �������
			{
				// ��������� ������ � ��������� �� ������
				if (!CompareRef(arDstTabFld, OP_UPDATE, iMFlag, strErr))
				{
					// ������ ������ ������ � ����
					// � ���������� ����������
					strErr = GetErrMsg(ER_COMP_DSTTAB, strErr);
					WarningHandle(strErr, nMsg);
				}
				switch (iMFlag)			// �������� ����� �����������
				{
				case MOD_NO:			// ������ �� ����������
				{
					iOpType = OP_UPDATE;// ��� �������� - Update
					goto cont;			// ������� � ��������� ����������
					break;
				}
				case MOD_YES:			// ������ ����������
				case MOD_CMD:			// �������-�������
				case MOD_EXEC:			// ���� ����������
				{
					iOpType = OP_UPDATE;	// ���������� ������
					nRet = ExecUpdate(pDst, hDstTabUpd, iOpType, arTabUpdPrm, 
										sDstTabUpd, TRUE, strErr);
					break;
				}
				case MOD_BACK:			// ���� ������
				{
					if (bComplex)		// ����� ������ � ����������
					{
						if (!RunCmdDetail(OP_DELETE,fileLog,nMsg,nErr,strErr))
							goto brk;
					}
					if (bTabRowCmd)		// ���������� SQL-�������
					{
						// ���������� ������� OnDelete
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_DELETE,NULL,strErr);
	
						if (nRet == SQL_ERROR)	// ������ SQL-�������
						{
							if (bComplex)	// �������������� ����������
							{
								BackDetail(TRUE, fileLog, nMsg, nErr);
							}
							goto brk;
						}
						else if (nRet == SQL_SUCCESS_WITH_INFO)
						{
							WarningHandle(strErr, nMsg);
						}
						if (bTxnCommit)		// ���������� ����������
						{
							::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
						}
					}
					// ������� ������ �����
					ClearBufArr(arSubRowBuf);

					// ������ ��������� � ���� ���������
					if (bMessage && bFileMsg)
					{
						strRet.FormatMessage(IDS_MS_REVERSED, strMsg);
						fileMsg.WriteString(strRet + _T("\n"));
					}
					iOpType = OP_UPDATE;	// ���������� ������
					nRet = ExecUpdate(pDst, hDstTabUpd, iOpType, arTabUpdPrm, 
										sDstTabUpd, TRUE, strErr);
					break;
				}
				case MOD_TRG:			// ������� ������
				{
					if (bComplex)		// �������� ������ �� ����������
					{
						if (!RunDetail(OP_DELETE,TRUE,fileLog,nMsg,nErr,strErr))
							goto brk;
					}
					if (bTabRowCmd)		// ���������� SQL-�������
					{
						// ���������� ������� OnDelete
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_DELETE,NULL,strErr);
	
						if (nRet == SQL_ERROR)	// ������ SQL-�������
						{
							if (bComplex)		// �������������� ����������
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
					// ���������� �������� �������� (�� ���������)
					nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
											sDstTabDel, FALSE, strErr);
					// �������� ���������� ��������
					if (nRet == SQL_ERROR)		// ������ ��������
					{
						strErr  = GetErrMsg(ER_DELTAB, strErr);
						strErr += FormatUpd(ER_DELTAB);
						if (!bTxnCommit)		// ���������� �������� �������
						{
							nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_DELETE,NULL,strRet);
							if (nRet == SQL_ERROR)	// ������ �������� �������
							{
								ErrorHandle(ER_BACK_CMD, strRet, nErr);
							}
						}
						if (bComplex)			// �������������� ����������
						{
							BackDetail(TRUE, fileLog, nMsg, nErr);
						}
						goto brk;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						// ������ ������ �������������� � ����
						// � ���������� ����������
						strErr = GetErrMsg(ER_DELTAB, strErr);
						WarningHandle(strErr, nMsg);
					}
					if (bTxnCommit)				// ���������� ����������
					{
						::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
					}
					iOpType = OP_INSERT;		// ���������� ������
					nRet = ExecUpdate(pDst, hDstTabIns, iOpType, arTabInsPrm, 
											sDstTabIns, TRUE, strErr);
					break;
				}
				default:
				{
					iOpType = OP_UPDATE;	// ���������� ������
					nRet = ExecUpdate(pDst, hDstTabUpd, iOpType, arTabUpdPrm, 
										sDstTabUpd, TRUE, strErr);
					break;
				}
				}
				break;
			}
			}
		}
		else							// ���� ����������� ����������..
		{
			iOpType = OP_INSERT;		// ���������� ������
			nRet = ExecUpdate(pDst, hDstTabIns, iOpType, arTabInsPrm, 
									sDstTabIns, TRUE, strErr);
		}
		// �������� ���������� ��������
		if (nRet == SQL_ERROR)			// ������ ������
		{
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSTAB, strErr);
							strErr+= FormatUpd(ER_INSTAB); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDTAB, strErr);
							strErr+= FormatUpd(ER_UPDTAB); break;
			}
			goto brk;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSTAB, strErr); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDTAB, strErr); break;
			}
			WarningHandle(strErr, nMsg);
		}
		if (bTabRowCmd)					// ���������� SQL-�������
		{
			switch (iOpType)			// ��� ��������
			{
			case OP_INSERT:				// ������� OnInsert
			{
				nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_INSERT,NULL,strErr);
				break;
			}
			case OP_UPDATE:				// ������� OnUpdate
			{
				switch (iMFlag)			// ���� �����������
				{
				case MOD_EXEC:			// �� ��������� ��� 
				case MOD_BACK:			// ����������/������
				{
					nRet = SQL_SUCCESS;
					break;
				}
				default:				// ������� OnUpdate
				{
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_UPDATE,NULL,strErr);
					break;
				}
				}
				break;
			}
			}
			if (nRet == SQL_ERROR)		// ������ SQL-�������
			{
				// ����� ��������
				if (!bTxnCommit && (iOpType == OP_INSERT) && bComplex)
				{
					// ���������� ������ ������
					strSav = strErr;
					// ���������� ������� CallBack
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
				goto errCmd;			// ������� �� ��������� ������
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
		}
		if (bTabFldCmd)					// ��������� �������� OnChange
		{
			if ((iOpType == OP_UPDATE) && (iMFlag == MOD_CMD))
			{
				// ���������� ������� OnChange ��� ���������� �����
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
		if (bTxnCommit)					// ���������� ����������
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}


		/////////////////////////////////////////////////////////////
		// ��������� ����������
		/////////////////////////////////////////////////////////////
cont:	if (bComplex)					// ��������� ����������
		{
			if (iOpType == OP_INSERT)
			{
				// ���������� ������ � ������� ���������
				// ��� ��������� ������ �� ������ #
				// � �������� ���������� ���������
				SetFldData(arDstTabFld, iOpType, strErr);
				// ��������� ����� ���������� ������
				if (nRet == SQL_ERROR)	bRetTabRow = FALSE;
				else					bRetTabRow = TRUE;
			}
			else
			{
				// ��������� ����� ���������� ������
				switch (iMFlag)	// ���� �����������
				{
				case MOD_EXEC:	bRetTabRow = FALSE;	break;
				case MOD_BACK:	bRetTabRow = FALSE;	break;
				default:		bRetTabRow = TRUE;	break;
				}
			}
			// ���������� ���������� ����������
			if (!RunDetail(iOpType,bRetTabRow,fileLog,nMsg,nErr,strErr))
			{
				if (bTabRowCmd)			// ���������� �������� �������
				{
					if ((iOpType == OP_INSERT) && bRetTabRow)
					{
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_INSERT,
											NULL,strRet);
					}
					else if ((iOpType == OP_UPDATE) && iMFlag)
					{
						switch (iMFlag)			// ���� �����������
						{
						case MOD_EXEC:			// �� ��������� ��� 
						case MOD_BACK:			// ����������/������
						{
							nRet = SQL_SUCCESS;
							break;
						}
						default:				// ������� ReUpdate
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
					if (nRet == SQL_ERROR)	// ������ �������� �������
					{
						ErrorHandle(ER_BACK_CMD, strRet, nErr);
						// �� ��������� ������
						goto brk;
					}
				}
				if (iOpType == OP_INSERT) // ���������� �������� ��������
				{
					// �������� ������ ����� ����������:
					// ���� � ������ �������� ������, 
					// ������ ���� ������ SQL-������� ����������, 
					// �� ���� �������� ������ ������� ����������
					if (arSubRowBuf.GetSize() > 0)	// ������� CallBack
					{
						nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_CALL_BACK,
											NULL,strRet);
					}
					else				// �������� ������ (�� ���������)
					{
						nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
												sDstTabDel, TRUE, strRet);
					}
				}
				else if ((iOpType == OP_UPDATE) && iMFlag)
				{
					if (bTabFldCmd && (iMFlag == MOD_CMD)) // ����� OnChange
					{
						// ���������� ReChange ��� ���������� �����
						nRet = ExecFldCmd(pDst, arDstTabFld, arDstTabCmd,
										  EV_RE_CHANGE, strRet);
						if (nRet == SQL_ERROR)
						{
							ErrorHandle(ER_BACK_CMD, strRet, nErr);
							// �� ��������� ������
							goto brk;
						}
						else if (nRet == SQL_SUCCESS_WITH_INFO)
						{
							WarningHandle(strRet, nMsg);
						}
					}
					// �������������� ������ (�� ���������)
					nRet = ExecUpdate(pDst, hDstTabUpd, OP_UPDATE, arTabUpdPrm, 
											sDstTabUpd, FALSE, strRet);
				}
				else
				{
					nRet = SQL_SUCCESS;
				}
				if (nRet == SQL_ERROR)	// ������ �������� ��������
				{
					ErrorHandle(ER_BACK_OP, strRet, nErr);
					// �� ��������� ������
					goto brk;
				}
				else if (bTxnCommit)	// ���������� ����������
				{
					::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
				}
				goto brk;
			}
			if ((iOpType == OP_INSERT) && !bRetTabRow)
			{
				strErr = strSav;		// �������������� ������ ������
				goto brk;				// ������� �� ��������� ������
			}
		}
		// ������ ��������� � ���� ���������
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
		// ��������� ����� ����������
		/////////////////////////////////////////////////////////////
		if ((iOpType == OP_UPDATE) && (iMFlag == MOD_EXEC))
		{
			// ���������� ������ � ������� ���������
			// ��� ��������� ������ �� ������ #
			// � �������� ���������� ���������
			SetFldData(arDstTabFld, iOpType, strErr);

			if (bTabRowCmd)		// ���������� SQL-�������
			{
				// ���������� ������� OnInsert
				nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_ON_INSERT,NULL,strErr);

				if (nRet == SQL_ERROR)	// ������ SQL-�������
				{
					// ���������� ������� CallBack
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_CALL_BACK, NULL,strRet);

					if (nRet == SQL_ERROR)	// ������ �������� ��������
					{
						ErrorHandle(ER_BACK_OP, strRet, nErr);
						// �� ��������� ������
						goto brk;
					}
					else if (bTxnCommit)	// ���������� ����������
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
				if (bTxnCommit)			// ���������� ����������
				{
					::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
				}
			}
			if (bComplex)		// ���������� ������ � ����������
			{
				if (!RunCmdDetail(OP_INSERT,fileLog,nMsg,nErr,strErr))
				{
					// ���������� ������� ReInsert
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_INSERT,NULL,strRet);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
					{
						ErrorHandle(ER_BACK_OP, strRet, nErr);
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strRet, nMsg);
					}
					// ���������� ������� CallBack
					nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_CALL_BACK, NULL,strRet);

					if (nRet == SQL_ERROR)	// ������ �������� ��������
					{
						ErrorHandle(ER_BACK_OP, strRet, nErr);
						// �� ��������� ������
						goto brk;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strRet, nMsg);
					}
					if (bTxnCommit)	// ���������� ����������
					{
						::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
					}
					goto brk;
				}
				// ������� ������ �����
				ClearBufArr(arSubRowBuf);
			}
			// ������ ��������� � ���� ���������
			if (bMessage && bFileMsg)
			{
				strRet.FormatMessage(IDS_MS_EXECUTED, strMsg);
				fileMsg.WriteString(strRet + _T("\n"));
			}
		}
#ifdef _TRIAL_VER
		// ����������� �� ���������� ������� � Trial ������
		if (n > 99) break;
#endif
		continue;

		/////////////////////////////////////////////////////////////
		// ��������� ������
		/////////////////////////////////////////////////////////////
		// ������ �������� OnChange
errEvt:	if (bTabRowCmd && !bTxnCommit)	// ���������� �������� �������
		{
			nRet = ExecSQLCmd(pDst,arDstTabCmd,EV_RE_UPDATE,NULL,strRet);
			if (nRet == SQL_ERROR)		// ������ �������� �������
			{
				ErrorHandle(ER_BACK_CMD, strRet, nErr);
			}
		}
		// ������ SQL-�������
errCmd:	if (!bTxnCommit)				// ���������� �������� ��������
		{
			switch (iOpType)			// ��� �������� ��������
			{
			case OP_INSERT:				// �������� ������ (�� ���������)
				nRet = ExecUpdate(pDst, hDstTabDel, OP_DELETE, arTabDelPrm, 
										sDstTabDel, TRUE, strRet);
				break;
			case OP_UPDATE:				// �������������� (�� ���������)
				nRet = ExecUpdate(pDst, hDstTabUpd, OP_UPDATE, arTabUpdPrm, 
										sDstTabUpd, FALSE, strRet);
				break;
			}
			if (nRet == SQL_ERROR)		// ������ �������� ��������
			{
				ErrorHandle(ER_BACK_OP, strRet, nErr);
			}
		}

brk:	nErr++;
		// ���������� � ������ ������ ������ ���������
		strErr = FormatErr(strErr);
		// ������ ������ ������ � ����
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// ���� ���� ���������� � �������
		if (bBreaked)
		{
			bCancel = TRUE; goto end;
		}
		// ����� ������� ���������
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
		// ����������� �� ���������� ������� � Trial ������
		if (n > 99) break;
#endif
		continue;
	}
	////////////////////////////////////////////////////////////////
	// ���������� ������� OnEnd
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_TABEND);
	pDlg->SetMessageText(strMsg);
	nRet = ExecSQLCmd(pDst, arDstTabCmd, EV_ON_END, NULL, strErr);
	if (nRet == SQL_ERROR)
	{
		nErr++;
		// ������ ������ ������ � ����
		if (bFileLog) fileLog.WriteString(strErr + _T("\n"));
		// ����� ��������� �� ������
		if (bShowErr) ShowErr(pTrn->sName, strErr);
	}
	else if (nRet == SQL_SUCCESS_WITH_INFO)
	{
		WarningHandle(strErr, nMsg);
	}
	if (bTxnCommit)					// ���������� ����������
	{
		::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
	}

end:AfxGetApp()->EndWaitCursor();
	// ������������ ������������ SQL
	FreeAllStmt();
	// ������� ���� ��������
	ClearAllArr();
	// �������� ���� ���������
	if (pDlg != NULL)
	{
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
	return bRetCode;
}

/////////////////////////////////////////////////////////////////////
// ���������� ���������� � ����������

BOOL CTransfer::RunDetail(OP_TYPE iMainType, BOOL bRunCmd, 
						  CStdioFile &fileLog, UINT &nMsg, UINT &nErr, 
						  CString &strErr) 
{
	// ���������:
	// iMainType - ��� �������� �������� �������
	// bRunCmd   - ������� ���������� SQL-�������
	// &fileLog  - ������ �� ���� ���������
	// &nErr     - ������ �� ������� ������
	// &nMsg     - ������ �� ������� ��������������
	// &strErr   - ������ �� ������ ������
	// ���������� ���������
	BOOL bRetCode = TRUE;		// ������������ ��������
	RETCODE nRet;				// ��������� SQL ���������
	MOD_FLAG iMFlag;			// ���� ����������� ������
	OP_TYPE iOpType;			// ��� ��������� SQL
	CString strSav;				// ������ ����������
	CString strMsg;				// ������ ���������
	CString strRet;				// ������ ����������
	int		mSize;				// ������ ������� ���������� ���������
	int		i,j;				// �������� ������
	long	n;					// ������� �����

	// ������� ������ �����
	ClearBufArr(arSubRowBuf);

	////////////////////////////////////////////////////////////////
	// ���������� ������� OnStart
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
		if (bTxnCommit)					// ���������� ����������
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
	}
	// ���� �� �������� ������ � �� ������ ���������, 
	// �� ������� �� ������ �������� ������
	if (iMainType != OP_DELETE && pTrn->iSubPrc != DST_FULL)
		goto trn;

	////////////////////////////////////////////////////////////////
	//              ��������� ���������
	////////////////////////////////////////////////////////////////
	strMsg.LoadString(IDS_MS_READ_SUBDST);
	pDlg->SetSubText(strMsg);
	////////////////////////////////////////////////////////////////
	// ���������� SQL Select ������������ ��������� (��� Fetch)
	////////////////////////////////////////////////////////////////
	if (ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubPrm, 
					sDstSubSQL, FALSE, strErr) == SQL_ERROR)
	{
		strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
		bRetCode = FALSE; goto err;
	}
	////////////////////////////////////////////////////////////////
	//              ���� ���������� ������
	////////////////////////////////////////////////////////////////
	// ���� �� ���������� ���������
	for (n = 1;; n++)
	{
		if (bCancel) goto end;
		// �������� �������
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			// ������� ������ �����
			ClearBufArr(arSubRowBuf);
			// ����� ��� ��������������
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// ��������� ������ ���������
		strMsg.FormatMessage(IDS_MS_FMT_SUBPRC, n);
		pDlg->SetSubText(strMsg);
		// ���������� ������ ������ �� ���������� ���������
		nRet = FetchData(hDstSubSQL, arDstSubFld, strErr);
		// �������� ���������� ������
		if (nRet == SQL_NO_DATA)	break;		// ��� ������
		else if (nRet == SQL_ERROR)				// ������ ������
		{
			// ������� ������ �����
			ClearBufArr(arSubRowBuf);
			// ����� ��� ��������������
			strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// ���� �������� ��������, �� ������ � ���������
		if (iMainType == OP_DELETE) nRet = SQL_NO_DATA;
		else
		{
			// ���������� SQL ������� � ���������� ���������
			// �� �������� ��������� ������ ����������
			nRet = ExecSelect(pSrc, hSrcSubSQL, arSrcSubFld, arSrcSubQry, 
								sSrcSubQry, TRUE, strErr);
		}
		switch (nRet)
		{
		case SQL_ERROR:					// ������ ������
		{
			// ������� ������ �����
			ClearBufArr(arSubRowBuf);
			// ����� ��� ��������������
			strErr = GetErrMsg(ER_FIND_SRCSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		case SQL_SUCCESS_WITH_INFO:		// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_FIND_SRCSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		case SQL_SUCCESS:				// ������ �������
		{
			// ��������� ������ � ��������� �� ������
			if (!CompareRef(arDstSubFld, OP_UPDATE, iMFlag, strErr))
			{
				// ������ ������ ������ � ����
				// � ���������� ����������
				strErr = GetErrMsg(ER_COMP_DSTSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			if (iMFlag == MOD_TRG)	// ���� �������� �������
			{
				// ���������� ������ � ����� ��� ��������
				AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
							ROW_UNKNOWN,FALSE,FALSE);
			}
			else						// ������� �� ��������
			{
				continue;
			}
			break;
		}
		case SQL_NO_DATA:				// �� ������� ������
		{
			// ���������� ������ � ����� ��� ��������
			AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
						ROW_UNKNOWN,FALSE,FALSE);
			break;
		}
		}
#ifdef _TRIAL_VER
		// ����������� �� ���������� ������� � Trial ������
		if (n > 99) break;
#endif
	}
	// ������������ ������������ SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// ������� ������ � ���������
	ResetFldArr(arSrcSubFld);
	////////////////////////////////////////////////////////////////
	//              ���� ��������� ������
	////////////////////////////////////////////////////////////////
	mSize = arDstSubFld.GetSize();
	// ��������� ������ ����� (�������� ����� �� ����������)
	for (i = 0; i < arSubRowBuf.GetSize(); i++)
	{
		if (bCancel) goto end;
		// �������� �������
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// ��������� ������ ���������
		strMsg.FormatMessage(IDS_MS_FMT_SUBDEL, arSubRowBuf[i].nRow);
		pDlg->SetSubText(strMsg);
		// ������� ������ � ��������
		for (j = 0; j < mSize; j++)
		{
			arDstSubFld[j].bMod  = FALSE;
			arDstSubFld[j].vData = arSubRowBuf[i].pArDstFld->GetAt(j).vData;
		}
		// ��������� ������
		if (bRunCmd && bSubRowCmd)	// ���������� SQL-�������
		{
			nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_DELETE,NULL,strErr);
			if (nRet == SQL_ERROR)	// ������ SQL-�������
			{
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
			arSubRowBuf[i].bRunCmd = TRUE;
		}
		// ���������� �������� �������� (�� ���������)
		nRet = ExecUpdate(pDst, hDstSubDel, OP_DELETE, arSubDelPrm, 
								sDstSubDel, FALSE, strErr);
		// �������� ���������� ��������
		if (nRet == SQL_ERROR)			// ������ ��������
		{
			strErr  = GetErrMsg(ER_DELSUB, strErr);
			strErr += FormatUpd(ER_DELSUB);
			if (bRunCmd && !bTxnCommit)	// ���������� �������� �������
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,NULL,strRet);
				if (nRet == SQL_ERROR)	// ������ �������� �������
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
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_DELSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		if (bTxnCommit)					// ���������� ����������
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
		// ��������� ������� ������ � ������
		arSubRowBuf[i].nStatus = ROW_DELETED;

#ifdef _TRIAL_VER
		// ����������� �� ���������� ������� � Trial ������
		if (i > 99) break;
#endif
	}
	// ���� �������� ��������, ��������� ���������
	if (iMainType == OP_DELETE) goto end;


	////////////////////////////////////////////////////////////////
	//              �������� ������
	////////////////////////////////////////////////////////////////
trn:strMsg.LoadString(IDS_MS_READ_SUBSRC);
	pDlg->SetSubText(strMsg);
	////////////////////////////////////////////////////////////////
	// ���������� SQL Select ������������ ��������� (��� Fetch)
	////////////////////////////////////////////////////////////////
	if (ExecSelect(pSrc, hSrcSubSQL, arSrcSubFld, arSrcSubPrm, 
					pTrn->sSubSQL, FALSE, strErr) == SQL_ERROR)
	{
		strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
		bRetCode = FALSE; goto err;
	}
	////////////////////////////////////////////////////////////////
	//              ���� �������� ������
	////////////////////////////////////////////////////////////////
	for (n = 1;; n++)
	{
		if (bCancel) goto end;
		// �������� �������
		pDlg->CheckState();
		if (pDlg->IsBreaked())
		{
			bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
			goto err;
		}
		// ��������� ������ ���������
		strMsg.FormatMessage(IDS_MS_FMT_SUBUPD, n);
		pDlg->SetSubText(strMsg);
		// ���������� ������ ������ �� ���������
		nRet = FetchData(hSrcSubSQL, arSrcSubFld, strErr);
		// �������� ���������� ������
		if (nRet == SQL_NO_DATA)	break;		// ��� ������
		else if (nRet == SQL_ERROR)				// ������ ������
		{
			strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
			WarningHandle(strErr, nMsg);
		}
		// ��������� ������
		if (bSubSearch && (iMainType == OP_UPDATE))	// ���� ����� �����
		{
			// ���������� SQL ������� � ���������� ���������
			// �� �������� ��������� ������ ����������
			nRet = ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubQry, 
								sDstSubQry, TRUE, strErr);
			switch (nRet)
			{
			case SQL_ERROR:				// ������ ������
			{
				strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
				bRetCode = FALSE; goto err;
			}
			case SQL_NO_DATA:			// �� ������� ������
			{
				iOpType = OP_INSERT;
				// ���������� ������ � ���������� ���������
				SetFldData(arDstSubFld, iOpType, strErr);
				// ���������� ������
				nRet = ExecUpdate(pDst, hDstSubIns, iOpType, arSubInsPrm, 
										sDstSubIns, TRUE, strErr);
				break;
			}
			case SQL_SUCCESS_WITH_INFO:	// ��������������
			{
				// ������ ������ �������������� � ����
				// � ���������� ����������
				strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			case SQL_SUCCESS:			// ������ �������
			{
				// ��������� ������ � ��������� �� ������
				if (!CompareRef(arDstSubFld, OP_UPDATE, iMFlag, strErr))
				{
					// ������ ������ ������ � ����
					// � ���������� ����������
					strErr = GetErrMsg(ER_COMP_DSTSUB, strErr);
					WarningHandle(strErr, nMsg);
				}
				switch (iMFlag)			// �������� ����� �����������
				{
				case MOD_NO:			// ������ �� ����������
				{
					iOpType = OP_UNDEF;
					continue;
				}
				case MOD_YES:			// ������ ����������
				case MOD_CMD:			// �������-�������
				case MOD_EXEC:			// ���� ����������
				case MOD_BACK:			// ���� ������
				{
					iOpType = OP_UPDATE;	// ����������� ������
					nRet = ExecUpdate(pDst, hDstSubUpd, iOpType, arSubUpdPrm, 
											sDstSubUpd, TRUE, strErr);
					break;
				}
				case MOD_TRG:			// ���� �������� �������
				{
					if (bRunCmd && bSubRowCmd)	// ���������� SQL-�������
					{
						nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_DELETE,NULL,strErr);
						if (nRet == SQL_ERROR)	// ������ SQL-�������
						{
							bRetCode = FALSE; goto err;
						}
						else if (nRet == SQL_SUCCESS_WITH_INFO)
						{
							WarningHandle(strErr, nMsg);
						}
					}
					// ���������� �������� �������� (�� ���������)
					nRet = ExecUpdate(pDst, hDstSubDel, OP_DELETE, arSubDelPrm, 
											sDstSubDel, FALSE, strErr);
					// �������� ���������� ��������
					if (nRet == SQL_ERROR)			// ������ ��������
					{
						strErr  = GetErrMsg(ER_DELSUB, strErr);
						strErr += FormatUpd(ER_DELSUB);
						if (bRunCmd && !bTxnCommit)	// ���������� �������� �������
						{
							nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,NULL,strRet);
							if (nRet == SQL_ERROR)	// ������ �������� �������
							{
								ErrorHandle(ER_BACK_CMD, strRet, nErr);
							}
						}
						bRetCode = FALSE; goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
					{
						// ������ ������ �������������� � ����
						// � ���������� ����������
						strErr = GetErrMsg(ER_DELSUB, strErr);
						WarningHandle(strErr, nMsg);
					}
					if (bTxnCommit)					// ���������� ����������
					{
						::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
					}
					// ���������� ������ � �����
					AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
								ROW_DELETED,bRunCmd,FALSE);

					iOpType = OP_INSERT;			// ���������� ������
					// ���������� ������ � ���������� ���������
					SetFldData(arDstSubFld, iOpType, strErr);
					nRet = ExecUpdate(pDst, hDstSubIns, iOpType, arSubInsPrm, 
											sDstSubIns, TRUE, strErr);
					break;
				}
				default:
				{
					iOpType = OP_UPDATE;	// ����������� ������
					nRet = ExecUpdate(pDst, hDstSubUpd, iOpType, arSubUpdPrm, 
											sDstSubUpd, TRUE, strErr);
					break;
				}
				}
				break;
			}
			}
		}
		else							// ���� ����������� ����������..
		{
			iOpType = OP_INSERT;		// ���������� ������
			// ���������� ������ � ���������� ���������
			SetFldData(arDstSubFld, iOpType, strErr);
			nRet = ExecUpdate(pDst, hDstSubIns, iOpType, arSubInsPrm, 
									sDstSubIns, TRUE, strErr);
		}
		// �������� ���������� ��������
		if (nRet == SQL_ERROR)			// ������ ������
		{
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSSUB, strErr);
							strErr+= FormatUpd(ER_INSSUB); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDSUB, strErr);
							strErr+= FormatUpd(ER_UPDSUB); break;
			}
			bRetCode = FALSE; goto err;
		}
		else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
		{
			// ������ ������ �������������� � ����
			// � ���������� ����������
			switch (iOpType) {
			case OP_INSERT:	strErr = GetErrMsg(ER_INSSUB, strErr); break;
			case OP_UPDATE:	strErr = GetErrMsg(ER_UPDSUB, strErr); break;
			}
			WarningHandle(strErr, nMsg);
		}
		if ((iMainType == OP_INSERT) && (iOpType == OP_INSERT))
		{
			// ���������� ����������
			if (bTxnCommit)
				::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}

		/////////////////////////////////////////////////////////////
		// ���������� SQL-�������
		/////////////////////////////////////////////////////////////
		if (bRunCmd && bSubRowCmd)		// ���������� SQL-�������
		{
			switch (iOpType)			// ��� ��������
			{
			case OP_INSERT:				// ������� OnInsert
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_INSERT,NULL,strErr);
				break;
			}
			case OP_UPDATE:				// ������� OnUpdate
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_UPDATE,NULL,strErr);
				break;
			}
			}
			if (nRet == SQL_ERROR)		// ������ SQL-�������
			{
				if ((iMainType == OP_INSERT) && (iOpType == OP_INSERT))
				{
					// ���������� ��� �������� � ��������� ������
					bRetCode = FALSE; strSav = strErr;

					// ����� ������ ������ ���������� ����� ����������!
					BackDetail(FALSE, fileLog, nMsg, nErr);

					// ���������� ������ � ����� (��� �������� SQL-�������)
					AddBufRow(arSubRowBuf,arSrcSubFld,arDstSubFld,n,
								ROW_INSERTED,FALSE,FALSE);

					// ���������� ��� ���������� ������
					bRunCmd = FALSE; continue;
				}
				if (!bTxnCommit)		// ���������� �������� ��������
				{
					switch (iOpType)	// ��� �������� ��������
					{
					case OP_INSERT:		// �������� ������ (�� ���������)
						nRet = ExecUpdate(pDst,hDstSubDel,OP_DELETE,arSubDelPrm,
											   sDstSubDel,TRUE,strRet);
						break;
					case OP_UPDATE:		// �������������� (�� ���������)
						nRet = ExecUpdate(pDst,hDstSubUpd,OP_UPDATE,arSubUpdPrm,
											   sDstSubUpd,FALSE,strRet);
						break;
					}
					if (nRet == SQL_ERROR)	// ������ �������� ��������
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
		// ��������� �������� OnChange
		if (bSubFldCmd && (iOpType == OP_UPDATE) && (iMFlag == MOD_CMD))
		{
			// ���������� ������� OnChange ��� ���������� �����
			nRet = ExecFldCmd(pDst, arDstSubFld, arDstSubCmd,
							  EV_ON_CHANGE, strErr);
			if (nRet == SQL_ERROR)
			{
				if (!bTxnCommit)
				{
					if (bRunCmd && bSubRowCmd)	// ���������� �������� �������
					{
						nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_UPDATE,NULL,strRet);
						if (nRet == SQL_ERROR)	// ������ �������� �������
						{
							ErrorHandle(ER_BACK_CMD, strRet, nErr);
							// �� ��������� ������
							bRetCode = FALSE; goto err;
						}
					}
					// �������������� ������ (�� ���������)
					nRet = ExecUpdate(pDst, hDstSubUpd,OP_UPDATE,arSubUpdPrm, 
											sDstSubUpd,FALSE,strRet);
					if (nRet == SQL_ERROR) // ������ �������� ��������
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
		if (bTxnCommit)					// ���������� ����������
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
		switch (iOpType)				// ���������� ������ � �����
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
		// ����������� �� ���������� ������� � Trial ������
		if (n > 99) break;
#endif
	}

	////////////////////////////////////////////////////////////////
	// ���������� ������� OnEnd
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
		if (bTxnCommit)					// ���������� ����������
		{
			::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
		}
	}

end:// ������������ ������������ SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// ������� ������ ���������
	pDlg->SetSubText("");
	// ������������ ����� ������
	if (!bRetCode) strErr = strSav;
	return bRetCode;

err:
	// ������������ ������������ SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// ����� ����� (��������� ������ ����� � ��� ��������)
	BackDetail(TRUE, fileLog, nMsg, nErr);
	// ������� ������ ���������
	pDlg->SetSubText("");
	return bRetCode;
}

/////////////////////////////////////////////////////////////////////
// ���������� ������ � ����������

BOOL CTransfer::RunCmdDetail(OP_TYPE iMainType, CStdioFile &fileLog, 
							 UINT &nMsg, UINT &nErr, CString &strErr) 
{
	// ���������:
	// iMainType - ��� �������� �������� �������
	// &fileLog  - ������ �� ���� ���������
	// &nErr     - ������ �� ������� ������
	// &nMsg     - ������ �� ������� ��������������
	// &strErr   - ������ �� ������ ������
	// ���������� ���������
	BOOL bRetCode = TRUE;		// ������������ ��������
	RETCODE nRet;				// ��������� SQL ���������
	MOD_FLAG iMFlag;			// ���� ����������� ������
	OP_TYPE iOpType;			// ��� ��������� SQL
	CString strSav;				// ������ ����������
	CString strMsg;				// ������ ���������
	CString strRet;				// ������ ����������
	long	n;					// ������� �����

	if (!bSubRowCmd) return TRUE;

	// ������� ������ �����
	ClearBufArr(arSubRowBuf);

	////////////////////////////////////////////////////////////////
	//              ��������� ���������
	////////////////////////////////////////////////////////////////
	if (iMainType == OP_DELETE)
	{
		strMsg.LoadString(IDS_MS_READ_SUBDST);
		pDlg->SetSubText(strMsg);
		////////////////////////////////////////////////////////////////
		// ���������� SQL Select ������������ ��������� (��� Fetch)
		////////////////////////////////////////////////////////////////
		if (ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubPrm, 
						sDstSubSQL, FALSE, strErr) == SQL_ERROR)
		{
			strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		// ���� �� ���������� ���������
		for (n = 1;; n++)
		{
			if (bCancel) goto end;
			// �������� �������
			pDlg->CheckState();
			if (pDlg->IsBreaked())
			{
				bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
				goto err;
			}
			// ��������� ������ ���������
			strMsg.FormatMessage(IDS_MS_FMT_SUBPRC, n);
			pDlg->SetSubText(strMsg);
			// ���������� ������ ������ �� ���������� ���������
			nRet = FetchData(hDstSubSQL, arDstSubFld, strErr);
			// �������� ���������� ������
			if (nRet == SQL_NO_DATA)	break;		// ��� ������
			else if (nRet == SQL_ERROR)				// ������ ������
			{
				strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
			{
				// ������ ������ �������������� � ����
				// � ���������� ����������
				strErr = GetErrMsg(ER_READ_DSTSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			// ���������� SQL-�������
			nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_DELETE,NULL,strErr);
			if (nRet == SQL_ERROR)	// ������ SQL-�������
			{
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
			// ���������� ������ � �����
			AddBufRow(arSubRowBuf,arDstSubFld,arDstSubFld,n,
					ROW_CMDONDEL,TRUE,FALSE);

#ifdef _TRIAL_VER
			// ����������� �� ���������� ������� � Trial ������
			if (n > 99) break;
#endif
		}
		// ������������ ����������� SQL
		FreeSQLStmt(hDstSubSQL, TRUE);
	}

	////////////////////////////////////////////////////////////////
	//              ��������� ���������
	////////////////////////////////////////////////////////////////
	else
	{
		strMsg.LoadString(IDS_MS_READ_SUBSRC);
		pDlg->SetSubText(strMsg);
		////////////////////////////////////////////////////////////////
		// ���������� SQL Select ������������ ��������� (��� Fetch)
		////////////////////////////////////////////////////////////////
		if (ExecSelect(pSrc, hSrcSubSQL, arSrcSubFld, arSrcSubPrm, 
						pTrn->sSubSQL, FALSE, strErr) == SQL_ERROR)
		{
			strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
			bRetCode = FALSE; goto err;
		}
		// ���� �� ���������� ���������
		for (n = 1;; n++)
		{
			if (bCancel) goto end;
			// �������� �������
			pDlg->CheckState();
			if (pDlg->IsBreaked())
			{
				bCancel = TRUE; bBreaked = TRUE; bRetCode = TRUE;
				goto err;
			}
			// ��������� ������ ���������
			strMsg.FormatMessage(IDS_MS_FMT_SUBUPD, n);
			pDlg->SetSubText(strMsg);
			// ���������� ������ ������ �� ���������
			nRet = FetchData(hSrcSubSQL, arSrcSubFld, strErr);
			// �������� ���������� ������
			if (nRet == SQL_NO_DATA)	break;		// ��� ������
			else if (nRet == SQL_ERROR)				// ������ ������
			{
				strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)	// ��������������
			{
				// ������ ������ �������������� � ����
				// � ���������� ����������
				strErr = GetErrMsg(ER_READ_SRCSUB, strErr);
				WarningHandle(strErr, nMsg);
			}
			// ��������� ������
			if (iMainType == OP_UPDATE)				// �����������
			{
				// ���������� SQL ������� � ���������� ���������
				// �� �������� ��������� ������ ����������
				nRet = ExecSelect(pDst, hDstSubSQL, arDstSubFld, arDstSubQry, 
									sDstSubQry, TRUE, strErr);
				switch (nRet)
				{
				case SQL_ERROR:				// ������ ������
				{
					strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
					bRetCode = FALSE;
					goto err;
				}
				case SQL_NO_DATA:			// �� ������� ������
				{
					iOpType = OP_INSERT;	// ������� ON_INSERT
					break;
				}
				case SQL_SUCCESS_WITH_INFO:	// ��������������
				{
					// ������ ������ �������������� � ����
					// � ���������� ����������
					strErr = GetErrMsg(ER_FIND_DSTSUB, strErr);
					WarningHandle(strErr, nMsg);
				}
				case SQL_SUCCESS:			// ������ �������
				{
					// ��������� ������ � ��������� �� ������
					if (!CompareRef(arDstSubFld, OP_UPDATE, iMFlag, strErr))
					{
						// ������ ������ ������ � ����
						// � ���������� ����������
						strErr = GetErrMsg(ER_COMP_DSTSUB, strErr);
						WarningHandle(strErr, nMsg);
					}
					if (iMFlag)				// �������� ����� �����������
					{
						iOpType = OP_UPDATE;// ������� ON_UPDATE
					}
					else					// ������ �� ����������
					{
						iOpType = OP_UNDEF;	// �� ��������� �������
						continue;
					}
					break;
				}
				}
			}
			else							// ����������
			{
				iOpType = OP_INSERT;		// ������� ON_INSERT
			}
			// ���������� ������� SQL
			switch (iOpType)			// ��� ��������
			{
			case OP_INSERT:				// ������� OnInsert
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_INSERT,NULL,strErr);
				break;
			}
			case OP_UPDATE:				// ������� OnUpdate
			{
				nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_ON_UPDATE,NULL,strErr);
				break;
			}
			}
			if (nRet == SQL_ERROR)		// ������ SQL-�������
			{
				bRetCode = FALSE; goto err;
			}
			else if (nRet == SQL_SUCCESS_WITH_INFO)
			{
				WarningHandle(strErr, nMsg);
			}
			if (bTxnCommit)				// ���������� ����������
			{
				::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
			}
			switch (iOpType)			// ���������� ������ � �����
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
			// ����������� �� ���������� ������� � Trial ������
			if (n > 99) break;
#endif
		}
	}

end:// ������������ ������������ SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// ������� ������ ���������
	pDlg->SetSubText("");
	return bRetCode;

err:
	// ������������ ������������ SQL
	FreeSQLStmt(hSrcSubSQL, TRUE);
	FreeSQLStmt(hDstSubSQL, TRUE);
	// ����� ����� (��������� ������ ����� � ��� ��������)
	BackDetail(TRUE, fileLog, nMsg, nErr);
	// ������� ������ ���������
	pDlg->SetSubText("");
	return bRetCode;
}


/////////////////////////////////////////////////////////////////////
// ����� ���������� ���������� (��������� ������ �����)

BOOL CTransfer::BackDetail(BOOL bDelBuf, CStdioFile &fileLog, 
						   UINT &nMsg, UINT &nErr) 
{
	// ���������:
	// bDelBuf   - ������� �������� ������ ������
	// &fileLog  - ������ �� ���� ���������
	// &nErr     - ������ �� ������� ������
	// &nMsg     - ������ �� ������� ��������������

	// ���������� ���������
	RETCODE nRet;
	CString strErr,strMsg;
	// ������ ������� ���������� ���������
	int nSize = arSrcSubFld.GetSize();
	// ������ ������� ���������� ���������
	int mSize = arDstSubFld.GetSize();
	// ��������� ������ ����� � �����
	for (int i = arSubRowBuf.GetSize() - 1; i >= 0; i--)
	{
		if (arSubRowBuf[i].pArSrcFld != NULL && 
			arSubRowBuf[i].pArDstFld != NULL)
		{
			// ������� ������ � ��������
			for (int j = 0; j < nSize; j++)
			{
				arSrcSubFld[j].bMod  = arSubRowBuf[i].pArSrcFld->GetAt(j).bMod;
				arSrcSubFld[j].vData = arSubRowBuf[i].pArSrcFld->GetAt(j).vData;
			}
			// ������� ������ � ��������
			for (int k = 0; k < mSize; k++)
			{
				arDstSubFld[k].bMod  = arSubRowBuf[i].pArDstFld->GetAt(k).bMod;
				arDstSubFld[k].vData = arSubRowBuf[i].pArDstFld->GetAt(k).vData;
			}
			// ��������� ������
			switch (arSubRowBuf[i].nStatus)	// ������ ������
			{
			case ROW_INSERTED:				// ������ ���������
			{
				// ��������� ���������
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// ����� SQL-������� ������
				if (arSubRowBuf[i].bRunCmd && bSubRowCmd)
				{
					// ���������� ������� ReInsert
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_INSERT,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				if (bDelBuf)				// ��������(�� ���������)
				{
					nRet = ExecUpdate(pDst, hDstSubDel,OP_DELETE,arSubDelPrm,
											sDstSubDel,TRUE,strErr);

					if (nRet == SQL_ERROR)	// ������ ��������
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
			case ROW_MODIFIED:				// ������ ��������
			{
				// ��������� ���������
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// ����� ������ OnChange
				if (arSubRowBuf[i].bEvChanged && bSubFldCmd)
				{
					// ���������� ������� ReChange ��� ���������� �����
					nRet = ExecFldCmd(pDst, arDstSubFld, arDstSubCmd,
									  EV_RE_CHANGE, strErr);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bEvChanged = FALSE;
				}
				// ����� SQL-������� ������
				if (arSubRowBuf[i].bRunCmd && bSubRowCmd)
				{
					// ���������� ������� ReUpdate
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_UPDATE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
					{
						goto err;
					}
					else if (nRet == SQL_SUCCESS_WITH_INFO)
					{
						WarningHandle(strErr, nMsg);
					}
					arSubRowBuf[i].bRunCmd = FALSE;
				}
				if (bDelBuf)				// ��������������(�� ���������)
				{
					nRet = ExecUpdate(pDst, hDstSubUpd,OP_UPDATE,arSubUpdPrm,
											sDstSubUpd,FALSE,strErr);

					if (nRet == SQL_ERROR)	// ������ ������
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
			case ROW_DELETED:				// ������ �������
			{
				// ��������� ���������
				strMsg.FormatMessage(IDS_MS_FMT_SUBRES,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				if (bDelBuf)				// �������(�� ���������)
				{
					nRet = ExecUpdate(pDst, hDstSubIns,OP_INSERT,arSubInsPrm,
											sDstSubIns,FALSE,strErr);

					if (nRet == SQL_ERROR)	// ������ ������
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
				// ����� SQL-������� ������
				if (arSubRowBuf[i].bRunCmd && bSubRowCmd)
				{
					// ���������� ������� ReDelete
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
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
			case ROW_CMDONINS:				// ������� ON_INSERT
			{
				// ��������� ���������
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// ����� SQL-������� ������
				if (arSubRowBuf[i].bRunCmd)
				{
					// ���������� ������� ReInsert
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_INSERT,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
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
			case ROW_CMDONUPD:				// ������� ON_UPDATE
			{
				// ��������� ���������
				strMsg.FormatMessage(IDS_MS_FMT_SUBBCK,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// ����� SQL-������� ������
				if (arSubRowBuf[i].bRunCmd)
				{
					// ���������� ������� ReUpdate
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_UPDATE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
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
			case ROW_CMDONDEL:				// ������� ON_DELETE
			{
				// ��������� ���������
				strMsg.FormatMessage(IDS_MS_FMT_SUBRES,arSubRowBuf[i].nRow);
				pDlg->SetSubText(strMsg);
				// ����� SQL-������� ������
				if (arSubRowBuf[i].bRunCmd)
				{
					// ���������� ������� ReDelete
					nRet = ExecSQLCmd(pDst,arDstSubCmd,EV_RE_DELETE,
										NULL,strErr);

					if (nRet == SQL_ERROR)	// ������ SQL-�������
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
			if (bTxnCommit)					// ���������� ����������
			{
				::SQLEndTran(SQL_HANDLE_DBC, pDst->hdbc, SQL_COMMIT);
			}
cont:		if (bDelBuf)					// �������� ��������
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
