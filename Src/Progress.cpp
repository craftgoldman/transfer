// Progress.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "Progress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	m_sText = _T("");
	m_sStatus = _T("");
	m_sSubText = _T("");
	//}}AFX_DATA_INIT
	m_bBreaked = FALSE;
	m_bMinimized = FALSE;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_cProgress);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	DDX_Text(pDX, IDC_STATUS, m_sStatus);
	DDX_Control(pDX, IDC_BREAK, m_bBREAK);
	DDX_Text(pDX, IDC_SUBTEXT, m_sSubText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BREAK, OnBreak)
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

BOOL CProgressDlg::DestroyWindow() 
{
	if (m_bMinimized && m_hwndMain != NULL)
	{
		m_bMinimized = FALSE;
		// �������������� ��������� ��������� ����
		::SetWindowText(m_hwndMain, m_sAppTitle);
	}
	return CDialog::DestroyWindow();
}

void CProgressDlg::OnClose() 
{
	m_bBreaked = TRUE;
	CDialog::OnClose();
}

void CProgressDlg::OnBreak() 
{
	CString	strTitle;
	GetWindowText(strTitle);
	if (MessageBox( _T("�������� �������?"),
		strTitle, MB_YESNO | MB_ICONQUESTION ) == IDYES)
	{
		m_bBreaked = TRUE;
		m_bBREAK.ModifyStyle(NULL, WS_DISABLED);
		RedrawWindow();
	}
	else if (m_hwndMain != NULL)
		::UpdateWindow(m_hwndMain);
}

void CProgressDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	// ����������� ���� ��������� ����
	if (m_hwndMain != NULL)
		::UpdateWindow(m_hwndMain);
}

void CProgressDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED && m_hwndMain != NULL)
	{
		// ���������� ������
		hwndPopup = m_hWnd;
		// ������� �����������
		m_bMinimized = TRUE;
		// ����������� ��������� ����
		::ShowWindow(m_hwndMain, SW_MINIMIZE);
		SetPctTitle(m_hwndMain);
	}
}

void CProgressDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow && m_bMinimized && m_hwndMain != NULL)
	{
		m_bMinimized = FALSE;
		// �������������� ��������� ��������� ����
		::SetWindowText(m_hwndMain, m_sAppTitle);
		::UpdateWindow(m_hwndMain);
		// �������������� �������� ����
		if (IsIconic()) ShowWindow(SW_RESTORE);
		// ��������� ������ � ������� ����
		PostMessage(WM_SETFOCUS, 0, 0);
	}
}

int CProgressDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (lpCreateStruct->hwndParent != NULL)
	{
		// ������ �� ����-��������
		m_hwndMain = lpCreateStruct->hwndParent;
		::GetWindowText(m_hwndMain, 
		m_sAppTitle.GetBufferSetLength(255), 255);
		m_sAppTitle.ReleaseBuffer();
	}
	else
	{
		m_hwndMain = NULL;
		m_sAppTitle = _T("");
	}
	return 0;
}

BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_hwndMain == NULL)
	{
		// ����������� ���� ����������
		ModifyStyle(WS_MINIMIZEBOX, 0);
		ShowWindow(SW_SHOW);
	}
	// ���� ����-�������� ��������������
	else if (::IsIconic(m_hwndMain))
	{
		// ���������� ������
		hwndPopup = m_hWnd;
		// ������� �����������
		m_bMinimized = TRUE;
	}
	else
	{
		ShowWindow(SW_SHOW);
	}
	return TRUE;
}

void CProgressDlg::SetBreakEnable(BOOL bEnable)
{
	if (bEnable)
		m_bBREAK.ModifyStyle(WS_DISABLED, NULL);
	else
		m_bBREAK.ModifyStyle(NULL, WS_DISABLED);
	m_bBREAK.SetFocus();
	RedrawWindow();
}

void CProgressDlg::CheckState()
{
	MSG  msg;
	HWND hwnd;

	if (m_bMinimized && m_hwndMain != NULL)
		hwnd = m_hwndMain;	// �������� ����
	else
		hwnd = m_hWnd;		// ������ ����

	// ��������� ������ ��������� ����
	if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CProgressDlg::SetStatusText(LPCTSTR szText) 
{
	m_sStatus = CString(szText);
	UpdateData(FALSE);
}

void CProgressDlg::SetMessageText(LPCTSTR szText) 
{
	m_sText = CString(szText);
	UpdateData(FALSE);
}

void CProgressDlg::SetSubText(LPCTSTR szText) 
{
	m_sSubText = CString(szText);
	UpdateData(FALSE);
}

void CProgressDlg::SetRange(int nLower, int nUpper) 
{
	m_cProgress.SetRange32(nLower, nUpper);
}

void CProgressDlg::SetPos(int nPos) 
{
	// ��������� �������
	m_cProgress.SetPos(nPos);
	// ��������� ������ � �������� ����
	if (m_bMinimized && m_hwndMain != NULL)
		SetPctTitle(m_hwndMain);
}

BOOL CProgressDlg::IsBreaked() 
{
	return m_bBreaked;
}

void CProgressDlg::SetPctTitle(HWND hwnd)
{
	// �������������� ������
	int nMinRange, nMaxRange;
	m_cProgress.GetRange(nMinRange, nMaxRange);
	int nOffset = m_cProgress.GetPos() - nMinRange;
	int nRange = nMaxRange - nMinRange;
	CString strTitle;
	CString strText;
	GetWindowText(strTitle);
	strText.Format("%i %%", (nOffset*100)/nRange);
	strText += _T(" - ") + strTitle;
	// ��������� % ���������� � ��������� ����
	::SetWindowText(hwnd, strText);
}
