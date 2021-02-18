// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "time64.h"
#include "Trans30.h"
#include "MainFrm.h"
#include "AboutDlg.h"
#include "PropDlg.h"
#include "TrnOptPg.h"
#include "TrnIntPg.h"
#include "TrnPrcPg.h"
#include "TrnExcPg.h"
#include "ChildFrm.h"
#include "ListPPL.h"
#include "Base64.h"
#include "tools.h"
#include "rsa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_MESSAGE(MDI_UPDATE_BAR,OnMDIUpdateBar)
	ON_MESSAGE(TRAY_NOTIFYICON,OnTrayNotifyIcon)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TRANSFER, OnTransfer)
	ON_COMMAND(ID_IMPORT, OnImport)
	ON_COMMAND(ID_EXPORT, OnExport)
	ON_COMMAND(ID_PROFILE, OnProfile)
	ON_COMMAND(ID_TRANS_OPTIONS, OnTransOptions)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_START_EXCHANGE, OnStartExchange)
	ON_COMMAND(ID_STOP_EXCHANGE, OnStopExchange)
	ON_WM_ACTIVATEAPP()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_START_EXCHANGE, OnUpdateStartExchange)
	ON_UPDATE_COMMAND_UI(ID_STOP_EXCHANGE, OnUpdateStopExchange)
	ON_UPDATE_COMMAND_UI(ID_EXPORT, OnUpdateExport)
	ON_UPDATE_COMMAND_UI(ID_IMPORT, OnUpdateImport)
	ON_UPDATE_COMMAND_UI(ID_TRANSFER, OnUpdateTransfer)
	ON_UPDATE_COMMAND_UI(ID_PROFILE, OnUpdateProfile)
	ON_UPDATE_COMMAND_UI(ID_TRANS_OPTIONS, OnUpdateTransOptions)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateToolbar)
	ON_COMMAND(ID_VIEW_PROFILE_TBR, OnViewProfileTbr)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROFILE_TBR, OnUpdateProfileTbr)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateStatusbar)
	ON_COMMAND(ID_CUSTOMIZE_TOOLBAR, OnCustomizeToolbar)
	ON_COMMAND(ID_CUSTOMIZE_PROFILE_TBR, OnCustomizeProfileTbr)
	ON_WM_INITMENUPOPUP()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bCreated = FALSE;
	m_bSkipTst = FALSE;
	m_bInstErr = FALSE;
	m_bTrialEx = FALSE;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::ShowAboutDlg()
{
	OnAppAbout();
}

BOOL CMainFrame::CreateToolbars()
{
#ifdef _TRIAL_VER
	long n;
	UINT nDataCntVal;
	CString sKey,sVal,sDat;
	CTime installtime;
	SYSTEMTIME sinsttime;
	FILETIME fitime,fctime,f1time,fptime;

	if (IsDebuggerPresent())
		return FALSE;

	// Если это не первый запуск
	if (!m_bSkipTst)
	{
		// Получение текущего системного времени
		_getCurrentSysDT(fctime);
		// Чтение метки даты-времени первого запуска в реестре
		sKey.LoadString(IDS_SYS_REG_DT1STKEY);
		sVal.LoadString(IDS_SYS_REG_DT1STVAL);
		if (QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, sVal, sDat, _T("")))
		{
			// Преобразование строки в FILETIME
			n = GetWord(sDat, 0,   sKey);	// Low word
			n = GetWord(sDat, n+1, sVal);	// High word
			f1time.dwLowDateTime  = atol(sKey);
			f1time.dwHighDateTime = atol(sVal);

			// Сравнение даты-времени первого запуска и текущего даты-времени
			if (CompareFileTime(&fctime, &f1time) < 0)
				m_bTrialEx = TRUE;
		}
		if (!m_bTrialEx)
		{
			// Проверка метки о количестве запусков в реестре
			sKey.LoadString(IDS_SYS_REG_COUNTKEY);
			sVal.LoadString(IDS_SYS_REG_COUNTVAL);
			if (!QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataCntVal))
				m_bInstErr = TRUE;
		}
		if (!m_bInstErr)
		{
			// Чтение метки даты-времени предыдущего запуска в реестре
			sKey.LoadString(IDS_SYS_REG_DTPREKEY);
			sVal.LoadString(IDS_SYS_REG_DTPREVAL);
			if (QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, sVal, sDat, _T("")))
			{
				// Преобразование строки в FILETIME
				n = GetWord(sDat, 0,   sKey);	// Low word
				n = GetWord(sDat, n+1, sVal);	// High word
				fptime.dwLowDateTime  = atol(sKey);
				fptime.dwHighDateTime = atol(sVal);
				// Сравнение даты-времени текущего и предыдущего запуска
				if (CompareFileTime(&fptime, &fctime) > 0)
				{
					m_bInstErr = TRUE;
				}
				// Сравнение даты-времени предыдущего и первого запуска
				if (CompareFileTime(&f1time, &fptime) > 0)
				{
					m_bInstErr = TRUE;
				}
			}
		}
		if (!m_bInstErr)
		{
			// Чтение метки инсталляции в реестре
			sKey.LoadString(IDS_SYS_REG_INSTDATA);
			if (QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("tappinstdata"), sDat, _T("")))
			{
				// Расшифровка строки даты из метки инсталляции
				sDat.Format("%ld", (atol(sDat.Mid(2)) - 137) / 17);
				// Установка значения даты-времени из строки
				installtime = CTime(atoi(sDat.Mid(0,4)), atoi(sDat.Mid(4,2)), atoi(sDat.Mid(6,2)), 0, 0, 0, -1);
				// Преобразование даты-времени в FILETIME
				installtime.GetAsSystemTime(sinsttime);
				::SystemTimeToFileTime(&sinsttime, &fitime);
				// Сравнение даты текущей и из метки
				if (CompareFileTime(&fctime, &fitime) < 0)
				{
					m_bInstErr = TRUE;
				}
			}
			else
			{
				m_bInstErr = TRUE;
			}
		}
	}
