#if !defined(AFX_VMTHREAD_H__CD7E4B8E_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
#define AFX_VMTHREAD_H__CD7E4B8E_484F_11D2_9F60_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VMThread.h : header file
//

#include "house.h"

// Function Declarations

int  v_main(void);

UINT VideoThreadFunction(LPVOID DlgObj);

void JaLoadParametersAndSeating(void);
void JaLoadSeating(void);
void JaLoadHeaderParms(void);
void JaUpdateSequence();

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VMTHREAD_H__CD7E4B8E_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
