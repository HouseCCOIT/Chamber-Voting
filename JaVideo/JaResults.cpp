// JaDisplay.cpp : implementation file
//

#include "stdafx.h"
//#include "JaVM.h"
#include "JaResults.h"
#include "JaStatic.h"

#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// JaDisplay dialog


JaResults::JaResults(CWnd* pParent /*=NULL*/)
   : CDialog(JaResults::IDD, pParent),
   m_FontName("Courier")
{
   //{{AFX_DATA_INIT(JaDisplay)
   //}}AFX_DATA_INIT
}


void JaResults::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(JaDisplay)
   DDX_Control(pDX, IDC_JD_STATIC12, m_JD_Static12);
   DDX_Control(pDX, IDC_JD_STATIC11, m_JD_Static11);
   DDX_Control(pDX, IDC_JD_STATIC10, m_JD_Static10);
   DDX_Control(pDX, IDC_JD_STATIC9, m_JD_Static9);
   DDX_Control(pDX, IDC_JD_STATIC8, m_JD_Static8);
   DDX_Control(pDX, IDC_JD_STATIC7, m_JD_Static7);
   DDX_Control(pDX, IDC_JD_STATIC6, m_JD_Static6);
   DDX_Control(pDX, IDC_JD_STATIC5, m_JD_Static5);
   DDX_Control(pDX, IDC_JD_STATIC3, m_JD_Static3);
   DDX_Control(pDX, IDC_JD_STATIC4, m_JD_Static4);
   DDX_Control(pDX, IDC_JD_STATIC2, m_JD_Static2);
   DDX_Control(pDX, IDC_JD_STATIC1, m_JD_Static1);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(JaResults, CDialog)
   //{{AFX_MSG_MAP(JaDisplay)
   ON_WM_TIMER()
   //}}AFX_MSG_MAP
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

#define MAX_SCREEN_X 1920 // 1280 //640
#define MAX_SCREEN_Y 1080 // 720  //480
#define MIN_X 25
#define MIN_Y 250
#define MAX_X 1880 // 1255 //615
#define MAX_Y 1040 // 708  //468
#define MAX_FIELD_X (MAX_X - MIN_X)
#define MAX_FIELD_Y ((MAX_Y - MIN_Y) / 8)
#define MAX_RESULT_X ((MAX_X - MIN_X) / 4)

const char * JaDispDat = "V:\\JaDisp.dat";

/////////////////////////////////////////////////////////////////////////////
// JaDisplay message handlers

BOOL JaResults::OnInitDialog()
{
   CDialog::OnInitDialog();

   SetWindowPos(&CWnd::wndBottom, ::GetSystemMetrics(SM_CXSCREEN), 0, MAX_SCREEN_X, MAX_SCREEN_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
                                                                                                      // SetWindowPos(&CWnd::wndTop, 0, 0, 640, 480, 0); //SWP_NOMOVE | SWP_NOSIZE);

   MoveWindow(::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y);
   // MoveWindow(0, 0, MAX_SCREEN_X, MAX_SCREEN_Y);  // For debugging only

   m_Watermark = "V:\\bitmapsTV\\ColorSeal.bmp";
   m_BackColor = TRANS_BACK;

   m_ShuttingDown = FALSE;

   m_ShowResults = TRUE;

   SetTimer(1, 2000, NULL);  // Update the Message Board every 2 seconds, which is the granularity of the OS file write time stamp

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void JaResults::SetFont(CString fontName)
{
   ClearAll();

   m_FontName = fontName;

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);

   UpdateDisplay();
}

void JaResults::SetFontSize(int fontSize)
{
   ClearAll();

   switch (fontSize) {
   case FontSizeLarge:
      m_FontSizePoints = 625; // 350;
      break;

   case FontSizeMedium:
      m_FontSizePoints = 575; // 300;
      break;

   case FontSizeSmall:
   default:
      m_FontSizePoints = 500; // 250;
      break;
   }

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);

   UpdateDisplay();
}

