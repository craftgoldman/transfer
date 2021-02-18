// CardPPL.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "ChildFrm.h"
#include "CardPPL.h"
#include "TrnParm.h"
#include "PropDlg.h"
#include "TrnGenPg.h"
#include "TrnTabPg.h"
#include "TrnCmdPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardPPLView

IMPLEMENT_DYNCREATE(CCardPPLView, CResizableFormView)

CCardPPLView::CCardPPLView()
	: CResizableFormView(CCardPPLView::IDD)
{
	m_bListFocus = FALSE;
	m_bHeadChanged = FALSE;
	m_bFileChanged = FALSE;
	m_nState = FORM_EDIT;
	m_PPLNum = -1;
	//{{AFX_DATA_INIT(CCardPPLView)
	m_PPLFile = _T("");
	m_PPLName = _T("");
	m_PPLKind = 0;
	m_PPLParm = _T("");
	m_PPLSrc = _T("");
	m_PPLSrcUID = _T("");
	m_PPLSrcPWD = _T("");
	m_PPLDst = _T("");
	m_PPLDstUID = _T("");
	m_PPLDstPWD = _T("");
	//}}AFX_DATA_INIT
}

CCardPPLView::~CCardPPLView()
{
}

void CCardPPLView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardPPLView)
	DDX_Control(pDX, IDC_PPL_SRC, m_cbSRC);
	DDX_Control(pDX, IDC_PPL_DST, m_cbDST);
	DDX_Control(pDX, IDC_PPL_FILE, m_EditFile);
	DDX_Control(pDX, IDC_PPL_NAME, m_EditName);
	DDX_Control(pDX, IDC_PPL_LIST, m_PPLList);
	DDX_Control(pDX, IDC_OK, m_bOK);
	DDX_Control(pDX, IDC_SAVE, m_bSAVE);
	DDX_Control(pDX, IDC_CANCEL, m_bCANCEL);
	DDX_Control(pDX, IDC_ADD, m_bADD);
	DDX_Control(pDX, IDC_INSERT, m_bINSERT);
	DDX_Control(pDX, IDC_EDIT, m_bEDIT);
	DDX_Control(pDX, IDC_DELETE, m_bDELETE);
	DDX_Text(pDX, IDC_PPL_FILE, m_PPLFile);
	DDX_Text(pDX, IDC_PPL_NAME, m_PPLName);
	DDX_CBIndex(pDX, IDC_PPL_KIND, m_PPLKind);
	DDX_Text(pDX, IDC_PPL_PARM, m_PPLParm);
	DDX_CBString(pDX, IDC_PPL_SRC, m_PPLSrc);
	DDX_Text(pDX, IDC_PPL_SRC_UID, m_PPLSrcUID);
	DDX_Text(pDX, IDC_PPL_SRC_PWD, m_PPLSrcPWD);
	DDX_CBString(pDX, IDC_PPL_DST, m_PPLDst);
	DDX_Text(pDX, IDC_PPL_DST_UID, m_PPLDstUID);
	DDX_Text(pDX, IDC_PPL_DST_PWD, m_PPLDstPWD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardPPLView, CResizableFormView)
	ON_MESSAGE(FORM_GET_STATE,OnFormGetState)
	ON_MESSAGE(FORM_SET_STATE,OnFormSetState)
	ON_MESSAGE(FORM_UPDATE_DATA,OnFormUpdateData)
	ON_MESSAGE(FORM_QUERY_SAVE,OnFormQuerySave)
	//{{AFX_MSG_MAP(CCardPPLView)
	ON_WM_SIZE()
	ON_EN_KILLFOCUS(IDC_PPL_FILE, OnKillfocusPplFile)
	ON_NOTIFY(NM_SETFOCUS, IDC_PPL_LIST, OnSetfocusPplList)
	ON_NOTIFY(NM_KILLFOCUS, IDC_PPL_LIST, OnKillfocusPplList)
	ON_NOTIFY(NM_RCLICK, IDC_PPL_LIST, OnRclickPplList)
	ON_NOTIFY(NM_DBLCLK, IDC_PPL_LIST, OnDblclkPplList)
	ON_BN_CLICKED(IDC_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_INSERT, OnBtnInsert)
	ON_BN_CLICKED(IDC_EDIT, OnBtnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnBtnDelete)
	ON_BN_CLICKED(IDC_EDIT_FILE, OnEditFile)
	ON_BN_CLICKED(IDC_EDIT_PARM, OnEditParm)
	ON_EN_CHANGE(IDC_PPL_FILE, OnHeadChanged)
	ON_CBN_SELENDOK(IDC_PPL_SRC, OnFileChanged)
	ON_COMMAND(ID_DATA_SAVE, OnDataSave)
	ON_COMMAND(ID_DATA_ADD, OnDataAdd)
	ON_COMMAND(ID_DATA_RETRIEVE, OnDataRetrieve)
	ON_COMMAND(ID_DATA_EDIT, OnDataEdit)
	ON_COMMAND(ID_DATA_DELETE, OnDataDelete)
	ON_COMMAND(ID_DATA_CLOSE, OnDataClose)
	ON_EN_CHANGE(IDC_PPL_NAME, OnHeadChanged)
	ON_CBN_SELENDOK(IDC_PPL_KIND, OnHeadChanged)
	ON_CBN_SELENDOK(IDC_PPL_DST, OnFileChanged)
	ON_CBN_EDITCHANGE(IDC_PPL_SRC, OnFileChanged)
	ON_CBN_EDITCHANGE(IDC_PPL_DST, OnFileChanged)
	ON_EN_CHANGE(IDC_PPL_SRC_UID, OnFileChanged)
	ON_EN_CHANGE(IDC_PPL_SRC_PWD, OnFileChanged)
	ON_EN_CHANGE(IDC_PPL_DST_UID, OnFileChanged)
	ON_EN_CHANGE(IDC_PPL_DST_PWD, OnFileChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardPPLView diagnostics

#ifdef _DEBUG
void CCardPPLView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

void CCardPPLView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCardPPLView message handlers

BOOL CCardPPLView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CResizableFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
		return FALSE;

	m_OrigSize.cx = rect.right - rect.left;
	m_OrigSize.cy = rect.bottom - rect.top;

	// ��������� �������������
	OnInitialUpdate();

	// ��������� ��������� �������������
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
	{
		pWnd->SetActiveView(this, TRUE);
		// ���������� ��������� ����
		pWnd->GetWindowText(m_strTitle);
	}
	return TRUE;
}