#endif

	// Создание ToolBar'ов
	EnableDocking(CBRS_ALIGN_ANY);

	// ToolBar для фрейма
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create frame ToolBar\n");
		return FALSE;
	}

	CString s;
	// Разрешить настройку
	s.LoadString(ID_TOOLBAR_FILE);
	m_wndToolBar.SetWindowText(s);
	m_wndToolBar.SetToolBarInfoForCustomization();
	// Установить начальное состояние для сброса
	m_wndToolBar.MarkDefaultState();
	// Восстановить состояние
	m_wndToolBar.RestoreState();
	// Позиционировать панели инструментов
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	FrameEnableDocking(this,CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// ToolBar для профилей - невидимый
	if (!m_proToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_proToolBar.LoadToolBar(IDR_DATA))
	{
		TRACE0("Failed to create Profile ToolBar\n");
		return FALSE;
	}

	// Разрешить настройку
	s.LoadString(ID_TOOLBAR_PROFILE);
	m_proToolBar.SetWindowText(s);
	m_proToolBar.SetToolBarInfoForCustomization();
	// Установить начальное состояние для сброса
	m_proToolBar.MarkDefaultState();
	// Восстановить состояние
	m_proToolBar.RestoreState();
	// Позиционировать панели инструментов
	m_proToolBar.EnableDocking(CBRS_ALIGN_ANY);
	FrameEnableDocking(this,CBRS_ALIGN_ANY);
	DockControlBar(&m_proToolBar);

	// Add Toolbars to the menu
	AddToolBar(&m_wndToolBar);
	AddToolBar(&m_proToolBar);

#ifdef _TRIAL_VER
	if (IsDebuggerPresent())
		return FALSE;

	if (m_bTrialEx)
	{
		sVal.LoadString(IDS_ER_TRIAL_EXPIRED);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(sVal);
		return FALSE;
	}
	if (m_bInstErr)
	{
		sVal.LoadString(IDS_ER_INCOR_INSTALL);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(sVal);
		return FALSE;
	}
#endif

	return TRUE;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	CString sErr;

	if (IsDebuggerPresent())
		return FALSE;

#ifdef _TRIAL_VER
	UINT nDataRegVal;
	CString sKey,sVal;

	// Проверка метки "Trial период истек" в реестре
	sKey.LoadString(IDS_SYS_REG_TRENDKEY);
	sVal.LoadString(IDS_SYS_REG_TRENDVAL);
	if (QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataRegVal))
	{
		sErr.LoadString(IDS_ER_TRIAL_EXPIRED);
		goto err;
	}
#else
	int i;
	CString sVal,sInput,sMD5,sSignature;

	// Данные из файла
	if ((i = sLicense.Find(_T("Value="))) > 0)
		sInput = sLicense.Left(i+6);

	// Подпись MD5 из файла
	sSignature = GetKeyValue(sLicense, _T("Value"));

	// Подпись MD5 вычисленная
	sMD5 = GetMD5String(sInput.GetBuffer(sInput.GetLength()), 
						sInput.GetLength()).c_str();

	// Проверка MD5 подписи
	if (sMD5.Compare(sSignature) != 0)
	{
		sErr.LoadString(IDS_ER_WRONG_KEYFILE);
		goto err;
	}
#endif

	return CMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);

err:
	MessageBeep(MB_ICONEXCLAMATION);
	AfxMessageBox(sErr);
	return FALSE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	CString sErr;

	if (IsDebuggerPresent())
		return FALSE;

