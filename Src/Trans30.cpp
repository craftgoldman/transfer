/////////////////////////////////////////////////////////////////////
// Trans30.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Trans30.h"
#include "MainFrm.h"
#include "CardPPL.h"
#include "GetPPL.h"
#include "MsgDlg.h"
#include "TrnStart.h"
#include "Transfer.h"
#include "LzwCom.h"
#include "Base64.h"
#include "tools.h"
#include "rsa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////
// CTrans30App

BEGIN_MESSAGE_MAP(CTrans30App, CWinApp)
	//{{AFX_MSG_MAP(CTrans30App)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CTrans30App construction

CTrans30App::CTrans30App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////
// The one and only CTrans30App object

CTrans30App theApp;

/////////////////////////////////////////////////////////////////////
// Глобальные переменные приложения
//

HWND			 hwndPopup;	// HWND Popup Window
HINSTANCE        hLangDLL;	// HINSTANCE Language DLL
CStdioFile       fileLog;	// Файл протокола
CStdioFile       fileMsg;	// Файл сообщений
CString          sFileLog;	// Наименование файла протокола
CString          sFileMsg;	// Наименование файла сообщений
CString          sLicense;	// Содержимое файла лицензии
BOOL             bFileLog;	// Признак файла протокола
BOOL             bFileMsg;	// Признак файла сообщений
BOOL             bExchange;	// Признак запуска обмена
BOOL             bImport;	// Признак импорта
BOOL             bExport;	// Признак экспорта
BOOL             bCancel;	// Признак останова
CMenu            menuMDI;	// Меню для всех MDI окон
TRN_PARM         TrnParm;	// Параметры Трансферта
CArray<PPL,PPL&> arPPL;		// Массив профили данных

/////////////////////////////////////////////////////////////////////
// Собщение в системный Tray

BOOL TrayMessage(HWND hWnd, DWORD dwMessage, UINT uID, HICON hIcon, LPCTSTR szTip)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize			= sizeof(NOTIFYICONDATA);
	tnd.hWnd			= hWnd;
	tnd.uID				= uID;
	tnd.uFlags			= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage= TRAY_NOTIFYICON;
	tnd.hIcon			= hIcon;

	if (szTip)
	{
		lstrcpyn(tnd.szTip, szTip, sizeof(tnd.szTip));
	}
	else
	{
		tnd.szTip[0] = '\0';
	}

	return Shell_NotifyIcon(dwMessage, &tnd);
}

////////////////////////////////////////////////////////////////////
// Открытие файла протокола

BOOL OpenFileLog()
{
	// Если файл протокола не открыт
	if (!bFileLog)
	{
		// Открытие файла протокола
		sFileLog = TrnParm.sDirLog + TrnParm.sFileLog;
		if (fileLog.Open(sFileLog, CFile::modeCreate|
			CFile::modeNoTruncate|CFile::modeReadWrite|
			CFile::typeText|CFile::shareDenyNone))
		{
			bFileLog = TRUE;
			fileLog.SeekToEnd();
		}
		else
		{
			if (TrnParm.iShowErr)
			{
				CString strErr;
				strErr = GetErrMsg(ER_OPEN_FILE, sFileLog);
				MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
			}
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////
// Закрытие файла протокола

BOOL CloseFileLog()
{
	if (bFileLog)
	{
		bFileLog = FALSE;
		fileLog.Close();
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Обработка таймера инициализации обмена (Timer #1)

void CALLBACK TimerExchange(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	// Экспорт данных в каталог обмена
	ExportDataExchange(hWnd);
}

/////////////////////////////////////////////////////////////////////
// Обработка таймера опроса файла обмена (Timer #2)

void CALLBACK TimerInterrogate(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	OFSTRUCT ofs;
	// Проверяем наличие файла архива в каталоге приема
	if (OpenFile(TrnParm.sDirGet + TrnParm.sNameArc, 
		&ofs, OF_EXIST) == HFILE_ERROR) return;

	// Импорт данных из каталога обмена
	ImportDataExchange(hWnd);
}

/////////////////////////////////////////////////////////////////////
// Запуск обмена данными

void StartExchange()
{
	CString	strMesg; // Строка сообщения

#ifdef _TRIAL_VER
	// Ограничение по схеме в Trial версии
	strMesg.FormatMessage(IDS_ER_TRIAL_EXCHANGE);
	AfxMessageBox(strMesg);
	return;
#else

	CTime	curTime; // Текущее время

	if (bExchange) return;
	// Установка флага выполнения
	bExchange = TRUE;
	bCancel  = FALSE;
	// Если приложение минимизировано в системный Tray
	if (TrnParm.iSysTray && 
		AfxGetApp()->m_pMainWnd->IsIconic())
	{
		// Установить иконку индикатора обмена данными
		TrayMessage(AfxGetApp()->m_pMainWnd->m_hWnd, NIM_MODIFY, 0, 
					LoadIcon(AfxGetResourceHandle(), 
					MAKEINTRESOURCE(IDI_CONVS)), 
					TrnParm.sNameApp);
	}
	// Запись протокола операции в файл
	if (TrnParm.iWriteOpr)
	{
		// Открытие файла протокола
		if (OpenFileLog())
		{
			// Запись ограничительной линии
			fileLog.WriteString(_T("\n"));
			fileLog.WriteString("********** StartDataExchange **********\n");
			fileLog.WriteString(_T("\n"));
			// Время начала операции
			curTime = CTime::GetCurrentTime();
			strMesg.LoadString(IDS_MS_START_EXCH);
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		// Закрытие файла протокола
		CloseFileLog();
	}
	// Если филиал, опрос файла
	if (TrnParm.iDepart > 0)
	{
		// Запуск таймера опроса
		theApp.m_pMainWnd->SetTimer(2, TrnParm.iIntTime * 1000, TimerInterrogate);
	}
	// Если центр, инициализировать обмен
	else
	{
		// Экспорт данных в каталог обмена
		ExportDataExchange(theApp.m_pMainWnd->m_hWnd);
	}

#endif
}

/////////////////////////////////////////////////////////////////////
// Останов обмена данными

void StopExchange()
{
	CString	strMesg; // Строка сообщения

#ifdef _TRIAL_VER
	// Ограничение по схеме в Trial версии
	strMesg.FormatMessage(IDS_ER_TRIAL_EXCHANGE);
	AfxMessageBox(strMesg);
	return;
#else

	CTime	curTime; // Текущее время

	if (!bExchange) return;
	// Если приложение минимизировано в системный Tray
	if (TrnParm.iSysTray && 
		AfxGetApp()->m_pMainWnd->IsIconic())
	{
		// Установить иконку приложения
		TrayMessage(AfxGetApp()->m_pMainWnd->m_hWnd, NIM_MODIFY, 0, 
					LoadIcon(AfxGetResourceHandle(), 
					MAKEINTRESOURCE(IDI_TRANS)), 
					TrnParm.sNameApp);
	}
	// Запись протокола операции в файл
	if (TrnParm.iWriteOpr)
	{
		// Открытие файла протокола
		if (OpenFileLog())
		{
			// Запись ограничительной линии
			fileLog.WriteString(_T("\n"));
			fileLog.WriteString("********** StopDataExchange **********\n");
			fileLog.WriteString(_T("\n"));
			// Время останова операции
			curTime = CTime::GetCurrentTime();
			strMesg.LoadString(IDS_MS_STOP_EXCH);
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		// Закрытие файла протокола
		CloseFileLog();
	}
	// Сброс таймера опроса
	theApp.m_pMainWnd->KillTimer(2);
	// Сброс таймера обмена
	theApp.m_pMainWnd->KillTimer(1);
	// Сброс флага выполнения
	bExchange = FALSE;
	bCancel   = TRUE;

#endif
}

/////////////////////////////////////////////////////////////////////
// Импорт данных из каталога обмена

BOOL ImportDataExchange(HWND hWnd)
{
	CString	strMesg;	// Строка сообщения

#ifdef _TRIAL_VER
	// Ограничение по схеме в Trial версии
	strMesg.FormatMessage(IDS_ER_TRIAL_EXCHANGE);
	AfxMessageBox(strMesg);
	return FALSE;
#else

	CLzwCom * lzw;		// LZW compressor
	CWnd*   pWnd;		// Окно приложения
	CTime	curTime;	// Текущее время
	BOOL	bShowMsg;	// Показ сообщений
	CMessageDlg	msgDlg;	// Окно сообщения
	CString	strCapt;	// Строка заголовка
	CString sDirImp;	// Каталог импорта
	int i;

	if (bImport || bExport) return FALSE;

	if (bCancel) goto cancel;
	// Определение окна приложения
	pWnd = AfxGetApp()->GetMainWnd();
	// Признак показа окон сообщений
	bShowMsg = !pWnd->IsIconic();

	// Открытие файла протокола
	OpenFileLog();
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		// Запись ограничительной линии
		fileLog.WriteString(_T("\n"));
		fileLog.WriteString("********** ImportDataExchange **********\n");
		fileLog.WriteString(_T("\n"));
		// Время начала операции
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_RUN_IMP);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Определение каталогов и файлов
	if ((i = TrnParm.sFileImp.ReverseFind('\\')) > 0)
	{
		sDirImp = TrnParm.sFileImp.Left(i + 1);
	}
	// Установка признака импорта
	bImport = TRUE;
	// Сброс таймера опроса
	KillTimer(hWnd, 2);

	if (bCancel) goto cancel;
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		// Время начала операции
		curTime = CTime::GetCurrentTime();
		strMesg.FormatMessage(IDS_MS_COPY_FILE, 
			TrnParm.sDirGet + TrnParm.sNameArc, sDirImp + TrnParm.sNameArc);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	if (bShowMsg)
	{
		// Создание окна сообщения
		AfxGetApp()->BeginWaitCursor();
		strCapt.LoadString(IDS_MS_SEND);
		msgDlg.Create(IDD_MESSAGEDLG, AfxGetApp()->m_pMainWnd);
		msgDlg.SetWindowText(strCapt);
		strMesg.FormatMessage(IDS_MS_COPY_FILE, 
			TrnParm.sDirGet + TrnParm.sNameArc, sDirImp + TrnParm.sNameArc);
		msgDlg.m_sText = strMesg;
		msgDlg.UpdateData(FALSE);
	}
	else
	{
		strCapt.LoadString(IDS_MS_SEND);
		pWnd->SetWindowText(strCapt);
	}
	// Копирование файла архива в каталог импорта
	// (обычно занимает мнгого времени, так как
	//  TrnParm.sDirGet - удаленный каталог)
	if (CopyFile(TrnParm.sDirGet + TrnParm.sNameArc, 
				sDirImp + TrnParm.sNameArc, FALSE))
	{
		if (bCancel) goto cancel;
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			// Время начала операции
			curTime = CTime::GetCurrentTime();
			strMesg.FormatMessage(IDS_MS_DELETE_FILE, 
				TrnParm.sDirGet + TrnParm.sNameArc);
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		// Удаление файла архива из каталога приема
		DeleteFile(TrnParm.sDirGet + TrnParm.sNameArc);
	}
	if (bShowMsg)
	{
		msgDlg.DestroyWindow();
		AfxGetApp()->EndWaitCursor();
	}
	else
	{
		pWnd->SetWindowText(TrnParm.sNameApp);
	}
	/////////////////////////////////////////
	// Разархивация файла архива
	/////////////////////////////////////////
	if (bCancel) goto cancel;
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		// Время начала операции
		curTime = CTime::GetCurrentTime();
		strMesg.FormatMessage(IDS_MS_DELETE_FILE, TrnParm.sFileImp);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Удаление файла импорта
	DeleteFile(TrnParm.sFileImp);

	if (bCancel) goto cancel;
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		// Время начала операции
		curTime = CTime::GetCurrentTime();
		strMesg.FormatMessage(IDS_MS_EXTRACT, 
			sDirImp + TrnParm.sNameArc, TrnParm.sFileImp);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Разархивация файла импорта
	lzw = new CLzwCom();
	if (lzw->extract(sDirImp + TrnParm.sNameArc, TrnParm.sFileImp))
	{
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			// Время начала операции
			curTime = CTime::GetCurrentTime();
			strMesg.FormatMessage(IDS_MS_DELETE_FILE, 
				sDirImp + TrnParm.sNameArc);
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		// Удаление файла архива
		DeleteFile(sDirImp + TrnParm.sNameArc);

		// Запуск импорта
		// (запись протокола внутри функции)
		if ((i = FindPPL(TrnParm.sPplImp)) >= 0)
			RunPipe(TrnParm.sDirPpl + TrnParm.sPplImp, 
					arPPL[i].sName, arPPL[i].iKind, FALSE);
		else
			RunPipe(TrnParm.sDirPpl + TrnParm.sPplImp, 
					"Transfer", 0, FALSE);
	}
	delete lzw;
	// Закрытие файла протокола
	CloseFileLog();
	// Сброс признака импорта
	bImport = FALSE;

	if (bCancel) goto cancel;
	// Если филиал, экспорт
	if (TrnParm.iDepart > 0)
	{
		// Экспорт данных в каталог обмена
		ExportDataExchange(hWnd);
	}
	if (bCancel) goto cancel;
	// Запуск таймера опроса
	SetTimer(hWnd, 2, TrnParm.iIntTime * 1000, TimerInterrogate);

	return TRUE;

cancel:
	// Останов обмена данными
	if (bExchange) StopExchange();
	// Закрытие файла протокола
	CloseFileLog();

	return FALSE;

#endif
}

/////////////////////////////////////////////////////////////////////
// Экспорт данных в каталог обмена

BOOL ExportDataExchange(HWND hWnd)
{
	CString	strMesg;	// Строка сообщения

#ifdef _TRIAL_VER
	// Ограничение по схеме в Trial версии
	strMesg.FormatMessage(IDS_ER_TRIAL_EXCHANGE);
	AfxMessageBox(strMesg);
	return FALSE;
#else

	CLzwCom * lzw;		// LZW compressor
	CWnd*   pWnd;		// Окно приложения
	CTime	curTime;	// Текущее время
	BOOL	bShowMsg;	// Показ сообщений
	CMessageDlg	msgDlg;	// Окно сообщения
	CString	strCapt;	// Строка заголовка
	CString sDirExp;	// Каталог экспорта
	CString	sFileImp;	// Файл импорта
	int i;

	if (bImport || bExport) return FALSE;

	if (bCancel) goto cancel;
	// Определенгие окна приложения
	pWnd = AfxGetApp()->GetMainWnd();
	// Признак показа окон сообщений
	bShowMsg = !pWnd->IsIconic();

	// Открытие файла протокола
	OpenFileLog();
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		// Запись ограничительной линии
		fileLog.WriteString(_T("\n"));
		fileLog.WriteString("********** ExportDataExchange **********\n");
		fileLog.WriteString(_T("\n"));
		// Время начала операции
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_RUN_EXP);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Установка признака экспорта
	bExport = TRUE;
	// Только для центра
	if (TrnParm.iDepart == 0)
	{
		// Сброс таймера опроса
		KillTimer(hWnd, 2);
		// Сброс таймера обмена
		KillTimer(hWnd, 1);
	}
	if (bCancel) goto cancel;
	// Определение каталогов и файлов
	if ((i = TrnParm.sFileExp.ReverseFind('\\')) > 0)
	{
		sDirExp = TrnParm.sFileExp.Left(i + 1);
	}
	if ((i = TrnParm.sFileImp.ReverseFind('\\')) > 0)
	{
		sFileImp = TrnParm.sFileImp.Mid(i + 1);
	}
	else
	{
		sFileImp = TrnParm.sFileImp;
	}
	// Замена файла экспорта на пустой файл
	if (!TrnParm.sFileEmp.IsEmpty())
	{
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			// Время начала операции
			curTime = CTime::GetCurrentTime();
			strMesg.FormatMessage(IDS_MS_COPY_FILE, 
				TrnParm.sFileEmp, TrnParm.sFileExp);
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		// Копирование фала экспорта в файл импорта
		// в каталоге экспорта
		CopyFile(TrnParm.sFileEmp, TrnParm.sFileExp, FALSE);
	}
	if (bCancel) goto cancel;
	// Запуск экспорта данных
	// (запись протокола внутри функции)
	if ((i = FindPPL(TrnParm.sPplExp)) >= 0)
		RunPipe(TrnParm.sDirPpl + TrnParm.sPplExp, 
				arPPL[i].sName, arPPL[i].iKind, FALSE);
	else
		RunPipe(TrnParm.sDirPpl + TrnParm.sPplExp, 
				"Transfer", 0, FALSE);

	if (bCancel) goto cancel;
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		// Время начала операции
		curTime = CTime::GetCurrentTime();
		strMesg.FormatMessage(IDS_MS_COPY_FILE, 
			TrnParm.sFileExp, sDirExp + sFileImp);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Копирование фала экспорта в файл импорта
	// в каталоге экспорта
	CopyFile(TrnParm.sFileExp, sDirExp + sFileImp, FALSE);

	/////////////////////////////////////////
	// Архивация файла импорта
	/////////////////////////////////////////
	if (bCancel) goto cancel;
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		// Время начала операции
		curTime = CTime::GetCurrentTime();
		strMesg.FormatMessage(IDS_MS_COMPRESS, 
			sDirExp + sFileImp, sDirExp + TrnParm.sNameArc);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	lzw = new CLzwCom();
	if (lzw->compress(sDirExp + sFileImp, sDirExp + TrnParm.sNameArc))
	{
		if (bCancel) { delete lzw; goto cancel; }
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			// Время начала операции
			curTime = CTime::GetCurrentTime();
			strMesg.FormatMessage(IDS_MS_DELETE_FILE, sDirExp + sFileImp);
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		// Удаление файла импорта из каталога экспорта
		DeleteFile(sDirExp + sFileImp);

		if (bCancel) { delete lzw; goto cancel; }

		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			// Время начала операции
			curTime = CTime::GetCurrentTime();
			strMesg.FormatMessage(IDS_MS_COPY_FILE, 
				sDirExp + TrnParm.sNameArc, TrnParm.sDirPut + "Tmp.arc");
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		if (bShowMsg)
		{
			// Создание окна сообщения
			AfxGetApp()->BeginWaitCursor();
			strCapt.LoadString(IDS_MS_SEND);
			msgDlg.Create(IDD_MESSAGEDLG, AfxGetApp()->m_pMainWnd);
			msgDlg.SetWindowText(strCapt);
			strMesg.FormatMessage(IDS_MS_COPY_FILE, 
				sDirExp + TrnParm.sNameArc, TrnParm.sDirPut + "Tmp.arc");
			msgDlg.m_sText = strMesg;
			msgDlg.UpdateData(FALSE);
		}
		else
		{
			strCapt.LoadString(IDS_MS_SEND);
			pWnd->SetWindowText(strCapt);
		}
		// Удаление файла архива в каталоге передачи
		DeleteFile(TrnParm.sDirPut + "Tmp.arc");
		// Копирование файла архива в каталог передачи
		// (обычно занимает мнгого времени, так как
		//  TrnParm.sDirPut - удаленный каталог)
		if (CopyFile(sDirExp + TrnParm.sNameArc, 
					TrnParm.sDirPut + "Tmp.arc", FALSE))
		{
			// Запись протокола операции в файл
			if (bFileLog && TrnParm.iWriteOpr)
			{
				// Время начала операции
				curTime = CTime::GetCurrentTime();
				strMesg.FormatMessage(IDS_MS_MOVE_FILE, 
					TrnParm.sDirPut + "Tmp.arc", 
					TrnParm.sDirPut + TrnParm.sNameArc);
				strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
				fileLog.WriteString(strMesg + _T("\n"));
			}
			// Удаление файла архива, если он есть
			DeleteFile(TrnParm.sDirPut + TrnParm.sNameArc);
			// Переименование файла архива 
			// в каталоге передачи
			MoveFile(TrnParm.sDirPut + "Tmp.arc",
					TrnParm.sDirPut + TrnParm.sNameArc);
		}
		if (bShowMsg)
		{
			msgDlg.DestroyWindow();
			AfxGetApp()->EndWaitCursor();
		}
		else
		{
			pWnd->SetWindowText(TrnParm.sNameApp);
		}
		if (bCancel) { delete lzw; goto cancel; }
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			// Время начала операции
			curTime = CTime::GetCurrentTime();
			strMesg.FormatMessage(IDS_MS_DELETE_FILE, 
				sDirExp + TrnParm.sNameArc);
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		// Удаление файла архива из каталога экспорта
		DeleteFile(sDirExp + TrnParm.sNameArc);
	}
	delete lzw;
	// Закрытие файла протокола
	CloseFileLog();
	// Сброс признака экспорта
	bExport = FALSE;
	if (bCancel) goto cancel;
	// Только для центра
	if (TrnParm.iDepart == 0)
	{
		// Запуск таймера обмена
		SetTimer(hWnd, 1, TrnParm.iExcTime*60000, TimerExchange);
		// Запуск таймера опроса
		SetTimer(hWnd, 2, TrnParm.iIntTime*1000, TimerInterrogate);
	}
	return TRUE;

cancel:
	// Останов обмена данными
	if (bExchange) StopExchange();
	// Закрытие файла протокола
	CloseFileLog();

	return FALSE;

#endif
}

/////////////////////////////////////////////////////////////////////
// Инициализация установок Трансферта

BOOL InitOptions(void) 
{
	UINT	i,n,m,nCount;
	CString	sKey,sVal,sNPP,sPPL;

	sKey.LoadString(IDS_SYS_REG_OPT);
	// Общие
	QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirPpl"),	TrnParm.sDirPpl, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirLog"),	TrnParm.sDirLog, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirMsg"),	TrnParm.sDirMsg, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileLog"),TrnParm.sFileLog, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileMsg"),TrnParm.sFileMsg, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("PplImp"),	TrnParm.sPplImp, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("PplExp"),	TrnParm.sPplExp, _T(""));
    QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("UseImp"),	TrnParm.iUseImp, 0);
    QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("UseExp"),	TrnParm.iUseExp, 0);
	// Интерфейс
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("LangID"),	TrnParm.sLangID, _T("ENG"));
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SplashW"),TrnParm.iSplashW, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("Minimize"),TrnParm.iMinimize, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SysTray"),TrnParm.iSysTray, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SavePos"),TrnParm.iSavePos, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SaveMDI"),TrnParm.iSaveMDI, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("MaxiMDI"),TrnParm.iMaxiMDI, 0);
	// Обработка
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("DefType"),TrnParm.iDefType, 1);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("AutoExc"),TrnParm.iAutoExc, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("DelFile"),TrnParm.iDelFile, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("WriteOpr"),TrnParm.iWriteOpr, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("WriteMSg"),TrnParm.iWriteMsg, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ShowErr"),TrnParm.iShowErr, 1);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("TraceOpr"),TrnParm.iTraceOpr, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("TraceCmd"),TrnParm.iTraceCmd, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("TraceSel"),TrnParm.iTraceSel, 0);
	// Обмен данными
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("Depart"),	TrnParm.iDepart, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ExcTime"),TrnParm.iExcTime, 0);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("IntTime"),TrnParm.iIntTime, 0);
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileImp"),TrnParm.sFileImp, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileExp"),TrnParm.sFileExp, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileEmp"),TrnParm.sFileEmp, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirPut"),	TrnParm.sDirPut, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirGet"),	TrnParm.sDirGet, _T(""));
    QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("NameArc"),TrnParm.sNameArc, _T(""));

	if (IsDebuggerPresent())
		return FALSE;

	// Чтение метки инсталляции в реестре
	sKey.LoadString(IDS_SYS_REG_INSTKEY);
	sVal.LoadString(IDS_SYS_REG_INSTVAL);
	if (!QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, sVal, i))
		return FALSE;

	// Заполнение списка профилей данных
	sKey.LoadString(IDS_SYS_REG_PIPE);
	QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("Count"), nCount);
	arPPL.SetSize(nCount);
	for (i = 0; i < nCount; i++)
	{
		sNPP.Format("%i", i);
		QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("PPL" + sNPP), sPPL);
		n = sPPL.Find(",", 0);
		m = sPPL.Find(",", n + 1);
		arPPL[i].iNum  = i;
		arPPL[i].iKind = atoi(sPPL.Left(n));
		arPPL[i].sFile = sPPL.Mid(n+1, m-n-1);
		arPPL[i].sName = sPPL.Mid(m+1 );
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Запись установок Трансферта

