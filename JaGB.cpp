#include "JaGB.h"

/////////////////////////////////////////////////////////////////////////////
// CJaStatic

CJaGB::CJaGB()
{
   m_BorderColor = RGB(128, 128, 128);
}

//CJaGB::~CJaGB()
//{
//}

BEGIN_MESSAGE_MAP(CJaGB, CStatic)
   ON_WM_PAINT()
END_MESSAGE_MAP()

void CJaGB::OnPaint()
{
   CPaintDC pDC(this); // device context for painting

   CPen pen;
   CBrush brush;

   pen.CreatePen(PS_SOLID, 10, RGB(0, 255, 0));
   CPen* pOldPen = pDC.SelectObject(&pen);

   brush.CreateSolidBrush(0x00FFFF00);
   CBrush* oldBrush = pDC.SelectObject(&brush);

   //pDC.Rectangle(&myRect);
   pDC.Rectangle(50, 50, 500, 200);

   //CDialog::OnPaint();
}