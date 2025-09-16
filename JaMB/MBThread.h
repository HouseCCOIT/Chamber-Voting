#if !defined(AFX_MBTHREAD_H__F1B86DE3_6136_11D2_9F8D_00104B9A2A27__INCLUDED_)
#define AFX_MBTHREAD_H__F1B86DE3_6136_11D2_9F8D_00104B9A2A27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MBThread.h : header file
//

int prn_init(void);
void vs_rx(void);
void tx_header(void);

int transmit_display(void);
int process_line(LPCTSTR s, int l, int update);
int process_result(char *s, int l);
void JaLoadParametersAndSeating(void);

void setActiveDisplay(int display);

void PrintLocks(unsigned int numPages);
void PrintRemotes(unsigned int numPages);
void PrintCallIns(unsigned int numPages);

int  TransmitDisplay(void);
int  ClearDisplays(void);
void ClearResults(void);

void JaMBVoteOpen(void);
void JaMBVoteActive(void);
void JaMBVoteResults(SockPacket *mbPacket);
void JaMBVoteSendHeader(void);

int  MD_ClearAll(void);
int  MD_ClearSix(void);
int  MD_Result(char *text1, char *text2, char *text3, char *text4);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MBTHREAD_H__F1B86DE3_6136_11D2_9F8D_00104B9A2A27__INCLUDED_)
