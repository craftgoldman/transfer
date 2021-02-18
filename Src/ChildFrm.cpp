// ChildFrm.cpp : implementation file
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
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

CChildFrame::CChildFrame()
{
	m_bOpen = FALSE;
}

CChildFrame::~CChildFrame()
{
}


BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_MESSAGE(FORM_GET_STATE,OnFormGetState)
	ON_MESSAGE(FORM_SET_STATE,OnFormSetState)
	ON_MESSAGE(FORM_UPDATE_DATA,OnFormUpdateData)
	ON_MESSAGE(FORM_UPDATE_MENU,OnFormUpdateMenu)
	ON_MESSAGE(FORM_QUERY_SAVE,OnFormQuerySave)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (menuMDI.m_hMenu == NULL)
		menuMDI.LoadMenu(IDR_CHILDWND);
	m_hMenuShared = menuMDI.m_hMenu;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Посылка сообщения главному окну
	// для обновления ToolBar'а
	// (Подразумеваем, что оно всегда существует)
	GetMDIFrame()->SendMessage(MDI_UPDATE_BAR, 1);
	return 0;
}

void CChildFrame::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	// Посылка сообщения главному окну
	// для обновления ToolBar'а
	// (Подразумеваем, что оно всегда существует)
	GetMDIFrame()->SendMessage(MDI_UPDATE_BAR, 0);
}

LRESULT CChildFrame::OnFormGetState(WPARAM wParam, LPARAM lParam)
{
	CView* pView = GetActiveView();
	if (pView != NULL)
		return pView->SendMessage(FORM_GET_STATE, wParam, lParam);
	return 0;
}

LRESULT CChildFrame::OnFormSetState(WPARAM wParam, LPARAM lParam)
{
	CView* pView = GetActiveView();
	if( pView != NULL )
		pView->SendMessage(FORM_SET_STATE, wParam, lParam);
	return 0;
}

LRESULT CChildFrame::OnFormUpdateData(WPARAM wParam, LPARAM lParam)
{
	CView* pView = GetActiveView();
	if (pView != NULL)
		pView->SendMessage(FORM_UPDATE_DATA, wParam, lParam);
	return 0;
}

LRESULT CChildFrame::OnFormUpdateMenu(WPARAM wParam, LPARAM lParam)
{
	if (menuMDI.m_hMenu != NULL)
	{
		menuMDI.DestroyMenu();
	}
	menuMDI.LoadMenu(IDR_CHILDWND);
	m_hMenuShared = menuMDI.m_hMenu;
	SetMenu(&menuMDI);
	return 0;
}

LRESULT CChildFrame::OnFormQuerySave(WPARAM wParam, LPARAM lParam)
{
	CView* pView = GetActiveView();
	if (pView != NULL) {
		return pView->SendMessage(FORM_QUERY_SAVE, wParam, lParam);
	}
	return 0;
}

void CChildFrame::OnClose() 
{
	CView*	pView = GetActiveView();
	if (pView != NULL) {
		if (pView->SendMessage(FORM_QUERY_SAVE) != 0)
			return;
	}
	CMDIChildWnd::OnClose();
}

void CChildFrame::OnMove(int x, int y) 
{
	CMDIChildWnd::OnMove(x, y);
	
	RECT	rClient;
	RECT	rMDIWnd;
	int		xpos, ypos;
	CString sKey;
	CView* pView = GetActiveView();
	CMDIFrameWnd* pMain = GetMDIFrame();

	// Сохранение позиции окна в реестре
	if (m_bOpen && TrnParm.iSaveMDI)
	{
		this->GetWindowRect(&rMDIWnd);
		pMain->GetClientRect(&rClient);
		ClientToScreen(&rClient);
		xpos = rMDIWnd.left + x - rClient.left;
		ypos = rMDIWnd.top + y - rClient.top;

		// Список Трансфертов
		if (pView->IsKindOf(RUNTIME_CLASS(CListPPLView)))
		{
			sKey.LoadString(IDS_SYS_REG_SET);
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplX"), (UINT)xpos);
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplY"), (UINT)ypos);
		}
		// Карточка Трансферта
		else if (pView->IsKindOf(RUNTIME_CLASS(CCardPPLView)))
		{
			sKey.LoadString(IDS_SYS_REG_SET);
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplX"), (UINT)xpos);
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplY"), (UINT)ypos);
		}
	}
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	RECT	rect;
	CString sKey;
	CView* pView = GetActiveView();

	// Сохранение позиции окна в реестре
	if (m_bOpen && TrnParm.iSaveMDI)
	{
		GetWindowRect(&rect);

		// Список Трансфертов
		if (pView->IsKindOf(RUNTIME_CLASS(CListPPLView)))
		{
			sKey.LoadString(IDS_SYS_REG_SET);
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplW"), (UINT)(rect.right - rect.left));
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplH"), (UINT)(rect.bottom - rect.top));
		}
		// Карточка Трансферта
		else if (pView->IsKindOf(RUNTIME_CLASS(CCardPPLView)))
		{
			sKey.LoadString(IDS_SYS_REG_SET);
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplW"), (UINT)(rect.right - rect.left));
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("CardPplH"), (UINT)(rect.bottom - rect.top));
		}
	}
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	m_bOpen = TRUE;
}
