// MsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "trans30.h"
#include "MsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDlg)
	m_sText = _T("");
	//}}AFX_DATA_INIT
	m_bBreaked = FALSE;
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	DDX_Control(pDX, IDC_BREAK, m_bBREAK);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BREAK, OnBreak)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

void CMessageDlg::OnClose() 
{
	m_bBreaked = TRUE;

	CDialog::OnClose();
}

void CMessageDlg::OnBreak() 
{
	CString	strCap,strMsg;
	GetWindowText(strCap);
	strMsg.LoadString(IDS_MS_BREAK);
	if (MessageBox(strMsg,strCap,MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		m_bBREAK.ModifyStyle(NULL, WS_DISABLED);
		RedrawWindow();
		m_bBreaked = TRUE;
	}
	else
		AfxGetApp()->m_pMainWnd->UpdateWindow();
}

void CMessageDlg::SetBreakEnable(BOOL bEnable)
{
	if (bEnable)
		m_bBREAK.ModifyStyle(WS_DISABLED, NULL);
	else
		m_bBREAK.ModifyStyle(NULL, WS_DISABLED);
	m_bBREAK.SetFocus();
	RedrawWindow();
}

void CMessageDlg::CheckState()
{
	MSG	msg;
	if (PeekMessage(&msg,m_hWnd,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