void CCardPPLView::OnInitialUpdate() 
{
	CResizableFormView::OnInitialUpdate();

	AddAnchor(IDC_PPL_FILE, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_EDIT_FILE,TOP_RIGHT);
	AddAnchor(IDC_PPL_TEXT, TOP_RIGHT);
	AddAnchor(IDC_PPL_KIND, TOP_RIGHT);
	AddAnchor(IDC_PPL_NAME, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_PPL_PARM, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_EDIT_PARM,TOP_RIGHT);

	AddAnchor(IDC_OK, TOP_RIGHT);
	AddAnchor(IDC_SAVE, TOP_RIGHT);
	AddAnchor(IDC_CANCEL, TOP_RIGHT);
	AddAnchor(IDC_ADD, TOP_RIGHT);
	AddAnchor(IDC_INSERT, TOP_RIGHT);
	AddAnchor(IDC_EDIT, TOP_RIGHT);
	AddAnchor(IDC_DELETE, TOP_RIGHT);

	AddAnchor(IDC_PPL_SRC, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_PPL_DST, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_SRC_UID_TXT, TOP_RIGHT);
	AddAnchor(IDC_DST_UID_TXT, TOP_RIGHT);
	AddAnchor(IDC_PPL_SRC_UID, TOP_RIGHT);
	AddAnchor(IDC_PPL_DST_UID, TOP_RIGHT);
	AddAnchor(IDC_SRC_PWD_TXT, TOP_RIGHT);
	AddAnchor(IDC_DST_PWD_TXT, TOP_RIGHT);
	AddAnchor(IDC_PPL_SRC_PWD, TOP_RIGHT);
	AddAnchor(IDC_PPL_DST_PWD, TOP_RIGHT);

	AddAnchor(IDC_PPL_LIST, TOP_LEFT, BOTTOM_RIGHT);

	// ��������� ����������� ��� ������
	m_bOK.SetIcon(IDI_BOK, 16, 16);
	m_bOK.SetDefaultButton();
	m_bSAVE.SetIcon(IDI_BSAVE, 16, 16);
	m_bCANCEL.SetIcon(IDI_BCANCEL, 16, 16);
	m_bADD.SetIcon(IDI_BADD, 16, 16);
	m_bINSERT.SetIcon(IDI_BINSERT, 16, 16);
	m_bEDIT.SetIcon(IDI_BEDIT, 16, 16);
	m_bDELETE.SetIcon(IDI_BDELETE, 16, 16);
	// ��������� ������������ ����� ListView
	m_PPLList.SetExtendedStyle(m_PPLList.GetExtendedStyle()|
		LVS_EX_FULLROWSELECT);
	// ��������� ������� � ������
	CRect rect;
	CString sTName,sSchem;
	m_PPLList.GetClientRect(rect);
	sTName.LoadString(TAB_FLD_TRANS);
	sSchem.LoadString(TAB_FLD_SCHEMA);
	m_PPLList.InsertColumn(0, sTName, LVCFMT_LEFT, rect.Width()*4/5);
	m_PPLList.InsertColumn(1, sSchem, LVCFMT_LEFT, rect.Width()*1/5);
	// ���������� ������ ���������� ODBC
	HENV henv;
	RETCODE nRetCode;
	SWORD cbName = -1;
	SWORD cbDesc = -1;
	UCHAR szName[SQL_MAX_DSN_LENGTH+1];
	UCHAR szDesc[512+1];
	char* lpName=(char *)szName;
	if (SQLAllocEnv(&henv) != SQL_SUCCESS)
		return;
	while (TRUE)
	{
		nRetCode = ::SQLDataSources(henv, SQL_FETCH_NEXT,
			szName, SQL_MAX_DSN_LENGTH, &cbName,
			szDesc, 512, &cbDesc);
		if (nRetCode != SQL_SUCCESS && nRetCode != SQL_SUCCESS_WITH_INFO)
			break;
		szName[cbName] = '\0';
		m_cbSRC.AddString(lpName);
		m_cbDST.AddString(lpName);
	}
	SQLFreeEnv(henv);
}

