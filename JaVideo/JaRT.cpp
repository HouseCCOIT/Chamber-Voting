// JaRT.cpp : implementation file
//

#include "stdafx.h"
//#include "JaVM.h"
#include "JaRT.h"
#include "house.h"
#include "VM_Sock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WS_EX_LAYERED           0x00080000

#define ULW_ALPHA               0x00000002

CString m_RTFontName = CString("Arial Bold");
CString m_RTFontThinName = CString("Calibri Bold");
int     m_RTFontSize = 220; // 110;
//extern CString m_RTFontName;
//extern int m_RTFontSize;
extern LEX m_JA_MemberSeatingList[MAX_SEATS];

/////////////////////////////////////////////////////////////////////////////
// JaRT dialog


JaRT::JaRT(CWnd* pParent /*=NULL*/)
   : CDialog(JaRT::IDD, pParent)
{
   //{{AFX_DATA_INIT(JaRT)
   //}}AFX_DATA_INIT
}


void JaRT::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(JaRT)
   //}}AFX_DATA_MAP

   DDX_Control(pDX, (IDC_STATIC_BORDERTOP), m_BorderTop);
   DDX_Control(pDX, (IDC_STATIC_BORDERBOTTOM), m_BorderBottom);

   for (int i = 0; i < 144; i++) {
      DDX_Control(pDX, (IDC_STATIC_YES1 + i), m_MemberYes[i]);
      DDX_Control(pDX, (IDC_STATIC_NO1 + i), m_MemberNo[i]);
      DDX_Control(pDX, (IDC_STATIC_NAME1 + i), m_MemberName[i]);
   }
   for (int i = 0; i < 5; i++) {
      DDX_Control(pDX, (IDC_STATIC_G1 + i), m_MemberGroupBox[i]);
   }
}


BEGIN_MESSAGE_MAP(JaRT, CDialog)
   //{{AFX_MSG_MAP(JaRT)
   ON_WM_TIMER()
   ON_WM_CTLCOLOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define BORDER_X 5
#define BORDER_Y 5
#define MAX_SCREEN_X 1920 // 1280 //640
#define MAX_SCREEN_Y 1080 // 720  //480
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 1920 // 1280 //640
#define MAX_Y 1080 // 720  //480
#define MAX_ROWS 27
#define MAX_COLUMNS 5
#define MAX_FIELD_X (((MAX_X - MIN_X) - (BORDER_X * 2)) / MAX_COLUMNS)
#define MAX_FIELD_Y (((MAX_Y - MIN_Y) - (BORDER_Y * 2)) / MAX_ROWS)
#define MAX_RESULT_X (MAX_X / 4)

/////////////////////////////////////////////////////////////////////////////
// JaRT message handlers

