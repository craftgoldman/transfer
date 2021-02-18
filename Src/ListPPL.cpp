// ListPPL.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "ChildFrm.h"
#include "ListPPL.h"
#include "CardPPL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListPPLView

IMPLEMENT_DYNCREATE(CListPPLView, CListView)

CListPPLView::CListPPLView()
{
}

CListPPLView::~CListPPLView()
{
}

BEGIN_MESSAGE_MAP(CListPPLView, CListView)
	ON_MESSAGE(FORM_QUERY_SAVE,OnFormQuerySave)
	ON_MESSAGE(FORM_UPDATE_DATA,OnFormUpdateData)
	//{{AFX_MSG_MAP(CListPPLView)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_DATA_ADD, OnDataAdd)
	ON_COMMAND(ID_DATA_RETRIEVE, OnDataRetrieve)
	ON_COMMAND(ID_DATA_SAVE, OnDataSave)
	ON_COMMAND(ID_DATA_EDIT, OnDataEdit)
	ON_COMMAND(ID_DATA_DELETE, OnDataDelete)
	ON_COMMAND(ID_DATA_FIRST, OnDataFirst)
	ON_COMMAND(ID_DATA_PREV, OnDataPrev)
	ON_COMMAND(ID_DATA_NEXT, OnDataNext)
	ON_COMMAND(ID_DATA_LAST, OnDataLast)
	ON_COMMAND(ID_DATA_CLOSE, OnDataClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListPPLView diagnostics

#ifdef _DEBUG
void CListPPLView::AssertValid() const
{
	CListView::AssertValid();
}

void CListPPLView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListPPLView message handlers

BOOL CListPPLView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Предустановка стиля ListView
	cs.style |= LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS;

	return CListView::PreCreateWindow(cs);
}

BOOL CListPPLView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if( !CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext) )
		return FALSE;
	// Начальная инициализация
	OnInitialUpdate();
	// Заполнение списка элементов
	OnDataRetrieve();
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

void CListPPLView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	CListCtrl&	ListCtrl = GetListCtrl();
	CRect		rect;
	GetParentFrame()->GetClientRect(rect);
	// Установка расширенного стиля ListView
	ListCtrl.SetExtendedStyle(ListCtrl.GetExtendedStyle()|
	LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	// Создание списка изображений
	m_imgSmall.Create(IDB_SMALLPPL, 16, 1, RGB(255, 255, 255));
	ListCtrl.SetImageList(&m_imgSmall, LVSIL_SMALL);
	// Установка колонок
	CString sFile,sName;
	sFile.LoadString(TAB_FLD_FILE);
	sName.LoadString(TAB_FLD_NAME);
	ListCtrl.InsertColumn(0, sFile, LVCFMT_LEFT, rect.Width()*1/3);
	ListCtrl.InsertColumn(1, sName, LVCFMT_LEFT, rect.Width()*2/3);
}

LRESULT CListPPLView::OnFormQuerySave(WPARAM wParam, LPARAM lParam)
{
	// Закрытие карточки
	CChildFrame* pWnd;
	if (IsWindow(m_hChildFrm))
	{
		pWnd = (CChildFrame* )FromHandle(m_hChildFrm);
		if (pWnd->SendMessage(FORM_QUERY_SAVE,
			wParam, lParam) != 0)
		{
			pWnd->MDIActivate();
			return 1;
		}
		pWnd->MDIDestroy();
	}
	return 0;
}

LRESULT CListPPLView::OnFormUpdateData(WPARAM wParam, LPARAM lParam)
{
	CListCtrl&	ListCtrl = GetListCtrl();
	PPL*		sPPL = (PPL* )wParam;
	// Обновление данных
	OnDataRetrieve();
	// Установка текущего элемента
	if (sPPL != NULL)
	{
		ListCtrl.SetItem(sPPL->iNum, 0,
			LVIF_STATE, "", 0,
			LVIS_SELECTED | LVIS_FOCUSED,
			LVIS_SELECTED | LVIS_FOCUSED, 0);
	}
	return 0;
}

