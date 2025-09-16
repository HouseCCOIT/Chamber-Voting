// CancelDlg.cpp : implementation file
//

#include "Digital Bank.h"
#include "CancelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCancelDlg dialog


CCancelDlg::CCancelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCancelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCancelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_bCancel = FALSE;
}


void CCancelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCancelDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCancelDlg, CDialog)
	//{{AFX_MSG_MAP(CCancelDlg)
	ON_BN_CLICKED(IDCANCEL, OnCancelBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCancelDlg message handlers

void CCancelDlg::OnCancelBtn() 
{
	m_bCancel = TRUE;
}