LRESULT CCardPPLView::OnFormSetState(WPARAM wParam, LPARAM lParam)
{
	// ��������� ������� �����
	m_nState = (INT)wParam;
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
	{
		switch (m_nState)
		{
		case FORM_EDIT:
		{
			m_bHeadChanged = FALSE;
			m_bFileChanged = FALSE;
			pWnd->SetWindowText(m_strTitle);
			break;
		}
		case FORM_NEW:
		{
			m_PPLNum = -1;
			m_bHeadChanged = TRUE;
			m_bFileChanged = TRUE;
			CString strCap;
			strCap.LoadString(IDS_MS_NEW);
			strCap = m_strTitle + strCap;
			pWnd->SetWindowText(strCap);
			break;
		}
		default:
		{
			m_bHeadChanged = FALSE;
			m_bFileChanged = FALSE;
			pWnd->SetWindowText(m_strTitle);
			break;
		}
		}
	}
	return 0;
}

LRESULT CCardPPLView::OnFormGetState(WPARAM wParam, LPARAM lParam)
{
	// ������ ������� �����
	return (LRESULT)m_nState;
}

LRESULT CCardPPLView::OnFormUpdateData(WPARAM wParam, LPARAM lParam)
{
	// ���������� ������ � �����
	PPL* sPPL = (PPL* )wParam;
	// ��������� ����� �����
	if (sPPL != NULL)
	{
		m_PPLNum  = sPPL->iNum;
		m_PPLKind = sPPL->iKind;
		m_PPLFile = sPPL->sFile;
		m_PPLName = sPPL->sName;
		UpdateData(FALSE);
	}
	// ������ ������ �� ����� ����������
	OnDataRetrieve();

	return 0;
}

LRESULT CCardPPLView::OnFormQuerySave(WPARAM wParam, LPARAM lParam)
{
	// ������ �� ���������� ������
	if (!QuerySave()) {
		return 1;
	}
	return 0;
}

void CCardPPLView::OnSize(UINT nType, int cx, int cy) 
{
	CResizableFormView::OnSize(nType, cx, cy);
	
	CRect rect;
	m_PPLList.GetClientRect(rect);
	m_PPLList.SetColumnWidth(0, rect.Width()*4/5);
	m_PPLList.SetColumnWidth(1, rect.Width()*1/5);
}

void CCardPPLView::OnKillfocusPplFile() 
{
	// �������� ����� ����� �� ���������
	if (m_EditFile.GetModify() && 
		m_nState == FORM_EDIT)
		CheckPPLFile();
}

void CCardPPLView::OnSetfocusPplList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_bListFocus = TRUE;
	*pResult = 0;
}

void CCardPPLView::OnKillfocusPplList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_bListFocus = FALSE;
	*pResult = 0;
}

void CCardPPLView::OnRclickPplList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// ����� ������������ ���� "��������"
	CMDIFrameWnd* pFrm = (CMDIFrameWnd* )AfxGetMainWnd();
	CMenu*	pMenu;
	CMenu*	pData;
	CPoint	point;

	if (pFrm != NULL) pMenu = pFrm->GetMenu();
	if (pMenu != NULL)
		pData = pMenu->GetSubMenu(2);
	if (pData != NULL)
	{
		::GetCursorPos( &point );
		pData->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, GetParent() );
	}
	*pResult = 0;
}

void CCardPPLView::OnDblclkPplList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// �������������� �������� ������
	OnDataEdit();
	*pResult = 0;
}

void CCardPPLView::OnBtnOk() 
{
	// ���������� ������
	if (!QuerySave(FALSE)) return;
	// �������� MDI ����
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
		pWnd->MDIDestroy();
}

void CCardPPLView::OnBtnSave() 
{
	// ���������� ������
	OnDataSave();
}

void CCardPPLView::OnBtnCancel() 
{
	// ���������� ������ � �������� ����
	OnDataClose();
}