void CListPPLView::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDataEdit();
	*pResult = 0;
}

void CListPPLView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDataEdit();
	*pResult = 0;
}

void CListPPLView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMDIFrameWnd* pFrm = (CMDIFrameWnd* )AfxGetMainWnd();
	CMenu*	pMenu;
	CPoint	point;

	// Показ подменю "Действия"
	if (pFrm != NULL)
	{
		::GetCursorPos(&point);
		pMenu = pFrm->GetMenu()->GetSubMenu(2);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, GetParent());
	}
	*pResult = 0;
}

void CListPPLView::OnDataAdd() 
{
	CChildFrame* pWnd;
	CRect		 rect(0,0,560,370);

	// Проверка карточки
	if (IsWindow(m_hChildFrm))
	{
		pWnd = (CChildFrame* )FromHandle( m_hChildFrm );
		pWnd->MDIActivate();
		// Если форма в режиме редактирования
		if (pWnd->SendMessage(FORM_GET_STATE) == FORM_EDIT)
			// Перевод формы в режим ввода
			pWnd->SendMessage(WM_COMMAND, ID_DATA_ADD, 0);
		return;
	}
	// Установка позиции и размеров окна
	if (TrnParm.iSaveMDI)
	{
		CString sKey;
		int x,y,w,h;
		// Получение координат и размеров окна из реестра
		sKey.LoadString(IDS_SYS_REG_SET);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplX"), (UINT&)x, 0);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplY"), (UINT&)y, 0);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplW"), (UINT&)w, 560);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplH"), (UINT&)h, 370);
		rect.left	= x;
		rect.right	= x + w;
		rect.top	= y;
		rect.bottom	= y + h;
	}
	// Открытие новой карточки
	pWnd = new CChildFrame;
	CCreateContext pContext;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CCardPPLView);
	CString strCap;
	strCap.LoadString(IDS_MS_PPL);
	pWnd->Create(NULL, strCap, WS_CHILD|WS_CAPTION|WS_SYSMENU|WS_SIZEBOX, rect,
				(CMDIFrameWnd *)AfxGetMainWnd(), &pContext);
	m_hChildFrm = pWnd->m_hWnd;
	pWnd->m_pParentWnd = (CMDIChildWnd* )GetParent();
	pWnd->SetIcon(AfxGetApp()->LoadIcon(IDI_CARDFILE), TRUE);
	if (TrnParm.iMaxiMDI)
		pWnd->ShowWindow(SW_SHOWMAXIMIZED);
	else
		pWnd->ShowWindow(SW_SHOWNORMAL);
	pWnd->MDIActivate();
	// Установка статуса формы
	pWnd->SendMessage(FORM_SET_STATE, FORM_NEW);
}

void CListPPLView::OnDataRetrieve() 
{
	UINT	idx;
	UINT	iItem;
	UINT	i,n,m;
	UINT	nCount;
	CString	sNPP;
	CString	sPPL;
	CString	sKey;
	LV_ITEM	item;
	CListCtrl& ListCtrl = GetListCtrl();

	// Сохранение текущего элемента
	iItem = ListCtrl.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem < 0) iItem = 0;
	// Удаление элементов
	ListCtrl.DeleteAllItems();
	// Заполнение списка элементов
	// из файла инициализации
	sKey.LoadString(IDS_SYS_REG_PIPE);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("Count"), nCount);
	arPPL.SetSize(nCount);
	for (i = 0; i < nCount; i++)
	{
		sNPP.Format("%i", i);
		QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("PPL" + sNPP), sPPL);
		n = sPPL.Find(",", 0);
		m = sPPL.Find(",", n + 1);
		// Заполнение структуры массива
		arPPL[i].iNum  = i;
		arPPL[i].iKind = atoi(sPPL.Left(n));
		arPPL[i].sFile = sPPL.Mid(n + 1, m - n - 1);
		arPPL[i].sName = sPPL.Mid(m + 1);
		// Добавление элемента в список
		item.iItem = i;
		// Файл
		item.iSubItem = 0;
		item.mask = LVIF_TEXT|LVIF_IMAGE;
		item.iImage = arPPL[i].iKind;
		item.pszText = arPPL[i].sFile.GetBuffer(255);
		idx = ListCtrl.InsertItem(&item);
		// Наименование
		item.iSubItem = 1;
		item.mask = LVIF_TEXT;
		item.pszText = arPPL[i].sName.GetBuffer(255);
		ListCtrl.SetItem(&item);
		// Вид
		ListCtrl.SetItemData(idx, arPPL[i].iKind);
	}
	// Установка активного элемента
	if( ListCtrl.GetItemCount() > 0 )
		ListCtrl.SetItem(iItem, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
}

