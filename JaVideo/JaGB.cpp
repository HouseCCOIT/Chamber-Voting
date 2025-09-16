#include "JaGB.h"

BEGIN_MESSAGE_MAP(CJaGB, CStatic)
   //{{AFX_MSG_MAP(CJaGB)
   ON_WM_CTLCOLOR_REFLECT()
   ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
   ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CJaGB::CtlColor(CDC* pDC, UINT nCtlColor)
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

   pDC->SetTextColor(m_BorderColor);

   return (HBRUSH)m_Brush;
}


void CJaGB::OnPaint()
{
   CPaintDC dc(this); // device context for painting
                      // TODO: Add your message handler code here
                      // Do not call CStatic::OnPaint() for painting messages
   CPen pen;
   CBrush brush;

   pen.CreatePen(PS_SOLID, 5, RGB(0xdb, 0x88, 0x20));
   CPen* pOldPen = dc.SelectObject(&pen);

   dc.SelectStockObject(NULL_BRUSH);

   CRect rect;
   GetClientRect(rect);

   dc.Rectangle(rect);
}

void CJaGB::OnUpdate()
{
   UpdateCtrl();
}

// UpdateCtrl - Currently not used
void CJaGB::UpdateCtrl()
{
   CWnd* pParent = GetParent();
   CRect rect;

   GetWindowRect(rect);
   pParent->ScreenToClient(rect);
   rect.DeflateRect(2, 2);

   pParent->InvalidateRect(rect, FALSE);

   OnPaint();
}