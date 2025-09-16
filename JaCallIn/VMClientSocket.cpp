// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "VMClientSocket.h"
#include "VM_Sock.h"

#include "JaCallinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVMClientSocket

CVMClientSocket::CVMClientSocket()
{
}

CVMClientSocket::~CVMClientSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CVMClientSocket, CSocket)
	//{{AFX_MSG_MAP(CVMClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CVMClientSocket member functions

void CVMClientSocket::OnReceive(int nErrorCode) 
{
	int action = 2;

	SockPacket* VMData = new SockPacket();
	SockPacket sPacket;

	int bytesReceived = Receive(VMData, sizeof(SockPacket));

   if (VMData->packetType == VM_VM) {
		int i = 0;
		switch (VMData->VMAction) {
		case VM_VOTE_CURRENT_VOTE:

			for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
				sPacket.dataByte[i] = VMData->dataByte[i];
			}

			((CJaCallInDlg*)pDlg)->UpdateCurrentVote(sPacket);
			break;

		default:
			break;
		}
	}


	//((CRTVDlg*)pDlg)->UpdateButtons(SockData);

	delete VMData;

	CSocket::OnReceive(nErrorCode);
}
