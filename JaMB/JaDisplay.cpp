// JaDisplay.cpp : implementation file
//

#include "stdafx.h"
#include "JaMB.h"
#include "JaDisplay.h"
#include "JaStatic.h"

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
	//DDX_Control(pDX, IDC_JD_STATIC12, m_JD_Static12);
	//DDX_Control(pDX, IDC_JD_STATIC11, m_JD_Static11);
	//DDX_Control(pDX, IDC_JD_STATIC10, m_JD_Static10);
	//DDX_Control(pDX, IDC_JD_STATIC9, m_JD_Static9);
	//DDX_Control(pDX, IDC_JD_STATIC8, m_JD_Static8);
	//DDX_Control(pDX, IDC_JD_STATIC7, m_JD_Static7);
	//DDX_Control(pDX, IDC_JD_STATIC6, m_JD_Static6);
	//DDX_Control(pDX, IDC_JD_STATIC5, m_JD_Static5);
	//DDX_Control(pDX, IDC_JD_STATIC3, m_JD_Static3);
	//DDX_Control(pDX, IDC_JD_STATIC4, m_JD_Static4);
	//DDX_Control(pDX, IDC_JD_STATIC2, m_JD_Static2);
	//DDX_Control(pDX, IDC_JD_STATIC1, m_JD_Static1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(JaResults, CDialog)
	//{{AFX_MSG_MAP(JaDisplay)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


#define MIN_X 0
#define MIN_Y 0
#define MAX_X 640
#define MAX_Y 480
#define MAX_FIELD_X MAX_X
#define MAX_FIELD_Y (MAX_Y / 8)
#define MAX_RESULT_X (MAX_X / 4)

/////////////////////////////////////////////////////////////////////////////
// JaDisplay message handlers

CString JaDispDat  = "V:\\JaDisp.dat";
CString JaDispDat2 = "V:\\JaDisp2.dat";

BOOL JaResults::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Watermark = "V:\\bitmaps\\ColorSeal.bmp";
	m_BackColor = TRANS_BACK;

   LoadDispOptions();
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void JaResults::SetFont(CString fontName)
{
   m_FontName = fontName;

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);
}

void JaResults::SetFontSize(int fontSize)
{
   m_FontSize = fontSize;

   switch (fontSize) {
      case FontSizeLarge:
         m_FontSizePoints = 450;
         break;

      case FontSizeMedium:
         m_FontSizePoints = 375;
         break;

      case FontSizeSmall:
      default:
         m_FontSizePoints = 300;
         break;
   }

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);
}

void JaResults::UpdateDisplay()
{
	// TBD - 02142021 Uncomment for showtime
	// MoveWindow(::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y);
	MoveWindow(0, 0, MAX_X, MAX_Y);
	ShowWindow(SW_HIDE);

////	MoveWindow(::GetSystemMetrics(SM_CXSCREEN) - 640, 0, MAX_X, MAX_Y);
////	MoveWindow(0, 0, MAX_X, MAX_Y);
//
//	COLORREF TColor = RGB(255, 255, 255);
//	m_TextColor = TColor;
//
//	m_JD_Static1.SetTextColor(m_TextColor);
//	m_JD_Static1.SetBackColor(m_BackColor);
//	m_JD_Static1.SetFont(&m_MyFont);
//	m_JD_Static1.SetWindowText(m_DisplayLine[0]);
//	m_JD_Static1.MoveWindow(MIN_X, MIN_Y, MAX_X, MAX_FIELD_Y);
//
//	m_JD_Static2.SetTextColor(m_TextColor);
//	m_JD_Static2.SetBackColor(m_BackColor);
//	m_JD_Static2.SetFont(&m_MyFont);
//   m_JD_Static2.SetWindowText(m_DisplayLine[1]);
//   m_JD_Static2.MoveWindow(MIN_X, MAX_FIELD_Y, MAX_X, MAX_FIELD_Y);
//
//	m_JD_Static3.SetTextColor(m_TextColor);
//	m_JD_Static3.SetBackColor(m_BackColor);
//	m_JD_Static3.SetFont(&m_MyFont);
//   m_JD_Static3.SetWindowText(m_DisplayLine[2]);
//   m_JD_Static3.MoveWindow(0, MAX_FIELD_Y * 2, MAX_X, MAX_FIELD_Y);
//
//	m_JD_Static4.SetTextColor(m_TextColor);
//	m_JD_Static4.SetBackColor(m_BackColor);
//	m_JD_Static4.SetFont(&m_MyFont);
//   m_JD_Static4.SetWindowText(m_DisplayLine[3]);
//   m_JD_Static4.MoveWindow(MIN_X, MAX_FIELD_Y * 3, MAX_X, MAX_FIELD_Y);
//
//	m_JD_Static5.SetTextColor(m_TextColor);
//	m_JD_Static5.SetBackColor(m_BackColor);
//	m_JD_Static5.SetFont(&m_MyFont);
//   m_JD_Static5.SetWindowText(m_DisplayLine[4]);
//   m_JD_Static5.MoveWindow(MIN_X, MAX_FIELD_Y * 4, MAX_X, MAX_FIELD_Y);
//
//	m_JD_Static6.SetTextColor(m_TextColor);
//	m_JD_Static6.SetBackColor(m_BackColor);
//	m_JD_Static6.SetFont(&m_MyFont);
//   m_JD_Static6.SetWindowText(m_DisplayLine[5]);
//   m_JD_Static6.MoveWindow(MIN_X, MAX_FIELD_Y * 5, MAX_X, MAX_FIELD_Y);
//
//	m_JD_Static7.SetTextColor(m_TextColor);
//   m_JD_Static7.SetBackColor(m_BackColor);
//   m_JD_Static7.SetFont(&m_MyFont);
//   m_JD_Static7.SetWindowText(m_DisplayLine[6]);
//   m_JD_Static7.MoveWindow(MIN_X, MAX_FIELD_Y * 6, MAX_X, MAX_FIELD_Y);
//
//   m_JD_Static8.SetTextColor(m_TextColor);
//   m_JD_Static8.SetBackColor(m_BackColor);
//   m_JD_Static8.SetFont(&m_MyFont);
//   m_JD_Static8.SetWindowText(m_DisplayLine[7]);
//   m_JD_Static8.MoveWindow(MIN_X, MAX_FIELD_Y * 7, MAX_X, MAX_FIELD_Y);
//
//	m_JD_Static9.SetTextColor(m_TextColor);
//	m_JD_Static9.SetBackColor(m_BackColor);
//	m_JD_Static9.SetFont(&m_MyFont);
//   m_JD_Static9.SetWindowText(m_ResultLine[0]);
//   m_JD_Static9.MoveWindow(MIN_X, MAX_FIELD_Y * 7, MAX_RESULT_X, MAX_FIELD_Y);
//
//	m_JD_Static10.SetTextColor(m_TextColor);
//	m_JD_Static10.SetBackColor(m_BackColor);
//	m_JD_Static10.SetFont(&m_MyFont);
//   m_JD_Static10.SetWindowText(m_ResultLine[1]);
//   m_JD_Static10.MoveWindow(MAX_RESULT_X, MAX_FIELD_Y * 7, MAX_RESULT_X, MAX_FIELD_Y);
//
//	m_JD_Static11.SetTextColor(m_TextColor);
//	m_JD_Static11.SetBackColor(m_BackColor);
//	m_JD_Static11.SetFont(&m_MyFont);
//   m_JD_Static11.SetWindowText(m_ResultLine[2]);
//   m_JD_Static11.MoveWindow(MAX_RESULT_X * 2, MAX_FIELD_Y * 7, MAX_RESULT_X, MAX_FIELD_Y);
//
//	m_JD_Static12.SetTextColor(m_TextColor);
//	m_JD_Static12.SetBackColor(m_BackColor);
//	m_JD_Static12.SetFont(&m_MyFont);
//   m_JD_Static12.SetWindowText(m_ResultLine[3]);
//   m_JD_Static12.MoveWindow(MAX_RESULT_X * 3, MAX_FIELD_Y * 7, MAX_RESULT_X, MAX_FIELD_Y);
//
   Invalidate();

	// Save display options for JaVideo
   StoreDispOptions(JaDispDat);

	// Save display options for RealTimeView
	StoreDispOptions(JaDispDat2);
}