BOOL WriteOptions(void) 
{
	CString sKey;

	sKey.LoadString(IDS_SYS_REG_OPT);
	// Общие
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirPpl"),	TrnParm.sDirPpl);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirLog"),	TrnParm.sDirLog);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirMsg"),	TrnParm.sDirMsg);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileLog"),TrnParm.sFileLog);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileMsg"),TrnParm.sFileMsg);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("PplImp"),	TrnParm.sPplImp);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("PplExp"),	TrnParm.sPplExp);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("UseImp"),	TrnParm.iUseImp);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("UseExp"),	TrnParm.iUseExp);
	// Интерфейс
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("LangID"), TrnParm.sLangID);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SplashW"),TrnParm.iSplashW);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("Minimize"),TrnParm.iMinimize);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SysTray"),TrnParm.iSysTray);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SavePos"),TrnParm.iSavePos);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("SaveMDI"),TrnParm.iSaveMDI);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("MaxiMDI"),TrnParm.iMaxiMDI);
	// Обработка
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("DefType"),TrnParm.iDefType);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("AutoExc"),TrnParm.iAutoExc);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("DelFile"),TrnParm.iDelFile);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("WriteOpr"),TrnParm.iWriteOpr);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("WriteMsg"),TrnParm.iWriteMsg);
    WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ShowErr"),TrnParm.iShowErr);
    WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("TraceOpr"),TrnParm.iTraceOpr);
    WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("TraceCmd"),TrnParm.iTraceCmd);
    WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("TraceSel"),TrnParm.iTraceSel);
	// Обмен данными
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("Depart"),	TrnParm.iDepart);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("ExcTime"),TrnParm.iExcTime);
	WriteRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("IntTime"),TrnParm.iIntTime);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileImp"),TrnParm.sFileImp);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileExp"),TrnParm.sFileExp);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("FileEmp"),TrnParm.sFileEmp);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirPut"),	TrnParm.sDirPut);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("DirGet"),	TrnParm.sDirGet);
    WriteRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("NameArc"),TrnParm.sNameArc);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Чтение из файла

