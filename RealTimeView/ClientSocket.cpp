// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "ClientSocket.h"
#include "VM_Sock.h"

#include "RealTimeViewDlg.h"

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

	SockPacket* VideoData = new SockPacket();
	SockPacket sPacket;

	int bytesReceived = Receive(VideoData, sizeof(SockPacket));

	if (VideoData->packetType == VM_VIDEO) {
		switch (VideoData->VideoAction) {
		case VM_VIDEO_SHOW_RESULTS:
			((CRTVDlg*)pDlg)->ShowResults();
			break;

		case VM_VIDEO_SHOW_REALTIME:
			((CRTVDlg*)pDlg)->ShowRealTime();
			break;

		default:
			break;
		}
	}
	else if (VideoData->packetType == VM_VM) {
		int i = 0;
		switch (VideoData->VMAction) {
		case VM_VOTE_CURRENT_VOTE:

			for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
				sPacket.dataByte[i] = VideoData->dataByte[i];
			}

			((CRTVDlg*)pDlg)->UpdateCurrentVote(sPacket);
			break;

		default:
			break;
		}
	}


	//((CRTVDlg*)pDlg)->UpdateButtons(SockData);

	delete VideoData;

	CSocket::OnReceive(nErrorCode);
}