void JaResults::UpdateDisplay()
{
   MoveWindow(::GetSystemMetrics(SM_CXSCREEN), 0, MAX_SCREEN_X, MAX_SCREEN_Y);
   // MoveWindow(0, 0, MAX_SCREEN_X, MAX_SCREEN_Y);  // For debugging only

   COLORREF TColor = RGB(255, 255, 255);
   m_TextColor = TColor;

   m_JD_Static1.SetTextColor(m_TextColor);
   m_JD_Static1.SetBackColor(m_BackColor);
   m_JD_Static1.SetFont(&m_MyFont);
   m_JD_Static1.SetWindowText(m_DisplayLine[0]);
   m_JD_Static1.MoveWindow(MIN_X, MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static2.SetTextColor(m_TextColor);
   m_JD_Static2.SetBackColor(m_BackColor);
   m_JD_Static2.SetFont(&m_MyFont);
   m_JD_Static2.SetWindowText(m_DisplayLine[1]);
   m_JD_Static2.MoveWindow(MIN_X, MIN_Y + MAX_FIELD_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static3.SetTextColor(m_TextColor);
   m_JD_Static3.SetBackColor(m_BackColor);
   m_JD_Static3.SetFont(&m_MyFont);
   m_JD_Static3.SetWindowText(m_DisplayLine[2]);
   m_JD_Static3.MoveWindow(MIN_X, MIN_Y + MAX_FIELD_Y * 2, MAX_X, MAX_FIELD_Y);

   m_JD_Static4.SetTextColor(m_TextColor);
   m_JD_Static4.SetBackColor(m_BackColor);
   m_JD_Static4.SetFont(&m_MyFont);
   m_JD_Static4.SetWindowText(m_DisplayLine[3]);
   m_JD_Static4.MoveWindow(MIN_X, MIN_Y + MAX_FIELD_Y * 3, MAX_X, MAX_FIELD_Y);

   m_JD_Static5.SetTextColor(m_TextColor);
   m_JD_Static5.SetBackColor(m_BackColor);
   m_JD_Static5.SetFont(&m_MyFont);
   m_JD_Static5.SetWindowText(m_DisplayLine[4]);
   m_JD_Static5.MoveWindow(MIN_X, MIN_Y + MAX_FIELD_Y * 4, MAX_X, MAX_FIELD_Y);

   m_JD_Static6.SetTextColor(m_TextColor);
   m_JD_Static6.SetBackColor(m_BackColor);
   m_JD_Static6.SetFont(&m_MyFont);
   m_JD_Static6.SetWindowText(m_DisplayLine[5]);
   m_JD_Static6.MoveWindow(MIN_X, MIN_Y + MAX_FIELD_Y * 5, MAX_X, MAX_FIELD_Y);

   m_JD_Static7.SetTextColor(m_TextColor);
   m_JD_Static7.SetBackColor(m_BackColor);
   m_JD_Static7.SetFont(&m_MyFont);
   m_JD_Static7.SetWindowText(m_DisplayLine[6]);
   m_JD_Static7.MoveWindow(MIN_X, MIN_Y + MAX_FIELD_Y * 6, MAX_X, MAX_FIELD_Y);

   m_JD_Static8.SetTextColor(m_TextColor);
   m_JD_Static8.SetBackColor(m_BackColor);
   m_JD_Static8.SetFont(&m_MyFont);
   m_JD_Static8.SetWindowText(m_DisplayLine[7]);
   m_JD_Static8.MoveWindow(MIN_X, MIN_Y + MAX_FIELD_Y * 7, MAX_X, MAX_FIELD_Y);

   m_JD_Static9.SetTextColor(m_TextColor);
   m_JD_Static9.SetBackColor(m_BackColor);
   m_JD_Static9.SetFont(&m_MyFont);
   m_JD_Static9.SetWindowText(m_ResultLine[0]);
   m_JD_Static9.MoveWindow(MIN_X + 40, MIN_Y + MAX_FIELD_Y * 7, MAX_RESULT_X, MAX_FIELD_Y);

   m_JD_Static10.SetTextColor(m_TextColor);
   m_JD_Static10.SetBackColor(m_BackColor);
   m_JD_Static10.SetFont(&m_MyFont);
   m_JD_Static10.SetWindowText(m_ResultLine[1]);
   m_JD_Static10.MoveWindow(MIN_X + MAX_RESULT_X, MIN_Y + MAX_FIELD_Y * 7, MAX_RESULT_X, MAX_FIELD_Y);

   m_JD_Static11.SetTextColor(m_TextColor);
   m_JD_Static11.SetBackColor(m_BackColor);
   m_JD_Static11.SetFont(&m_MyFont);
   m_JD_Static11.SetWindowText(m_ResultLine[2]);
   m_JD_Static11.MoveWindow(MIN_X + MAX_RESULT_X * 2, MIN_Y + MAX_FIELD_Y * 7, MAX_RESULT_X, MAX_FIELD_Y);

   m_JD_Static12.SetTextColor(m_TextColor);
   m_JD_Static12.SetBackColor(m_BackColor);
   m_JD_Static12.SetFont(&m_MyFont);
   m_JD_Static12.SetWindowText(m_ResultLine[3]);
   m_JD_Static12.MoveWindow(MIN_X + MAX_RESULT_X * 3, MIN_Y + MAX_FIELD_Y * 7, MAX_RESULT_X - 40, MAX_FIELD_Y);

   Invalidate();
}

void JaResults::ClearSix()
{
   for (int i = 0; i < 6; i++) {
      m_DisplayLine[i] = "";
   }

   UpdateDisplay();
}

void JaResults::ClearAll()
{
   int i;

   for (i = 0; i < 8; i++) {
      m_DisplayLine[i] = "";
   }

   for (i = 0; i < 4; i++) {
      m_ResultLine[i] = "        ";
   }

   UpdateDisplay();
}

void JaResults::SetLine(int i, char *text)
{
   m_DisplayLine[i] = text;

   UpdateDisplay();
}

void JaResults::SetResultLine(char *text1, char *text2, char *text3, char *text4)
{
   m_ResultLine[0] = text1;
   m_ResultLine[1] = text2;
   m_ResultLine[2] = text3;
   m_ResultLine[3] = text4;

   UpdateDisplay();
}

void JaResults::ClearResults()
{
   int i;

   for (i = 0; i < 4; i++) {
      m_ResultLine[i] = "       ";
   }

   UpdateDisplay();
}

BOOL JaResults::OnEraseBkgnd(CDC* pDC)
{
   CRect rc;

   if (m_ShuttingDown) {
      return FALSE;
   }

   GetClientRect(rc);

   m_bmpBackground.Load(m_Watermark);
   m_bmpBackground.DrawDIB(pDC, 0, 0, rc.Width(), rc.Height());

   return TRUE; // tell Windows we handled it
}


void JaResults::SetWaterMark(CString watermark)
{
   m_Watermark = CString("V:\\bitmapsTV\\") + watermark;
}

BOOL JaResults::LoadDispOptions()
{
   FILE *dispFile;

   char tmpStr[256];

   int i;

   SetWaterMark(CString("Register.bmp"));

   if ((dispFile = fopen(JaDispDat, "r+")) == NULL) {
      return FALSE;
   }

   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   SetFont(CString(tmpStr));

   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   SetFontSize(atoi(tmpStr));

   for (i = 0; i < 8; i++) {
      fgets(tmpStr, 256, dispFile);
      tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
      m_DisplayLine[i] = tmpStr;
   }

   for (i = 0; i < 4; i++) {
      fgets(tmpStr, 256, dispFile);
      tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
      m_ResultLine[i] = tmpStr;
   }

   fclose(dispFile);

   UpdateDisplay();

   return TRUE;
}

void JaResults::ShuttingDown()
{
   m_ShuttingDown = TRUE;
}


FILETIME JaResults::JaGetFileTime(CString fName)
{
   HANDLE hFile = NULL;
   FILETIME ftCreate, ftAccess, ftWrite;

   ftWrite.dwHighDateTime = 0;
   ftWrite.dwLowDateTime = 0;

   hFile = CreateFile(fName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

   if (hFile != INVALID_HANDLE_VALUE) {
      // Retrieve the file times for the file.
      if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
         ftWrite.dwHighDateTime = 0;
         ftWrite.dwLowDateTime = 0;
      }

      CloseHandle(hFile);
   }

   return ftWrite;
}

BOOL JaResults::DisplayFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(JaDispDat);

   if ((ftWrite.dwLowDateTime != 0) && (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime)) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

void JaResults::OnCancel()
{
   CDialog::OnCancel();

   exit(0); // Just get the heck out.
}

void JaResults::OnTimer(UINT_PTR nIDEvent)
{
   if (DisplayFileChanged() & m_ShowResults) {
      LoadDispOptions();
   }

   CDialog::OnTimer(nIDEvent);
}

void JaResults::DisplayResults(BOOL DisplayOn)
{
   m_ShowResults = DisplayOn;
}
