// ServerSocket.cpp : implementation file
//

#include "stdafx.h"
#include "JaVM.h"
#include "VideoSocketServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL m_VoteOpen;

/////////////////////////////////////////////////////////////////////////////
// CServerSocket

CVideoSocketServer::CVideoSocketServer()
{
	for (int i = 0; i < 3; i++) {
		m_VideoClient[i].m_Index = -1;
	}
}

CVideoSocketServer::CVideoSocketServer(LPVOID pDlgN)
{
	pDlg = (CDialog*)pDlgN;

	for (int i = 0; i < 3; i++) {
		m_VideoClient[i].m_Index = -1;
	}
}

CVideoSocketServer::~CVideoSocketServer()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CVideoSocketServer, CSocket)
	//{{AFX_MSG_MAP(CVideoSocketServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CVideoSocketServer member functions

void CVideoSocketServer::OnAccept(int nErrorCode)
{
	SOCKADDR sockAddress;
	int sockAddressLen;
	int videoIndex = -1;

	// Search through array to find an unused Video Client
	for (int i = 0; i < 3; i++) {
		if (m_VideoClient[i].m_Index == -1) {
			videoIndex = i;
			break;
		}
	}

	if (videoIndex < 0) {
		goto Leave;
	}

	m_VideoClient[videoIndex].m_hSocket = INVALID_SOCKET;
	m_VideoClient[videoIndex].pDlg = pDlg;
	m_VideoClient[videoIndex].VMServer = this;

	//if (Accept(m_VideoClient[videoIndex], &sockAddress, &sockAddressLen)) {
	if (Accept(m_VideoClient[videoIndex], NULL, NULL)) {
		m_VideoClient[videoIndex].m_Index = videoIndex;
		m_VideoClient[videoIndex].SendVoteOpen(m_VoteOpen);
	}
	else {
		int lastError = GetLastError();
		m_VideoClient[videoIndex].m_Index = -1;
	}

Leave:
	CSocket::OnAccept(nErrorCode);
}

void CVideoSocketServer::SendVideoClientMessage(SockPacket* videoPacket)
{
	int bytesSent = 0;

	// Search through array to find active Video Clients
	for (int i = 0; i < 3; i++) {
		if (m_VideoClient[i].m_Index != -1) {
			bytesSent = m_VideoClient[i].Send((char*)videoPacket, sizeof(SockPacket));
		}
	}
}

void CVideoSocketServer::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CSocket::OnClose(nErrorCode);
}