BOOL FileGet(LPCTSTR szName, CString &strData)
{
	CStdioFile file;
	DWORD      nSize;
	char*      pbuf;

	// Открытие файла
	if (!file.Open(szName, CFile::modeRead|CFile::typeText))
		return FALSE;
	// Определение длины файла
	nSize = file.GetLength();
	// Выделение памяти под буфер
	pbuf = new char[nSize];
	// Чтение данных из файла
	file.Read(pbuf, nSize);
	strData = CString(pbuf);
	// Закрытие файла
	file.Close();
	// Уничтожение буфера
	delete[] pbuf;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Запись в файл

BOOL FilePut(LPCTSTR szName, CString strData)
{
	CFile	file;
	DWORD	nSize;

	if (!file.Open(szName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
		return FALSE;
	// Определение длины данных
	nSize = strData.GetLength();
	// Запись данных в файл
	file.Write(strData.GetBuffer(nSize), nSize);
	// Закрытие файла
	file.Close();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Найти MDI окно по заголовку

CMDIChildWnd* FindMDIChildWnd(CMDIFrameWnd *pFrameWnd, LPCTSTR szTitle)
{
	if (pFrameWnd == NULL) return NULL;

	CMDIChildWnd* pChildWnd = (CMDIChildWnd* )pFrameWnd->MDIGetActive();
	CString	strTitle;
	while (pChildWnd != NULL) {
		pChildWnd->GetWindowText(strTitle);
		if (strTitle == szTitle) break;
		pChildWnd = (CMDIChildWnd* )pChildWnd->GetWindow(GW_HWNDNEXT);
	}
	return pChildWnd;
}

/////////////////////////////////////////////////////////////////////
// Поиск индекса профиля по имени файла

UINT FindPPL(LPCTSTR szFile) 
{
	for (int i = 0; i < arPPL.GetSize(); i++)
	{
		if (arPPL[i].sFile.CompareNoCase(szFile) == 0)
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////
// Получить текст ошибки по идентификатору

BSTR GetErrMsg(ER_TYPE iErrType, LPCTSTR szErr) 
{
	CString s;
	switch (iErrType)		// Тип ошибки
	{
    case ER_UNKNOWN:		s = szErr;									break;
    case ER_NOTVALID:		s.FormatMessage(IDS_ER_NOTVALID);			break;
	case ER_NO_PPL:			s.FormatMessage(IDS_ER_NO_PPL);				break;
	case ER_NO_EXPR:		s.FormatMessage(IDS_ER_NO_EXPR);			break;
	case ER_NO_TABLE:		s.FormatMessage(IDS_ER_NO_TABLE);			break;
	case ER_NO_KEYS:		s.FormatMessage(IDS_ER_NO_KEYS);			break;
	case ER_NO_FIELDS:		s.FormatMessage(IDS_ER_NO_FIELDS);			break;
	case ER_NO_MODFLD:		s.FormatMessage(IDS_ER_NO_MODFLD);			break;
	case ER_NEED_CODE:		s.FormatMessage(IDS_ER_NEED_CODE);			break;
	case ER_NEED_NAME:		s.FormatMessage(IDS_ER_NEED_NAME);			break;
	case ER_NEED_FILE:		s.FormatMessage(IDS_ER_NEED_FILE);			break;
	case ER_NEED_VALUE:		s.FormatMessage(IDS_ER_NEED_VALUE);			break;
	case ER_NEED_FIELD:		s.FormatMessage(IDS_ER_NEED_FIELD);			break;
	case ER_NEED_COMMAND:	s.FormatMessage(IDS_ER_NEED_COMMAND);		break;
	case ER_NEED_SQLTAB:	s.FormatMessage(IDS_ER_NEED_SQLTAB);		break;
	case ER_NEED_SQLSUB:	s.FormatMessage(IDS_ER_NEED_SQLSUB);		break;
	case ER_IGNORE_SQLSUB:	s.FormatMessage(IDS_ER_IGNORE_SQLSUB);		break;
	case ER_DOUBLE_FIELD:	s.FormatMessage(IDS_ER_DOUBLE_FIELD,szErr);	break;
	case ER_DOUBLE_COND:	s.FormatMessage(IDS_ER_DOUBLE_COND,szErr);	break;
	case ER_DOUBLE_CODE:	s.FormatMessage(IDS_ER_DOUBLE_CODE,szErr);	break;
	case ER_FIND_FILE:		s.FormatMessage(IDS_ER_FIND_FILE,szErr);	break;
	case ER_READ_FILE:		s.FormatMessage(IDS_ER_READ_FILE,szErr);	break;
	case ER_OPEN_FILE:		s.FormatMessage(IDS_ER_OPEN_FILE,szErr);	break;
	case ER_WRITE_FILE:		s.FormatMessage(IDS_ER_WRITE_FILE,szErr);	break;
	case ER_CONN_SRC:		s.FormatMessage(IDS_ER_CONN_SRC,szErr);		break;
	case ER_CONN_DST:		s.FormatMessage(IDS_ER_CONN_DST,szErr);		break;
    case ER_INSTAB:			s.FormatMessage(IDS_ER_INSTAB,szErr);		break;
    case ER_UPDTAB:			s.FormatMessage(IDS_ER_UPDTAB,szErr);		break;
	case ER_DELTAB:			s.FormatMessage(IDS_ER_DELTAB,szErr);		break;
    case ER_INSSUB:			s.FormatMessage(IDS_ER_INSSUB,szErr);		break;
    case ER_UPDSUB:			s.FormatMessage(IDS_ER_UPDSUB,szErr);		break;
	case ER_DELSUB:			s.FormatMessage(IDS_ER_DELSUB,szErr);		break;
	case ER_SNTX_PARAM:		s.FormatMessage(IDS_ER_SNTX_PARAM,szErr);	break;
	case ER_SNTX_PARAMS:	s.FormatMessage(IDS_ER_SNTX_PARAMS);		break;
	case ER_SNTX_MAPTAB:	s.FormatMessage(IDS_ER_SNTX_MAPTAB);		break;
	case ER_SNTX_MAPSUB:	s.FormatMessage(IDS_ER_SNTX_MAPSUB);		break;
	case ER_SNTX_CMDTAB:	s.FormatMessage(IDS_ER_SNTX_CMDTAB,szErr);	break;
	case ER_SNTX_CMDSUB:	s.FormatMessage(IDS_ER_SNTX_CMDSUB,szErr);	break;
	case ER_MAIN_TABLE:		s.FormatMessage(IDS_ER_MAIN_TABLE);			break;
	case ER_CRT_SQL:		s.FormatMessage(IDS_ER_CRT_SQL);			break;
	case ER_CRT_TABLE:		s.FormatMessage(IDS_ER_CRT_TABLE,szErr);	break;
	case ER_CRT_INDEX:		s.FormatMessage(IDS_ER_CRT_INDEX,szErr);	break;
	case ER_CRT_TAB:		s.FormatMessage(IDS_ER_CRT_TAB,szErr);		break;
	case ER_CRT_SUB:		s.FormatMessage(IDS_ER_CRT_SUB,szErr);		break;
	case ER_PREP_SRCTAB:	s.FormatMessage(IDS_ER_PREP_SRCTAB,szErr);	break;
	case ER_PREP_SRCSUB:	s.FormatMessage(IDS_ER_PREP_SRCSUB,szErr);	break;
	case ER_PREP_DSTTAB:	s.FormatMessage(IDS_ER_PREP_DSTTAB,szErr);	break;
	case ER_PREP_DSTSUB:	s.FormatMessage(IDS_ER_PREP_DSTSUB,szErr);	break;
	case ER_PREP_MSG:		s.FormatMessage(IDS_ER_PREP_MSG,szErr);		break;
	case ER_PREP_INSTAB:	s.FormatMessage(IDS_ER_PREP_INSTAB,szErr);	break;
	case ER_PREP_UPDTAB:	s.FormatMessage(IDS_ER_PREP_UPDTAB,szErr);	break;
	case ER_PREP_DELTAB:	s.FormatMessage(IDS_ER_PREP_DELTAB,szErr);	break;
	case ER_PREP_INSSUB:	s.FormatMessage(IDS_ER_PREP_INSSUB,szErr);	break;
	case ER_PREP_UPDSUB:	s.FormatMessage(IDS_ER_PREP_UPDSUB,szErr);	break;
	case ER_PREP_DELSUB:	s.FormatMessage(IDS_ER_PREP_DELSUB,szErr);	break;
	case ER_PREP_CMDTAB:	s.FormatMessage(IDS_ER_PREP_CMDTAB,szErr);	break;
	case ER_PREP_CMDSUB:	s.FormatMessage(IDS_ER_PREP_CMDSUB,szErr);	break;
	case ER_PREP_PARSRCTAB:	s.FormatMessage(IDS_ER_PREP_PARSRCTAB,szErr);break;
	case ER_PREP_SELSRCTAB:	s.FormatMessage(IDS_ER_PREP_SELSRCTAB,szErr);break;
	case ER_PREP_FNDSRCTAB:	s.FormatMessage(IDS_ER_PREP_FNDSRCTAB,szErr);break;
	case ER_PREP_PARSRCSUB:	s.FormatMessage(IDS_ER_PREP_PARSRCSUB,szErr);break;
	case ER_PREP_SELSRCSUB:	s.FormatMessage(IDS_ER_PREP_SELSRCSUB,szErr);break;
	case ER_PREP_FNDSRCSUB:	s.FormatMessage(IDS_ER_PREP_FNDSRCSUB,szErr);break;
	case ER_PREP_PARDSTTAB:	s.FormatMessage(IDS_ER_PREP_PARDSTTAB,szErr);break;
	case ER_PREP_SELDSTTAB:	s.FormatMessage(IDS_ER_PREP_SELDSTTAB,szErr);break;
	case ER_PREP_FNDDSTTAB:	s.FormatMessage(IDS_ER_PREP_FNDDSTTAB,szErr);break;
	case ER_PREP_PARDSTSUB:	s.FormatMessage(IDS_ER_PREP_PARDSTSUB,szErr);break;
	case ER_PREP_SELDSTSUB:	s.FormatMessage(IDS_ER_PREP_SELDSTSUB,szErr);break;
	case ER_PREP_FNDDSTSUB:	s.FormatMessage(IDS_ER_PREP_FNDDSTSUB,szErr);break;
	case ER_COUNT_SRCTAB:	s.FormatMessage(IDS_ER_COUNT_SRCTAB,szErr);	break;
	case ER_COUNT_DSTTAB:	s.FormatMessage(IDS_ER_COUNT_DSTTAB,szErr);	break;
	case ER_READ_SRCTAB:	s.FormatMessage(IDS_ER_READ_SRCTAB,szErr);	break;
	case ER_READ_DSTTAB:	s.FormatMessage(IDS_ER_READ_DSTTAB,szErr);	break;
	case ER_READ_SRCSUB:	s.FormatMessage(IDS_ER_READ_SRCSUB,szErr);	break;
	case ER_READ_DSTSUB:	s.FormatMessage(IDS_ER_READ_DSTSUB,szErr);	break;
	case ER_FIND_SRCTAB:	s.FormatMessage(IDS_ER_FIND_SRCTAB,szErr);	break;
	case ER_FIND_DSTTAB:	s.FormatMessage(IDS_ER_FIND_DSTTAB,szErr);	break;
	case ER_FIND_SRCSUB:	s.FormatMessage(IDS_ER_FIND_SRCSUB,szErr);	break;
	case ER_FIND_DSTSUB:	s.FormatMessage(IDS_ER_FIND_DSTSUB,szErr);	break;
	case ER_FIND_PARAM:		s.FormatMessage(IDS_ER_FIND_PARAM,szErr);	break;
	case ER_FIND_CONST:		s.FormatMessage(IDS_ER_FIND_CONST,szErr);	break;
	case ER_FIND_FLDKEY:	s.FormatMessage(IDS_ER_FIND_FLDKEY,szErr);	break;
	case ER_FIND_FLDMAP:	s.FormatMessage(IDS_ER_FIND_FLDMAP,szErr);	break;
	case ER_FIND_FLDSRCTAB:	s.FormatMessage(IDS_ER_FIND_FLDSRCTAB,szErr);break;
	case ER_FIND_FLDSRCSUB:	s.FormatMessage(IDS_ER_FIND_FLDSRCSUB,szErr);break;
	case ER_FIND_FLDDSTREF:	s.FormatMessage(IDS_ER_FIND_FLDDSTREF,szErr);break;
	case ER_FIND_FLDDSTTAB:	s.FormatMessage(IDS_ER_FIND_FLDDSTTAB,szErr);break;
	case ER_FIND_FLDDSTSUB:	s.FormatMessage(IDS_ER_FIND_FLDDSTSUB,szErr);break;
	case ER_FIND_FUNC:		s.FormatMessage(IDS_ER_FIND_FUNC,szErr);	break;
	case ER_FIND_PROC:		s.FormatMessage(IDS_ER_FIND_PROC,szErr);	break;
	case ER_BAD_TYPE:		s.FormatMessage(IDS_ER_BAD_TYPE,szErr);		break;
	case ER_BAD_DATA:		s.FormatMessage(IDS_ER_BAD_DATA,szErr);		break;
	case ER_BAD_CHAR:		s.FormatMessage(IDS_ER_BAD_CHAR,szErr);		break;
	case ER_BAD_VAL:		s.FormatMessage(IDS_ER_BAD_VAL,szErr);		break;
	case ER_BAD_REF:		s.FormatMessage(IDS_ER_BAD_REF,szErr);		break;
	case ER_BAD_FLDCMD:		s.FormatMessage(IDS_ER_BAD_FLDCMD,szErr);	break;
	case ER_COMP_DSTTAB:	s.FormatMessage(IDS_ER_COMP_DSTTAB,szErr);	break;
	case ER_COMP_DSTSUB:	s.FormatMessage(IDS_ER_COMP_DSTSUB,szErr);	break;
	case ER_CONV_PARTYPE:	s.FormatMessage(IDS_ER_CONV_PARTYPE,szErr);	break;
	case ER_CONV_FLDTYPE:	s.FormatMessage(IDS_ER_CONV_FLDTYPE,szErr);	break;
	case ER_CONV_PARAM:		s.FormatMessage(IDS_ER_CONV_PARAM,szErr);	break;
	case ER_CONV_CONST:		s.FormatMessage(IDS_ER_CONV_CONST,szErr);	break;
	case ER_CONV_FIELD:		s.FormatMessage(IDS_ER_CONV_FIELD,szErr);	break;
	case ER_CONV_FUNC:		s.FormatMessage(IDS_ER_CONV_FUNC,szErr);	break;
	case ER_CONV_PROC:		s.FormatMessage(IDS_ER_CONV_PROC,szErr);	break;
	case ER_DEFINE:			s.FormatMessage(IDS_ER_DEFINE,szErr);		break;
	case ER_DEF_INDEX:		s.FormatMessage(IDS_ER_DEF_INDEX,szErr);	break;
	case ER_DEF_REFTYPE:	s.FormatMessage(IDS_ER_DEF_REFTYPE,szErr);	break;
	case ER_DEF_FIELDS:		s.FormatMessage(IDS_ER_DEF_FIELDS,szErr);	break;
	case ER_DEF_TABLE:		s.FormatMessage(IDS_ER_DEF_TABLE,szErr);	break;
	case ER_DEF_KEYS:		s.FormatMessage(IDS_ER_DEF_KEYS,szErr);		break;
	case ER_DEF_REFS:		s.FormatMessage(IDS_ER_DEF_REFS,szErr);		break;
	case ER_MOD_FLDKEY:		s.FormatMessage(IDS_ER_MOD_FLDKEY,szErr);	break;
	case ER_REPL_FLDKEY:	s.FormatMessage(IDS_ER_REPL_FLDKEY,szErr);	break;
	case ER_REPL_FLDDEF:	s.FormatMessage(IDS_ER_REPL_FLDDEF,szErr);	break;
	case ER_PARSE_PARAMS:	s.FormatMessage(IDS_ER_PARSE_PARAMS,szErr);	break;
	case ER_CMD_PARAMS:		s.FormatMessage(IDS_ER_CMD_PARAMS,szErr);	break;
	case ER_DESC_FIELDS:	s.FormatMessage(IDS_ER_DESC_FIELDS,szErr);	break;
	case ER_DESC_PARAMS:	s.FormatMessage(IDS_ER_DESC_PARAMS,szErr);	break;
	case ER_DESC_CATTAB:	s.FormatMessage(IDS_ER_DESC_CATTAB,szErr);	break;
	case ER_UNS_PARAMS:		s.FormatMessage(IDS_ER_UNS_PARAMS);			break;
	case ER_BIND_PARAM:		s.FormatMessage(IDS_ER_BIND_PARAM,szErr);	break;
	case ER_BIND_FIELD:		s.FormatMessage(IDS_ER_BIND_FIELD,szErr);	break;
	case ER_BIND_PARAMS:	s.FormatMessage(IDS_ER_BIND_PARAMS,szErr);	break;
	case ER_BIND_FIELDS:	s.FormatMessage(IDS_ER_BIND_FIELDS,szErr);	break;
	case ER_SET_PARAMS:		s.FormatMessage(IDS_ER_SET_PARAMS,szErr);	break;
	case ER_BACK_DETAIL:	s.FormatMessage(IDS_ER_BACK_DETAIL,szErr);	break;
	case ER_BACK_CMD:		s.FormatMessage(IDS_ER_BACK_CMD,szErr);		break;
	case ER_BACK_OP:		s.FormatMessage(IDS_ER_BACK_OP,szErr);		break;
	default:				s = szErr;									break;
	}
	return s.AllocSysString();
}

/////////////////////////////////////////////////////////////////////
// Значение по ключевому слову

BOOL GetKeyVal(LPCTSTR szText, LPCTSTR szKey, CString &strData) 
{
	int		n,m;
	CString	tmp(szText),strKey(szKey);
	CString	pattern = " \t\r\n.,:;()=";

	if (strKey.IsEmpty()) return FALSE;
	strKey.MakeLower();
	tmp.MakeLower();
	if ((n = tmp.Find(strKey + "=")) < 0)
		return FALSE;
	n += strKey.GetLength() + 1;
	tmp = tmp.Mid( n );
	if ((m = tmp.FindOneOf(pattern)) < 0)
		m = tmp.GetLength();
	strData = CString(szText).Mid(n, m);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Значение по ключевому слову: Key=Value
// Вызов: GetKeyValue(input_string, "Key")

CString GetKeyValue(LPCTSTR szData, LPCTSTR szKey)
{
	int i,n;
	CString sVal,sData(szData);

	if ((i = sData.Find(szKey)) < 0)
		return _T("");

	if ((n = sData.Find(_T("\n"),i)) < 0)
		sVal = sData.Mid(i);
	else
		sVal = sData.Mid(i + strlen(szKey) + 1, n - i - strlen(szKey) - 1);

	return sVal;
}

/////////////////////////////////////////////////////////////////////
// Значение строки по ключевому слову

BOOL GetKeyStr(LPCTSTR szText, LPCTSTR szKey, CString &strData) 
{
	int		i,n,m;
	CString	tmp(szText),strKey(szKey);

	if (strKey.IsEmpty()) return FALSE;
	tmp.MakeLower();
	strKey.MakeLower();
	if ((n = tmp.Find(strKey + "(")) < 0)
		return FALSE;
	n += strKey.GetLength();
	if ((m = tmp.Find(")", n + 1)) < 0)
		return FALSE;
	i = n;
	while ((i = tmp.Find("(",i + 1)) > 0)
	{
		if (i > m)	break;
		if ((m = tmp.Find(")", m + 1)) < 0)
			return FALSE;
	}
	strData = CString(szText).Mid(n + 1, m - n - 1);
	strData.TrimLeft();
	strData.TrimRight();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Получить лексему с указанной позиции

int GetLex(LPCTSTR szText, int nStart, CString &strLex) 
{
	// Возвращает позицию после лексемы
	// strLex - содержит лексему
	CString tmp(szText);
	CString	pdlm = _T("~!@#$%^&*(){}[].,;:+-=/><");
	CString	ptrn = _T(" \t\r\n*(){}[].,;:+-=/><");
	int		nLen = tmp.GetLength();

	strLex.Empty();
	if (nStart < 0) return -1;
	// Пропуск лидирующих SPACE,TAB,CR,LF
	for (; nStart < nLen; nStart++)
	{
		if (tmp.Mid(nStart, 1).FindOneOf(_T(" \t\r\n")) < 0)
			break;
	}
	if (nStart >= nLen) return -1;
	// Проверка первого символа
	if (tmp.Mid(nStart, 1).FindOneOf(pdlm) >= 0)
	{
		strLex = tmp.Mid(nStart, 1);
		return nStart + 1;
	}
	// Выделение слова до разделителя
	int	nPos = tmp.Mid(nStart).FindOneOf(ptrn);
	if (nPos >= 0)	nPos+= nStart;
	else			nPos = nLen;
	strLex = tmp.Mid(nStart, nPos - nStart);

	return nPos;
}

/////////////////////////////////////////////////////////////////////
// Выделить слово с указанной позиции

int GetWord(LPCTSTR szText, int nStart, CString &strWord) 
{
	// Возвращает длину выделенного слова
	// strWord - содержит выделенное слово
	int		nPos;
	CString	tmp = CString(szText).Mid(nStart);
	CString	ptrn = _T(" \t\r\n*(){}[].,;:+-=/><");

	// Удаление лидирующих SPACE,TAB,CR,LF
	strWord.Empty();
	tmp.TrimLeft();
	if (tmp.IsEmpty()) return 0;
	if ((nPos = tmp.FindOneOf(ptrn)) < 0)
		nPos = tmp.GetLength();
	strWord = tmp.Mid(0, nPos);

	return nPos;
}

/////////////////////////////////////////////////////////////////////
// Заполнение массива параметров

BOOL FillParmArray(LPCTSTR szText, CFldArray &arPrm) 
{
	CString s,tmp(szText);
	int		i,n,m,nPos,nLen;
	arPrm.SetSize(0);
	tmp.TrimLeft();
	tmp.TrimRight();
	nLen = tmp.GetLength();
	for (i=0, n=0; n < nLen; n = m + 1, i++)
	{
		// Выделение блока
		if ((m = tmp.Find(",", n)) < 0)
			m = nLen + 1;
		s = tmp.Mid(n, m - n);
		s.TrimLeft();
		s.TrimRight();
		// Выделение типа
		if ((nPos = s.Find(" ")) < 0)
			return FALSE;
		arPrm.SetSize(i + 1);
		arPrm[i].sType = s.Left(nPos);
		arPrm[i].sType.MakeLower();
		arPrm[i].iSQLType = SQLGetTypeID(arPrm[i].sType);
		s = s.Mid(nPos + 1);
		s.TrimLeft();
		// Выделение идентификатора
		if ((nPos = s.Find("(")) < 0)
			return FALSE;
		arPrm[i].sID = s.Left(nPos);
		arPrm[i].sID.TrimRight();
		s = s.Mid(nPos + 1);
		s.TrimLeft();
		// Выделение наименования
		if ((nPos = s.Find(")")) < 0)
			return FALSE;
		arPrm[i].sName = s.Left(nPos);
		arPrm[i].sName.TrimRight();
		s = s.Mid(nPos + 1);
		s.TrimLeft();
		// Выделение значения (опционально)
		if ((nPos = s.Find("=")) >= 0)
		{
			arPrm[i].sValue = s.Mid(nPos + 1);
			arPrm[i].sValue.TrimLeft();
			arPrm[i].sValue.TrimRight();
		}
	};
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Заполнение массива полей

BOOL FillFieldArray(LPCTSTR szText, CFldArray &arFld) 
{
	CString	s,tmp,strText(szText);
	int		i,n,m,k,nPos,nLen;

	strText.TrimLeft();
	strText.TrimRight();
	nLen = strText.GetLength();
	for (i = 0, n = 0; n < nLen; i++, n = m + 1)
	{
		// Выделение блока
		if ((m = strText.Find(_T(";"), n)) < 0)
			m = nLen + 1;
		s = strText.Mid(n, m - n);
		s.TrimLeft();
		s.TrimRight();
		// Установка элемента массива
		arFld.SetSize(i + 1);
		// Выделение значения (опционально)
		if ((nPos = s.Find(_T("="))) >= 0)
		{
			// Установка источника данных
			tmp = s.Mid(nPos + 1, 1); k = 2;
			if      (tmp == _T("^")) arFld[i].iSrc = DATA_LOCAL;
			else if (tmp == _T(":")) arFld[i].iSrc = DATA_PARAM;
			else if (tmp == _T("&")) arFld[i].iSrc = DATA_CONST;
			else if (tmp == _T("$")) arFld[i].iSrc = DATA_TABSRC;
			else if (tmp == _T("~")) arFld[i].iSrc = DATA_SUBSRC;
			else if (tmp == _T("#")) arFld[i].iSrc = DATA_TABDST;
			else if (tmp == _T("!")) arFld[i].iSrc = DATA_SUBDST;
			else if (tmp == _T("<")) arFld[i].iSrc = DATA_REF;
			else if (tmp == _T("+")) arFld[i].iSrc = DATA_AUTO;
			else if (tmp == _T("%")) arFld[i].iSrc = DATA_FUNC;
			else if (tmp == _T("@")) arFld[i].iSrc = DATA_PROC;
			else    {  k = 1;        arFld[i].iSrc = DATA_LOCAL; }
			// Установка значения
			arFld[i].sValue = s.Mid(nPos + k);
			arFld[i].sValue.TrimLeft();
			arFld[i].sValue.TrimRight();
			s = s.Left(nPos);
			s.TrimRight();
		}
		else
		{
			arFld[i].iSrc = DATA_LOCAL;
			arFld[i].sValue = _T("");
		}
		// Выделение флага (опционально)
		if ((nPos = s.Find(_T(":"))) >= 0)
		{
			tmp = s.Left(nPos);
			tmp.TrimRight();
			tmp.MakeUpper();
			s = s.Mid(nPos + 1);
			s.TrimLeft();
			if (tmp == _T("K"))
				arFld[i].iFlag = FLD_KEY;
			else if (tmp == _T("N"))
				arFld[i].iFlag = FLD_NOCHANGE;
			else if (tmp == _T("F"))
				arFld[i].iFlag = FLD_FRUNBACK;
			else if (tmp == _T("T"))
				arFld[i].iFlag = FLD_TRIGGER;
			else
				arFld[i].iFlag = FLD_DATA;
		}
		else
		{
			arFld[i].iFlag = FLD_DATA;
		}
		// Выделение имени поля и типа данных
		if ((nPos = s.Find(_T("("))) >= 0)
		{
			// Имя поля
			arFld[i].sName = s.Left(nPos);
			arFld[i].sName.TrimRight();
			s = s.Mid(nPos + 1);
			s.TrimLeft();
			// Тип данных
			if ((nPos = s.Find(_T(")"))) < 0)
				return FALSE;
			s = s.Left(nPos);
			s.TrimRight();
			s.MakeUpper();
			if ((nPos = s.Find(_T(","))) >= 0)
			{
				arFld[i].sType = s.Left(nPos);
				arFld[i].sType.TrimRight();
				s = s.Mid(nPos + 1);
				s.TrimLeft();
				arFld[i].iSQLType = SQLGetTypeID(arFld[i].sType);
				// Длина и точность
				if ((nPos = s.Find(",")) >= 0)
				{
					// Длина
					tmp = s.Left(nPos);
					tmp.TrimRight();
					arFld[i].iSize = atoi(tmp);
					// Точность
					tmp = s.Mid(nPos + 1);
					tmp.TrimLeft();
					arFld[i].iPrec = atoi(tmp);
				}
				else
				{
					arFld[i].iSize = atoi(s);
					arFld[i].iPrec = 0;
				}
			}
			else
			{
				arFld[i].sType    = s;
				arFld[i].iSQLType = SQLGetTypeID(s);
				arFld[i].iSize    = 0;
				arFld[i].iPrec    = 0;
			}
		}
		else
		{
			arFld[i].sName    = s;
			arFld[i].sType    = _T("*");
			arFld[i].iSQLType = 0;
			arFld[i].iSize    = 0;
			arFld[i].iPrec    = 0;
		}
	};
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Заполнение массива команд

BOOL FillCmdArray(LPCTSTR szText, CCmdArray &arCmd) 
{
	CString	s,sFld,strText(szText);
	int		i = 0, j, n;

	arCmd.RemoveAll();
	// OnStart
	if (GetKeyStr(szText, _T("OnStart"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_ON_START;
		arCmd[i].sEvent = _T("OnStart");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// OnEnd
	if (GetKeyStr(szText, _T("OnEnd"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_ON_END;
		arCmd[i].sEvent = _T("OnEnd");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// CallExec
	if (GetKeyStr(szText, _T("CallExec"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_CALL_EXEC;
		arCmd[i].sEvent = _T("CallExec");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// CallBack
	if (GetKeyStr(szText, _T("CallBack"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_CALL_BACK;
		arCmd[i].sEvent = _T("CallBack");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// OnInsert
	if (GetKeyStr(szText, _T("OnInsert"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_ON_INSERT;
		arCmd[i].sEvent = _T("OnInsert");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// OnUpdate
	if (GetKeyStr(szText, _T("OnUpdate"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_ON_UPDATE;
		arCmd[i].sEvent = _T("OnUpdate");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// OnDelete
	if (GetKeyStr(szText, _T("OnDelete"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_ON_DELETE;
		arCmd[i].sEvent = _T("OnDelete");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// ReInsert
	if (GetKeyStr(szText, _T("ReInsert"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_RE_INSERT;
		arCmd[i].sEvent = _T("ReInsert");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// ReUpdate
	if (GetKeyStr(szText, _T("ReUpdate"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_RE_UPDATE;
		arCmd[i].sEvent = _T("ReUpdate");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// ReDelete
	if (GetKeyStr(szText, _T("ReDelete"), s))
	{
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_RE_DELETE;
		arCmd[i].sEvent = _T("ReDelete");
		arCmd[i].sField = _T("");
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
		i++;
	}
	// OnChange
	for (j = 0; (j = strText.Find(_T("OnChange."), j)) > 0; i++)
	{
		if ((n = strText.Find(_T("("), j)) < 0)
			return FALSE;
		// Выделение имени поля
		sFld = strText.Mid(j + 9, n - j - 9);
		sFld.TrimLeft();
		sFld.TrimRight();
		// Выделение команды
		if (!GetKeyStr(szText, _T("OnChange."+sFld), s))
			return FALSE;
		j = n + s.GetLength() + 2;
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_ON_CHANGE;
		arCmd[i].sEvent = _T("OnChange");
		arCmd[i].sField = sFld;
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
	}
	// ReChange
	for (j = 0; (j = strText.Find(_T("ReChange."), j)) > 0; i++)
	{
		if ((n = strText.Find(_T("("), j)) < 0)
			return FALSE;
		// Выделение имени поля
		sFld = strText.Mid(j + 9, n - j - 9);
		sFld.TrimLeft();
		sFld.TrimRight();
		// Выделение команды
		if (!GetKeyStr(szText, _T("ReChange."+sFld), s))
			return FALSE;
		j = n + s.GetLength() + 2;
		arCmd.SetSize(i + 1);
		arCmd[i].hstmt  = SQL_NULL_HSTMT;
		arCmd[i].iEvent = EV_RE_CHANGE;
		arCmd[i].sEvent = _T("ReChange");
		arCmd[i].sField = sFld;
		arCmd[i].sCmd   = s;
		arCmd[i].nCount = 0;
		arCmd[i].pArPrm = NULL;
		arCmd[i].pArCmd = NULL;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Заполнение структуры описания трансферта

BOOL FillTrnInfo(LPCTSTR szText, TRN_INFO &trnInfo, CString &strErr) 
{
	CString	s,tmp;

	// Наименование
	if (!GetKeyStr(szText, "Name", trnInfo.sName))
	{
		strErr = GetErrMsg(ER_DEFINE, _T("<Name>"));
		return FALSE;
	}
	// Схема
	GetKeyStr(szText, "Schema", s);
	s.MakeUpper();
	if (s == "COLUMNS")
		trnInfo.iSchema = TRN_COLUMNS;
	if (s == "COPY")
		trnInfo.iSchema = TRN_COPY;
	if (s == "COMPLEX")
		trnInfo.iSchema = TRN_COMPLEX;
	// Конвертация текста
	GetKeyStr(szText, "ConvertText", s);
	s.MakeUpper();
	if (s == "UNCHANGED")
		trnInfo.iConvert = TRN_UNCHANGED;
	if (s == "OEMTOANSI")
		trnInfo.iConvert = TRN_OEMTOANSI;
	if (s == "ANSITOOEM")
		trnInfo.iConvert = TRN_ANSITOOEM;
	// Строка сообщения
	GetKeyStr(szText, "Message", trnInfo.sMsg);
	// SQL Select источника
	if (!GetKeyStr(szText, "Source", trnInfo.sTabSQL))
	{
		strErr = GetErrMsg(ER_DEFINE, _T("<Source>"));
		return FALSE;
	}
	// SQL Select субтаблицы
	GetKeyStr(szText, "SubSQL", trnInfo.sSubSQL);

	////////////////////////////////////////
	// Приемник
	////////////////////////////////////////
	if (!GetKeyStr(szText, "Destination", tmp))
	{
		strErr = GetErrMsg(ER_DEFINE, _T("<Destination>"));
		return FALSE;
	}
	// Процесс
	GetKeyVal(tmp, "process", s);
	s.MakeUpper();
	if (s == "CREATE")
		trnInfo.iTabPrc = DST_CREATE;
	if (s == "ADD")
		trnInfo.iTabPrc = DST_ADD;
	if (s == "REPLACE")
		trnInfo.iTabPrc = DST_REPLACE;
	if (s == "FULL")
		trnInfo.iTabPrc = DST_FULL;
	// Схема БД
	GetKeyVal(tmp, "owner", trnInfo.sTabOwn);
	// Таблица
	GetKeyVal(tmp, "table", trnInfo.sTabDst);
	// SQL Where
	GetKeyStr(tmp, "condition", trnInfo.sTabCond);
	// Карта полей
	GetKeyStr(tmp, "columns", trnInfo.sTabMap);
	// Команды приемника
	GetKeyStr(szText, "TabCommands", trnInfo.sTabCmd);

	////////////////////////////////////////
	// Субтаблица
	////////////////////////////////////////
	if (GetKeyStr(szText, "Detail", tmp))
	{
		// Процесс
		GetKeyVal(tmp, "process", s);
		s.MakeUpper();
		if (s == "CREATE")
			trnInfo.iSubPrc = DST_CREATE;
		if (s == "ADD")
			trnInfo.iSubPrc = DST_ADD;
		if (s == "REPLACE")
			trnInfo.iSubPrc = DST_REPLACE;
		if (s == "FULL")
			trnInfo.iSubPrc = DST_FULL;
		// Схема БД
		GetKeyVal(tmp, "owner", trnInfo.sSubOwn);
		// Таблица 
		GetKeyVal(tmp, "table", trnInfo.sSubDst);
		// SQL Where
		GetKeyStr(tmp, "condition", trnInfo.sSubCond);
		// Карта полей
		GetKeyStr(tmp, "columns", trnInfo.sSubMap);
	}
	// Команды субтаблицы
	GetKeyStr(szText, "SubCommands", trnInfo.sSubCmd);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Разбор выражения и замена ссылок на '?'

BOOL ParseRefExp(CString &strExp, CString &strErr) 
{
	if (strExp.IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_EXPR);
		return FALSE;
	}
	// Удаление всех символов '?'
	// на всякий случай...
	strExp.Remove('?');
	// Обработка выражения
	CString	s,sRef,sType,sLen,sDec;
	for (int i = 0, n = 0, l = 0;; i++)
	{
		// Поиск префикса ссылки
		if ((n = strExp.FindOneOf(_T("^:&$~#!@"))) < 0)
			break;
		// Определение значения ссылки
		GetWord(strExp, n+1, sRef);
		// Замена ссылки на знак ? в выражении
		strExp.Delete(n, sRef.GetLength()+1);
		strExp.Insert(n, '?');
		// Проверить, не указан ли тип
		l = GetLex(strExp, n + 1, s);
		if (s == _T("("))
		{
			// Получить сокращение типа
			l = GetLex(strExp, l, sType);
			// Проверка следующей лексемы
			l = GetLex(strExp, l, s);
			// Если указан размер
			if (s == _T(","))
			{
				// Получить размер поля
				l = GetLex(strExp, l, sLen);
				// Проверка следующей лексемы
				l = GetLex(strExp, l, s);
				// Если указана точность
				if (s == _T(","))
				{
					// Получить точность поля
					l = GetLex(strExp, l, sDec);
					// Проверка следующей лексемы
					l = GetLex(strExp, l, s);
					if (s != _T(")"))
					{
						strErr = GetErrMsg(ER_SNTX_PARAM, sRef);
						return FALSE;
					}
				}
				else if (s != _T(")"))
				{
					strErr = GetErrMsg(ER_SNTX_PARAM, sRef);
					return FALSE;
				}
			}
			else if (s != _T(")"))
			{
				strErr = GetErrMsg(ER_SNTX_PARAM, sRef);
				return FALSE;
			}
			// Удаление информации о типе
			strExp.Delete(n + 1, l - n - 1);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Чтение определения профиля

BOOL ReadPPL(LPCTSTR szFile, PPL_INFO &info) 
{
	// Чтение определения профиля
	CString	s,str,strErr;
	int		i,n,m;

	// Чтение данных из файла профиля
	if (!FileGet(szFile, str)) return FALSE;
	// Чтение параметров
	if ((n = str.Find("PARAMETERS{")) > 0)
	{
		m = str.Find("}", n + 11);
		s = str.Mid(n + 11, m - n - 12);
		s.TrimLeft();
		s.TrimRight();
		info.sParm = s;
	}
	// Чтение установок соединения
	if ((n = str.Find("CONNECT{")) > 0)
	{
		m = str.Find("}", n + 8);
		s = str.Mid(n + 8, m - n - 9);
		// Источник
		GetKeyStr(s, "Source", info.sSrc);
		// Приемник
		GetKeyStr(s, "Destination", info.sDst);
	}
	// Чтение списка трансфертов
	n = 0;
	i = 0;
	while ((n = str.Find("TRANSFER{",n)) >= 0)
	{
		if ((m = str.Find("}", n + 9)) < 0)
			break;
		// Чтение блока трансферта
		s = str.Mid(n + 9, m - n - 10);
		s.TrimLeft();
		s.TrimRight();
		if (!s.IsEmpty())
		{
			// Заполнение структуры трансферта
			info.arTrn.SetSize(i + 1);
			if (FillTrnInfo(s, info.arTrn[i], strErr))
			{
#ifdef _TRIAL_VER
				if (info.arTrn[i].iSchema == TRN_COMPLEX)
				{
					// Ограничение по схеме в Trial версии
					strErr.FormatMessage(IDS_ER_TRIAL_COMPLEX);
					strErr.Insert(0, ":\n");
					strErr.Insert(0, LPCTSTR(info.arTrn[i].sName));
					AfxMessageBox(strErr);
				}
#endif
				i++;
			}
			else
			{
				MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(strErr + ":\n" + s, 
					MB_ICONEXCLAMATION | MB_OK);
			}
		}
		n = m + 1;
	}
	// Установка размера массива трансфертов
	info.arTrn.SetSize( i );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Запись определения профиля

BOOL WritePPL(LPCTSTR szFile, CString strName, PPL_INFO &info) 
{
	CString	str,tmp,err;

	// Наименование
	str += "// " + strName + "\r\n";
	// Параметры
	str += "PARAMETERS{\r\n";
	if (!info.sParm.IsEmpty())
		str += info.sParm + "\r\n";
	str += "}\r\n";
	// Соединение
	str += "CONNECT{\r\n";
	str += "Source(" + info.sSrc + ")\r\n";
	str += "Destination(" + info.sDst + ")\r\n";
	str += "}\r\n";
	// Список трансфертов
	int	nCount = info.arTrn.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		// Заголовок трансферта
		str += "TRANSFER{\r\n";
		// Наименование
		str += "Name(" + info.arTrn[i].sName + ")\r\n";
		// Схема
		switch (info.arTrn[i].iSchema) {
		case TRN_COPY:    tmp = "COPY";    break;
		case TRN_COLUMNS: tmp = "COLUMNS"; break;
		case TRN_COMPLEX:
			{
#ifdef _TRIAL_VER
				// Ограничение по схеме в Trial версии
				err.FormatMessage(IDS_ER_TRIAL_COMPLEX);
				err.Insert(0, ":\n");
				err.Insert(0, info.arTrn[i].sName);
				AfxMessageBox(err);
#endif
				tmp = "COMPLEX"; break;
			}
		}
		str += "Schema(" + tmp + ")\r\n";
		// Конвертация текста
		switch (info.arTrn[i].iConvert) {
		case TRN_UNCHANGED: tmp = "UNCHANGED"; break;
		case TRN_OEMTOANSI: tmp = "OEMTOANSI"; break;
		case TRN_ANSITOOEM: tmp = "ANSITOOEM"; break;
		}
		str += "ConvertText(" + tmp + ")\r\n";
		// SQL Select источника
		str += "Source(" + info.arTrn[i].sTabSQL + ")\r\n";
		// SQL Select субтаблицы
		if (info.arTrn[i].iSchema == TRN_COMPLEX)
			str += "SubSQL(" + info.arTrn[i].sSubSQL + ")\r\n";
		// Строка сообщения
		str += "Message(" + info.arTrn[i].sMsg + ")\r\n";
		// Приемник
		str += "Destination(";
		// Процесс
		switch (info.arTrn[i].iTabPrc) {
		case DST_CREATE:  tmp = "CREATE";  break;
		case DST_ADD:     tmp = "ADD";     break;
		case DST_REPLACE: tmp = "REPLACE"; break;
		case DST_FULL:    tmp = "FULL";    break;
		}
		str += "process=" + tmp;
		// Схема БД таблицы
		str += " owner=" + info.arTrn[i].sTabOwn;
		// Таблица приемника
		str += " table=" + info.arTrn[i].sTabDst;
		// SQL Where приемника
		if (!info.arTrn[i].sTabCond.IsEmpty())
			str += "\r\ncondition(" + info.arTrn[i].sTabCond + ")";
		// Карта полей приемника
		if (!info.arTrn[i].sTabMap.IsEmpty())
			str += "\r\ncolumns(" + info.arTrn[i].sTabMap + ")";
		str += ")\r\n";
		// Субтаблица
		if (info.arTrn[i].iSchema == TRN_COMPLEX)
		{
			str += "Detail(";
			// Процесс
			switch (info.arTrn[i].iSubPrc) {
			case DST_CREATE:  tmp = "CREATE";  break;
			case DST_ADD:     tmp = "ADD";     break;
			case DST_REPLACE: tmp = "REPLACE"; break;
			case DST_FULL:    tmp = "FULL";    break;
			}
			str += "process=" + tmp;
			// Схема БД субтаблицы
			str += " owner=" + info.arTrn[i].sSubOwn;
			// Таблица субтаблицы
			str += " table=" + info.arTrn[i].sSubDst;
			// SQL Where субтаблицы
			if (!info.arTrn[i].sSubCond.IsEmpty())
				str += "\r\ncondition(" + info.arTrn[i].sSubCond + ")";
			// Карта полей субтаблицы
			if (!info.arTrn[i].sSubMap.IsEmpty())
				str += "\r\ncolumns(" + info.arTrn[i].sSubMap + ")";
			str += ")\r\n";
		}
		// Команды приемника
		if (!info.arTrn[i].sTabCmd.IsEmpty())
			str += "TabCommands(\r\n" + info.arTrn[i].sTabCmd + ")\r\n";
		// Команды субтаблицы
		if (!info.arTrn[i].sSubCmd.IsEmpty())
			str += "SubCommands(\r\n" + info.arTrn[i].sSubCmd + ")\r\n";
		// Завершение блока трансферта
		str += "}\r\n";
	}
	// Запись в файл
	if (!FilePut(szFile, str)) return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// Запуск передачи данных с выбором профиля

BOOL RunPPLSel(int nKind) 
{
	// nKind - вид профиля для выбора
	// -1 - все виды (по умолчанию)
	//  0 - передача, 1 - импорт, 2 - экспорт

	// Выбор профиля передачи
	CGetPPLDlg	pplDlg;
	pplDlg.m_nKind = nKind;
	if (pplDlg.DoModal() != IDOK)
		return FALSE;
	if (pplDlg.m_nIndex < 0)
		return FALSE;
	// Обновление основного окна приложения
	AfxGetApp()->m_pMainWnd->UpdateWindow();
	// Установка файла профиля
	int i = pplDlg.m_nIndex;

	// Запуск профиля на выполнение
	return RunPipe(TrnParm.sDirPpl+arPPL[i].sFile, 
					arPPL[i].sName, arPPL[i].iKind);
}

/////////////////////////////////////////////////////////////////////
// Запуск передачи данных

BOOL RunPipe(LPCTSTR szFile, LPCTSTR szTitle, int nKind, 
			 BOOL bUseDlg, LPCTSTR szParm) 
{
	// szFile  - файл профиля (полный путь)
	// szTitle - заголовок окна диалога
	// nKind   - вид профиля: 0-общ, 1-имп, 2-эксп
	// bUseDlg - использовать диалог параметров
	// szParm  - строка (параметр=значение,...)
	int			i;
	BOOL		bStart = 0;	// Признак запуска
	BOOL		bOpenLog;	// Признак открытия Log-файла
	BOOL		bShowMsg;	// Показ сообщений
	UINT		nMsgCount;	// Число предупреждений
	UINT		nErrCount;	// Число ошибок
	CString		strCapt;	// Строка заголовка
	CString		strMesg;	// Строка сообщения
	CString		strSQL;		// Оператор SQL
	CString		strErr;		// Строка ошибок
	CTime		curTime;	// Текущее время
	CDatabase	dbSrc;		// БД источника
	CDatabase	dbDst;		// БД приемника
	CFldArray	arPrm;		// Массив параметров
	PPL_INFO	PPL;		// Описание Pipeline
	ODBC_INFO	srcInfo;	// Описание источника
	ODBC_INFO	dstInfo;	// Описание приемника
	CMessageDlg	msgDlg;		// Окно сообщения
	CWnd*		pWnd;		// Главное окно приложения
	CArray<CTransfer*, CTransfer*> arObTrn;

#ifndef _TRIAL_VER
	CString		sData;		// Строка даты
	CString		sInput;		// Строка данных
	CString		sMD5;		// Строка подписи MD5
	CString		sSignature;	// Строка подписи из файла
	CString		sVer;		// Строка номер версии программы
	CString		sVersion;	// Строка номер версии из файла
	CString		sNumber;	// Строка номер лицензии
	CString		sName;		// Строка имя пользователя
	CString		sEmail;		// Строка email пользователя
	CString		sOrg;		// Строка организация
	CString		sSerial;	// Серийный номер
	MD5STRUCT	md;
	RSAPubkey	rk;
	BYTE*		lpKeyData;
	ZZ			cipher,plain;
	HMODULE		hModule;
	HRSRC		hRes;
	HGLOBAL		hGlob;

	/////////////////////////////////////////////////////////////////
	// Проверка ключа
	/////////////////////////////////////////////////////////////////
	if (IsDebuggerPresent())
		return FALSE;

	// Данные из файла
	if ((i = sLicense.Find(_T("Value="))) > 0)
		sInput = sLicense.Left(i+6);
	// Подпись MD5 вычисленная
	sMD5 = GetMD5String(sInput.GetBuffer(sInput.GetLength()), 
						sInput.GetLength()).c_str();
	// Подпись MD5 из файла
	sSignature = GetKeyValue(sLicense, _T("Value"));

	// Проверка MD5 подписи
	if (sMD5.Compare(sSignature) != 0)
	{
		strErr.LoadString(IDS_ER_WRONG_KEYFILE);
		goto err;
	}
	// Чтение номера версии
	sVersion = GetKeyValue(sLicense, _T("Version"));
	// Чтение номера лицензии
	sNumber = GetKeyValue(sLicense, _T("Number"));
	// Чтение даты создания
	sData = GetKeyValue(sLicense, _T("Created"));
	// Чтение имени пользователя
	sName = GetKeyValue(sLicense, _T("Name"));
	// Чтение email пользователя
	sEmail = GetKeyValue(sLicense, _T("Email"));
	// Чтение организации пользователя
	sOrg = GetKeyValue(sLicense, _T("Company"));
	// Чтение зашифрованного серийного номера
	sSerial = GetKeyValue(sLicense, _T("Code"));
	// Расшифровка серийного номера методом Base64
	sSerial = Base64::decode((LPCTSTR)sSerial);
	// Набор данных для получения серийного номера
	sInput = sData + _T(" ") + sNumber + _T(" ") + sName;
	sInput+= _T(" ") + sEmail + _T(" ") + sOrg;

	// Загрузка номера текущей версии
	sVer.LoadString(IDS_VERSION_NUMBER);
	// Проверка версии лицензии
	if (sVersion.Compare(sVer) != 0)
	{
		strErr.LoadString(IDS_ER_WRONG_KEYFILE);
		goto err;
	}
	// Загрузить публичный ключ из ресурсов
	hModule = GetModuleHandle(NULL);
	hRes = FindResource(hModule, MAKEINTRESOURCE(IDR_BINARY), _T("BINARY"));
	hGlob = LoadResource(hModule, hRes);
    lpKeyData = (BYTE*)LockResource(hGlob);
	// Прочитать публичный ключ
	rk = ReadPublicKey(lpKeyData);
	FreeResource(hGlob);

	// Расшифровка серийного номера
	cipher = to_ZZ((LPCTSTR)sSerial);
	plain = RSA_SigDecrypt(cipher, rk);
	sSerial = to_string(plain).c_str(); 

	// Получить Message Digest
	md = GetBufferMD5(sInput.GetBuffer(sInput.GetLength()), 
						sInput.GetLength());
	sMD5 = bytes_to_string(md.digest,16).c_str();

	i = sMD5.GetLength() - sSerial.GetLength();
	if (i > 0) sMD5 = sMD5.Mid(i);

	// Проверка серийного номера
	if (sMD5.Compare(sSerial) != 0)
	{
		strErr.LoadString(IDS_ER_WRONG_KEYFILE);
		goto err;
	}
#endif

	// Время начала операции
	curTime = CTime::GetCurrentTime();
	// Открытие файла протокола
	bOpenLog = OpenFileLog();
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		fileLog.WriteString(_T("\n"));
		fileLog.WriteString(_T(">>> START TRANSFER\n"));
		strMesg.LoadString(IDS_MS_START_PIPE);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Если импорт, открыть файл сообщений
	if (nKind == 1 && TrnParm.iWriteMsg)
	{
		bFileMsg = TRUE;
		strMesg = curTime.Format("%d-%m-%Y_%H-%M-%S_");
		// Открытие файла сообщений
		// в каталоге файла протокола
		sFileMsg = strMesg + TrnParm.sFileMsg;
		if (!fileMsg.Open(TrnParm.sDirLog + sFileMsg, 
			CFile::modeCreate|CFile::modeReadWrite|
			CFile::typeText|CFile::shareDenyNone))
		{
			bFileMsg = FALSE;
			strErr = GetErrMsg(ER_OPEN_FILE, sFileMsg);
			// Запись текста ошибки в файл
			if (bFileLog)
			{
				fileLog.WriteString(strErr + _T("\n"));
			}
			if (TrnParm.iShowErr)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
			}
		}
	}
	else
	{
		bFileMsg = FALSE;
		sFileMsg = "";
	}
	// Главное окно приложения
	pWnd = AfxGetApp()->GetMainWnd();
	// Признак показа окон сообщений
	bShowMsg = !pWnd->IsIconic();

	// Проверка файла
	if (CString(szFile).IsEmpty())
	{
		strErr = GetErrMsg(ER_NO_PPL);
		goto err;
	}
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_READ_PPL);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Чтение определения профиля из файла
	if (bShowMsg)
	{
		// Создание окна сообщения
		AfxGetApp()->BeginWaitCursor();
		strCapt.LoadString(IDS_MS_INIT_PPL);
		msgDlg.Create(IDD_MESSAGEDLG, AfxGetApp()->m_pMainWnd);
		msgDlg.SetWindowText(strCapt);
		msgDlg.m_sText.LoadString(IDS_MS_READ_PPL);
		msgDlg.UpdateData(FALSE);
	}
	else
	{
		strCapt.LoadString(IDS_MS_READ_PPL);
		pWnd->SetWindowText(strCapt);
	}
	if (!ReadPPL(szFile, PPL))
	{
		strErr = GetErrMsg(ER_READ_FILE, szFile);
		goto err;
	}
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_INIT_PARM);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Заполнение массива параметров
	if (bShowMsg)
	{
		msgDlg.m_sText.LoadString(IDS_MS_INIT_PARM);
		msgDlg.UpdateData(FALSE);
	}
	else
	{
		strCapt.LoadString(IDS_MS_INIT_PARM);
		pWnd->SetWindowText(strCapt);
	}
	if (!FillParmArray(PPL.sParm, arPrm))
	{
		strErr = GetErrMsg(ER_SNTX_PARAMS);
		goto err;
	}
	// Установка значений параметров
	if (!CString(szParm).IsEmpty())
	{
		int		n,m;
		CString	sKey;
		CString sPrm = CString(szParm);
		sPrm.MakeLower();
		for (i = 0; i < arPrm.GetSize(); i++)
		{
			sKey = arPrm[i].sID;
			sKey.MakeLower();
			if ((n = sPrm.Find(sKey + "=")) >= 0)
			{
				n += sKey.GetLength() + 1;
				if ((m = sPrm.Mid(n).FindOneOf(" \t,")) < 0)
					 m = sPrm.Mid(n).GetLength();
				arPrm[i].sValue = CString(szParm).Mid(n, m);
			}
		}
	}
	if (bCancel) goto err;

	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_CONN_SRC);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Соединение с БД источника
	if (bShowMsg)
	{
		strCapt.LoadString(IDS_MS_CONNECT);
		msgDlg.SetWindowText(strCapt);
		msgDlg.m_sText.LoadString(IDS_MS_CONN_SRC);
		msgDlg.UpdateData(FALSE);
	}
	else
	{
		strCapt.LoadString(IDS_MS_CONN_SRC);
		pWnd->SetWindowText(strCapt);
	}
	dbSrc.SetLoginTimeout(5);
	try
	{
		dbSrc.OpenEx(PPL.sSrc, CDatabase::noOdbcDialog);
	}
	catch (CDBException* e)
	{
		strErr = PPL.sSrc + _T("\n") + e->m_strError;
		strErr = GetErrMsg(ER_CONN_SRC, strErr);
		goto err;
	}
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_READ_SRC_INFO);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Заполнение информации об источнике
	if (bShowMsg)
	{
		msgDlg.m_sText.LoadString(IDS_MS_READ_SRC_INFO);
		msgDlg.UpdateData(FALSE);
	}
	else
	{
		strCapt.LoadString(IDS_MS_READ_SRC_INFO);
		pWnd->SetWindowText(strCapt);
	}
	SQLGetODBCInfo(dbSrc.m_hdbc, srcInfo);
	srcInfo.hdbc = dbSrc.m_hdbc;
	// Установка параметров сессии для источника
	if (srcInfo.sDbmsName == _T("Oracle"))
		dbSrc.ExecuteSQL("alter session set nls_date_format='dd.mm.yyyy'");
	if (srcInfo.sDbmsName == _T("MS SQL"))
		dbSrc.ExecuteSQL("set dateformat dmy");

	if (bCancel) goto err;

	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_CONN_DST);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Соединение с БД приемника
	if (bShowMsg)
	{
		msgDlg.m_sText.LoadString(IDS_MS_CONN_DST);
		msgDlg.UpdateData(FALSE);
	}
	else
	{
		strCapt.LoadString(IDS_MS_CONN_DST);
		pWnd->SetWindowText(strCapt);
	}
	dbDst.SetLoginTimeout(5);
	try
	{
		dbDst.OpenEx(PPL.sDst, CDatabase::noOdbcDialog);
	}
	catch (CDBException* e)
	{
		strErr = PPL.sDst + _T("\n") + e->m_strError;
		strErr = GetErrMsg(ER_CONN_DST, strErr);
		goto err;
	}
	// Запись протокола операции в файл
	if (bFileLog && TrnParm.iWriteOpr)
	{
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_READ_DST_INFO);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
	}
	// Заполнение информации о приемнике
	if (bShowMsg)
	{
		msgDlg.m_sText.LoadString(IDS_MS_READ_DST_INFO);
		msgDlg.UpdateData(FALSE);
	}
	else
	{
		strCapt.LoadString(IDS_MS_READ_DST_INFO);
		pWnd->SetWindowText(strCapt);
	}
	SQLGetODBCInfo(dbDst.m_hdbc, dstInfo);
	dstInfo.hdbc = dbDst.m_hdbc;
	// Установка параметров сессии для приемника
	if (dstInfo.sDbmsName == _T("Oracle"))
		dbDst.ExecuteSQL("alter session set nls_date_format='dd.mm.yyyy'");
	if (dstInfo.sDbmsName == _T("MS SQL"))
		dbDst.ExecuteSQL("set dateformat dmy");

	if (bCancel) goto err;

	// Инициализация трансфертов
	if (bShowMsg)
	{
		strCapt.LoadString(IDS_MS_INIT_TRN);
		msgDlg.SetWindowText(strCapt);
	}
	else
	{
		strCapt.LoadString(IDS_MS_INIT_TRN);
		pWnd->SetWindowText(strCapt);
	}
	for (i = 0; i < PPL.arTrn.GetSize(); i++)
	{
		// Запись протокола операции в файл
		if (bFileLog && TrnParm.iWriteOpr)
		{
			curTime = CTime::GetCurrentTime();
			strMesg.LoadString(IDS_MS_INIT_TRN);
			strMesg += _T(": ") + PPL.arTrn[i].sName;
			strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
			fileLog.WriteString(strMesg + _T("\n"));
		}
		if (bShowMsg)
		{
			msgDlg.m_sText = PPL.arTrn[i].sName;
			msgDlg.UpdateData(FALSE);
		}
		arObTrn.SetSize(i + 1);
		arObTrn[i] = new CTransfer(nKind, &srcInfo, &dstInfo, 
									&PPL.arTrn[i], &arPrm);
		PPL.arTrn[i].bValid  = arObTrn[i]->Parse(strErr);
		PPL.arTrn[i].bSelect = arObTrn[i]->IsValid();
		if (!PPL.arTrn[i].bValid)
		{
			// Запись текста ошибки в файл
			if (bFileLog)
			{
				fileLog.WriteString(strErr + _T("\n"));
			}
			// Показ сообщения об ошибке
			if (TrnParm.iShowErr)
			{
				MessageBeep(MB_ICONEXCLAMATION);
				AfxGetApp()->m_pMainWnd->MessageBox(strErr,
							PPL.arTrn[i].sName,
							MB_OK|MB_ICONEXCLAMATION);
				AfxGetApp()->m_pMainWnd->UpdateWindow();
			}
		}
	}
	if (bShowMsg)
	{
		msgDlg.DestroyWindow();
		AfxGetApp()->EndWaitCursor();
	}
	else
	{
		pWnd->SetWindowText(TrnParm.sNameApp);
	}
	nMsgCount = 0;
	nErrCount = 0;

	if (bCancel) goto end;

	// Вызов диалога для установки параметров
	// и отбора трансфертов на выполнение
	if (bUseDlg)
	{
		CTrnStartDlg dlg(AfxGetApp()->m_pMainWnd);
		dlg.m_sTitle = szTitle;
		dlg.m_pArPrm = &arPrm;
		dlg.m_pArTrn = &PPL.arTrn;
		if (dlg.DoModal() == IDCANCEL) goto end;
		AfxGetApp()->m_pMainWnd->UpdateWindow();
	}
	// Выполнение трансфертов
	bStart = 1;
	for (i = 0; i < arObTrn.GetSize(); i++)
	{
		if (bCancel) goto end;

		if (PPL.arTrn[i].bValid &&
			PPL.arTrn[i].bSelect)
		{
			// Запись протокола операции в файл
			if (bFileLog && TrnParm.iWriteOpr)
			{
				// Время начала операции
				curTime = CTime::GetCurrentTime();
				strMesg.FormatMessage(IDS_MS_RUN_TRN, PPL.arTrn[i].sName);
				strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
				fileLog.WriteString(strMesg + _T("\n"));
			}
			if (!arObTrn[i]->Run(strErr, nMsgCount, nErrCount))
			{
				nErrCount++;
				strErr = PPL.arTrn[i].sName + _T(":\n") + strErr;
				// Запись текста ошибки в файл
				if (bFileLog)
				{
					curTime = CTime::GetCurrentTime();
					strErr = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strErr;
					fileLog.WriteString(strErr + _T("\n"));
				}
				// Сообщение об ошибке
				if (TrnParm.iShowErr)
				{
					MessageBeep(MB_ICONEXCLAMATION);
					AfxGetApp()->m_pMainWnd->MessageBox(strErr,
								PPL.arTrn[i].sName,
								MB_OK|MB_ICONEXCLAMATION);
					AfxGetApp()->m_pMainWnd->UpdateWindow();
				}
			}
		}
	}

end:
	// Уничтожение трансфертов
	for (i = arObTrn.GetSize() - 1; i >= 0; i--)
		delete arObTrn[i];
	// Закрытие соединений
	if (dbSrc.IsOpen()) dbSrc.Close();
	if (dbDst.IsOpen()) dbDst.Close();
	// Завершающая запись в протокол
	if (bFileLog)
	{
		curTime = CTime::GetCurrentTime();
		strMesg.LoadString(IDS_MS_STOP_PIPE);
		strMesg = curTime.Format("%d.%m.%Y %H:%M:%S: ") + strMesg;
		fileLog.WriteString(strMesg + _T("\n"));
		fileLog.WriteString(_T(">>> STOP TRANSFER\n"));
	}
	// Закрытие файла протокола
	if (bOpenLog) CloseFileLog();
	// Закрытие файла сообщений
	if (bFileMsg) fileMsg.Close();
	// Копирование файла сообщений в каталог сообщений
	// еслм каталоги протокола и сообщений не идентичны
	if (bFileMsg && TrnParm.sDirLog.CompareNoCase(TrnParm.sDirMsg) != 0)
	{
		CopyFile(TrnParm.sDirLog + sFileMsg, 
				TrnParm.sDirMsg + sFileMsg, FALSE);
	}
	// Сброс статуса файла сообщений
	bFileMsg = FALSE;
	// Проверка статуса завершения
	if ((nMsgCount > 0 || nErrCount > 0) &&
		TrnParm.iShowErr && bUseDlg)
	{
		strErr.FormatMessage(IDS_MS_FMT_RESULTS, nMsgCount, nErrCount);
		strCapt.LoadString(IDS_MS_ERRORS);
		strMesg.LoadString(IDS_MS_VIEW_LOG);
		strErr  = strCapt + _T("\n\n")+strErr + _T("\n\n")+strMesg;
		MessageBeep(MB_ICONEXCLAMATION);
		i = AfxGetApp()->m_pMainWnd->MessageBox(strErr, szTitle, 
						MB_YESNO|MB_ICONEXCLAMATION|MB_DEFBUTTON1);
		if (i == IDYES)
		{
			// Показ файла протокола
			AfxGetApp()->OpenDocumentFile(sFileLog);
		}
	}
	else if (bStart && bShowMsg && bUseDlg)
	{
		MessageBeep(MB_ICONASTERISK);
		strMesg.LoadString(IDS_MS_SUCCESS);
		AfxGetApp()->m_pMainWnd->MessageBox(strMesg, szTitle, 
									MB_OK|MB_ICONASTERISK);
	}
	return TRUE;

err:
	// Закрытие открытых соединений
	if (dbSrc.IsOpen()) dbSrc.Close();
	if (dbDst.IsOpen()) dbDst.Close();
	// Запись сообщения об ошибке
	if (bFileLog)
	{
		fileLog.WriteString(strErr + _T("\n"));
		// Завершающая запись в протокол
		fileLog.WriteString(_T(">>> STOP TRANSFER\n"));
	}
	// Закрытие файла протокола
	if (bOpenLog) CloseFileLog();
	// Закрытие файла сообщений
	if (bFileMsg) fileMsg.Close();
	// Копирование файла сообщений в каталог сообщений
	// еслм каталоги протокола и сообщений не идентичны
	if (bFileMsg && TrnParm.sDirLog.CompareNoCase(TrnParm.sDirMsg) != 0)
	{
		CopyFile(TrnParm.sDirLog + sFileMsg, 
				TrnParm.sDirMsg + sFileMsg, FALSE);
	}
	// Сброс статуса файла сообщений
	bFileMsg = FALSE;
	// Уничтожение окна статуса
	if (bShowMsg)
	{
		msgDlg.DestroyWindow();
		AfxGetApp()->EndWaitCursor();
	}
	else
	{
		pWnd->SetWindowText(TrnParm.sNameApp);
	}
	if (TrnParm.iShowErr && bUseDlg)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(strErr, MB_OK|MB_ICONEXCLAMATION);
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// SQL функции - помощники

UDWORD SQLGetError(SQLHSTMT hstmt, CString &strErr) 
{
	// Чтение текста ошибки
	SQLCHAR		szState[6];
	SQLCHAR		szMsg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER	iNumErr;
	SQLSMALLINT	iMsgLen;
	SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, 
				szState, &iNumErr, szMsg, 
				sizeof(szMsg), &iMsgLen);
	strErr = CString(szMsg);

	return iNumErr;
}

CString SQLGetInfoString(SQLHDBC hdbc, UWORD fInfoType) 
{
	UCHAR buffer[255];
	SWORD cbData;

	::SQLGetInfo(hdbc,fInfoType, (PTR)buffer,255,&cbData);
	return CString(buffer);
}

UWORD SQLGetInfoShort(SQLHDBC hdbc, UWORD fInfoType) 
{
	UWORD value;
	SWORD cbData;

	::SQLGetInfo(hdbc,fInfoType, (PTR)&value,2,&cbData);
	return value;
}

UDWORD SQLGetInfoLong(SQLHDBC hdbc, UWORD fInfoType) 
{
	UDWORD value;
	SWORD  cbData;

	::SQLGetInfo(hdbc,fInfoType, (PTR)&value,4,&cbData);
	return value;
}

BOOL SQLGetInfoBool(SQLHDBC hdbc, UWORD fInfoType) 
{
	if (SQLGetInfoString(hdbc, fInfoType) == _T("Y"))
		return TRUE;

	return FALSE;
}

int SQLGetTypeID(CString strType) 
{
	// Получить ODBC SQL Type ID по наименованию
	strType.MakeUpper();

	if (strType == _T("BIT") || strType == _T("BT"))
		return SQL_BIT;
	if (strType == _T("TINYINT") || strType == _T("TI"))
		return SQL_TINYINT;
	if (strType == _T("SMALLINT") || strType == _T("SI"))
		return SQL_SMALLINT;
	if (strType == _T("INTEGER") || strType == _T("I"))
		return SQL_INTEGER;
	if (strType == _T("BIGINT") || strType == _T("BI"))
		return SQL_BIGINT;
	if (strType == _T("REAL") || strType == _T("R"))
		return SQL_REAL;
	if (strType == _T("FLOAT") || strType == _T("F"))
		return SQL_FLOAT;
	if (strType == _T("DOUBLE") || strType == _T("DB"))
		return SQL_DOUBLE;
	if (strType == _T("DECIMAL") || strType == _T("DC"))
		return SQL_DECIMAL;
	if (strType == _T("NUMERIC") || strType == _T("N"))
		return SQL_NUMERIC;
	if (strType == _T("CHAR") || strType == _T("A"))
		return SQL_CHAR;
	if (strType == _T("VARCHAR") || strType == _T("VA"))
		return SQL_VARCHAR;
	if (strType == _T("LONGVARCHAR") || strType == _T("LVA"))
		return SQL_LONGVARCHAR;
	if (strType == _T("BINARY") || strType == _T("B"))
		return SQL_BINARY;
	if (strType == _T("VARBINARY") || strType == _T("VB"))
		return SQL_VARBINARY;
	if (strType == _T("LONGVARBINARY") || strType == _T("LVB"))
		return SQL_LONGVARBINARY;
	if (strType == _T("DATE") || strType == _T("D"))
		return SQL_DATE;
	if (strType == _T("TIME") || strType == _T("T"))
		return SQL_TIME;
	if (strType == _T("TIMESTAMP") || strType == _T("TS"))
		return SQL_TIMESTAMP;

	return 0;
}

int SQLGetTypeIndex(ODBC_INFO &info, UINT iSQLType) 
{
	// Получить индекс типа данных ODBC
	int i, n = info.arType.GetSize();
	for (i = 0; i < n; i++)
	{
		if (info.arType[i].iType == iSQLType)
			return i;
	}
	return -1;
}

TYPE_INFO SQLGetTypeDef(ODBC_INFO &info, UINT iSQLType) 
{
	// Получить информацию о типе данных ODBC
	TYPE_INFO typeInfo;
	int i, n = info.arType.GetSize();
	for (i = 0; i < n; i++)
	{
		if (info.arType[i].iType == iSQLType)
		{
			typeInfo = info.arType[i];
			break;
		}
	}
	return typeInfo;
}

BOOL SQLConvertType(ODBC_INFO &dstInfo, UINT iSrcType, UINT &iDstType) 
{
	// Сконвертировать тип данных
	switch (iSrcType)
	{
	case SQL_BIT:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_BIT) >= 0)
			iDstType = SQL_BIT;
		else if (SQLGetTypeIndex(dstInfo, SQL_TINYINT) >= 0)
			iDstType = SQL_TINYINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_SMALLINT) >= 0)
			iDstType = SQL_SMALLINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_INTEGER) >= 0)
			iDstType = SQL_INTEGER;
		else if (SQLGetTypeIndex(dstInfo, SQL_BIGINT) >= 0)
			iDstType = SQL_BIGINT;
		else
			return FALSE;
		break;
	}
	case SQL_TINYINT:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_TINYINT) >= 0)
			iDstType = SQL_TINYINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_SMALLINT) >= 0)
			iDstType = SQL_SMALLINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_INTEGER) >= 0)
			iDstType = SQL_INTEGER;
		else if (SQLGetTypeIndex(dstInfo, SQL_BIGINT) >= 0)
			iDstType = SQL_BIGINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_REAL) >= 0)
			iDstType = SQL_REAL;
		else if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else
			return FALSE;
		break;
	}
	case SQL_SMALLINT:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_SMALLINT) >= 0)
			iDstType = SQL_SMALLINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_INTEGER) >= 0)
			iDstType = SQL_INTEGER;
		else if (SQLGetTypeIndex(dstInfo, SQL_BIGINT) >= 0)
			iDstType = SQL_BIGINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_REAL) >= 0)
			iDstType = SQL_REAL;
		else if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else
			return FALSE;
		break;
	}
	case SQL_INTEGER:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_INTEGER) >= 0)
			iDstType = SQL_INTEGER;
		else if (SQLGetTypeIndex(dstInfo, SQL_BIGINT) >= 0)
			iDstType = SQL_BIGINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_REAL) >= 0)
			iDstType = SQL_REAL;
		else if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else
			return FALSE;
		break;
	}
	case SQL_BIGINT:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_BIGINT) >= 0)
			iDstType = SQL_BIGINT;
		else if (SQLGetTypeIndex(dstInfo, SQL_REAL) >= 0)
			iDstType = SQL_REAL;
		else if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else
			return FALSE;
		break;
	}
	case SQL_REAL:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_REAL) >= 0)
			iDstType = SQL_REAL;
		else if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else
			return FALSE;
		break;
	}
	case SQL_FLOAT:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else
			return FALSE;
		break;
	}
	case SQL_DOUBLE:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else
			return FALSE;
		break;
	}
	case SQL_DECIMAL:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_DECIMAL) >= 0)
			iDstType = SQL_DECIMAL;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_REAL) >= 0)
			iDstType = SQL_REAL;
		else
			return FALSE;
		break;
	}
	case SQL_NUMERIC:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_NUMERIC) >= 0)
			iDstType = SQL_NUMERIC;
		else if (SQLGetTypeIndex(dstInfo, SQL_DOUBLE) >= 0)
			iDstType = SQL_DOUBLE;
		else if (SQLGetTypeIndex(dstInfo, SQL_FLOAT) >= 0)
			iDstType = SQL_FLOAT;
		else if (SQLGetTypeIndex(dstInfo, SQL_REAL) >= 0)
			iDstType = SQL_REAL;
		else
			return FALSE;
		break;
	}
	case SQL_CHAR:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_CHAR) >= 0)
			iDstType = SQL_CHAR;
		else if (SQLGetTypeIndex(dstInfo, SQL_VARCHAR) >= 0)
			iDstType = SQL_VARCHAR;
		else
			return FALSE;
		break;
	}
	case SQL_VARCHAR:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_VARCHAR) >= 0)
			iDstType = SQL_VARCHAR;
		else if (SQLGetTypeIndex(dstInfo, SQL_CHAR) >= 0)
			iDstType = SQL_CHAR;
		else
			return FALSE;
		break;
	}
	case SQL_LONGVARCHAR:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_LONGVARCHAR) >= 0)
			iDstType = SQL_LONGVARCHAR;
		else
			return FALSE;
		break;
	}
	case SQL_BINARY:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_BINARY) >= 0)
			iDstType = SQL_BINARY;
		else if (SQLGetTypeIndex(dstInfo, SQL_VARBINARY) >= 0)
			iDstType = SQL_VARBINARY;
		else
			return FALSE;
		break;
	}
	case SQL_VARBINARY:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_VARBINARY) >= 0)
			iDstType = SQL_VARBINARY;
		else if (SQLGetTypeIndex(dstInfo, SQL_BINARY) >= 0)
			iDstType = SQL_BINARY;
		else
			return FALSE;
		break;
	}
	case SQL_LONGVARBINARY:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_LONGVARBINARY) >= 0)
			iDstType = SQL_LONGVARBINARY;
		else if (SQLGetTypeIndex(dstInfo, SQL_VARBINARY) >= 0)
			iDstType = SQL_VARBINARY;
		else if (SQLGetTypeIndex(dstInfo, SQL_BINARY) >= 0)
			iDstType = SQL_BINARY;
		else
			return FALSE;
		break;
	}
	case SQL_DATE:
	case SQL_TYPE_DATE:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_DATE) >= 0)
			iDstType = SQL_DATE;
		else if (SQLGetTypeIndex(dstInfo, SQL_TIMESTAMP) >= 0)
			iDstType = SQL_TIMESTAMP;
		else
			return FALSE;
		break;
	}
	case SQL_TIME:
	case SQL_TYPE_TIME:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_TIME) >= 0)
			iDstType = SQL_TIME;
		else if (SQLGetTypeIndex(dstInfo, SQL_TIMESTAMP) >= 0)
			iDstType = SQL_TIMESTAMP;
		else
			return FALSE;
		break;
	}
	case SQL_TIMESTAMP:
	case SQL_TYPE_TIMESTAMP:
	{
		if (SQLGetTypeIndex(dstInfo, SQL_TIMESTAMP) >= 0)
			iDstType = SQL_TIMESTAMP;
		else if (SQLGetTypeIndex(dstInfo, SQL_DATE) >= 0)
			iDstType = SQL_DATE;
		else if (SQLGetTypeIndex(dstInfo, SQL_TIME) >= 0)
			iDstType = SQL_TIME;
		else
			return FALSE;
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

CString SQLGetDefStrVal(UINT iSQLType) 
{
	// Значение по умолчанию для типа данных
	switch (iSQLType)
	{
	case SQL_BIT:
	case SQL_TINYINT:
	case SQL_SMALLINT:
	case SQL_INTEGER:
	case SQL_BIGINT:
	case SQL_REAL:
	case SQL_FLOAT:
	case SQL_DOUBLE:
	case SQL_DECIMAL:
	case SQL_NUMERIC:
		return _T("0");
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR:
	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY:
		return _T("");
	case SQL_DATE:
	case SQL_TYPE_DATE:
	{
		CTime  t = CTime::GetCurrentTime();
		return t.Format("%d.%m.%Y");
	}
	case SQL_TIME:
	case SQL_TYPE_TIME:
	{
		CTime  t = CTime::GetCurrentTime();
		return t.Format("%H:%M");
	}
	case SQL_TIMESTAMP:
	case SQL_TYPE_TIMESTAMP:
	{
		CTime  t = CTime::GetCurrentTime();
		return t.Format("%d.%m.%Y %H:%M");
	}
	default:
		return _T("");
	}
}

CString SQLGetTypeStr(UINT iSQLType) 
{
	// Сокращение для типа данных
	switch (iSQLType)
	{
	case SQL_BIT:			return CString(_T("BT"));
	case SQL_TINYINT:		return CString(_T("TI"));
	case SQL_SMALLINT:		return CString(_T("SI"));
	case SQL_INTEGER:		return CString(_T("I"));
	case SQL_BIGINT:		return CString(_T("BI"));
	case SQL_REAL:			return CString(_T("R"));
	case SQL_FLOAT:			return CString(_T("F"));
	case SQL_DOUBLE:		return CString(_T("DB"));
	case SQL_DECIMAL:		return CString(_T("DC"));
	case SQL_NUMERIC:		return CString(_T("N"));
	case SQL_CHAR:			return CString(_T("A"));
	case SQL_VARCHAR:		return CString(_T("VA"));
	case SQL_LONGVARCHAR:	return CString(_T("LVA"));
	case SQL_BINARY:		return CString(_T("B"));
	case SQL_VARBINARY:		return CString(_T("VB"));
	case SQL_LONGVARBINARY:	return CString(_T("LVB"));
	case SQL_DATE:			return CString(_T("D"));
	case SQL_TYPE_DATE:		return CString(_T("D"));
	case SQL_TIME:			return CString(_T("T"));
	case SQL_TYPE_TIME:		return CString(_T("T"));
	case SQL_TIMESTAMP:		return CString(_T("TS"));
	case SQL_TYPE_TIMESTAMP:return CString(_T("TS"));
	default:				return CString(_T(""));
	}
	return CString(_T(""));
}

/////////////////////////////////////////////////////////////////////
// Заполнение информации об источнике ODBC

void SQLGetODBCInfo(SQLHDBC hdbc, ODBC_INFO &info) 
{
	if (hdbc == SQL_NULL_HDBC) return;

	// Driver Information
	info.sUserName	= SQLGetInfoString(hdbc, SQL_USER_NAME);
	info.sDbmsName	= SQLGetInfoString(hdbc, SQL_DBMS_NAME);
	info.sDbmsVer	= SQLGetInfoString(hdbc, SQL_DBMS_VER);
	info.sDrvName	= SQLGetInfoString(hdbc, SQL_DRIVER_NAME);
	info.sDrvVer	= SQLGetInfoString(hdbc, SQL_DRIVER_VER);
	info.sOdbcVer	= SQLGetInfoString(hdbc, SQL_ODBC_VER);
	info.sDrvOdbcVer= SQLGetInfoString(hdbc, SQL_DRIVER_ODBC_VER);
	info.iApiConf	= SQLGetInfoShort(hdbc, SQL_ODBC_API_CONFORMANCE);
	info.iSqlConf	= SQLGetInfoShort(hdbc, SQL_ODBC_SQL_CONFORMANCE);
	info.bSqlOptIef	= SQLGetInfoBool(hdbc, SQL_ODBC_SQL_OPT_IEF);
	info.iSagCliConf= SQLGetInfoShort(hdbc, SQL_ODBC_SAG_CLI_CONFORMANCE);
	info.iActConns	= SQLGetInfoShort(hdbc, SQL_ACTIVE_CONNECTIONS);
	info.iActStmts	= SQLGetInfoShort(hdbc, SQL_ACTIVE_STATEMENTS);
	info.iFileUsage	= SQLGetInfoShort(hdbc, SQL_FILE_USAGE);
	info.bMultSets	= SQLGetInfoBool(hdbc, SQL_MULT_RESULT_SETS);
	info.bReadOnly	= SQLGetInfoBool(hdbc, SQL_DATA_SOURCE_READ_ONLY);
	info.bAcsProc	= SQLGetInfoBool(hdbc, SQL_ACCESSIBLE_PROCEDURES);
	info.bAcsTabl	= SQLGetInfoBool(hdbc, SQL_ACCESSIBLE_TABLES);

	// Limits
	info.lMaxBinLitLen	= SQLGetInfoLong(hdbc, SQL_MAX_BINARY_LITERAL_LEN);
	info.lMaxChrLitLen	= SQLGetInfoLong(hdbc, SQL_MAX_CHAR_LITERAL_LEN);
	info.iMaxColNameLen	= SQLGetInfoShort(hdbc, SQL_MAX_COLUMN_NAME_LEN);
	info.iMaxColInGrpBy	= SQLGetInfoShort(hdbc, SQL_MAX_COLUMNS_IN_GROUP_BY);
	info.iMaxColInIndex	= SQLGetInfoShort(hdbc, SQL_MAX_COLUMNS_IN_INDEX);
	info.iMaxColInOrdBy	= SQLGetInfoShort(hdbc, SQL_MAX_COLUMNS_IN_ORDER_BY);
	info.iMaxColInSelect= SQLGetInfoShort(hdbc, SQL_MAX_COLUMNS_IN_SELECT);
	info.iMaxColInTable	= SQLGetInfoShort(hdbc, SQL_MAX_COLUMNS_IN_TABLE);
	info.iMaxCurNameLen	= SQLGetInfoShort(hdbc, SQL_MAX_CURSOR_NAME_LEN);
	info.lMaxIndexSize	= SQLGetInfoLong(hdbc, SQL_MAX_INDEX_SIZE);
	info.iMaxOwnNameLen	= SQLGetInfoShort(hdbc, SQL_MAX_OWNER_NAME_LEN);
	info.iMaxPrcNameLen	= SQLGetInfoShort(hdbc, SQL_MAX_PROCEDURE_NAME_LEN);
	info.iMaxQlfNameLen	= SQLGetInfoShort(hdbc, SQL_MAX_QUALIFIER_NAME_LEN);
	info.lMaxRowSize	= SQLGetInfoLong(hdbc, SQL_MAX_ROW_SIZE);
	info.bMaxRowSzIncL	= SQLGetInfoBool(hdbc, SQL_MAX_ROW_SIZE_INCLUDES_LONG);
	info.lMaxStmtLen	= SQLGetInfoLong(hdbc, SQL_MAX_STATEMENT_LEN);
	info.iMaxTabNameLen	= SQLGetInfoShort(hdbc, SQL_MAX_TABLE_NAME_LEN);
	info.iMaxTabInSelect= SQLGetInfoShort(hdbc, SQL_MAX_TABLES_IN_SELECT);
	info.iMaxUsrNameLen	= SQLGetInfoShort(hdbc, SQL_MAX_USER_NAME_LEN);

	// Supported SQL
	SDWORD lValue;
	lValue = SQLGetInfoLong(hdbc, SQL_ALTER_TABLE);
	if (lValue & SQL_AT_ADD_COLUMN)
		info.bAlterAddCol = TRUE;
	else
		info.bAlterAddCol = FALSE;
	if (lValue & SQL_AT_DROP_COLUMN)
		info.bAlterDropCol = TRUE;
	else
		info.bAlterDropCol = FALSE;

	// Can Describe Parameter?
	SQLUSMALLINT FuncExists;
	SQLGetFunctions(hdbc, SQL_API_SQLDESCRIBEPARAM, &FuncExists);
	if (FuncExists)
		info.bDescParam = TRUE;
	else
		info.bDescParam = FALSE;
	// Can Get Procedure Columns?
	SQLGetFunctions(hdbc, SQL_API_SQLPROCEDURECOLUMNS, &FuncExists);
	if (FuncExists)
		info.bProcColumns = TRUE;
	else
		info.bProcColumns = FALSE;

	info.bColAlias	= SQLGetInfoBool(hdbc, SQL_COLUMN_ALIAS);
	info.iCorrName	= SQLGetInfoShort(hdbc, SQL_CORRELATION_NAME);
	info.bExpInOrdBy= SQLGetInfoBool(hdbc, SQL_EXPRESSIONS_IN_ORDERBY);
	info.iGroupBy	= SQLGetInfoShort(hdbc, SQL_GROUP_BY);
	info.bLikeEsc	= SQLGetInfoBool(hdbc, SQL_LIKE_ESCAPE_CLAUSE);
	info.bOrdByInSel= SQLGetInfoBool(hdbc, SQL_ORDER_BY_COLUMNS_IN_SELECT);
	info.sOuterJoins= SQLGetInfoString(hdbc, SQL_OUTER_JOINS);

	lValue = SQLGetInfoLong(hdbc, SQL_POSITIONED_STATEMENTS);
	if (lValue & SQL_PS_POSITIONED_DELETE)
		info.bPsPosDelete = TRUE;
	else
		info.bPsPosDelete = FALSE;
	if (lValue & SQL_PS_POSITIONED_UPDATE)
		info.bPsPosUpdate = TRUE;
	else
		info.bPsPosUpdate = FALSE;
	if (lValue & SQL_PS_SELECT_FOR_UPDATE)
		info.bPsSelForUpdate = TRUE;
	else
		info.bPsSelForUpdate = FALSE;
	
	lValue = SQLGetInfoLong(hdbc, SQL_SUBQUERIES);
	if (lValue & SQL_SQ_CORRELATED_SUBQUERIES)
		info.bSqCorrSubQry = TRUE;
	else
		info.bSqCorrSubQry = FALSE;
	if (lValue & SQL_SQ_COMPARISON)
		info.bSqComparison = TRUE;
	else
		info.bSqComparison = FALSE;
	if (lValue & SQL_SQ_EXISTS)
		info.bSqExists = TRUE;
	else
		info.bSqExists = FALSE;
	if (lValue & SQL_SQ_IN)
		info.bSqIn = TRUE;
	else
		info.bSqIn = FALSE;
	if (lValue & SQL_SQ_QUANTIFIED)
		info.blSqQuantif = TRUE;
	else
		info.blSqQuantif = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_UNION);
	if (lValue & SQL_U_UNION)
		info.bUUnion = TRUE;
	else
		info.bUUnion = FALSE;
	if (lValue & SQL_U_UNION_ALL)
		info.bUUnionAll = TRUE;
	else
		info.bUUnionAll = FALSE;

	// Miscellaneous
	info.iTxnCapable	= SQLGetInfoShort(hdbc, SQL_TXN_CAPABLE);
	info.iCurComBhvr	= SQLGetInfoShort(hdbc, SQL_CURSOR_COMMIT_BEHAVIOR);
	info.iCurRlbBhvr	= SQLGetInfoShort(hdbc, SQL_CURSOR_ROLLBACK_BEHAVIOR);

	lValue = SQLGetInfoLong(hdbc, SQL_TXN_ISOLATION_OPTION);
	if (lValue & SQL_TXN_READ_COMMITTED)
		info.bTxnReadComm = TRUE;
	else
		info.bTxnReadComm = FALSE;
	if (lValue & SQL_TXN_READ_UNCOMMITTED)
		info.bTxnReadUncom = TRUE;
	else
		info.bTxnReadUncom = FALSE;
	if (lValue & SQL_TXN_REPEATABLE_READ)
		info.bTxnRepRead = TRUE;
	else
		info.bTxnRepRead = FALSE;
	if (lValue & SQL_TXN_SERIALIZABLE)
		info.bTxnSerial = TRUE;
	else
		info.bTxnSerial = FALSE;

	info.lDefTxnIsol	= SQLGetInfoLong(hdbc, SQL_DEFAULT_TXN_ISOLATION);
	info.bMultActTxn	= SQLGetInfoBool(hdbc, SQL_MULTIPLE_ACTIVE_TXN);

	lValue = SQLGetInfoLong(hdbc, SQL_BOOKMARK_PERSISTENCE);
	if (lValue & SQL_BP_CLOSE)
		info.bBpClose = TRUE;
	else
		info.bBpClose = FALSE;
	if (lValue & SQL_BP_DELETE)
		info.bBpDelete = TRUE;
	else
		info.bBpDelete = FALSE;
	if (lValue & SQL_BP_DROP)
		info.bBpDrop = TRUE;
	else
		info.bBpDrop = FALSE;
	if (lValue & SQL_BP_SCROLL)
		info.bBpScroll = TRUE;
	else
		info.bBpScroll = FALSE;
	if (lValue & SQL_BP_TRANSACTION)
		info.bBpTrans = TRUE;
	else
		info.bBpTrans = FALSE;
	if (lValue & SQL_BP_UPDATE)
		info.bBpUpdate = TRUE;
	else
		info.bBpUpdate = FALSE;
	if (lValue & SQL_BP_OTHER_HSTMT)
		info.bBpOtherHstmt = TRUE;
	else
		info.bBpOtherHstmt = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_LOCK_TYPES);
	if (lValue & SQL_LCK_NO_CHANGE)
		info.bLckNoChange = TRUE;
	else
		info.bLckNoChange = FALSE;
	if (lValue & SQL_LCK_EXCLUSIVE)
		info.bLckExclusive = TRUE;
	else
		info.bLckExclusive = FALSE;
	if (lValue & SQL_LCK_UNLOCK)
		info.bLckUnlock = TRUE;
	else
		info.bLckUnlock = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_STATIC_SENSITIVITY);
	if (lValue & SQL_SS_ADDITIONS)
		info.bSsAdditions = TRUE;
	else
		info.bSsAdditions = FALSE;
	if (lValue & SQL_SS_DELETIONS)
		info.bSsDeletions = TRUE;
	else
		info.bSsDeletions = FALSE;
	if (lValue & SQL_SS_UPDATES)
		info.bSsUpdates = TRUE;
	else
		info.bSsUpdates = FALSE;

	// Others
	info.bNeedLDataLen	= SQLGetInfoBool(hdbc, SQL_NEED_LONG_DATA_LEN);
	info.iNonNullCols	= SQLGetInfoShort(hdbc, SQL_NON_NULLABLE_COLUMNS);
	info.iConcNullBhvr	= SQLGetInfoShort(hdbc, SQL_CONCAT_NULL_BEHAVIOR);
	info.iNullCollat	= SQLGetInfoShort(hdbc, SQL_NULL_COLLATION);
	info.bProcedures	= SQLGetInfoBool(hdbc, SQL_PROCEDURES);
	info.bRowUpdates	= SQLGetInfoBool(hdbc, SQL_ROW_UPDATES);

	lValue = SQLGetInfoLong(hdbc, SQL_FETCH_DIRECTION);
	if (lValue & SQL_FD_FETCH_NEXT)
		info.bFdFetchNext = TRUE;
	else
		info.bFdFetchNext = FALSE;
	if (lValue & SQL_FD_FETCH_FIRST)
		info.bFdFetchFirst = TRUE;
	else
		info.bFdFetchFirst = FALSE;
	if (lValue & SQL_FD_FETCH_LAST)
		info.bFdFetchLast = TRUE;
	else
		info.bFdFetchLast = FALSE;
	if (lValue & SQL_FD_FETCH_PRIOR)
		info.bFdFetchPrior = TRUE;
	else
		info.bFdFetchPrior = FALSE;
	if (lValue & SQL_FD_FETCH_ABSOLUTE)
		info.bFdFetchAbs = TRUE;
	else
		info.bFdFetchAbs = FALSE;
	if (lValue & SQL_FD_FETCH_RELATIVE)
		info.bFdFetchRltv = TRUE;
	else
		info.bFdFetchRltv = FALSE;
	if (lValue & SQL_FD_FETCH_BOOKMARK)
		info.bFdFetchBmrk = TRUE;
	else
		info.bFdFetchBmrk = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_GETDATA_EXTENSIONS);
	if (lValue & SQL_GD_ANY_COLUMN)
		info.bGdAnyColumn = TRUE;
	else
		info.bGdAnyColumn = FALSE;
	if (lValue & SQL_GD_ANY_ORDER)
		info.bGdAnyOrder = TRUE;
	else
		info.bGdAnyOrder = FALSE;
	if (lValue & SQL_GD_BLOCK)
		info.bGdBlock = TRUE;
	else
		info.bGdBlock = FALSE;
	if (lValue & SQL_GD_BOUND)
		info.bGdBound = TRUE;
	else
		info.bGdBound = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_POS_OPERATIONS);
	if (lValue & SQL_POS_POSITION)
		info.bPosPosition = TRUE;
	else
		info.bPosPosition = FALSE;
	if (lValue & SQL_POS_REFRESH)
		info.bPosRefresh = TRUE;
	else
		info.bPosRefresh = FALSE;
	if (lValue & SQL_POS_UPDATE)
		info.bPosUpdate = TRUE;
	else
		info.bPosUpdate = FALSE;
	if (lValue & SQL_POS_DELETE)
		info.bPosDelete = TRUE;
	else
		info.bPosDelete = FALSE;
	if (lValue & SQL_POS_ADD)
		info.bPosAdd = TRUE;
	else
		info.bPosAdd = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_SCROLL_CONCURRENCY);
	if (lValue & SQL_SCCO_READ_ONLY)
		info.bSccoReadOnly = TRUE;
	else
		info.bSccoReadOnly = FALSE;
	if (lValue & SQL_SCCO_LOCK)
		info.bSccoLock = TRUE;
	else
		info.bSccoLock = FALSE;
	if (lValue & SQL_SCCO_OPT_ROWVER)
		info.bSccoOptRow = TRUE;
	else
		info.bSccoOptRow = FALSE;
	if (lValue & SQL_SCCO_OPT_VALUES)
		info.bSccoOptVal = TRUE;
	else
		info.bSccoOptVal = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_SCROLL_OPTIONS);
	if (lValue & SQL_SO_FORWARD_ONLY)
		info.bSoForwOnly = TRUE;
	else
		info.bSoForwOnly = FALSE;
	if (lValue & SQL_SO_STATIC)
		info.bSoStatic = TRUE;
	else
		info.bSoStatic = FALSE;
	if (lValue & SQL_SO_KEYSET_DRIVEN)
		info.bSoKeyDriven = TRUE;
	else
		info.bSoKeyDriven = FALSE;
	if (lValue & SQL_SO_DYNAMIC)
		info.bSoDynamic = TRUE;
	else
		info.bSoDynamic = FALSE;
	if (lValue & SQL_SO_MIXED)
		info.bSoMixed = TRUE;
	else
		info.bSoMixed = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_OWNER_USAGE);
	if (lValue & SQL_OU_DML_STATEMENTS)
		info.bOuDmlStmts = TRUE;
	else
		info.bOuDmlStmts = FALSE;
	if (lValue & SQL_OU_PROCEDURE_INVOCATION)
		info.bOuProcInv = TRUE;
	else
		info.bOuProcInv = FALSE;
	if (lValue & SQL_OU_TABLE_DEFINITION)
		info.bOuTableDef = TRUE;
	else
		info.bOuTableDef = FALSE;
	if (lValue & SQL_OU_INDEX_DEFINITION)
		info.bOuIndexDef = TRUE;
	else
		info.bOuIndexDef = FALSE;
	if (lValue & SQL_OU_PRIVILEGE_DEFINITION)
		info.bOuPrivDef = TRUE;
	else
		info.bOuPrivDef = FALSE;

	lValue = SQLGetInfoLong(hdbc, SQL_QUALIFIER_USAGE);
	if (lValue & SQL_QU_DML_STATEMENTS)
		info.bQuDmlStmts = TRUE;
	else
		info.bQuDmlStmts = FALSE;
	if (lValue & SQL_QU_PROCEDURE_INVOCATION)
		info.bQuProcInv = TRUE;
	else
		info.bQuProcInv = FALSE;
	if (lValue & SQL_QU_TABLE_DEFINITION)
		info.bQuTableDef = TRUE;
	else
		info.bQuTableDef = FALSE;
	if (lValue & SQL_QU_INDEX_DEFINITION)
		info.bQuIndexDef = TRUE;
	else
		info.bQuIndexDef = FALSE;
	if (lValue & SQL_QU_PRIVILEGE_DEFINITION)
		info.bQuPrivDef = TRUE;
	else
		info.bQuPrivDef = FALSE;

	// List of Data Types
	// Check for accessing SQLGetTypeInfo
	SQLRETURN	nRet;
	UWORD		bFuncSupported;
	nRet = SQLGetFunctions(hdbc, SQL_API_SQLGETTYPEINFO,
							&bFuncSupported);
	if (nRet != SQL_SUCCESS || !bFuncSupported)
		return;
	// Allocate HSTMT
	SQLHSTMT hstmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, 
		hdbc, &hstmt) != SQL_SUCCESS)
		return;
	// Call SQLGetTypeInfo
	if (::SQLGetTypeInfo(hstmt, SQL_ALL_TYPES) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		return;
	}
	SQLCHAR*	szTName;
	SQLCHAR*	szLTName;
	SQLCHAR*	szCrtPar;
	SQLCHAR*	szLitPfx;
	SQLCHAR*	szLitSfx;
	SQLINTEGER	lPrec;
	SQLSMALLINT	iType, fSearchable;
	SQLSMALLINT	iMinScale, iMaxScale;
	SQLSMALLINT	bNullable, bCaseSens, bUnsigned;
	SQLSMALLINT	bFxPrcScale, bAutoInc;
	SQLINTEGER	nmTName, nmLTName;
	SQLINTEGER	nmCrtPar, nmLitPfx, nmLitSfx;
	SQLINTEGER	nmPrec, nmType, nmSearchable;
	SQLINTEGER	nmMinScale, nmMaxScale;
	SQLINTEGER	nmNullable, nmCaseSens;
	SQLINTEGER	nmUnsigned;
	SQLINTEGER	nmFxPrcScale, nmAutoInc;
	// Allocate memory
	nmTName  = 100;
	nmLTName = 100;
	nmCrtPar = 100;
	nmLitPfx = 10;
	nmLitSfx = 10;
	szTName  = (SQLCHAR *)malloc((nmTName+1) *sizeof(SQLCHAR));
	szLTName = (SQLCHAR *)malloc((nmLTName+1)*sizeof(SQLCHAR));
	szCrtPar = (SQLCHAR *)malloc((nmCrtPar+1)*sizeof(SQLCHAR));
	szLitPfx = (SQLCHAR *)malloc((nmLitPfx+1)*sizeof(SQLCHAR));
	szLitSfx = (SQLCHAR *)malloc((nmLitSfx+1)*sizeof(SQLCHAR));
	// Binding Columns
	SQLBindCol(hstmt, 1,  SQL_C_CHAR,   szTName, nmTName, &nmTName);
	SQLBindCol(hstmt, 2,  SQL_C_SSHORT, &iType, 0, &nmType);
	SQLBindCol(hstmt, 3,  SQL_C_SLONG,  &lPrec, 0, &nmPrec);
	SQLBindCol(hstmt, 4,  SQL_C_CHAR,   szLitPfx, nmLitPfx, &nmLitPfx);
	SQLBindCol(hstmt, 5,  SQL_C_CHAR,   szLitSfx, nmLitSfx, &nmLitSfx);
	SQLBindCol(hstmt, 6,  SQL_C_CHAR,   szCrtPar, nmCrtPar, &nmCrtPar);
	SQLBindCol(hstmt, 7,  SQL_C_SSHORT, &bNullable, 0, &nmNullable);
	SQLBindCol(hstmt, 8,  SQL_C_SSHORT, &bCaseSens, 0, &nmCaseSens);
	SQLBindCol(hstmt, 9,  SQL_C_SSHORT, &fSearchable, 0, &nmSearchable);
	SQLBindCol(hstmt, 10, SQL_C_SSHORT, &bUnsigned, 0, &nmUnsigned);
	SQLBindCol(hstmt, 11, SQL_C_SSHORT, &bFxPrcScale, 0, &nmFxPrcScale);
	SQLBindCol(hstmt, 12, SQL_C_SSHORT, &bAutoInc, 0, &nmAutoInc);
	SQLBindCol(hstmt, 13, SQL_C_CHAR,   szLTName, nmLTName, &nmLTName);
	SQLBindCol(hstmt, 14, SQL_C_SSHORT, &iMinScale, 0, &nmMinScale);
	SQLBindCol(hstmt, 15, SQL_C_SSHORT, &iMaxScale, 0, &nmMaxScale);
	int i = 0;
	while (SQLFetch(hstmt) == SQL_SUCCESS)
	{
		info.arType.SetSize(i + 1);
		info.arType[i].sTypeName = szTName;
		info.arType[i].sLTypeName = szLTName;
		info.arType[i].sLitPfx = szLitPfx;
		info.arType[i].sLitSfx = szLitSfx;
		info.arType[i].sCrtParams = szCrtPar;
		info.arType[i].iType = iType;
		info.arType[i].lPrec = lPrec;
		info.arType[i].iMinScale = iMinScale;
		info.arType[i].iMaxScale = iMaxScale;
		info.arType[i].fSearchable = fSearchable;

		if (bNullable == SQL_NULLABLE)
			info.arType[i].bNullable = TRUE;
		else
			info.arType[i].bNullable = FALSE;
		if (bCaseSens == SQL_TRUE)
			info.arType[i].bCaseSens = TRUE;
		else
			info.arType[i].bCaseSens = FALSE;
		if (bUnsigned == SQL_TRUE)
			info.arType[i].bUnsigned = TRUE;
		else
			info.arType[i].bUnsigned = FALSE;
		if (bFxPrcScale == SQL_TRUE)
			info.arType[i].bFxPrcScale = TRUE;
		else
			info.arType[i].bFxPrcScale = FALSE;
		if (bAutoInc == SQL_TRUE)
			info.arType[i].bAutoInc = TRUE;
		else
			info.arType[i].bAutoInc = FALSE;
		i++;
	}
	// Release HSTMT
	SQLFreeStmt(hstmt, SQL_UNBIND);
	// Release memory
	free(szTName);
	free(szLTName);
	free(szCrtPar);
	free(szLitPfx);
	free(szLitSfx);
}

