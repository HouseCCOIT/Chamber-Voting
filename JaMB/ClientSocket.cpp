// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "ClientSocket.h"
#include "VM_Sock.h"

#include "JaMBDlg.h"
#include "MBThread.h"

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
	int action = 2;

	SockPacket* mbPacket = new SockPacket();

	int bytesReceived = Receive(mbPacket, sizeof(SockPacket));

	if (mbPacket->packetType == VM_MB) {
		switch (mbPacket->MBAction) {
		case VM_MB_VOTE_OPEN:
			//((CJaMBDlg*)pDlg)->JaMBVoteOpen();
			JaMBVoteOpen();
			break;

		case VM_MB_VOTE_ACTIVE:
			//((CJaMBDlg*)pDlg)->JaMBVoteActive();
			JaMBVoteActive();
			break;

		case VM_MB_VOTE_RESULTS:
			//((CJaMBDlg*)pDlg)->JaMBVoteResults();
			JaMBVoteResults(mbPacket);
			break;

		default:
			break;
		}
	}

	delete mbPacket;

	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::SendVMMessage(SockPacket *mbPacket)
{
	//TRACE("Entry");

	Send((char*)mbPacket, sizeof(SockPacket));
}