#ifdef _TRIAL_VER
	UINT nDataRunVal;
	CString sKey,sVal;

	// Проверка метки "Кол-во запусков истекло" в реестре
	sKey.LoadString(IDS_SYS_REG_STENDKEY);
	sVal.LoadString(IDS_SYS_REG_STENDVAL);
	if (QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataRunVal))
	{
		if (!m_bCreated)
		{
			m_bCreated = TRUE;
			sErr.LoadString(IDS_ER_TRIAL_EXPIRED);
			goto err;
		}
		return FALSE;
	}
#else
	CString sVer,sVersion,sData,sNumber,sName,sEmail,sOrg;

	// Чтение номера версии
	sVersion= GetKeyValue(sLicense, _T("Version"));
	// Чтение номера лицензии
	sNumber	= GetKeyValue(sLicense, _T("Number"));
	// Чтение даты создания
	sData	= GetKeyValue(sLicense, _T("Created"));
	// Чтение имени пользователя
	sName	= GetKeyValue(sLicense, _T("Name"));
	// Чтение email пользователя
	sEmail	= GetKeyValue(sLicense, _T("Email"));
	// Чтение организации пользователя
	sOrg	= GetKeyValue(sLicense, _T("Company"));
	// Чтение зашифрованного серийного номера
	m_strSerial = GetKeyValue(sLicense, _T("Code"));
	// Расшифровка серийного номера методом Base64
	m_strSerial = Base64::decode((LPCTSTR)m_strSerial);
	// Набор данных для получения серийного номера
	m_strInput = sData + _T(" ") + sNumber + _T(" ") + sName;
	m_strInput+= _T(" ") + sEmail + _T(" ") + sOrg;

	// Загрузка номера текущей версии
	sVer.LoadString(IDS_VERSION_NUMBER);
	// Проверка версии лицензии
	if (sVersion.Compare(sVer) != 0)
	{
		if (!m_bCreated)
		{
			m_bCreated = TRUE;
			sErr.LoadString(IDS_ER_WRONG_KEYFILE);
			goto err;
		}
		return FALSE;
	}
#endif

	return CMDIFrameWnd::PreCreateWindow(cs);

err:
	MessageBeep(MB_ICONEXCLAMATION);
	AfxMessageBox(sErr);
	return FALSE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (IsDebuggerPresent())
		return FALSE;

#ifdef _TRIAL_VER
	UINT nDataCntVal;
	CString sKey,sVal,sDat;
	CTime installtime;
	FILETIME fitime;

	// Проверка метки даты-времени первого запуска в реестре
	sKey.LoadString(IDS_SYS_REG_DT1STKEY);
	sVal.LoadString(IDS_SYS_REG_DT1STVAL);
	if (!QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, sVal, sDat, _T("")))
	{
		// Установить признак первого запуска
		m_bSkipTst = TRUE;
		// Проверка метки даты-времени предыдущего запуска в реестре
		sKey.LoadString(IDS_SYS_REG_DTPREKEY);
		sVal.LoadString(IDS_SYS_REG_DTPREVAL);
		if (QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, sVal, sDat, _T("")))
		{
			m_bInstErr = TRUE;
		}
		if (!m_bInstErr)
		{
			// Проверка метки о количестве запусков в реестре
			sKey.LoadString(IDS_SYS_REG_COUNTKEY);
			sVal.LoadString(IDS_SYS_REG_COUNTVAL);
			if (QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataCntVal))
			{
				m_bInstErr = TRUE;
			}
		}
		if (!m_bInstErr)
		{
			// Проверка метки инсталляции в реестре
			sKey.LoadString(IDS_SYS_REG_INSTDATA);
			if (!QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("tappinstdata"), sDat, _T("")))
			{
				m_bInstErr = TRUE;
			}
		}
		if (!m_bInstErr)
		{
			// Получение текущего системного времени
			_getCurrentSysDT(fitime);
			installtime = CTime(fitime);
			// Шифрование строки текущего даты-времени
			sDat.Format("0x%ld", (installtime.GetYear() * 10000 + 
								  installtime.GetMonth() * 100 + 
								  installtime.GetDay()) * 17 + 137);
			// Запись метки в реестр с текущим датой-временем
			sKey.LoadString(IDS_SYS_REG_INSTDATA);
			WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("tappinstdata"), sDat);
			// Форматирование строки даты-времени
			sDat.Format("%Iu %Iu",	fitime.dwLowDateTime, 
									fitime.dwHighDateTime);
			// Запись даты-времени первого запуска в реестр
			sKey.LoadString(IDS_SYS_REG_DT1STKEY);
			sVal.LoadString(IDS_SYS_REG_DT1STVAL);
			WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, sVal, sDat);
			// Запись метки о количестве запусков в реестр
			sKey.LoadString(IDS_SYS_REG_COUNTKEY);
			sVal.LoadString(IDS_SYS_REG_COUNTVAL);
			// Шифрование значения
			nDataCntVal = 777^0xEF770033;
			WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataCntVal);
		}
	}
	if (m_bInstErr)
	{
		sVal.LoadString(IDS_ER_INCOR_INSTALL);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(sVal);
		return FALSE;
	}
