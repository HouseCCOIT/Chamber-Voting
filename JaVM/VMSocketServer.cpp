// VMSocketServer.cpp : implementation file
//

#include "stdafx.h"
#include "JaVM.h"
#include "VMSocketServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CServerSocket

CVMSocketServer::CVMSocketServer()
{
	for (int i = 0; i < MAX_VM_SOCKETS; i++) {
		m_VMClient[i].m_Index = -1;
	}
}

CVMSocketServer::CVMSocketServer(LPVOID pDlgN)
{
	pDlg = (CDialog*)pDlgN;

	for (int i = 0; i < MAX_VM_SOCKETS; i++) {
		m_VMClient[i].m_Index = -1;
	}
}

CVMSocketServer::~CVMSocketServer()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CVMSocketServer, CSocket)
	//{{AFX_MSG_MAP(CVMSocketServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CVMSocketServer member functions

void CVMSocketServer::OnAccept(int nErrorCode)
{
	int VMIndex = -1;

	for (int i = 0; i < MAX_VM_SOCKETS; i++) {
		if (m_VMClient[i].m_Index == -1) {
			VMIndex = i;
			break;
		}
	}

	if (VMIndex < 0) {
		goto Leave;
	}

	m_VMClient[VMIndex].m_hSocket = INVALID_SOCKET;
	m_VMClient[VMIndex].pDlg = pDlg;
	m_VMClient[VMIndex].VMServer = this;

	//if (Accept(m_MBClient[MBIndex], &sockAddress, &sockAddressLen)) {
	if (Accept(m_VMClient[VMIndex], NULL, NULL)) {
		m_VMClient[VMIndex].m_Index = VMIndex;
	}
	else {
		int lastError = GetLastError();
		m_VMClient[VMIndex].m_Index = -1;
	}

Leave:
	CSocket::OnAccept(nErrorCode);
}

void CVMSocketServer::SendVMClientMessage(SockPacket* sockPacket)
{
	int bytesSent = 0;

	// Search through array to find active vote clients
	for (int i = 0; i < MAX_VM_SOCKETS; i++) {
		if (m_VMClient[i].m_Index != -1) {
			bytesSent = m_VMClient[i].Send((char*)sockPacket, sizeof(SockPacket));
		}
	}
}

void CVMSocketServer::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CSocket::OnClose(nErrorCode);
}