void CListPPLView::OnDataSave() 
{
}

void CListPPLView::OnDataEdit() 
{
	CListCtrl&	 ListCtrl = GetListCtrl();
	CChildFrame* pWnd;
	CRect		 rect(0,0,560,370);

	if (ListCtrl.GetItemCount() == 0) return;
	// Проверка карточки
	if (IsWindow(m_hChildFrm))
	{
		pWnd = (CChildFrame* )FromHandle( m_hChildFrm );
		pWnd->MDIActivate();
		// Если форма в режиме редактирования
		if (pWnd->SendMessage(FORM_GET_STATE) == FORM_EDIT)
			// Обновление данных в форме
			UpdateCardData( pWnd );
		return;
	}
	// Установка позиции и размеров окна
	if (TrnParm.iSaveMDI)
	{
		CString sKey;
		int x,y,w,h;
		// Получение координат и размеров окна из реестра
		sKey.LoadString(IDS_SYS_REG_SET);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplX"), (UINT&)x, 0);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplY"), (UINT&)y, 0);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplW"), (UINT&)w, 560);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplH"), (UINT&)h, 370);
		rect.left	= x;
		rect.right	= x + w;
		rect.top	= y;
		rect.bottom	= y + h;
	}
	// Открытие новой карточки
	pWnd = new CChildFrame;
	CCreateContext pContext;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CCardPPLView);
	CString strCap;
	strCap.LoadString(IDS_MS_PPL);
	pWnd->Create(NULL, strCap, WS_CHILD|WS_CAPTION|WS_SYSMENU|WS_SIZEBOX, rect,
				(CMDIFrameWnd *)AfxGetMainWnd(), &pContext);
	m_hChildFrm = pWnd->m_hWnd;
	pWnd->m_pParentWnd = (CMDIChildWnd* )GetParent();
	pWnd->SetIcon(AfxGetApp()->LoadIcon(IDI_CARDFILE), TRUE);
	if (TrnParm.iMaxiMDI)
		pWnd->ShowWindow(SW_SHOWMAXIMIZED);
	else
		pWnd->ShowWindow(SW_SHOWNORMAL);
	pWnd->MDIActivate();
	// Обновление данных в форме
	UpdateCardData( pWnd );
}

