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

	// Начальная инициализация
	OnInitialUpdate();

	// Установка активного представления
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
	{
		pWnd->SetActiveView(this, TRUE);
		// Назначение заголовка окна
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

	// Установка изображений для кнопок
	m_bOK.SetIcon(IDI_BOK, 16, 16);
	m_bOK.SetDefaultButton();
	m_bSAVE.SetIcon(IDI_BSAVE, 16, 16);
	m_bCANCEL.SetIcon(IDI_BCANCEL, 16, 16);
	m_bADD.SetIcon(IDI_BADD, 16, 16);
	m_bINSERT.SetIcon(IDI_BINSERT, 16, 16);
	m_bEDIT.SetIcon(IDI_BEDIT, 16, 16);
	m_bDELETE.SetIcon(IDI_BDELETE, 16, 16);
	// Установка расширенного стиля ListView
	m_PPLList.SetExtendedStyle(m_PPLList.GetExtendedStyle()|
		LVS_EX_FULLROWSELECT);
	// Установка колонок в списке
	CRect rect;
	CString sTName,sSchem;
	m_PPLList.GetClientRect(rect);
	sTName.LoadString(TAB_FLD_TRANS);
	sSchem.LoadString(TAB_FLD_SCHEMA);
	m_PPLList.InsertColumn(0, sTName, LVCFMT_LEFT, rect.Width()*4/5);
	m_PPLList.InsertColumn(1, sSchem, LVCFMT_LEFT, rect.Width()*1/5);
	// Заполнение списка источников ODBC
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
	// Установка статуса формы
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
	// Чтение статуса формы
	return (LRESULT)m_nState;
}

LRESULT CCardPPLView::OnFormUpdateData(WPARAM wParam, LPARAM lParam)
{
	// Обновление данных в форме
	PPL* sPPL = (PPL* )wParam;
	// Установка полей формы
	if (sPPL != NULL)
	{
		m_PPLNum  = sPPL->iNum;
		m_PPLKind = sPPL->iKind;
		m_PPLFile = sPPL->sFile;
		m_PPLName = sPPL->sName;
		UpdateData(FALSE);
	}
	// Чтение данных из файла трансферта
	OnDataRetrieve();

	return 0;
}

LRESULT CCardPPLView::OnFormQuerySave(WPARAM wParam, LPARAM lParam)
{
	// Запрос на сохранение данных
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
	// Проверка имени файла на изменение
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
	// Показ контекстного меню "Действия"
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
	// Редактирование элемента списка
	OnDataEdit();
	*pResult = 0;
}

void CCardPPLView::OnBtnOk() 
{
	// Сохранение данных
	if (!QuerySave(FALSE)) return;
	// Закрытие MDI окна
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
		pWnd->MDIDestroy();
}

void CCardPPLView::OnBtnSave() 
{
	// Сохранение данных
	OnDataSave();
}

void CCardPPLView::OnBtnCancel() 
{
	// Сохранение данных и закрытие окна
	OnDataClose();
}

