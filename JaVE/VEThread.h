#if !defined(AFX_VETHREAD_H__64078A97_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_)
#define AFX_VETHREAD_H__64078A97_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VEThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VEThread thread


int JaReadSequenceFile(CString seqFileName);
int JaWriteEditedSequenceFile(CString seqFileName);

void JaLoadParametersAndSeating(void);
int JaSend(CString editedResultName);
int JaSendTabbed(CString editedResultName);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VETHREAD_H__64078A97_6EC1_11D2_AD3B_00A024C38E5B__INCLUDED_)