/////////////////////////////////////////////////////////////////////
// Чтение значения ключа из реестра

BOOL QueryRegSzValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, CString &sData, LPCTSTR szDefault)
{
	LONG	retCode;
	HKEY	hSubkey;
	DWORD	dwType;
	DWORD	dwSize = 256;
	LPTSTR	szData = new char[256];

	if ((retCode = RegOpenKeyEx(hKey, szSubkey, NULL, KEY_READ, &hSubkey)) != ERROR_SUCCESS)
		goto err;

	if ((retCode = RegQueryValueEx(hSubkey, szValue, NULL, &dwType, (LPBYTE)szData, &dwSize)) != ERROR_SUCCESS)
	{
		RegCloseKey(hSubkey);
		goto err;
	}

	RegCloseKey(hSubkey);
	sData = CString(szData);
	delete[] szData;
	return TRUE;

err:
	if (szDefault != NULL)
		sData = CString(szDefault);
#ifdef _DEBUG
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, retCode, 0, szData, 256, NULL);
	CString sErr(szData);
	sErr.Insert(0,":\n");
	sErr.Insert(0, szValue);
	sErr.Insert(0, _T("\\"));
	sErr.Insert(0, szSubkey);
	AfxMessageBox(sErr);
#endif
	delete[] szData;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Чтение значения ключа из реестра