#else
	RSAPubkey rk;
	BYTE *lpKeyData;
	ZZ cipher,plain;

	// Загрузить публичный ключ из ресурсов
	HMODULE hModule = GetModuleHandle(NULL);
	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(IDR_BINARY), _T("BINARY"));
	HGLOBAL hGlob = LoadResource(hModule, hRes);
    lpKeyData = (BYTE*)LockResource(hGlob);
	// Прочитать публичный ключ
	rk = ReadPublicKey(lpKeyData);
	FreeResource(hGlob);

	// Расшифровка серийного номера
	cipher = to_ZZ((LPCTSTR)m_strSerial);
	plain = RSA_SigDecrypt(cipher, rk);
	m_strSerial = to_string(plain).c_str(); 
#endif

	return CMDIFrameWnd::OnCreateClient(lpcs, pContext);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Загрузка общей таблицы акселераторов
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hWndAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));

	// Создание строки статуса
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Создание панелей инструментов
	// Проверка даты запуска
	if (!CreateToolbars())
		return -1;

	if (IsDebuggerPresent())
		return -1;

CString sErr;

#ifdef _TRIAL_VER
	long n;
	UINT nDataCntVal;
	CString sKey,sVal,sDat;
	FILETIME ftimec,ftime1;
	__int64 timec,time1;

	// Получение текущего системного времени
	_getCurrentSysDT(ftimec);
	sDat.Format("%Iu %Iu", ftimec.dwLowDateTime, ftimec.dwHighDateTime);
	// Запись даты-времени предыдущего запуска в реестр
	sKey.LoadString(IDS_SYS_REG_DTPREKEY);
	sVal.LoadString(IDS_SYS_REG_DTPREVAL);
	WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, sVal, sDat);
	// Проверка метки о количестве запусков в реестре
	sKey.LoadString(IDS_SYS_REG_COUNTKEY);
	sVal.LoadString(IDS_SYS_REG_COUNTVAL);
	if (QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataCntVal))
	{
		// Расшифровка значения
		nDataCntVal ^= 0xEF770033;
		// Увеличение счетчика запусков
		nDataCntVal++;

		// Проверка ограничения на макс. кол-во запусков
		if (nDataCntVal > 877)
		{
			m_bTrialEx = TRUE;
			// Проверка метки "Кол-во запусков истекло" в реестре
			sKey.LoadString(IDS_SYS_REG_STENDKEY);
			sVal.LoadString(IDS_SYS_REG_STENDVAL);
			if (!QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataCntVal))
			{
				WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, 1);
			}
		}
		// Шифрование значения
		nDataCntVal ^= 0xEF770033;
		WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataCntVal);
	}
	else
	{
		sErr.LoadString(IDS_ER_INCOR_INSTALL);
		goto err;
	}
	// Проверка ограничения на кол-во дней работы
	// Чтение метки даты-времени первого запуска в реестре
	sKey.LoadString(IDS_SYS_REG_DT1STKEY);
	sVal.LoadString(IDS_SYS_REG_DT1STVAL);
	if (QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, sVal, sDat, _T("")))
	{
		// Преобразование строки в FILETIME
		n = GetWord(sDat, 0,   sKey);	// Low word
		n = GetWord(sDat, n+1, sVal);	// High word
		ftime1.dwLowDateTime  = atol(sKey);
		ftime1.dwHighDateTime = atol(sVal);
		// Преобразование даты-времени первого запуска в T64
		FileTimeToT64(&ftime1, &time1);
		// Преобразование текущего даты-времени в T64
		FileTimeToT64(&ftimec, &timec);
		// Добавление 30 дней к дате-времени первого запуска
		time1 +=Int32x32To64(30*24,3600);
		// Сравнение даты-времени текущего и первого запуска
		if (timec > time1)
		{
			m_bTrialEx = TRUE;
			// Проверка метки "Trial период истек" в реестре
			sKey.LoadString(IDS_SYS_REG_TRENDKEY);
			sVal.LoadString(IDS_SYS_REG_TRENDVAL);
			if (!QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, nDataCntVal))
			{
				WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, 1);
			}
		}
	}
	if (m_bTrialEx)
	{
		sErr.LoadString(IDS_ER_TRIAL_EXPIRED);
		goto err;
	}

