// JaStatic.cpp : implementation file
//

#include "stdafx.h"
//#include "JaVM.h"
#include "JaRound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJaStatic

CJaRound::CJaRound()
{
   m_TextColor = RGB(0, 0, 0);
   //m_BackColor = TRANS_BACK;
   m_Name = CString("");
   m_County = CString("");
   m_Seat = CString("");

   hFont = CreateFont(15, 7, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS,
      CLIP_STROKE_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Lucida Console"));
}

CJaRound::~CJaRound()
{
}


BEGIN_MESSAGE_MAP(CJaRound, CStatic)
   //{{AFX_MSG_MAP(CJaStatic)
   ON_WM_CTLCOLOR_REFLECT()
   ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
   ON_WM_PAINT()
   //    ON_WM_LBUTTONDOWN()
   //    ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaStatic message handlers

HBRUSH CJaRound::CtlColor(CDC* pDC, UINT nCtlColor)
{
   m_Brush.DeleteObject();

   if (m_BackColor == TRANS_BACK) {
      m_Brush.CreateStockObject(HOLLOW_BRUSH);
      pDC->SetBkMode(TRANSPARENT);
   }
   else {
      m_Brush.CreateSolidBrush(m_BackColor);
      pDC->SetBkColor(m_BackColor);
   }

   pDC->SetTextColor(m_TextColor);

   return (HBRUSH)m_Brush;
}

void CJaRound::OnUpdate()
{
   UpdateCtrl();
}
void CJaRound::UpdateCtrl()
{
   CWnd* pParent = GetParent();
   CRect   rect;

   GetWindowRect(rect);
   pParent->ScreenToClient(rect);
   //rect.DeflateRect(2, 2);

   pParent->InvalidateRect(rect, FALSE);
}


void CJaRound::OnPaint()
{
   CPaintDC cdc(this); // device context for painting
   DrawRoundRect(&cdc);

   CStatic::OnPaint();
}

void CJaRound::DrawRoundRect(CPaintDC* pDC)
{
   // create and select a solid blue brush
   CBrush m_Brush(m_BackColor);
   CBrush* pOldBrush = pDC->SelectObject(&m_Brush);

   // create and select a thick, black pen
   //CPen penBlack;
   //penBlack.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
   //CPen* pOldPen = pDC->SelectObject(&penBlack);

   // get our client rectangle
   CRect rect;
   GetClientRect(rect);

   // shrink our rect 20 pixels in each direction
   //rect.DeflateRect(20, 20);

   // Draw a thick black rectangle filled with blue
   // corners rounded at a 17-unit radius. Note that
   // a radius of three or less is not noticable because
   // the pen is three units wide.
   //pDC->RoundRect(rect, CPoint((rect.right - rect.left) /2, rect.bottom - rect.top));
   //pDC->Rectangle(rect);
   pDC->RoundRect(rect, CPoint(10, 10));

   HFONT hOldFont;

   // Retrieve a handle to the variable stock font.  
   //hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
   //hFont = CreateFont(10, 5, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
   //   CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times"));

   // Select the variable stock font into the specified device context. 
   if (hOldFont = (HFONT)pDC->SelectObject(hFont)) {
      pDC->SetBkColor(m_BackColor);

      int lLen = m_Name.ReverseFind('-') + 1;
      int rLen = m_Name.GetLength() - lLen;

      pDC->TextOut(80, 4, m_RTS);
      if (lLen > 0) {
         pDC->TextOut(5, 4, m_Name.Left(lLen));
         pDC->TextOut(5, 22, m_Name.Right(rLen));
         pDC->TextOut(5, 40, m_County);
      }
      else {
         pDC->TextOut(5, 13, m_Name);
         pDC->TextOut(5, 31, m_County);
      }
      pDC->TextOut(90, 40, m_RorC);
   }

   pDC->SelectObject(hOldFont);

   // put back the old objects
   pDC->SelectObject(pOldBrush);
   //pDC->SelectObject(pOldPen);
}