void CListPPLView::OnDataDelete() 
{
	CListCtrl& ListCtrl = GetListCtrl();
	int		nCount = ListCtrl.GetItemCount();
	int		i,iItem;
	CString	sNum;
	CString	sKind;
	CString	sFile;
	CString	sName;
	CString	sKey;

	// Поиск текущего элемента
	iItem = ListCtrl.GetNextItem(-1, LVNI_SELECTED);
	if (iItem < 0) return;
	CString strMsg;
	strMsg.LoadString(IDS_MS_DELETE);
	if (MessageBox(strMsg, m_strTitle, 
		MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		nCount--;
		// Удаление текущего элемента
		ListCtrl.DeleteItem(iItem);
		ListCtrl.SetItemCount(nCount);

		sKey.LoadString(IDS_SYS_REG_PIPE);
		// Очистка ключа [pipeline] в реестре
		RegDeleteKey(HKEY_LOCAL_MACHINE, sKey);

		// Запись ключа [pipeline] в реестр
		arPPL.SetSize(nCount);
		WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("Count"), nCount);
		for (i = 0; i < nCount; i++)
		{
			sNum.Format("%i", i);
			sKind.Format("%i", ListCtrl.GetItemData(i));
			sFile = ListCtrl.GetItemText(i, 0);
			sName = ListCtrl.GetItemText(i, 1);
			WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, 
			_T("PPL"+sNum), _T(sKind+","+sFile+","+sName));
		}
		// Установка активного элемента
		if (nCount > 0)
		{
			if (iItem == nCount)
				iItem = nCount - 1;
			ListCtrl.SetItem(iItem, 0, 
			LVIF_STATE, "", 0,
			LVIS_SELECTED | LVIS_FOCUSED,
			LVIS_SELECTED | LVIS_FOCUSED, 0);
		}
	}
}

void CListPPLView::OnDataFirst() 
{
	CListCtrl&	ListCtrl = GetListCtrl();
	// Активизация первого элемента
	if (ListCtrl.GetItemCount() > 0)
		ListCtrl.SetItem(0, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
}

void CListPPLView::OnDataPrev() 
{
	CListCtrl&	ListCtrl = GetListCtrl();
	int			iItem;
	// Активизация предыдущего элемента
	iItem = ListCtrl.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem > 0)
	{
		iItem = ListCtrl.GetNextItem(iItem, LVNI_ABOVE);
		ListCtrl.SetItem(iItem, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
	}
}

void CListPPLView::OnDataNext() 
{
	CListCtrl&	ListCtrl = GetListCtrl();
	int			iCount = ListCtrl.GetItemCount();
	int			iItem;
	// Активизация следующего элемента
	iItem = ListCtrl.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem < iCount - 1)
	{
		iItem = ListCtrl.GetNextItem(iItem, LVNI_BELOW);
		ListCtrl.SetItem(iItem, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
	}
}

void CListPPLView::OnDataLast() 
{
	CListCtrl&	ListCtrl = GetListCtrl();
	int			iCount = ListCtrl.GetItemCount();
	// Активизация последнего элемента
	if (iCount > 0)
		ListCtrl.SetItem(iCount-1, 0, LVIF_STATE, "", 0,
		LVIS_SELECTED|LVIS_FOCUSED,
		LVIS_SELECTED|LVIS_FOCUSED, 0);
}

void CListPPLView::OnDataClose() 
{
	// Закрытие карточки
	CChildFrame* pWnd;
	if (IsWindow(m_hChildFrm))
	{
		pWnd = (CChildFrame* )FromHandle( m_hChildFrm );
		if (pWnd->SendMessage(FORM_QUERY_SAVE) != 0)
		{
			pWnd->MDIActivate();
			return;
		}
		pWnd->MDIDestroy();
	}
	// Закрытие собственного MDI окна
	pWnd = (CChildFrame* )GetParent();
	if (pWnd != NULL)
		pWnd->MDIDestroy();
}

void CListPPLView::UpdateCardData(CChildFrame* pWnd)
{
	// Обновление данных в карточке
	if (pWnd == NULL) return;
	// Запрос на сохранение данных
	if (pWnd->SendMessage(FORM_QUERY_SAVE) != 0)
		return;

	CListCtrl&	ListCtrl = GetListCtrl();
	int			iItem;
	PPL			sPPL;

	iItem = ListCtrl.GetNextItem(-1, LVNI_FOCUSED);
	if (iItem >= 0)
	{
		sPPL.iNum  = iItem;
		sPPL.iKind = ListCtrl.GetItemData(iItem);
		sPPL.sFile = ListCtrl.GetItemText(iItem, 0);
		sPPL.sName = ListCtrl.GetItemText(iItem, 1);
		// Запрос на обновление данных
		pWnd->SendMessage(FORM_UPDATE_DATA, (DWORD)(&sPPL));
	}
}