BOOL QueryRegDwValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, UINT &nData, UINT nDefault)
{
	LONG	retCode;
	HKEY	hSubkey;
	DWORD	dwType;
	DWORD	dwSize = sizeof(DWORD);
	DWORD	dwData;

	if ((retCode = RegOpenKeyEx(hKey, szSubkey, NULL, KEY_READ, &hSubkey)) != ERROR_SUCCESS)
		goto err;

	if ((retCode = RegQueryValueEx(hSubkey, szValue, NULL, &dwType, (LPBYTE)&dwData, &dwSize)) != ERROR_SUCCESS)
	{
		RegCloseKey(hSubkey);
		goto err;
	}

	RegCloseKey(hSubkey);
	if (dwType != REG_DWORD)
		return FALSE;
	else
		nData = (UINT)dwData;

	return TRUE;

err:
	nData = nDefault;
#ifdef _DEBUG
	LPTSTR	szError = new char[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, retCode, 0, szError, 256, NULL);
	CString sErr(szError);
	sErr.Insert(0,":\n");
	sErr.Insert(0, szValue);
	sErr.Insert(0, _T("\\"));
	sErr.Insert(0, szSubkey);
	AfxMessageBox(sErr);
	delete[] szError;
#endif
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Запись строкового значения ключа в реестр

BOOL WriteRegSzValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, LPCTSTR szData)
{
	LONG	retCode;
	HKEY	hSubkey;
	DWORD	dwDisp;

	if ((retCode = RegOpenKeyEx(hKey, szSubkey, NULL, KEY_WRITE, &hSubkey)) != ERROR_SUCCESS)
	{
		if ((retCode = RegCreateKeyEx(hKey, szSubkey, 0, NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hSubkey, &dwDisp)) != ERROR_SUCCESS)
			goto err;
	}

	if ((retCode = RegSetValueEx(hSubkey, szValue, NULL, REG_SZ, (LPBYTE)szData, strlen(szData))) != ERROR_SUCCESS)
	{
		RegCloseKey(hSubkey);
		goto err;
	}

	RegCloseKey(hSubkey);
	return TRUE;

err:
#ifdef _DEBUG
	LPTSTR	szError = new char[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, retCode, 0, szError, 256, NULL);
	CString sErr(szError);
	sErr.Insert(0,":\n");
	sErr.Insert(0, szValue);
	sErr.Insert(0, _T("\\"));
	sErr.Insert(0, szSubkey);
	AfxMessageBox(sErr);
	delete[] szError;
#endif
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Запись числового значения ключа в реестр

BOOL WriteRegDwValue(HKEY hKey, LPCTSTR szSubkey, LPCTSTR szValue, UINT nData)
{
	LONG	retCode;
	HKEY	hSubkey;
	DWORD	dwData = (DWORD)nData;
	DWORD	dwDisp;

	if (RegOpenKeyEx(hKey, szSubkey, NULL, KEY_WRITE, &hSubkey) != ERROR_SUCCESS)
	{
		if ((retCode = RegCreateKeyEx(hKey, szSubkey, 0, NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hSubkey, &dwDisp)) != ERROR_SUCCESS)
			goto err;
	}

	if ((retCode = RegSetValueEx(hSubkey, szValue, NULL, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData))) != ERROR_SUCCESS)
	{
		RegCloseKey(hSubkey);
		goto err;
	}

	RegCloseKey(hSubkey);
	return TRUE;

err:
#ifdef _DEBUG
	LPTSTR	szError = new char[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, retCode, 0, szError, 256, NULL);
	CString sErr(szError);
	sErr.Insert(0,":\n");
	sErr.Insert(0, szValue);
	sErr.Insert(0, _T("\\"));
	sErr.Insert(0, szSubkey);
	AfxMessageBox(sErr);
	delete[] szError;
#endif
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
//
// Функции-помощники по защите программы
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Получение времени создания файла

BOOL _getFileCreateDT(LPCTSTR lpFileName, FILETIME &time)
{
	WIN32_FILE_ATTRIBUTE_DATA fad;

	if (GetFileAttributesEx(lpFileName, GetFileExInfoStandard, &fad) == 0)
		goto err;

	time.dwLowDateTime	= fad.ftCreationTime.dwLowDateTime;
	time.dwHighDateTime	= fad.ftCreationTime.dwLowDateTime;

	return TRUE;

err:
#ifdef _DEBUG
	LONG	retCode = GetLastError();
	LPTSTR	szError = new char[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, retCode, 0, szError, 256, NULL);
	AfxMessageBox(szError);
	delete[] szError;
#endif
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Получение последнего времени доступа к файлу

BOOL _getFileAccessDT(LPCTSTR lpFileName, FILETIME &time)
{
	WIN32_FILE_ATTRIBUTE_DATA fad;

	if (GetFileAttributesEx(lpFileName, GetFileExInfoStandard, &fad) == 0)
		goto err;

	time.dwLowDateTime	= fad.ftLastAccessTime.dwLowDateTime;
	time.dwHighDateTime	= fad.ftLastAccessTime.dwHighDateTime;

	return TRUE;

err:
#ifdef _DEBUG
	LONG	retCode = GetLastError();
	LPTSTR	szError = new char[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, retCode, 0, szError, 256, NULL);
	AfxMessageBox(szError);
	delete[] szError;
#endif
	return FALSE;
}

/////////////////////////////////////////////////////////////////////
// Получение текущего системного времени "неявно" по файлу реестра

BOOL _getCurrentSysDT(FILETIME &time)
{
	char	szWdir[MAX_PATH];
	CString	sFilePath;
	FILETIME ft;
	SYSTEMTIME sst,sft;

	// Получить системное время
	GetSystemTime(&sst);
	SystemTimeToFileTime(&sst, &time);

	// Получить время по дате обращения к системному файлу
	GetWindowsDirectory(szWdir,MAX_PATH);
	sFilePath.LoadString(IDS_SYS_REG_FILE);
	sFilePath.Insert(0, szWdir);
	_getFileAccessDT(sFilePath, ft);
	FileTimeToSystemTime(&ft, &sft);
	// Проверка на возможный перевод времени
	if (sft.wDay != sst.wDay) return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// CTrans30App initialization

BOOL CTrans30App::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.

	CString sKey,sDat,sErr;

	// Параметры командной строки
	CString s = CString(m_lpCmdLine);
	// Установка глобальных переменных
	hwndPopup = NULL;
	hLangDLL  = NULL;
	bFileLog  = FALSE;
	bFileMsg  = FALSE;
	bExchange = FALSE;
	bImport   = FALSE;
	bExport   = FALSE;
	bCancel   = FALSE;
	// Установка текущего директория
	char sBuffer[513];
	GetCurrentDirectory(512, sBuffer);
	TrnParm.sDirApp = CString(sBuffer);
	TrnParm.sDirApp+= "\\";

	if (IsDebuggerPresent())
		return FALSE;

	/////////////////////////////////////////////
	// Инициализация установок системы
	// Проверка метки инсталляции
	/////////////////////////////////////////////
	if (!InitOptions())
	{
		sErr.LoadString(IDS_ER_INCOR_INSTALL);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(sErr);
		return FALSE;
	}
	// Если установлен признак удаления
	if (TrnParm.iDelFile)
	{
		// Удаление файла протокола
		DeleteFile(TrnParm.sDirLog + TrnParm.sFileLog);
	}
	/////////////////////////////////////////////
	// Обработка параметров командной строки
	/////////////////////////////////////////////
	// Параметр - минимизировать при запуске
	if (s.Find("-m") >= 0) TrnParm.iMinimize = 1;
	// Параметр - минимизировать в SysTray
	if (s.Find("-t") >= 0) TrnParm.iSysTray = 1;
	// Параметр - отсутствие сообщений
	if (s.Find("-v") >= 0) TrnParm.iShowErr = 0;

	// Проверка метки инсталляции в реестре
	sKey.LoadString(IDS_SYS_REG_INSTDATA);
	if (!QueryRegSzValue(HKEY_LOCAL_MACHINE, sKey, _T("tappinstdata"), sDat, _T("")))
	{
		sErr.LoadString(IDS_ER_INCOR_INSTALL);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(sErr);
		return FALSE;
	}

	/////////////////////////////////////////////
	// Установка идентификатора ресурсов
	/////////////////////////////////////////////
	if (TrnParm.sLangID.CompareNoCase(_T("ENU")) == 0)
	{
		// Установка английской версии ресурсов
		if ((hLangDLL = AfxLoadLibrary("Trn33enu.dll")) == NULL)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			AfxMessageBox(CString(GetErrMsg(ER_OPEN_FILE, _T("Trn33enu.dll"))));
			// Установка основного языка
			TrnParm.sLangID = _T("RUS");
		}
		else
		{
			AfxSetResourceHandle(hLangDLL);
		}
	}
	else
	{
		// Использование текущих ресурсов
		hLangDLL = NULL;
	}

#ifndef _TRIAL_VER
	/////////////////////////////////////////////
	// Чтение файла лицензии
	/////////////////////////////////////////////
	CStdioFile	file;
	CString sLicFile;

	sLicFile.LoadString(IDS_LICENSE_FILE);

	if (!FileGet(sLicFile, sLicense))
	{
		sErr = GetErrMsg(ER_READ_FILE, sLicFile);
		MessageBeep(MB_ICONEXCLAMATION);
		AfxMessageBox(sErr);
		return FALSE;
	}
#endif

	// Установка названия приложения
	TrnParm.sNameApp.LoadString(IDR_MAINFRAME);
	// Создание основного MDI окна
	CMainFrame* pMainWnd = new CMainFrame;
	if (!pMainWnd->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// Назначение главного окна приложения
	m_pMainWnd = (CMDIFrameWnd*)pMainWnd;
	// Установка позиции и размеров окна
	if (TrnParm.iSavePos)
	{
		int x,y,w,h;
		// Получение координат и размеров окна из реестра
		sKey.LoadString(IDS_SYS_REG_SET);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameX"), (UINT&)x, 100);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameY"), (UINT&)y, 100);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameW"), (UINT&)w, 600);
		QueryRegDwValue(HKEY_LOCAL_MACHINE, sKey, _T("FrameH"), (UINT&)h, 400);
		pMainWnd->MoveWindow(x, y, w, h);
	}
	// Создание шаблона документа
	AddDocTemplate(
		new CMultiDocTemplate(IDR_TEXTTYPE,
		RUNTIME_CLASS(CPadDoc),
		RUNTIME_CLASS(CPadFrame),
		RUNTIME_CLASS(CPadView)));

	// Отображение главного окна программы
	if (TrnParm.iMinimize > 0)
	{
		pMainWnd->ShowWindow(SW_SHOWMINIMIZED);
	}
	else
	{
		pMainWnd->ShowWindow(SW_SHOWNORMAL);
		pMainWnd->UpdateWindow();
	}
	// Параметр - файл трансферта
	if (s.Find("-f") >= 0)
	{
		int	i;
		CString	sFilePpl, sParmPpl;
		CString	tmp = s.Mid(s.Find("-f") + 3);
		tmp.TrimLeft();
		if ((i = tmp.FindOneOf(" \t")) < 0)
			 i = tmp.GetLength();
		sFilePpl = tmp.Mid(0, i);
		if (sFilePpl.IsEmpty())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			AfxMessageBox(IDS_ER_NO_PPL, 
				MB_OK|MB_ICONEXCLAMATION);
		}
		// Параметр - список параметров
		if (s.Find("-p") >= 0)
		{
			int n, m;
			n = s.Find("-p");
			n = s.Find("[", n + 3);
			m = s.Find("]", n + 1);
			sParmPpl = s.Mid(n+1, m - n - 1);
		}
		// Запуск трансферта на выполнение
		RunPipe(sFilePpl, "Transfer", 0, FALSE, sParmPpl);

		return FALSE;
	}
	// Автозапуск обмена данными
	else if (TrnParm.iAutoExc)
	{
	if (bImport || bExport) return FALSE;

#ifdef _TRIAL_VER
	// Ограничение по схеме в Trial версии
	sErr.FormatMessage(IDS_ER_TRIAL_EXCHANGE);
	AfxMessageBox(sErr);
	return FALSE;
#else
		StartExchange();
#endif
	}
	// Показ диалога "О программе"
	else if (TrnParm.iSplashW)
	{
		pMainWnd->ShowAboutDlg();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// CTrans30App message handlers

int CTrans30App::ExitInstance() 
{
	if (menuMDI.m_hMenu != NULL)
	{
		menuMDI.DestroyMenu();
	}
	if (hLangDLL) AfxFreeLibrary(hLangDLL);

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////
// CPadDoc

IMPLEMENT_DYNCREATE(CPadDoc, CRichEditDoc)

CPadDoc::CPadDoc()
{
	m_bRTF = FALSE;
}

CPadDoc::~CPadDoc()
{
}

BEGIN_MESSAGE_MAP(CPadDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CPadDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPadDoc Operations

CRichEditCntrItem* CPadDoc::CreateClientItem(REOBJECT* preo) const
{
	return new CRichEditCntrItem(preo, (CPadDoc*)this);
}

/////////////////////////////////////////////////////////////////////
// CPadDoc serialization

void CPadDoc::Serialize(CArchive& ar)
{
	CRichEditDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////
// CPadView

IMPLEMENT_DYNCREATE(CPadView, CRichEditView)

CPadView::CPadView()
{
	// Определение системы единиц:
	// 0 - метрическая, 1 - U.S.
	TCHAR buf[2] = _T("\0");
	GetLocaleInfo(GetUserDefaultLCID(), LOCALE_IMEASURE, buf, 2);
	m_nUnits = (buf[0] == '0') ? 0 : 1;
}

CPadView::~CPadView()
{
}

BEGIN_MESSAGE_MAP(CPadView, CRichEditView)
	//{{AFX_MSG_MAP(CPadView)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnPageSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPadView Operations

void CPadView::RoundRect(LPRECT r1)
{
	r1->left = roundleast(r1->left);
	r1->right = roundleast(r1->right);
	r1->top = roundleast(r1->top);
	r1->bottom = roundleast(r1->bottom);
}

void CPadView::MulDivRect(LPRECT r1, LPRECT r2, int num, int div)
{
	r1->left = MulDiv(r2->left, num, div);
	r1->top = MulDiv(r2->top, num, div);
	r1->right = MulDiv(r2->right, num, div);
	r1->bottom = MulDiv(r2->bottom, num, div);
}

BOOL CPadView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
}

void CPadView::OnFilePrint() 
{
	CRichEditView::OnFilePrint();
}

void CPadView::DeleteContents()
{
	CRichEditView::DeleteContents();
	SetDefaultFont();
	SetDefaultMargins();
}

BOOL CPadView::SetDefaultFont()
{
	// Установка шрифта Arial
	CHARFORMAT cf;
	TCHAR* szFontName = _T("Arial");
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_STRIKEOUT|CFM_SIZE|
				CFM_CHARSET|CFM_COLOR|CFM_OFFSET|CFM_PROTECTED|CFM_FACE;
	cf.dwEffects = CFE_AUTOCOLOR;
	cf.yHeight = 200; //10pt
	cf.yOffset = 0;
	cf.crTextColor = RGB(0, 0, 0);
	cf.bCharSet = RUSSIAN_CHARSET;
	cf.bPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	lstrcpyn(cf.szFaceName, T2A(szFontName), LF_FACESIZE);
	return GetRichEditCtrl().SetDefaultCharFormat(cf);
}

BOOL CPadView::SetDefaultMargins()
{
	// Установка отступов страницы при печати
	m_rectMargin.SetRect(1140, 1440, 1140, 1440);
	SetMargins(m_rectMargin);
	return TRUE;
}

void CPadView::OnPageSetup() 
{
	CPageSetupDialog dlg;
	PAGESETUPDLG& psd = dlg.m_psd;
	BOOL bMetric = (m_nUnits == 0); //centimeters
	int nUnitsPerInch = bMetric ? 2540 : 1000;
	psd.Flags |= PSD_MARGINS | (bMetric ? PSD_INHUNDREDTHSOFMILLIMETERS :
		PSD_INTHOUSANDTHSOFINCHES);

	MulDivRect(&psd.rtMargin, m_rectMargin, nUnitsPerInch, 1440);
	RoundRect(&psd.rtMargin);
	PRINTDLG pd;
	pd.hDevNames = NULL;
	pd.hDevMode = NULL;
	AfxGetApp()->GetPrinterDeviceDefaults(&pd);
	psd.hDevNames = pd.hDevNames;
	psd.hDevMode = pd.hDevMode;
	if (dlg.DoModal() == IDOK)
	{
		RoundRect(&psd.rtMargin);
		MulDivRect(m_rectMargin, &psd.rtMargin, 1440, nUnitsPerInch);
		SetMargins(m_rectMargin);
		AfxGetApp()->SelectPrinter(psd.hDevNames, psd.hDevMode);
	}
}

/////////////////////////////////////////////////////////////////////
// CPadFrame

IMPLEMENT_DYNCREATE(CPadFrame, CMDIChildWnd)

CPadFrame::CPadFrame()
{
}

CPadFrame::~CPadFrame()
{
}

BEGIN_MESSAGE_MAP(CPadFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPadFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_DATA_CLOSE, OnDataClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Меню для всех MDI окон
CMenu CPadFrame::menu;

/////////////////////////////////////////////////////////////////////
// CPadFrame message handlers


int CPadFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Посылка сообщения главному окну
	// для обновления ToolBar'а
	GetMDIFrame()->SendMessage(MDI_UPDATE_BAR, 1);
	// Распахнуть на весь экран?
	if (TrnParm.iMaxiMDI)
		MDIMaximize();
	return 0;
}

void CPadFrame::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	// Посылка сообщения главному окну
	// для обновления ToolBar'а
	GetMDIFrame()->SendMessage(MDI_UPDATE_BAR, 0);
}

void CPadFrame::OnDataClose() 
{
	MDIDestroy();
}

BOOL CPadFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (menu.m_hMenu == NULL)
		menu.LoadMenu(IDR_CHILDWND);
	m_hMenuShared = menu.m_hMenu;

	return CMDIChildWnd::PreCreateWindow(cs);
}

void CTrans30App::OnFileNew() 
{
	// Открытие нового документа
	CWinApp::OnFileNew();
	// Установка текущего директория
	::SetCurrentDirectory(TrnParm.sDirApp);
}

void CTrans30App::OnFileOpen() 
{
	// Открытие документа
	CWinApp::OnFileOpen();
	// Установка текущего директория
	::SetCurrentDirectory(TrnParm.sDirApp);
}
