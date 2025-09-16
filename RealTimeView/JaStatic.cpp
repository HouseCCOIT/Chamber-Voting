// JaStatic.cpp : implementation file
//

#include "pch.h"
//#include "JaVM.h"
#include "JaStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJaStatic

CJaStatic::CJaStatic()
{
   m_TextColor = RGB(0, 0, 0);
   m_BackColor = TRANS_BACK;
}

CJaStatic::~CJaStatic()
{
}


BEGIN_MESSAGE_MAP(CJaStatic, CStatic)
   //{{AFX_MSG_MAP(CJaStatic)
   ON_WM_CTLCOLOR_REFLECT()
   ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
   //    ON_WM_LBUTTONDOWN()
   //    ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaStatic message handlers

HBRUSH CJaStatic::CtlColor(CDC* pDC, UINT nCtlColor)
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

void CJaStatic::OnUpdate()
{
   UpdateCtrl();
}
void CJaStatic::UpdateCtrl()
{
   CWnd* pParent = GetParent();
   CRect   rect;

   GetWindowRect(rect);
   pParent->ScreenToClient(rect);
   rect.DeflateRect(2, 2);

   pParent->InvalidateRect(rect, FALSE);
}