void CCardPPLView::OnBtnAdd() 
{
	// ���������� �������� � ������
	m_bListFocus = TRUE;
	OnDataAdd();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnBtnInsert() 
{
	// ������� �������� � ������
	m_bListFocus = TRUE;
	OnDataInsert();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnBtnEdit() 
{
	// �������������� �������� ������
	m_bListFocus = TRUE;
	OnDataEdit();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnBtnDelete() 
{
	// �������� �������� �� ������
	m_bListFocus = TRUE;
	OnDataDelete();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnEditFile() 
{
	// ��������� ����� ����� ����������
	CWnd*		wnd = GetParent();
	CString		ext = "ppl";
	CString		flt;
	flt.LoadString(IDS_MS_FILTER);
	CFileDialog	dlg(TRUE,ext,NULL,0,flt,wnd);

	// �������� ������
	UpdateData(TRUE);
	dlg.m_ofn.lpstrInitialDir = TrnParm.sDirPpl;
	dlg.m_ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|
		OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|
		OFN_LONGNAMES|OFN_NOCHANGEDIR|
		OFN_NODEREFERENCELINKS;
	if (dlg.DoModal() == IDOK)
	{
		// ��������� ������ � �����
		m_PPLFile = dlg.GetFileName();
		UpdateData(FALSE);
		// ������ ���������� � ����������
		ReadPPLInfo();
		// ��������� ������� ���������
		OnHeadChanged();
	}
}

void CCardPPLView::OnEditParm() 
{
	// �������������� ����������
	CTrnParmDlg	parmDlg;
	CString		sParm;

	// �������� ������� ������
	UpdateData(TRUE);
	// ��������� ����������
	parmDlg.m_strParm = m_PPLParm;
	if (parmDlg.DoModal() == IDOK)
	{
		// ��������� ������ � �����
		m_PPLInfo.sParm = parmDlg.m_strParm;
		m_PPLParm = parmDlg.m_strParm;
		UpdateData(FALSE);
		OnFileChanged();
	}
}

void CCardPPLView::OnHeadChanged() 
{
	// ��������� ������� ���������
	if (!m_bHeadChanged && 
		!m_bFileChanged &&
		m_nState == FORM_EDIT)
		MarkTitle();
	m_bHeadChanged = TRUE;
}

void CCardPPLView::OnFileChanged() 
{
	// ��������� ������� ���������
	if (!m_bHeadChanged &&
		!m_bFileChanged && 
		m_nState == FORM_EDIT)
		MarkTitle();
	m_bFileChanged = TRUE;
}

void CCardPPLView::OnDataAdd() 
{
	// ����� ������� � ������
	if (m_bListFocus)
	{
		// �������� ������
		UpdateData();
		// �������� ������� �������
		CPropDlg trnSheet(IDS_MS_TRN_NEW, GetParent());
		// �������� ������� �������
		CTrnGenPage genPg;
		CTrnTabPage tabPg(TAB_DSTTAB_CAPTION);
		CTrnTabPage subPg(TAB_SUBTAB_CAPTION);
		CTrnCmdPage tCmPg(TAB_DSTCMD_CAPTION);
		CTrnCmdPage	sCmPg(TAB_SUBCMD_CAPTION);
		// ���������� ������� �������
		trnSheet.AddPage( &genPg );
		trnSheet.AddPage( &tabPg );
		trnSheet.AddPage( &subPg );
		trnSheet.AddPage( &tCmPg );
		trnSheet.AddPage( &sCmPg );
		// ��������� ������� �� ���������
		genPg.m_sODBCDSN	= "DSN=" + m_PPLSrc + 
							  ";UID=" + m_PPLSrcUID + 
							  ";PWD=" + m_PPLSrcPWD;
		tabPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
							  ";UID=" + m_PPLDstUID + 
							  ";PWD=" + m_PPLDstPWD;
		subPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
							  ";UID=" + m_PPLDstUID + 
							  ";PWD=" + m_PPLDstPWD;

		// ����� ������� �������
		if (trnSheet.DoModal() != IDOK) return;

		/////////////////////////////////////////
		// ��������� ������� ����������
		/////////////////////////////////////////
		int i = m_PPLList.GetItemCount();
		m_PPLInfo.arTrn.SetSize(i + 1);
		// �������� ��������
		m_PPLInfo.arTrn[i].sName	= genPg.m_sName;
		m_PPLInfo.arTrn[i].iSchema	= genPg.m_nSchema;
		m_PPLInfo.arTrn[i].sMsg		= genPg.m_sMsg;
		m_PPLInfo.arTrn[i].sTabSQL	= genPg.m_sTabSQL;
		m_PPLInfo.arTrn[i].sSubSQL	= genPg.m_sSubSQL;
		// ��������
		m_PPLInfo.arTrn[i].iTabPrc	= tabPg.m_nProcess;
		m_PPLInfo.arTrn[i].sTabOwn	= tabPg.m_sOwner;
		m_PPLInfo.arTrn[i].sTabDst	= tabPg.m_sTable;
		m_PPLInfo.arTrn[i].sTabCond	= tabPg.m_sSQLCond;
		m_PPLInfo.arTrn[i].sTabMap	= tabPg.m_sColumns;
		// ����������
		m_PPLInfo.arTrn[i].iSubPrc	= subPg.m_nProcess;
		m_PPLInfo.arTrn[i].sSubOwn	= subPg.m_sOwner;
		m_PPLInfo.arTrn[i].sSubDst	= subPg.m_sTable;
		m_PPLInfo.arTrn[i].sSubCond	= subPg.m_sSQLCond;
		m_PPLInfo.arTrn[i].sSubMap	= subPg.m_sColumns;
		// ������� ���������
		m_PPLInfo.arTrn[i].sTabCmd	= tCmPg.m_sTabCmd;
		// ������� ����������
		m_PPLInfo.arTrn[i].sSubCmd	= sCmPg.m_sTabCmd;
		// ���������� ���������� � ������
		InsertTRN(i, m_PPLInfo.arTrn[i].sName, 
					 m_PPLInfo.arTrn[i].iSchema);
		// ��������� �������� ��������
		m_PPLList.SetItem(i, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
		m_PPLList.EnsureVisible(i, FALSE);
		// ��������� �������� ���������
		OnFileChanged();
	}
	// ����� �����
	else
	{
		// ���������� ������
		if (!QuerySave(FALSE)) return;
		// ������� ������ � �����
		m_PPLFile = "";
		m_PPLName = "";
		m_PPLKind = 0;
		UpdateData(FALSE);
		// ������� ���������� � ����������
		ClearPPLInfo();
		// ��������� ��������� ����
		CChildFrame* pWnd = (CChildFrame* )GetParent();
		if (pWnd != NULL)
		{
			CString strCap;
			strCap.LoadString(IDS_MS_NEW);
			strCap = m_strTitle + strCap;
			pWnd->SetWindowText(strCap);
		}
		// ��������� �������
		m_PPLNum = -1;
		m_nState = FORM_NEW;
		m_bHeadChanged = TRUE;
		m_bFileChanged = TRUE;
	}
}

void CCardPPLView::OnDataRetrieve() 
{
	// ������ ������ �� ����� ����������
	if (m_nState == FORM_NEW) return;
	// ���������� ������
	if (!QuerySave()) return;
	// ��������� ��������� ����
	CChildFrame*	pWnd;
	pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
		pWnd->SetWindowText(m_strTitle);
	// ���������� ��������� ������
	ReadPPLInfo();
	// ��������� ������� �����
	m_nState = FORM_EDIT;
	m_bHeadChanged = FALSE;
	m_bFileChanged = FALSE;
}

void CCardPPLView::OnDataSave() 
{
	// ���������� ������
	CChildFrame* pWnd;
	PPL			 sPPL;
	CString		 errText;
	CString		 sNum;
	CString		 sKind;
	CString		 sText;
	CString		 sKey;

	// �������� ������
	UpdateData();
	if (!m_bHeadChanged &&
		!m_bFileChanged)
		return;
	// �������� ������
	if (m_PPLFile.IsEmpty()) {
		m_EditFile.SetFocus();
		errText = GetErrMsg(ER_NEED_FILE);
		goto err;
	}
	if (m_PPLName.IsEmpty()) {
		m_EditName.SetFocus();
		errText = GetErrMsg(ER_NEED_NAME);
		goto err;
	}
	// ������ �������� ���������� � ����
	// (����������� ������, ��� ��� ������)
	if (m_bFileChanged)
	{
		if (!WritePPLInfo())
		{
			errText = GetErrMsg(ER_WRITE_FILE, 
						TrnParm.sDirPpl + m_PPLFile);
			goto err;
		}
		m_bFileChanged = FALSE;
	}
	// ������ ������ ����� � ������ � ���� [pipeline]
	if (m_bHeadChanged)
	{
		sKey.LoadString(IDS_SYS_REG_PIPE);
		if (m_nState == FORM_NEW) {
			m_PPLNum = arPPL.GetSize();
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, 
				_T("Count"), m_PPLNum + 1);
		}
		sNum.Format("%i", m_PPLNum);
		sKind.Format("%i", m_PPLKind);
		WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("PPL"+sNum), 
			_T(sKind+","+m_PPLFile+","+m_PPLName));
		m_bHeadChanged = FALSE;
	}
	// ������� ��������� ����-��������
	// ��� ���������� ������
	pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
	{
		if (pWnd->m_pParentWnd != NULL)
		{
			sPPL.iNum	= m_PPLNum;
			sPPL.iKind	= m_PPLKind;
			sPPL.sFile	= m_PPLFile;
			sPPL.sName	= m_PPLName;
			pWnd->m_pParentWnd->SendMessage(
				FORM_UPDATE_DATA, (DWORD)(&sPPL));
		}
		// ��������� ��������� ����
		pWnd->SetWindowText(m_strTitle);
	}
	// ��������� �������
	m_nState = FORM_EDIT;
	return;

err:
	MessageBeep(MB_ICONEXCLAMATION);
	MessageBox(errText, m_strTitle, 
		MB_ICONEXCLAMATION|MB_OK);
	return;
}

void CCardPPLView::OnDataEdit() 
{
	// �������������� �������� ������
	if (!m_bListFocus) return;
	// ����� �������� ��������
	int	i = m_PPLList.GetNextItem(-1, LVNI_FOCUSED);
	if (i < 0 || i > m_PPLInfo.arTrn.GetUpperBound())
		return;

#ifdef _TRIAL_VER
	if (m_PPLInfo.arTrn[i].iSchema == TRN_COMPLEX)
	{
		CString sErr;
		// ����������� �� ����� � Trial ������
		sErr.FormatMessage(IDS_ER_TRIAL_COMPLEX);
		sErr.Insert(0, ":\n");
		sErr.Insert(0, LPCTSTR(m_PPLInfo.arTrn[i].sName));
		AfxMessageBox(sErr);
	}
#endif

	// �������� ������
	UpdateData();
	CPropDlg trnSheet(IDS_MS_TRN_EDIT, GetParent());
	// �������� ������� �������
	CTrnGenPage genPg;
	CTrnTabPage tabPg(TAB_DSTTAB_CAPTION);
	CTrnTabPage subPg(TAB_SUBTAB_CAPTION);
	CTrnCmdPage tCmPg(TAB_DSTCMD_CAPTION);
	CTrnCmdPage	sCmPg(TAB_SUBCMD_CAPTION);
	// ���������� ������� �������
	trnSheet.AddPage( &genPg );
	trnSheet.AddPage( &tabPg );
	trnSheet.AddPage( &subPg );
	trnSheet.AddPage( &tCmPg );
	trnSheet.AddPage( &sCmPg );

	/////////////////////////////////////////
	// ��������� ������ �� ���������
	/////////////////////////////////////////
	// �������� ��������
	genPg.m_sODBCDSN	= "DSN=" + m_PPLSrc + 
						  ";UID=" + m_PPLSrcUID + 
						  ";PWD=" + m_PPLSrcPWD;
	genPg.m_sName		= m_PPLInfo.arTrn[i].sName;
	genPg.m_nSchema		= m_PPLInfo.arTrn[i].iSchema;
	genPg.m_nConvert	= m_PPLInfo.arTrn[i].iConvert;
	genPg.m_sMsg		= m_PPLInfo.arTrn[i].sMsg;
	genPg.m_sTabSQL		= m_PPLInfo.arTrn[i].sTabSQL;
	genPg.m_sSubSQL		= m_PPLInfo.arTrn[i].sSubSQL;
	// ��������
	tabPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;
	// �������� ������� �������
	tabPg.m_nProcess	= m_PPLInfo.arTrn[i].iTabPrc;
	tabPg.m_sOwner		= m_PPLInfo.arTrn[i].sTabOwn;
	tabPg.m_sTable		= m_PPLInfo.arTrn[i].sTabDst;
	tabPg.m_sSQLCond	= m_PPLInfo.arTrn[i].sTabCond;
	tabPg.m_sColumns	= m_PPLInfo.arTrn[i].sTabMap;
	// ����������
	subPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;
	subPg.m_nProcess	= m_PPLInfo.arTrn[i].iSubPrc;
	subPg.m_sOwner		= m_PPLInfo.arTrn[i].sSubOwn;
	subPg.m_sTable		= m_PPLInfo.arTrn[i].sSubDst;
	subPg.m_sSQLCond	= m_PPLInfo.arTrn[i].sSubCond;
	subPg.m_sColumns	= m_PPLInfo.arTrn[i].sSubMap;
	// ������� ���������
	tCmPg.m_sTabCmd		= m_PPLInfo.arTrn[i].sTabCmd;
	// ������� ����������
	sCmPg.m_sTabCmd		= m_PPLInfo.arTrn[i].sSubCmd;

	// ����� ������� �������
	if (trnSheet.DoModal() != IDOK) return;

	/////////////////////////////////////////
	// ��������� ������� ����������
	/////////////////////////////////////////
	// �������� ��������
	m_PPLInfo.arTrn[i].sName	= genPg.m_sName;
	m_PPLInfo.arTrn[i].iSchema	= genPg.m_nSchema;
	m_PPLInfo.arTrn[i].iConvert	= genPg.m_nConvert;
	m_PPLInfo.arTrn[i].sMsg		= genPg.m_sMsg;
	m_PPLInfo.arTrn[i].sTabSQL	= genPg.m_sTabSQL;
	m_PPLInfo.arTrn[i].sSubSQL	= genPg.m_sSubSQL;
	// ��������
	m_PPLInfo.arTrn[i].iTabPrc	= tabPg.m_nProcess;
	m_PPLInfo.arTrn[i].sTabOwn	= tabPg.m_sOwner;
	m_PPLInfo.arTrn[i].sTabDst	= tabPg.m_sTable;
	m_PPLInfo.arTrn[i].sTabCond	= tabPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sTabMap	= tabPg.m_sColumns;
	// ����������
	m_PPLInfo.arTrn[i].iSubPrc	= subPg.m_nProcess;
	m_PPLInfo.arTrn[i].sSubOwn	= subPg.m_sOwner;
	m_PPLInfo.arTrn[i].sSubDst	= subPg.m_sTable;
	m_PPLInfo.arTrn[i].sSubCond	= subPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sSubMap	= subPg.m_sColumns;
	// ������� ���������
	m_PPLInfo.arTrn[i].sTabCmd	= tCmPg.m_sTabCmd;
	// ������� ����������
	m_PPLInfo.arTrn[i].sSubCmd	= sCmPg.m_sTabCmd;
	// ��������� ��������� � ������
	// ������������
	m_PPLList.SetItemText(i, 0, m_PPLInfo.arTrn[i].sName);
	// �����
	CString	sSchema;
	switch (m_PPLInfo.arTrn[i].iSchema)
	{
	case TRN_COPY:    sSchema = "�����";	break;
	case TRN_COLUMNS: sSchema = "�������";	break;
	case TRN_COMPLEX: sSchema = "��������";	break;
	}
	m_PPLList.SetItemText(i, 1, sSchema);
	// ��������� �������� ���������
	OnFileChanged();
}

void CCardPPLView::OnDataDelete() 
{
	// �������� �������� � ������
	if (!m_bListFocus) return;
	// ����� �������� ��������
	int	iItem = m_PPLList.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem < 0 || iItem > m_PPLInfo.arTrn.GetUpperBound())
		return;
	CString strMsg;
	strMsg.LoadString(IDS_MS_DELETE);
	if (AfxMessageBox(strMsg, MB_YESNO|MB_ICONQUESTION ) != IDYES)
		return;

	// ����� ��������� �������
	int	i, nCount = m_PPLInfo.arTrn.GetSize();
	for (i = iItem; i < nCount - 1; i++)
		m_PPLInfo.arTrn[i] = m_PPLInfo.arTrn[i+1];
	// ���������� ������� �������
	m_PPLInfo.arTrn.SetSize(nCount - 1);
	// �������� ���������� �� ������
	m_PPLList.DeleteItem(iItem);
	// ��������� �������� ��������
	if ((nCount = m_PPLList.GetItemCount()) > 0) {
		if (iItem >= nCount) iItem--;
		m_PPLList.SetItem(iItem, 
		0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
	}
	// ��������� �������� ���������
	OnFileChanged();
}

void CCardPPLView::OnDataClose() 
{
	// �������� ������������ MDI ����
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL) {
		// ���������� ������ � �������� ����
		if( QuerySave() ) pWnd->MDIDestroy();
	}
}

BOOL CCardPPLView::QuerySave(BOOL bQuestion)
{
	// ������ �� ���������� ���������
	// �������� ����� ����� �� ���������
	if (m_EditFile.GetModify() && 
		m_nState == FORM_EDIT)
	{
		if (!CheckPPLFile()) return FALSE;
	}
	// �������� ���������
	if (m_bHeadChanged ||
		m_bFileChanged)
	{
		if (bQuestion)
		{
			CString strMsg;
			strMsg.LoadString(IDS_MS_SAVE);
			int	retcode = MessageBox(strMsg, m_strTitle, 
									MB_YESNOCANCEL|MB_ICONQUESTION);
			switch (retcode) {
			case IDNO:
				// ��������� �������
				m_bHeadChanged = FALSE;
				m_bFileChanged = FALSE;
				return TRUE;
			case IDCANCEL:
				return FALSE;
			}
		}
		// ���������� ������
		OnDataSave();
		// �������� �������
		if (m_bHeadChanged ||
			m_bFileChanged)
			return FALSE;
	}
	return TRUE;
}

void CCardPPLView::MarkTitle()
{
	// ������� ��������� ����
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
		pWnd->SetWindowText(m_strTitle + " *");
}

void CCardPPLView::ClearPPLInfo()
{
	// ������� ����������� ����������
	// ������� ����� �����
	m_PPLSrc = "";
	m_PPLDst = "";
	m_PPLParm = "";
	// ��������� ������ � ����� �����
	UpdateData(FALSE);
	// ������� ������� �����������
	m_PPLInfo.arTrn.RemoveAll();
	// ������� ������ �����������
	m_PPLList.DeleteAllItems();
}

BOOL CCardPPLView::CheckPPLFile()
{
	// ��� ��������� ����� ����������
	CString		fName;
	CFileFind	fFind;
	// ����� �����
	m_EditFile.GetWindowText(fName);
	if (!fFind.FindFile(TrnParm.sDirPpl + fName))
	{
		m_EditFile.SetWindowText(m_PPLFile);
		m_EditFile.SetModify(FALSE );
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(CString(GetErrMsg(ER_FIND_FILE, TrnParm.sDirPpl+fName)), 
						m_strTitle, MB_ICONEXCLAMATION|MB_OK);
		return FALSE;
	}
	// ��������� ������ � �����
	m_PPLFile = fName;
	UpdateData( FALSE );
	// ������ ���������� � ����������
	ReadPPLInfo();

	return TRUE;
}

BOOL CCardPPLView::InsertTRN(int iItem, CString sName, int iSchema)
{
	// ���������� ���������� � ������
	// ������������
	LV_ITEM	item;
	item.iItem = iItem;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT;
	item.pszText = sName.GetBuffer(255);
	m_PPLList.InsertItem(&item);
	// �����
	item.iSubItem = 1;
	item.mask = LVIF_TEXT;
	CString sSchema;
	switch (iSchema) {
	case TRN_COPY:    sSchema.LoadString(IDS_SCHEMA_COPY);    break;
	case TRN_COLUMNS: sSchema.LoadString(IDS_SCHEMA_COLUMNS); break;
	case TRN_COMPLEX: sSchema.LoadString(IDS_SCHEMA_COMPLEX); break;
	}
	item.pszText = sSchema.GetBuffer(sSchema.GetLength());
	m_PPLList.SetItem(&item);

	return TRUE;
}

BOOL CCardPPLView::ReadPPLInfo()
{
	// ������ ����������� �������
	CString	sFile = TrnParm.sDirPpl + m_PPLFile;
	int		i,n;
	// ������� ���������� � �������
	ClearPPLInfo();
	// ������ ������ �� ����� �������
	if (!ReadPPL(sFile, m_PPLInfo))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(CString(GetErrMsg(ER_READ_FILE, sFile)), 
					m_strTitle, MB_ICONEXCLAMATION|MB_OK);
		return FALSE;
	}
	// ��������� ����������
	m_PPLParm = m_PPLInfo.sParm;
	// ��������� ����������
	// ��������
	GetKeyVal(m_PPLInfo.sSrc, "DSN", m_PPLSrc);
	GetKeyVal(m_PPLInfo.sSrc, "UID", m_PPLSrcUID);
	GetKeyVal(m_PPLInfo.sSrc, "PWD", m_PPLSrcPWD);
	// ��������
	GetKeyVal(m_PPLInfo.sDst, "DSN", m_PPLDst);
	GetKeyVal(m_PPLInfo.sDst, "UID", m_PPLDstUID);
	GetKeyVal(m_PPLInfo.sDst, "PWD", m_PPLDstPWD);
	// ��������� ������ � ����� �����
	UpdateData(FALSE);
	// ���������� ������ �����������
	n = m_PPLInfo.arTrn.GetSize();
	for (i = 0; i < n; i++)
	{
		// ���������� ���������� � ������
		InsertTRN(i, m_PPLInfo.arTrn[i].sName, 
					 m_PPLInfo.arTrn[i].iSchema);
	}
	// ����������� ������� �������� ������
	if (m_PPLList.GetItemCount() > 0) {
		m_PPLList.SetItem(0, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
	}
	return TRUE;
}

BOOL CCardPPLView::WritePPLInfo()
{
	// ������ �������� ������� � ����
	CString	sFile = TrnParm.sDirPpl + m_PPLFile;
	// ��������� �������� � ���������
	m_PPLInfo.sSrc = "DSN=" + m_PPLSrc + 
					";UID=" + m_PPLSrcUID + 
					";PWD=" + m_PPLSrcPWD;
	m_PPLInfo.sDst = "DSN=" + m_PPLDst + 
					";UID=" + m_PPLDstUID + 
					";PWD=" + m_PPLDstPWD;

	return WritePPL(sFile, m_PPLName, m_PPLInfo);
}

void CCardPPLView::OnDataInsert()
{
	// ������� �������� � ������
	// �������� ������
	UpdateData();
	// �������� ������� �������
	CPropDlg trnSheet(IDS_MS_TRN_NEW, GetParent());
	// �������� ������� �������
	CTrnGenPage genPg;
	CTrnTabPage tabPg(TAB_DSTTAB_CAPTION);
	CTrnTabPage subPg(TAB_SUBTAB_CAPTION);
	CTrnCmdPage tCmPg(TAB_DSTCMD_CAPTION);
	CTrnCmdPage	sCmPg(TAB_SUBCMD_CAPTION);
	// ���������� ������� �������
	trnSheet.AddPage( &genPg );
	trnSheet.AddPage( &tabPg );
	trnSheet.AddPage( &subPg );
	trnSheet.AddPage( &tCmPg );
	trnSheet.AddPage( &sCmPg );
	// ��������� ������� �� ���������
	genPg.m_sODBCDSN	= "DSN=" + m_PPLSrc + 
						  ";UID=" + m_PPLSrcUID + 
						  ";PWD=" + m_PPLSrcPWD;
	tabPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;
	subPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;

	// ����� ������� �������
	if (trnSheet.DoModal() != IDOK) return;

	/////////////////////////////////////////
	// ��������� ������� ����������
	/////////////////////////////////////////
	int	iItem = m_PPLList.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem < 0) iItem = 0;
	int	i, nCount = m_PPLInfo.arTrn.GetSize();
	// ���������� ������� �������
	m_PPLInfo.arTrn.SetSize(nCount + 1);
	// ����� ��������� �������
	for (i = iItem; i < nCount; i++)
		m_PPLInfo.arTrn[i+1] = m_PPLInfo.arTrn[i];
	// ��������� ������� ��������
	i = iItem;
	// �������� ��������
	m_PPLInfo.arTrn[i].sName	= genPg.m_sName;
	m_PPLInfo.arTrn[i].iSchema	= genPg.m_nSchema;
	m_PPLInfo.arTrn[i].iConvert	= genPg.m_nConvert;
	m_PPLInfo.arTrn[i].sMsg		= genPg.m_sMsg;
	m_PPLInfo.arTrn[i].sTabSQL	= genPg.m_sTabSQL;
	m_PPLInfo.arTrn[i].sSubSQL	= genPg.m_sSubSQL;
	// ��������
	m_PPLInfo.arTrn[i].iTabPrc	= tabPg.m_nProcess;
	m_PPLInfo.arTrn[i].sTabOwn	= tabPg.m_sOwner;
	m_PPLInfo.arTrn[i].sTabDst	= tabPg.m_sTable;
	m_PPLInfo.arTrn[i].sTabCond	= tabPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sTabMap	= tabPg.m_sColumns;
	// ����������
	m_PPLInfo.arTrn[i].iSubPrc	= subPg.m_nProcess;
	m_PPLInfo.arTrn[i].sSubOwn	= subPg.m_sOwner;
	m_PPLInfo.arTrn[i].sSubDst	= subPg.m_sTable;
	m_PPLInfo.arTrn[i].sSubCond	= subPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sSubMap	= subPg.m_sColumns;
	// ������� ���������
	m_PPLInfo.arTrn[i].sTabCmd	= tCmPg.m_sTabCmd;
	// ������� ����������
	m_PPLInfo.arTrn[i].sSubCmd	= sCmPg.m_sTabCmd;
	// ������� ���������� � ������
	InsertTRN(i, m_PPLInfo.arTrn[i].sName, 
				 m_PPLInfo.arTrn[i].iSchema);
	// ��������� �������� ��������
	m_PPLList.SetItem(i, 0, LVIF_STATE, "", 0,
	LVIS_SELECTED|LVIS_FOCUSED,
	LVIS_SELECTED|LVIS_FOCUSED, 0);
	m_PPLList.EnsureVisible(i, FALSE);
	// ��������� �������� ���������
	OnFileChanged();
}
