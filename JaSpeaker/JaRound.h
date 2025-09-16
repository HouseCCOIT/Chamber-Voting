#if !defined(AFX_JAROUND_H__C4999578_3523_4D1B_AEA7_C02C88EB171A__INCLUDED_)
#define AFX_JAROUND_H__C4999578_3523_4D1B_AEA7_C02C88EB171A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JaStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJaStatic window

#define TRANS_BACK 555

class CJaRound : public CStatic
{
   // Construction
public:
   CJaRound();
   void DrawRoundRect(CPaintDC* pDC);

   // Attributes
public:

private:
   COLORREF    m_TextColor;
   COLORREF    m_BackColor;
   CBrush      m_Brush;
   HRGN        m_Rgn;
   CBitmap     m_BitMap;
   CString		m_Seat;
   CString     m_Speaker = "";
   CString		m_Name;
   CString		m_County;
   CString     m_RorC;
   CString     m_RTS;
   HFONT       hFont;

   // Operations
public:
   void SetSeat(CString seat) {
      m_Seat = seat;
      UpdateCtrl();
   }
   void SetSpeaker(CString spkr) {
      m_Speaker = spkr;
      UpdateCtrl();
   }
   void SetName(CString name) {
      m_Name = name;
      UpdateCtrl();
   }
   void SetCounty(CString county) {
      m_County = county;
      UpdateCtrl();
   }
   void SetRorC(CString rorc) {
      m_RorC = rorc;
      UpdateCtrl();
   }
   void SetRTS(CString rts) {
      m_RTS = rts;
      UpdateCtrl();
   }
   void SetTextColor(COLORREF col) {
      m_TextColor = col;
      UpdateCtrl();
   }
   void SetBackColor(COLORREF col) {
      m_BackColor = col;
      UpdateCtrl();
   }
private:
   void UpdateCtrl();

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CJaStatic)
   //}}AFX_VIRTUAL

   // Implementation
public:
   virtual ~CJaRound();

   // Generated message map functions
protected:
   //{{AFX_MSG(CJaStatic)
   afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
   afx_msg void OnUpdate();
   afx_msg void OnPaint();
   //    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   //    afx_msg void OnKillfocus();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAROUND_H__C4999578_3523_4D1B_AEA7_C02C88EB171A__INCLUDED_)