void JaResults::ClearSix()
{
	for (int i = 0; i < 7; i++) {
		m_DisplayLine[i] = "";
	}

	COLORREF BColor = RGB(0, 0, 0);
	m_BackColor = BColor;

	UpdateDisplay();

   m_BackColor = TRANS_BACK;

	Invalidate();
}

void JaResults::ClearAll()
{
	int i;

	for (i = 0; i < 8; i++) {
		m_DisplayLine[i] = "";
	}

	for (i = 0; i < 4; i++) {
		m_ResultLine[i] = "";
	}

	COLORREF BColor = RGB(0, 0, 0);
	m_BackColor = BColor;

	UpdateDisplay();

   //m_BackColor = TRANS_BACK;

	Invalidate();
}

void JaResults::SetLine(int i, char *text)
{
   m_DisplayLine[i] = text;

	//m_BackColor = TRANS_BACK;
}

void JaResults::SetResultLine(char *text1, char *text2, char *text3, char *text4)
{
   m_ResultLine[0] = text1;
   m_ResultLine[1] = text2;
	m_ResultLine[2] = text3;
   m_ResultLine[3] = text4;

	m_BackColor = TRANS_BACK;
}

void JaResults::ClearResults()
{
	int i;

	for (i = 0; i < 4; i++) {
		m_ResultLine[i] = "     ";
	}

	COLORREF BColor = RGB(0, 0, 0);
	m_BackColor = BColor;

	UpdateDisplay();

 //  m_BackColor = TRANS_BACK;

	//Invalidate();
}

//BOOL JaDisplay::OnEraseBkgnd(CDC* pDC) 
//{
//   CRect rc;
//
//   GetClientRect(rc);
//
//   m_bmpBackground.Load(m_Watermark);
//   m_bmpBackground.DrawDIB(pDC, MIN_X, MIN_X, rc.Width(), rc.Height());
//
//   return TRUE; // tell Windows we handled it
//}
//
//
//void JaDisplay::SetWaterMark(CString watermark)
//{
//	m_Watermark = "V:\\bitmaps\\" + watermark;
//}

void JaResults::StoreDispOptions(CString dispDat)
{
   FILE *dispFile;

   int i;

   if (!(dispFile = fopen(dispDat, "w"))) {
      return;
   }

   fprintf(dispFile, "%s\n", m_FontName.GetString());

   fprintf(dispFile, "%d\n", m_FontSize);

   for (i = 0; i < 8; i++) {
      fprintf(dispFile, "%s\n", m_DisplayLine[i].GetString());
   }

   for (i = 0; i < 4; i++) {
      fprintf(dispFile, "%s\n", m_ResultLine[i].GetString());
   }

   fclose(dispFile);
}

BOOL JaResults::LoadDispOptions()
{
   FILE *dispFile;

   char tmpStr[256];

   if ((dispFile = fopen(JaDispDat, "r+")) == NULL) {
      return FALSE;
   }

   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   SetFont(tmpStr);

   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   SetFontSize(atoi(tmpStr));

   fclose(dispFile);

   return TRUE;
}
