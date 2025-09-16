#if !defined(AFX_VMTHREAD_H__CD7E4B8E_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
#define AFX_VMTHREAD_H__CD7E4B8E_484F_11D2_9F60_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VMThread.h : header file
//

#include "house.h"
#include "VM_Sock.h"

// Definitions

#define DELAY_TIME 2000
#define DELAY { for (loop = 0; loop < DELAY_TIME; loop++) ;}


// Function Declarations

int  vote_open_check(bool resetLatch);
int  vote_lock_check(void);
int  vote_close_check(void);
int  reset_check(void);

void initialize(void);

void tx_roll_call(void);

void DisplayVote(unsigned char seat, unsigned char vote);
void DisplayVoteRemote(unsigned char seat, unsigned char vote);

void JaLoadParametersAndSeating(void);
void JaLoadSeating(void);
void JaLoadHeaderParms(void);
void JaUpdateSequence();
void JaSetHeader(SockPacket *mbPacket);

void JaPrintVoteResults(unsigned int numPages);
BOOL JaInit(void);
BOOL JaTerm(void);

void JaWriteCurrentVote(CString voteFilename);
void JaSendCurrentVote(void);
//void JaWriteCurrentLocks(CString lockFilename);
void JaWriteRemoteActive(CString RAFilename);

void KillVM(void);
UINT JaVMThreadFunction(LPVOID pParam);
UINT JaWriteThreadFunction(LPVOID voidObj);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VMTHREAD_H__CD7E4B8E_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
