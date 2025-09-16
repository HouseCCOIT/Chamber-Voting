// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
//#include "JaRemote.h"
#include "ClientSocket.h"
#include "VM_Sock.h"


#include "JaSperDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions

void CClientSocket::OnReceive(int nErrorCode) 
{
	SockRemotePacket* SockData = new SockRemotePacket();
	int bytesReceived = Receive(SockData, sizeof(SockRemotePacket));

	if (SockData->packetType == VM_VM) {
		switch (SockData->remoteAction) {
		case VM_VOTE_SETINDEX:
			//m_Index = SockData->index;
			break;
		default:
			break;
		}
	}

	((CJaSperDlg*)pDlg)->UpdateButtons(SockData);

	delete SockData;

	CSocket::OnReceive(nErrorCode);
}


