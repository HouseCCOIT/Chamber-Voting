// MBSocketServer.cpp : implementation file
//

#include "stdafx.h"
#include "JaVM.h"
#include "MBSocketServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CServerSocket

CMBSocketServer::CMBSocketServer()
{
	for (int i = 0; i < 3; i++) {
		m_MBClient[i].m_Index = -1;
	}
}

CMBSocketServer::CMBSocketServer(LPVOID pDlgN)
{
	pDlg = (CDialog*)pDlgN;

	for (int i = 0; i < 3; i++) {
		m_MBClient[i].m_Index = -1;
	}
}

CMBSocketServer::~CMBSocketServer()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMBSocketServer, CSocket)
	//{{AFX_MSG_MAP(CMBSocketServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CMBSocketServer member functions

void CMBSocketServer::OnAccept(int nErrorCode)
{
	int MBIndex = -1;

	for (int i = 0; i < 3; i++) {
		if (m_MBClient[i].m_Index == -1) {
			MBIndex = i;
			break;
		}
	}

	if (MBIndex < 0) {
		goto Leave;
	}

	m_MBClient[MBIndex].m_hSocket = INVALID_SOCKET;
	m_MBClient[MBIndex].pDlg = pDlg;
	m_MBClient[MBIndex].VMServer = this;

	//if (Accept(m_MBClient[MBIndex], &sockAddress, &sockAddressLen)) {
	if (Accept(m_MBClient[MBIndex], NULL, NULL)) {
		m_MBClient[MBIndex].m_Index = MBIndex;
	}
	else {
		int lastError = GetLastError();
		m_MBClient[MBIndex].m_Index = -1;
	}

Leave:
	CSocket::OnAccept(nErrorCode);
}

void CMBSocketServer::SendMBClientMessage(SockPacket* mbPacket)
{
	int bytesSent = 0;

	// Search through array to find active RemoteClients
	for (int i = 0; i < 3; i++) {
		if (m_MBClient[i].m_Index != -1) {
			bytesSent = m_MBClient[i].Send((char*)mbPacket, sizeof(SockPacket));
		}
	}
}

void CMBSocketServer::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CSocket::OnClose(nErrorCode);
}