#else
	int i;
	MD5STRUCT md;
	CString sMD5;
	// Получить Message Digest
	md = GetBufferMD5(m_strInput.GetBuffer(m_strInput.GetLength()), 
						m_strInput.GetLength());
	sMD5 = bytes_to_string(md.digest,16).c_str();

	i = sMD5.GetLength() - m_strSerial.GetLength();
	if (i > 0) sMD5 = sMD5.Mid(i);
	// Проверка серийного номера
	if (sMD5.Compare(m_strSerial) != 0)
	{
		sErr.LoadString(IDS_ER_WRONG_KEYFILE);
		goto err;
	}
#endif

	return 0;

err:
	MessageBeep(MB_ICONEXCLAMATION);
	AfxMessageBox(sErr);
	return -1;
}

void CMainFrame::OnDestroy() 
{
	// Останов обмена данными
	OnStopExchange();
	// Удаление Toolbar-ов из меню
	RemoveToolBar(&m_proToolBar);
	RemoveToolBar(&m_wndToolBar);
	// Освобождение занятых ресурсов
	if (m_hWndAccel != NULL)
		FreeResource(m_hWndAccel);
	if (m_menuTray.m_hMenu != NULL)
		m_menuTray.DestroyMenu();
	// Удаление иконки из системного Tray
	TrayMessage(this->m_hWnd, NIM_DELETE, 0, NULL, NULL);
	// Уничтожение всех MDI Child окон перед уничтожением 
	// основного окна, так как они могут использовать 
	// указатель на главное окно в событии при закрытии
	CFrameWnd* pWnd;
	while ((pWnd = GetActiveFrame()) != this)
	{
		pWnd->DestroyWindow();
	}
	CMDIFrameWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


LRESULT CMainFrame::OnMDIUpdateBar(WPARAM wParam, LPARAM lParam)
{
	if (wParam > 0) {
		ShowControlBar(&m_proToolBar, TRUE, FALSE);
	}
	else {
		if (MDIGetActive() == NULL)
			ShowControlBar(&m_proToolBar, FALSE, FALSE);
	}
	m_proToolBar.UpdateWindow();

	return 0;
}

LRESULT CMainFrame::OnTrayNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	CPoint	point;
	switch (lParam)
	{
	case WM_LBUTTONDBLCLK:
		// Удаление иконки из системного Tray
		TrayMessage(this->m_hWnd, NIM_DELETE, 0, NULL, NULL);
		// Показ окна
		ShowWindow(SW_SHOWNORMAL);
		SetForegroundWindow();
		break;
	case WM_RBUTTONDOWN:
		// Показ меню "Файл"
		::GetCursorPos(&point);
		if (m_menuTray.m_hMenu == NULL)
			m_menuTray.LoadMenu(IDR_TRAY);
		m_menuTray.GetSubMenu(0)->TrackPopupMenu(
					TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
					point.x, point.y, this);
		break;
	default:
		break;
	}
	return 0;
}

void CMainFrame::OnTransfer() 
{
	bCancel = FALSE;

	// Запуск передачи данных
	RunPPLSel();
}

void CMainFrame::OnImport() 
{
	bCancel = FALSE;

	// Запуск импорта
	if (TrnParm.iUseImp > 0)
	{
		int	i = FindPPL(TrnParm.sPplImp);
		if (i >= 0)
		{
			RunPipe(TrnParm.sDirPpl + TrnParm.sPplImp, 
					arPPL[i].sName, arPPL[i].iKind);
		}
	}
	else
		RunPPLSel(1);
}

void CMainFrame::OnExport() 
{
	bCancel = FALSE;

	// Запуск экспорта
	if (TrnParm.iUseExp > 0)
	{
		int	i = FindPPL(TrnParm.sPplExp);
		if (i >= 0)
		{
			RunPipe(TrnParm.sDirPpl + TrnParm.sPplExp, 
					arPPL[i].sName, arPPL[i].iKind);
		}
	}
	else
		RunPPLSel(2);
}

void CMainFrame::OnProfile() 
{
	CChildFrame* pWnd;
	CString	sTitle,sKey;
	RECT	rect;

	sTitle.LoadString(IDS_MS_PROFILES);
	pWnd = (CChildFrame* )FindMDIChildWnd(this, sTitle);
	if (pWnd != NULL) {
		pWnd->MDIActivate();
		return;
	}
	// Установка позиции и размеров окна
	if (TrnParm.iSaveMDI)
	{
		int x,y,w,h;
		// Получение координат и размеров окна из реестра
		sKey.LoadString(IDS_SYS_REG_SET);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplX"), (UINT&)x, 0);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplY"), (UINT&)y, 0);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplW"), (UINT&)w, 400);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ListPplH"), (UINT&)h, 300);
		rect.left	= x;
		rect.right	= x + w;
		rect.top	= y;
		rect.bottom	= y + h;
	}
	else
	{
		rect = rectDefault;
	}
	pWnd = new CChildFrame;
	CCreateContext pContext;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CListPPLView);
	pWnd->Create(NULL, sTitle, WS_CHILD|WS_OVERLAPPEDWINDOW,
				rect, this, &pContext);
	pWnd->SetIcon(AfxGetApp()->LoadIcon(IDI_CRDFILES), TRUE);
	if (TrnParm.iMaxiMDI)
		pWnd->ShowWindow(SW_SHOWMAXIMIZED);
	else
		pWnd->ShowWindow(SW_SHOWNORMAL);
	pWnd->MDIActivate();
}

