// ServerSocket.cpp : implementation file
//

#include "stdafx.h"
#include "JaVM.h"
#include "RemoteSocketServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL m_VoteOpen;

extern char debateQueue[MAX_VOTE_ARRAY];
extern unsigned char boardMap[MAX_SEATS];

/////////////////////////////////////////////////////////////////////////////
// CRemoteSocketServer

CRemoteSocketServer::CRemoteSocketServer()
{
	for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
		m_RemoteClient[i].m_Index = -1;
		m_RemoteClient[i].m_Seat = 0;
	}
}

CRemoteSocketServer::CRemoteSocketServer(LPVOID pDlgN)
{
	pDlg = (CDialog*)pDlgN;

	for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
		m_RemoteClient[i].m_Index = -1;
		m_RemoteClient[i].m_Seat = 0;
	}
}

CRemoteSocketServer::~CRemoteSocketServer()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CRemoteSocketServer, CSocket)
	//{{AFX_MSG_MAP(CRemoteSocketServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CRemoteSocketServer member functions

void CRemoteSocketServer::OnAccept(int nErrorCode) 
{
	int remoteIndex = -1;
	int i;

	// Search through array to find an unused RemoteClient
	for (i = 0; i < MAX_VOTE_ARRAY; i++) {
		if (m_RemoteClient[i].m_Index == -1) {
			remoteIndex = i;
			break;
		}
	}

	if (remoteIndex < 0) {

		//char message[80];
		//sprintf(message, "RemoteClients > %0x", i);
		//AfxMessageBox(message);

		goto Leave;
	}

	m_RemoteClient[remoteIndex].m_hSocket = INVALID_SOCKET;
	m_RemoteClient[remoteIndex].pDlg = pDlg;
	m_RemoteClient[remoteIndex].VMServer = this;

   if (Accept(m_RemoteClient[remoteIndex], NULL, NULL)) {
		m_RemoteClient[remoteIndex].SendVoteOpen(m_VoteOpen);
		m_RemoteClient[remoteIndex].m_Index = remoteIndex;
	}
	else {
		//char message[80];
		//int lastError = GetLastError();
		//sprintf(message, "Error = %0x", lastError);
		//AfxMessageBox(message);
		m_RemoteClient[remoteIndex].m_Index = -1;
	}

Leave:
	CSocket::OnAccept(nErrorCode);
}

void CRemoteSocketServer::SendSockRemoteMessage(SockVMPacket* remotePacket)
{
	// Search through array to find active RemoteClients
	for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
		if (m_RemoteClient[i].m_Index != -1) {
			m_RemoteClient[i].Send((char*)remotePacket, sizeof(SockVMPacket));
		}
	}
}

void CRemoteSocketServer::SendSockRemoteSeatMessage(unsigned char seat, SockRemotePacket* SockPacket)
{
	// Search through array to find active RemoteClients. Look for specific seat. Send message to that seat.
	for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
		if ((m_RemoteClient[i].m_Index != -1) && (m_RemoteClient[i].m_Seat == seat)){
			m_RemoteClient[i].Send((char*)SockPacket, sizeof(SockRemotePacket));
		}
	}
}

void CRemoteSocketServer::SendSockRemoteMessageToRTS(SockRemotePacket* SockPacket)
{
	// Search through array to find active RemoteClients
	for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
		if ((m_RemoteClient[i].m_Index != -1) && (!m_VoteOpen)) {
			if (debateQueue[boardMap[m_RemoteClient[i].m_Seat - 1]] == 1) {
				m_RemoteClient[i].Send((char*)SockPacket, sizeof(SockVMPacket));
			}
		}
	}
}

void CRemoteSocketServer::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CSocket::OnClose(nErrorCode);
}