void CCardPPLView::OnBtnAdd() 
{
	// Добавление элемента в список
	m_bListFocus = TRUE;
	OnDataAdd();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnBtnInsert() 
{
	// Вставка элемента в список
	m_bListFocus = TRUE;
	OnDataInsert();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnBtnEdit() 
{
	// Редактирование элемента списка
	m_bListFocus = TRUE;
	OnDataEdit();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnBtnDelete() 
{
	// Удаление элемента из списка
	m_bListFocus = TRUE;
	OnDataDelete();
	m_PPLList.SetFocus();
}

void CCardPPLView::OnEditFile() 
{
	// Изменение имени файла трансферта
	CWnd*		wnd = GetParent();
	CString		ext = "ppl";
	CString		flt;
	flt.LoadString(IDS_MS_FILTER);
	CFileDialog	dlg(TRUE,ext,NULL,0,flt,wnd);

	// Принятие данных
	UpdateData(TRUE);
	dlg.m_ofn.lpstrInitialDir = TrnParm.sDirPpl;
	dlg.m_ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|
		OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|
		OFN_LONGNAMES|OFN_NOCHANGEDIR|
		OFN_NODEREFERENCELINKS;
	if (dlg.DoModal() == IDOK)
	{
		// Установка данных в форме
		m_PPLFile = dlg.GetFileName();
		UpdateData(FALSE);
		// Чтение информации о трансферте
		ReadPPLInfo();
		// Установка статуса изменения
		OnHeadChanged();
	}
}

void CCardPPLView::OnEditParm() 
{
	// Редактирование параметров
	CTrnParmDlg	parmDlg;
	CString		sParm;

	// Принятие текущих данных
	UpdateData(TRUE);
	// Установка параметров
	parmDlg.m_strParm = m_PPLParm;
	if (parmDlg.DoModal() == IDOK)
	{
		// Установка данных в форме
		m_PPLInfo.sParm = parmDlg.m_strParm;
		m_PPLParm = parmDlg.m_strParm;
		UpdateData(FALSE);
		OnFileChanged();
	}
}

void CCardPPLView::OnHeadChanged() 
{
	// Установка статуса изменения
	if (!m_bHeadChanged && 
		!m_bFileChanged &&
		m_nState == FORM_EDIT)
		MarkTitle();
	m_bHeadChanged = TRUE;
}

void CCardPPLView::OnFileChanged() 
{
	// Установка статуса изменения
	if (!m_bHeadChanged &&
		!m_bFileChanged && 
		m_nState == FORM_EDIT)
		MarkTitle();
	m_bFileChanged = TRUE;
}

void CCardPPLView::OnDataAdd() 
{
	// Новый элемент в списке
	if (m_bListFocus)
	{
		// Принятие данных
		UpdateData();
		// Создание объекта свойств
		CPropDlg trnSheet(IDS_MS_TRN_NEW, GetParent());
		// Создание страниц свойств
		CTrnGenPage genPg;
		CTrnTabPage tabPg(TAB_DSTTAB_CAPTION);
		CTrnTabPage subPg(TAB_SUBTAB_CAPTION);
		CTrnCmdPage tCmPg(TAB_DSTCMD_CAPTION);
		CTrnCmdPage	sCmPg(TAB_SUBCMD_CAPTION);
		// Добавление страниц свойств
		trnSheet.AddPage( &genPg );
		trnSheet.AddPage( &tabPg );
		trnSheet.AddPage( &subPg );
		trnSheet.AddPage( &tCmPg );
		trnSheet.AddPage( &sCmPg );
		// Установка свойств на страницах
		genPg.m_sODBCDSN	= "DSN=" + m_PPLSrc + 
							  ";UID=" + m_PPLSrcUID + 
							  ";PWD=" + m_PPLSrcPWD;
		tabPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
							  ";UID=" + m_PPLDstUID + 
							  ";PWD=" + m_PPLDstPWD;
		subPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
							  ";UID=" + m_PPLDstUID + 
							  ";PWD=" + m_PPLDstPWD;

		// Вызов диалога свойств
		if (trnSheet.DoModal() != IDOK) return;

		/////////////////////////////////////////
		// Установка свойств трансферта
		/////////////////////////////////////////
		int i = m_PPLList.GetItemCount();
		m_PPLInfo.arTrn.SetSize(i + 1);
		// Основная страница
		m_PPLInfo.arTrn[i].sName	= genPg.m_sName;
		m_PPLInfo.arTrn[i].iSchema	= genPg.m_nSchema;
		m_PPLInfo.arTrn[i].sMsg		= genPg.m_sMsg;
		m_PPLInfo.arTrn[i].sTabSQL	= genPg.m_sTabSQL;
		m_PPLInfo.arTrn[i].sSubSQL	= genPg.m_sSubSQL;
		// Приемник
		m_PPLInfo.arTrn[i].iTabPrc	= tabPg.m_nProcess;
		m_PPLInfo.arTrn[i].sTabOwn	= tabPg.m_sOwner;
		m_PPLInfo.arTrn[i].sTabDst	= tabPg.m_sTable;
		m_PPLInfo.arTrn[i].sTabCond	= tabPg.m_sSQLCond;
		m_PPLInfo.arTrn[i].sTabMap	= tabPg.m_sColumns;
		// Субтаблица
		m_PPLInfo.arTrn[i].iSubPrc	= subPg.m_nProcess;
		m_PPLInfo.arTrn[i].sSubOwn	= subPg.m_sOwner;
		m_PPLInfo.arTrn[i].sSubDst	= subPg.m_sTable;
		m_PPLInfo.arTrn[i].sSubCond	= subPg.m_sSQLCond;
		m_PPLInfo.arTrn[i].sSubMap	= subPg.m_sColumns;
		// Команды приемника
		m_PPLInfo.arTrn[i].sTabCmd	= tCmPg.m_sTabCmd;
		// Команды субтаблицы
		m_PPLInfo.arTrn[i].sSubCmd	= sCmPg.m_sTabCmd;
		// Добавление трансферта в список
		InsertTRN(i, m_PPLInfo.arTrn[i].sName, 
					 m_PPLInfo.arTrn[i].iSchema);
		// Установка текущего элемента
		m_PPLList.SetItem(i, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
		m_PPLList.EnsureVisible(i, FALSE);
		// Установка признака изменения
		OnFileChanged();
	}
	// Новая форма
	else
	{
		// Сохранение данных
		if (!QuerySave(FALSE)) return;
		// Очистка данных в форме
		m_PPLFile = "";
		m_PPLName = "";
		m_PPLKind = 0;
		UpdateData(FALSE);
		// Очистка информации о трансферте
		ClearPPLInfo();
		// Установка заголовка окна
		CChildFrame* pWnd = (CChildFrame* )GetParent();
		if (pWnd != NULL)
		{
			CString strCap;
			strCap.LoadString(IDS_MS_NEW);
			strCap = m_strTitle + strCap;
			pWnd->SetWindowText(strCap);
		}
		// Установка статуса
		m_PPLNum = -1;
		m_nState = FORM_NEW;
		m_bHeadChanged = TRUE;
		m_bFileChanged = TRUE;
	}
}

void CCardPPLView::OnDataRetrieve() 
{
	// Чтение данных из файла трансферта
	if (m_nState == FORM_NEW) return;
	// Сохранение данных
	if (!QuerySave()) return;
	// Установка заголовка окна
	CChildFrame*	pWnd;
	pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
		pWnd->SetWindowText(m_strTitle);
	// Заполнение элементов списка
	ReadPPLInfo();
	// Установка статуса формы
	m_nState = FORM_EDIT;
	m_bHeadChanged = FALSE;
	m_bFileChanged = FALSE;
}

void CCardPPLView::OnDataSave() 
{
	// Сохранение данных
	CChildFrame* pWnd;
	PPL			 sPPL;
	CString		 errText;
	CString		 sNum;
	CString		 sKind;
	CString		 sText;
	CString		 sKey;

	// Принятие данных
	UpdateData();
	if (!m_bHeadChanged &&
		!m_bFileChanged)
		return;
	// Проверка данных
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
	// Запись описания трансферта в файл
	// (выполняется первым, так как важнее)
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
	// Запись данных формы в реестр в ключ [pipeline]
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
	// Посылка сообщения окну-родителю
	// для обновления данных
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
		// Установка заголовка окна
		pWnd->SetWindowText(m_strTitle);
	}
	// Установка статуса
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
	// Редактирование элемента списка
	if (!m_bListFocus) return;
	// Поиск текущего элемента
	int	i = m_PPLList.GetNextItem(-1, LVNI_FOCUSED);
	if (i < 0 || i > m_PPLInfo.arTrn.GetUpperBound())
		return;

#ifdef _TRIAL_VER
	if (m_PPLInfo.arTrn[i].iSchema == TRN_COMPLEX)
	{
		CString sErr;
		// Ограничение по схеме в Trial версии
		sErr.FormatMessage(IDS_ER_TRIAL_COMPLEX);
		sErr.Insert(0, ":\n");
		sErr.Insert(0, LPCTSTR(m_PPLInfo.arTrn[i].sName));
		AfxMessageBox(sErr);
	}
#endif

	// Принятие данных
	UpdateData();
	CPropDlg trnSheet(IDS_MS_TRN_EDIT, GetParent());
	// Создание страниц свойств
	CTrnGenPage genPg;
	CTrnTabPage tabPg(TAB_DSTTAB_CAPTION);
	CTrnTabPage subPg(TAB_SUBTAB_CAPTION);
	CTrnCmdPage tCmPg(TAB_DSTCMD_CAPTION);
	CTrnCmdPage	sCmPg(TAB_SUBCMD_CAPTION);
	// Добавление страниц свойств
	trnSheet.AddPage( &genPg );
	trnSheet.AddPage( &tabPg );
	trnSheet.AddPage( &subPg );
	trnSheet.AddPage( &tCmPg );
	trnSheet.AddPage( &sCmPg );

	/////////////////////////////////////////
	// Установка данных на страницах
	/////////////////////////////////////////
	// Основная страница
	genPg.m_sODBCDSN	= "DSN=" + m_PPLSrc + 
						  ";UID=" + m_PPLSrcUID + 
						  ";PWD=" + m_PPLSrcPWD;
	genPg.m_sName		= m_PPLInfo.arTrn[i].sName;
	genPg.m_nSchema		= m_PPLInfo.arTrn[i].iSchema;
	genPg.m_nConvert	= m_PPLInfo.arTrn[i].iConvert;
	genPg.m_sMsg		= m_PPLInfo.arTrn[i].sMsg;
	genPg.m_sTabSQL		= m_PPLInfo.arTrn[i].sTabSQL;
	genPg.m_sSubSQL		= m_PPLInfo.arTrn[i].sSubSQL;
	// Приемник
	tabPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;
	// Создание объекта свойств
	tabPg.m_nProcess	= m_PPLInfo.arTrn[i].iTabPrc;
	tabPg.m_sOwner		= m_PPLInfo.arTrn[i].sTabOwn;
	tabPg.m_sTable		= m_PPLInfo.arTrn[i].sTabDst;
	tabPg.m_sSQLCond	= m_PPLInfo.arTrn[i].sTabCond;
	tabPg.m_sColumns	= m_PPLInfo.arTrn[i].sTabMap;
	// Субтаблица
	subPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;
	subPg.m_nProcess	= m_PPLInfo.arTrn[i].iSubPrc;
	subPg.m_sOwner		= m_PPLInfo.arTrn[i].sSubOwn;
	subPg.m_sTable		= m_PPLInfo.arTrn[i].sSubDst;
	subPg.m_sSQLCond	= m_PPLInfo.arTrn[i].sSubCond;
	subPg.m_sColumns	= m_PPLInfo.arTrn[i].sSubMap;
	// Команды приемника
	tCmPg.m_sTabCmd		= m_PPLInfo.arTrn[i].sTabCmd;
	// Команды субтаблицы
	sCmPg.m_sTabCmd		= m_PPLInfo.arTrn[i].sSubCmd;

	// Вызов диалога свойств
	if (trnSheet.DoModal() != IDOK) return;

	/////////////////////////////////////////
	// Изменение свойств трансферта
	/////////////////////////////////////////
	// Основная страница
	m_PPLInfo.arTrn[i].sName	= genPg.m_sName;
	m_PPLInfo.arTrn[i].iSchema	= genPg.m_nSchema;
	m_PPLInfo.arTrn[i].iConvert	= genPg.m_nConvert;
	m_PPLInfo.arTrn[i].sMsg		= genPg.m_sMsg;
	m_PPLInfo.arTrn[i].sTabSQL	= genPg.m_sTabSQL;
	m_PPLInfo.arTrn[i].sSubSQL	= genPg.m_sSubSQL;
	// Приемник
	m_PPLInfo.arTrn[i].iTabPrc	= tabPg.m_nProcess;
	m_PPLInfo.arTrn[i].sTabOwn	= tabPg.m_sOwner;
	m_PPLInfo.arTrn[i].sTabDst	= tabPg.m_sTable;
	m_PPLInfo.arTrn[i].sTabCond	= tabPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sTabMap	= tabPg.m_sColumns;
	// Субтаблица
	m_PPLInfo.arTrn[i].iSubPrc	= subPg.m_nProcess;
	m_PPLInfo.arTrn[i].sSubOwn	= subPg.m_sOwner;
	m_PPLInfo.arTrn[i].sSubDst	= subPg.m_sTable;
	m_PPLInfo.arTrn[i].sSubCond	= subPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sSubMap	= subPg.m_sColumns;
	// Команды приемника
	m_PPLInfo.arTrn[i].sTabCmd	= tCmPg.m_sTabCmd;
	// Команды субтаблицы
	m_PPLInfo.arTrn[i].sSubCmd	= sCmPg.m_sTabCmd;
	// Установка изменений в списке
	// Наименование
	m_PPLList.SetItemText(i, 0, m_PPLInfo.arTrn[i].sName);
	// Схема
	CString	sSchema;
	switch (m_PPLInfo.arTrn[i].iSchema)
	{
	case TRN_COPY:    sSchema = "Копия";	break;
	case TRN_COLUMNS: sSchema = "Таблица";	break;
	case TRN_COMPLEX: sSchema = "Комплекс";	break;
	}
	m_PPLList.SetItemText(i, 1, sSchema);
	// Установка признака изменения
	OnFileChanged();
}

void CCardPPLView::OnDataDelete() 
{
	// Удаление элемента в списке
	if (!m_bListFocus) return;
	// Поиск текущего элемента
	int	iItem = m_PPLList.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem < 0 || iItem > m_PPLInfo.arTrn.GetUpperBound())
		return;
	CString strMsg;
	strMsg.LoadString(IDS_MS_DELETE);
	if (AfxMessageBox(strMsg, MB_YESNO|MB_ICONQUESTION ) != IDYES)
		return;

	// Сдвиг элементов массива
	int	i, nCount = m_PPLInfo.arTrn.GetSize();
	for (i = iItem; i < nCount - 1; i++)
		m_PPLInfo.arTrn[i] = m_PPLInfo.arTrn[i+1];
	// Уменьшение размера массива
	m_PPLInfo.arTrn.SetSize(nCount - 1);
	// Удаление трансферта из списка
	m_PPLList.DeleteItem(iItem);
	// Установка текущего элемента
	if ((nCount = m_PPLList.GetItemCount()) > 0) {
		if (iItem >= nCount) iItem--;
		m_PPLList.SetItem(iItem, 
		0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
	}
	// Установка признака изменения
	OnFileChanged();
}

void CCardPPLView::OnDataClose() 
{
	// Закрытие собственного MDI окна
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL) {
		// Сохранение данных и закрытие окна
		if( QuerySave() ) pWnd->MDIDestroy();
	}
}

BOOL CCardPPLView::QuerySave(BOOL bQuestion)
{
	// Запрос на сохранение изменений
	// Проверка имени файла на изменение
	if (m_EditFile.GetModify() && 
		m_nState == FORM_EDIT)
	{
		if (!CheckPPLFile()) return FALSE;
	}
	// Проверка изменений
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
				// Установка статуса
				m_bHeadChanged = FALSE;
				m_bFileChanged = FALSE;
				return TRUE;
			case IDCANCEL:
				return FALSE;
			}
		}
		// Сохранение данных
		OnDataSave();
		// Проверка статуса
		if (m_bHeadChanged ||
			m_bFileChanged)
			return FALSE;
	}
	return TRUE;
}

