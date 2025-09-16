// JaVM.h : main header file for the JAVM application
//

#if !defined(AFX_JAVM_H__CD7E4B84_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
#define AFX_JAVM_H__CD7E4B84_484F_11D2_9F60_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CJaVMApp:
// See JaVM.cpp for the implementation of this class
//

class CJaVMApp : public CWinApp
{
public:
   CJaVMApp();

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CJaVMApp)
public:
   virtual BOOL InitInstance();
   //}}AFX_VIRTUAL

   // Implementation

   //{{AFX_MSG(CJaVMApp)
   // NOTE - the ClassWizard will add and remove member functions here.
   //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAVM_H__CD7E4B84_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