void CMainFrame::OnTransOptions() 
{
	// Создание диалога свойств
	CPropDlg optSheet(TAB_TRNOPT_CAPTION, this);
	// Создание страниц свойств
	CTrnOptPage optPg;
	CTrnExcPage excPg;
	CTrnPrcPage prcPg;
	CTrnIntPage intPg;

	// Добавление страниц свойств
	optSheet.AddPage(&optPg);
	optSheet.AddPage(&excPg);
	optSheet.AddPage(&prcPg);
	optSheet.AddPage(&intPg);

	/////////////////////////////////////////
	// Установка данных на страницах
	/////////////////////////////////////////
	CString	sLangID = TrnParm.sLangID;
	// Общие
	optPg.m_DirPpl	= TrnParm.sDirPpl;
	optPg.m_DirLog	= TrnParm.sDirLog;
	optPg.m_DirMsg	= TrnParm.sDirMsg;
	optPg.m_FileLog	= TrnParm.sFileLog;
	optPg.m_FileMsg	= TrnParm.sFileMsg;
	optPg.m_PplImp	= TrnParm.sPplImp;
	optPg.m_PplExp	= TrnParm.sPplExp;
	optPg.m_UseImp	= TrnParm.iUseImp;
	optPg.m_UseExp	= TrnParm.iUseExp;
	// Интерфейс
	intPg.m_LangID	= TrnParm.sLangID;
	intPg.m_SplashW	= TrnParm.iSplashW;
	intPg.m_Minimize= TrnParm.iMinimize;
	intPg.m_SysTray	= TrnParm.iSysTray;
	intPg.m_SavePos	= TrnParm.iSavePos;
	intPg.m_SaveMDI	= TrnParm.iSaveMDI;
	intPg.m_MaxiMDI	= TrnParm.iMaxiMDI;
	// Обработка
	prcPg.m_DefType	= TrnParm.iDefType;
	prcPg.m_AutoExc	= TrnParm.iAutoExc;
	prcPg.m_DelFile = TrnParm.iDelFile;
	prcPg.m_WriteOpr= TrnParm.iWriteOpr;
	prcPg.m_WriteMsg= TrnParm.iWriteMsg;
	prcPg.m_ShowErr	= TrnParm.iShowErr;
	prcPg.m_TraceOpr= TrnParm.iTraceOpr;
	prcPg.m_TraceCmd= TrnParm.iTraceCmd;
	prcPg.m_TraceSel= TrnParm.iTraceSel;
	// Обмен данными
	excPg.m_Depart	= TrnParm.iDepart;
	excPg.m_ExcTime	= TrnParm.iExcTime;
	excPg.m_IntTime	= TrnParm.iIntTime;
	excPg.m_FileImp	= TrnParm.sFileImp;
	excPg.m_FileExp	= TrnParm.sFileExp;
	excPg.m_FileEmp	= TrnParm.sFileEmp;
	excPg.m_DirPut	= TrnParm.sDirPut;
	excPg.m_DirGet	= TrnParm.sDirGet;
	excPg.m_NameArc	= TrnParm.sNameArc;

	// Вызов диалога свойств
	if (optSheet.DoModal() != IDOK) return;

	// Установка параметров
	// Общие
	TrnParm.sDirPpl  = optPg.m_DirPpl;
	TrnParm.sDirLog  = optPg.m_DirLog;
	TrnParm.sDirMsg  = optPg.m_DirMsg;
	TrnParm.sFileLog = optPg.m_FileLog;
	TrnParm.sFileMsg = optPg.m_FileMsg;
	TrnParm.sPplImp  = optPg.m_PplImp;
	TrnParm.sPplExp  = optPg.m_PplExp;
	TrnParm.iUseImp  = optPg.m_UseImp;
	TrnParm.iUseExp  = optPg.m_UseExp;
	// Интерфейс
	TrnParm.sLangID  = intPg.m_LangID;
	TrnParm.iSplashW = intPg.m_SplashW;
	TrnParm.iMinimize= intPg.m_Minimize;
	TrnParm.iSysTray = intPg.m_SysTray;
	TrnParm.iSavePos = intPg.m_SavePos;
	TrnParm.iSaveMDI = intPg.m_SaveMDI;
	TrnParm.iMaxiMDI = intPg.m_MaxiMDI;
	// Обработка
	TrnParm.iDefType = prcPg.m_DefType;
	TrnParm.iAutoExc = prcPg.m_AutoExc;
	TrnParm.iDelFile = prcPg.m_DelFile;
	TrnParm.iWriteOpr= prcPg.m_WriteOpr;
	TrnParm.iWriteMsg= prcPg.m_WriteMsg;
	TrnParm.iShowErr = prcPg.m_ShowErr;
	TrnParm.iTraceOpr= prcPg.m_TraceOpr;
	TrnParm.iTraceCmd= prcPg.m_TraceCmd;
	TrnParm.iTraceSel= prcPg.m_TraceSel;
	// Обмен данными
	TrnParm.iDepart  = excPg.m_Depart;
	TrnParm.iExcTime = excPg.m_ExcTime;
	TrnParm.iIntTime = excPg.m_IntTime;
	TrnParm.sFileImp = excPg.m_FileImp;
	TrnParm.sFileExp = excPg.m_FileExp;
	TrnParm.sFileEmp = excPg.m_FileEmp;
	TrnParm.sDirPut  = excPg.m_DirPut;
	TrnParm.sDirGet  = excPg.m_DirGet;
	TrnParm.sNameArc = excPg.m_NameArc;

	// Если изменился язык интерфейса
	if (TrnParm.sLangID.CompareNoCase(sLangID) != 0)
	{
		// Освобождение библиотеки ресурсов
		if (hLangDLL != NULL)
		{
			AfxFreeLibrary(hLangDLL);
			hLangDLL = NULL;
		}
		// Установка идентификатора ресурсов
		if (TrnParm.sLangID.CompareNoCase(_T("RUS")) == 0)
		{
			// Установка идентификатора ресурсов приложения
			AfxSetResourceHandle(AfxGetApp()->m_hInstance);
		}
		else if (TrnParm.sLangID.CompareNoCase(_T("ENU")) == 0)
		{
			// Установка идентификатора английской версии ресурсов
			if ((hLangDLL = AfxLoadLibrary("Trn33enu.dll")) == NULL)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(CString(GetErrMsg(ER_OPEN_FILE, _T("Trn33enu.dll"))));
				// Установка основного языка
				TrnParm.sLangID = _T("RUS");
				// Установка идентификатора ресурсов приложения
				AfxSetResourceHandle(AfxGetApp()->m_hInstance);
			}
			else
			{
				AfxSetResourceHandle(hLangDLL);
			}
		}
		else
		{
			// Установка идентификатора ресурсов приложения
			AfxSetResourceHandle(AfxGetApp()->m_hInstance);
		}
		// Предупреждающее сообщение
		AfxMessageBox(IDS_MS_LOAD_LANG);

		// СДЕЛАТЬ - обновление меню приложения
		// нельзя выполнить простым удалением меню
		// и назначением нового, так как
		// приложение может зависнуть

		// Обновление меню приложения
		// Удаление старого меню
		// CMenu *pMenu = GetMenu();
		// pMenu->DestroyMenu();
		// Загрузка нового меню
		// pMenu->LoadMenu(IDR_MAINFRAME);
		// SetMenu(pMenu);
		// Обновление меню MDI-окон
		// CChildFrame* pChildWnd;
		// if ((pChildWnd = (CChildFrame*)MDIGetActive()) != NULL)
		// {
		//	pChildWnd->SendMessage(FORM_UPDATE_MENU);
		// }
	}
	// Запись параметров
	WriteOptions();
}