void CCardPPLView::MarkTitle()
{
	// Пометка заголовка окна
	CChildFrame* pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
		pWnd->SetWindowText(m_strTitle + " *");
}

void CCardPPLView::ClearPPLInfo()
{
	// Очистка определения трансферта
	// Очистка полей формы
	m_PPLSrc = "";
	m_PPLDst = "";
	m_PPLParm = "";
	// Установка данных в полях формы
	UpdateData(FALSE);
	// Очистка массива трансфертов
	m_PPLInfo.arTrn.RemoveAll();
	// Очистка списка трансфертов
	m_PPLList.DeleteAllItems();
}

BOOL CCardPPLView::CheckPPLFile()
{
	// При изменении файла трансферта
	CString		fName;
	CFileFind	fFind;
	// Поиск файла
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
	// Установка данных в форме
	m_PPLFile = fName;
	UpdateData( FALSE );
	// Чтение информации о трансферте
	ReadPPLInfo();

	return TRUE;
}

BOOL CCardPPLView::InsertTRN(int iItem, CString sName, int iSchema)
{
	// Добавление трансферта в список
	// Наименование
	LV_ITEM	item;
	item.iItem = iItem;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT;
	item.pszText = sName.GetBuffer(255);
	m_PPLList.InsertItem(&item);
	// Схема
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
	// Чтение определения профиля
	CString	sFile = TrnParm.sDirPpl + m_PPLFile;
	int		i,n;
	// Очистка информации о профиле
	ClearPPLInfo();
	// Чтение данных из файла профиля
	if (!ReadPPL(sFile, m_PPLInfo))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(CString(GetErrMsg(ER_READ_FILE, sFile)), 
					m_strTitle, MB_ICONEXCLAMATION|MB_OK);
		return FALSE;
	}
	// Установка параметров
	m_PPLParm = m_PPLInfo.sParm;
	// Установки соединения
	// Источник
	GetKeyVal(m_PPLInfo.sSrc, "DSN", m_PPLSrc);
	GetKeyVal(m_PPLInfo.sSrc, "UID", m_PPLSrcUID);
	GetKeyVal(m_PPLInfo.sSrc, "PWD", m_PPLSrcPWD);
	// Приемник
	GetKeyVal(m_PPLInfo.sDst, "DSN", m_PPLDst);
	GetKeyVal(m_PPLInfo.sDst, "UID", m_PPLDstUID);
	GetKeyVal(m_PPLInfo.sDst, "PWD", m_PPLDstPWD);
	// Установка данных в полях формы
	UpdateData(FALSE);
	// Заполнение списка трансфертов
	n = m_PPLInfo.arTrn.GetSize();
	for (i = 0; i < n; i++)
	{
		// Добавление трансферта в список
		InsertTRN(i, m_PPLInfo.arTrn[i].sName, 
					 m_PPLInfo.arTrn[i].iSchema);
	}
	// Активизация первого элемента списка
	if (m_PPLList.GetItemCount() > 0) {
		m_PPLList.SetItem(0, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
	}
	return TRUE;
}