BOOL JaRT::OnInitDialog()
{
   CDialog::OnInitDialog();

   SetWindowPos(&CWnd::wndBottom, ::GetSystemMetrics(SM_CXSCREEN), 0, MAX_SCREEN_X, MAX_SCREEN_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
                                                                                                      // SetWindowPos(&CWnd::wndTop, 0, 0, 640, 480, 0); //SWP_NOMOVE | SWP_NOSIZE);

   MoveWindow(::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y);
   //MoveWindow(0, 0, MAX_SCREEN_X, MAX_SCREEN_Y);  // For debugging only

   // Try to get the address of the SetLayeredWindowAttributes function. It may not be available.
   HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));

   m_pSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, "SetLayeredWindowAttributes");

   COLORREF TColor = RGB(255, 255, 255);
   COLORREF BColor = RGB(0, 0, 0);   // RGB(10, 10, 10);
   COLORREF YColor = RGB(0, 255, 0);
   COLORREF NColor = RGB(255, 0, 0);
   COLORREF GColor = RGB(0xdb, 0x88, 0x20);

   m_TextColor = TColor;
   m_BackColor = BColor;
   m_YesColor = YColor;
   m_NoColor = NColor;
   m_GBColor = GColor;

   m_RTFont.DeleteObject();
   m_RTFont.CreatePointFont(m_RTFontSize, m_RTFontName);
   m_RTFontThin.DeleteObject();
   m_RTFontThin.CreatePointFont(m_RTFontSize, m_RTFontThinName);

   int i, j;
   for (i = 0; i < MAX_COLUMNS; i++) {
      for (j = 0; j < MAX_ROWS; j++) {
         CString tmpStr = CString(m_JA_MemberSeatingList[j + (i * MAX_ROWS)].name);
         tmpStr.MakeUpper();

         // TBD: Remove when representative wakes up.
         CString ZZZ("ZZZ"); // = "ZZZ";
         if (tmpStr.Find(ZZZ) != -1) {
            tmpStr = CString("   ");
         }

         if (j + (i * MAX_ROWS) < 133) {
            m_MemberName[j + (i * MAX_ROWS)].SetTextColor(m_TextColor);
            m_MemberName[j + (i * MAX_ROWS)].SetBackColor(m_BackColor);
            CString space(" ");
            int n = tmpStr.Find(space);
            if ((n > 16) || (n == -1)) {
               m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFontThin);
            }
            else {
               m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFont);
            }
            m_MemberName[j + (i * MAX_ROWS)].SetWindowText(CString("  ") + tmpStr);
            m_MemberName[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (i * MAX_FIELD_X) + BORDER_X, MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 2, MAX_FIELD_X - BORDER_X, MAX_FIELD_Y);
            m_MemberName[j + (i * MAX_ROWS)].ShowWindow(TRUE);

            m_MemberYes[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (60 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberYes[j + (i * MAX_ROWS)].EnableWindow(FALSE);

            m_MemberNo[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (30 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberNo[j + (i * MAX_ROWS)].EnableWindow(FALSE);
         }
         else {
            // Special case for the Speaker

            int lLen = tmpStr.ReverseFind('.');
            int rLen = (tmpStr.GetLength() - lLen) - 1;

            m_MemberName[j + (i * MAX_ROWS)].SetTextColor(m_TextColor);
            m_MemberName[j + (i * MAX_ROWS)].SetBackColor(m_BackColor);
            m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFont);
            m_MemberName[j + (i * MAX_ROWS)].SetWindowText(CString("  ") + "SPEAKER"); // tmpStr.Left(lLen)); // "SPEAKER");
            m_MemberName[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (i * MAX_FIELD_X) + BORDER_X, MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 2, MAX_FIELD_X - BORDER_X, MAX_FIELD_Y);
            m_MemberName[j + (i * MAX_ROWS)].ShowWindow(TRUE);

            m_MemberYes[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (60 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberYes[j + (i * MAX_ROWS)].EnableWindow(FALSE);

            m_MemberNo[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (30 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberNo[j + (i * MAX_ROWS)].EnableWindow(FALSE);
            j++;
            m_MemberName[j + (i * MAX_ROWS)].SetTextColor(m_TextColor);
            m_MemberName[j + (i * MAX_ROWS)].SetBackColor(m_BackColor);
            m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFont);
            m_MemberName[j + (i * MAX_ROWS)].SetWindowText(CString("  ") + tmpStr.Right(rLen));
            m_MemberName[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (i * MAX_FIELD_X + BORDER_X), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 2, MAX_FIELD_X - BORDER_X, MAX_FIELD_Y);
            m_MemberName[j + (i * MAX_ROWS)].ShowWindow(TRUE);

            m_MemberYes[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (60 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberYes[j + (i * MAX_ROWS)].EnableWindow(FALSE);

            m_MemberNo[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (30 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberNo[j + (i * MAX_ROWS)].EnableWindow(FALSE);
         }
      }
   }
   
   for (i = 0; i < 5; i++) {
      m_MemberGroupBox->SetBorderColor(m_GBColor);
      m_MemberGroupBox->SetBackColor(TRANS_BACK);
      m_MemberGroupBox[i].MoveWindow((i * MAX_FIELD_X) + BORDER_X, MIN_Y + BORDER_Y / 2, MAX_FIELD_X, MAX_Y - BORDER_Y );
   }
   
   // Set WS_EX_LAYERED on this window
   SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);

   return TRUE;  // return TRUE unless you set the focus to a control
}

void JaRT::OnCancel()
{
   CDialog::OnCancel();

   exit(0); // Just get the heck out.
}

void JaRT::SetMemberVote(int member, int vote)
{
   if (vote == 1) {
      m_MemberNo[member].ShowWindow(TRUE);
      m_MemberYes[member].ShowWindow(FALSE);
  }
   else if (vote == 2) {
      m_MemberYes[member].ShowWindow(TRUE);
      m_MemberNo[member].ShowWindow(FALSE);
   }
   else {
      m_MemberYes[member].ShowWindow(FALSE);
      m_MemberNo[member].ShowWindow(FALSE);
   }
}


void JaRT::FadeOut()
{
   m_FadeOut = TRUE;

   m_FadeLevel = 255;
   // Calculate the timer interval required to complete the fade in the specified time.
   UINT    nElapse = 255 / (255 / 5);
   SetTimer(1, nElapse, NULL);
}


void JaRT::FadeIn()
{
   m_FadeOut = FALSE;

   m_FadeLevel = 0;

   // Calculate the timer interval required to complete the fade in the specified time.
   UINT    nElapse = 510 / (255 / 5);
   SetTimer(1, nElapse, NULL);
}


void JaRT::OnTimer(UINT_PTR nIDEvent)
{
   if (m_FadeOut) {
      if (m_FadeLevel >= 5) {
         m_FadeLevel -= 5;
         m_pSetLayeredWindowAttributes(GetSafeHwnd(), 0, m_FadeLevel, ULW_ALPHA);
      }
      else {
         // Reached minimum alpha. Kill the timer and destroy our window.
         // PostNcDestroy will delete this CFaderWnd instance.
         KillTimer(nIDEvent);
      }
   }
   else {
      if (m_FadeLevel < 255) {
         m_FadeLevel += 5;
         m_pSetLayeredWindowAttributes(GetSafeHwnd(), 0, m_FadeLevel, ULW_ALPHA);
      }
      else {
         // Reached minimum alpha. Kill the timer and destroy our window.
         // PostNcDestroy will delete this CFaderWnd instance.
         KillTimer(nIDEvent);
      }
   }

   CDialog::OnTimer(nIDEvent);
}

HBRUSH JaRT::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   // TODO:  Change any attributes of the DC here

   // TODO:  Return a different brush if the default is not desired

   return (HBRUSH)GetStockObject(BLACK_BRUSH);

   return hbr;
}