void CMainFrame::OnAppAbout() 
{
	CAboutDlg dlg;
	CString sKey;
	CString sUser;
	CString sComp;
	CString sLcns;

	dlg.m_ShowDlg = TrnParm.iSplashW;

	// Set registration information
#ifdef _TRIAL_VER
	sKey.LoadString(IDS_SYS_REG_ROOT);
	QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("RegisteredOwner"), sUser, _T(""));
	QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("RegisteredOrganization"), sComp, _T(""));
	sLcns.LoadString(IDS_MS_TXT_TRIALVER);
#else
	sUser = GetKeyValue(sLicense, _T("Name"));
	sComp = GetKeyValue(sLicense, _T("Company"));
	sLcns = GetKeyValue(sLicense, _T("Number"));
#endif
	dlg.m_Registration.FormatMessage(IDS_MS_REGISTRATION, sUser, sComp, sLcns);

	dlg.DoModal();

	if ((UINT)dlg.m_ShowDlg != TrnParm.iSplashW)
	{
		TrnParm.iSplashW = dlg.m_ShowDlg;
		sKey.LoadString(IDS_SYS_REG_OPT);
		WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SplashW"),TrnParm.iSplashW);
	}
}

void CMainFrame::OnStartExchange() 
{
#ifndef _TRIAL_VER
	StartExchange();
#endif
}