BOOL CCardPPLView::WritePPLInfo()
{
	// Запись описания профиля в файл
	CString	sFile = TrnParm.sDirPpl + m_PPLFile;
	// Установка значений в структуре
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
	// Вставка элемента в список
	// Принятие данных
	UpdateData();
	// Создание объекта свойств
	CPropDlg trnSheet(IDS_MS_TRN_NEW, GetParent());
	// Создание страниц свойств
	CTrnGenPage genPg;
	CTrnTabPage tabPg(TAB_DSTTAB_CAPTION);
	CTrnTabPage subPg(TAB_SUBTAB_CAPTION);
	CTrnCmdPage tCmPg(TAB_DSTCMD_CAPTION);
	CTrnCmdPage	sCmPg(TAB_SUBCMD_CAPTION);
	// Добавление страниц свойств
	trnSheet.AddPage( &genPg );
	trnSheet.AddPage( &tabPg );
	trnSheet.AddPage( &subPg );
	trnSheet.AddPage( &tCmPg );
	trnSheet.AddPage( &sCmPg );
	// Установка свойств на страницах
	genPg.m_sODBCDSN	= "DSN=" + m_PPLSrc + 
						  ";UID=" + m_PPLSrcUID + 
						  ";PWD=" + m_PPLSrcPWD;
	tabPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;
	subPg.m_sODBCDSN	= "DSN=" + m_PPLDst + 
						  ";UID=" + m_PPLDstUID + 
						  ";PWD=" + m_PPLDstPWD;

	// Вызов диалога свойств
	if (trnSheet.DoModal() != IDOK) return;

	/////////////////////////////////////////
	// Установка свойств трансферта
	/////////////////////////////////////////
	int	iItem = m_PPLList.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem < 0) iItem = 0;
	int	i, nCount = m_PPLInfo.arTrn.GetSize();
	// Увеличение размера массива
	m_PPLInfo.arTrn.SetSize(nCount + 1);
	// Сдвиг элементов массива
	for (i = iItem; i < nCount; i++)
		m_PPLInfo.arTrn[i+1] = m_PPLInfo.arTrn[i];
	// Установка индекса элемента
	i = iItem;
	// Основная страница
	m_PPLInfo.arTrn[i].sName	= genPg.m_sName;
	m_PPLInfo.arTrn[i].iSchema	= genPg.m_nSchema;
	m_PPLInfo.arTrn[i].iConvert	= genPg.m_nConvert;
	m_PPLInfo.arTrn[i].sMsg		= genPg.m_sMsg;
	m_PPLInfo.arTrn[i].sTabSQL	= genPg.m_sTabSQL;
	m_PPLInfo.arTrn[i].sSubSQL	= genPg.m_sSubSQL;
	// Приемник
	m_PPLInfo.arTrn[i].iTabPrc	= tabPg.m_nProcess;
	m_PPLInfo.arTrn[i].sTabOwn	= tabPg.m_sOwner;
	m_PPLInfo.arTrn[i].sTabDst	= tabPg.m_sTable;
	m_PPLInfo.arTrn[i].sTabCond	= tabPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sTabMap	= tabPg.m_sColumns;
	// Субтаблица
	m_PPLInfo.arTrn[i].iSubPrc	= subPg.m_nProcess;
	m_PPLInfo.arTrn[i].sSubOwn	= subPg.m_sOwner;
	m_PPLInfo.arTrn[i].sSubDst	= subPg.m_sTable;
	m_PPLInfo.arTrn[i].sSubCond	= subPg.m_sSQLCond;
	m_PPLInfo.arTrn[i].sSubMap	= subPg.m_sColumns;
	// Команды приемника
	m_PPLInfo.arTrn[i].sTabCmd	= tCmPg.m_sTabCmd;
	// Команды субтаблицы
	m_PPLInfo.arTrn[i].sSubCmd	= sCmPg.m_sTabCmd;
	// Вставка трансферта в список
	InsertTRN(i, m_PPLInfo.arTrn[i].sName, 
				 m_PPLInfo.arTrn[i].iSchema);
	// Установка текущего элемента
	m_PPLList.SetItem(i, 0, LVIF_STATE, "", 0,
	LVIS_SELECTED|LVIS_FOCUSED,
	LVIS_SELECTED|LVIS_FOCUSED, 0);
	m_PPLList.EnsureVisible(i, FALSE);
	// Установка признака изменения
	OnFileChanged();
}