void CMainFrame::OnStopExchange() 
{
#ifndef _TRIAL_VER
	StopExchange();
#endif
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID) 
{
	CMDIFrameWnd::OnActivateApp(bActive, dwThreadID);

	if (bActive && m_hWnd) ::UpdateWindow(m_hWnd);
}

void CMainFrame::OnMove(int x, int y) 
{
	CMDIFrameWnd::OnMove(x, y);
	
	CString sKey;
	RECT	rect;

	// Сохранение позиции окна в реестре
	if (TrnParm.iSavePos)
	{
		GetWindowRect(&rect);
		sKey.LoadString(IDS_SYS_REG_SET);
		WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameX"), (UINT)rect.left);
		WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameY"), (UINT)rect.top);
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	CString sKey;
	RECT	rect;

	// Сохранение размеров окна в реестре
	if (nType == SIZE_RESTORED && TrnParm.iSavePos)
	{
		GetWindowRect(&rect);
		sKey.LoadString(IDS_SYS_REG_SET);
		WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameW"), (UINT)(rect.right - rect.left));
		WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameH"), (UINT)(rect.bottom - rect.top));
	}
	if (nType == SIZE_RESTORED ||
		nType == SIZE_MAXIMIZED)
	{
		// Восстановить окно ProgressBar
		if (::IsWindow(hwndPopup) &&
		   !::IsWindowVisible(hwndPopup))
			::ShowWindow(hwndPopup, SW_SHOW);
	}
	else if (nType == SIZE_MINIMIZED &&
			 TrnParm.iSysTray)
	{
		// Сокрытие окна
		ShowWindow(SW_HIDE);
		// Минимизация в системный Tray
		if (bExchange)
			// Установка иконки индикатора обмена
			TrayMessage(this->m_hWnd, NIM_ADD, 0, 
				LoadIcon(AfxGetResourceHandle(), 
				MAKEINTRESOURCE(IDI_CONVS)), 
				TrnParm.sNameApp);
		else
			// Установка иконки приложения
			TrayMessage(this->m_hWnd, NIM_ADD, 0, 
				LoadIcon(AfxGetResourceHandle(), 
				MAKEINTRESOURCE(IDI_TRANS)), 
				TrnParm.sNameApp);
	}
}

void CMainFrame::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!bExchange);
}

void CMainFrame::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!bExchange);
}

void CMainFrame::OnUpdateStartExchange(CCmdUI* pCmdUI) 
{
#ifdef _TRIAL_VER
	pCmdUI->Enable(FALSE);
#else
	pCmdUI->Enable(!bExchange);
#endif
}

void CMainFrame::OnUpdateStopExchange(CCmdUI* pCmdUI) 
{
#ifdef _TRIAL_VER
	pCmdUI->Enable(FALSE);
#else
	pCmdUI->Enable(bExchange);
#endif
}

void CMainFrame::OnUpdateExport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!bExchange);
}

void CMainFrame::OnUpdateImport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!bExchange);
}

void CMainFrame::OnUpdateTransfer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!bExchange);
}

void CMainFrame::OnUpdateProfile(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!bExchange);
}

void CMainFrame::OnUpdateTransOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!bExchange);
}

void CMainFrame::OnViewToolbar() 
{
	BOOL bVisible = ((m_wndToolBar.GetStyle() & WS_VISIBLE) != 0);

	ShowControlBar(&m_wndToolBar, !bVisible, FALSE);
	RecalcLayout();
}

void CMainFrame::OnUpdateToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndToolBar.GetStyle() & WS_VISIBLE) != 0);
}

void CMainFrame::OnViewProfileTbr() 
{
	BOOL bVisible = ((m_proToolBar.GetStyle() & WS_VISIBLE) != 0);

	ShowControlBar(&m_proToolBar, !bVisible, FALSE);
	RecalcLayout();
}

void CMainFrame::OnUpdateProfileTbr(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_proToolBar.GetStyle() & WS_VISIBLE) != 0);
}

void CMainFrame::OnViewStatusbar() 
{
	BOOL bVisible = ((m_wndStatusBar.GetStyle() & WS_VISIBLE) != 0);

	ShowControlBar(&m_wndStatusBar, !bVisible, FALSE);
	RecalcLayout();
}

void CMainFrame::OnUpdateStatusbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndStatusBar.GetStyle() & WS_VISIBLE) != 0);
}

void CMainFrame::OnCustomizeToolbar() 
{
	m_wndToolBar.Customize();
}

void CMainFrame::OnCustomizeProfileTbr() 
{
	m_proToolBar.Customize();
}
