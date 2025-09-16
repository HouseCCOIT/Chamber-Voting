#if !defined(VM_Sock_h)
#define VM_Sock_h

// Definitions

#define MAX_SEATS 136
#define MAX_VOTE_ARRAY 138
#define MAX_VM_SOCKETS 5

typedef enum _SockPacketType {
   VM_NULL,
   VM_RESPONSE,
   VM_VM,
   VM_MB,
   VM_VIDEO,
   VM_REMOTE
} SockPacketType;

typedef enum _SockVMAction {
   VM_VOTE_DISCONNECT_REMOTE,
   VM_VOTE_RESET_REMOTE,
   VM_VOTE_SETINDEX,
   VM_VOTE_OPEN_REMOTE,
   VM_VOTE_CURRENT_VOTE
} SockVMAction;

typedef enum _SockRemoteAction {
   VM_VOTE_RESET,
   VM_VOTE_NAY,
   VM_VOTE_YEA,
   VM_VOTE_PAGE,
   VM_VOTE_RTS,
   VM_VOTE_GET
} SockRemoteAction;

typedef enum _VideoActionType {
   VM_VIDEO_NULL,
   VM_VIDEO_SHOW_RESULTS,
   VM_VIDEO_SHOW_REALTIME,
   VM_VIDEO_SHUTDOWN
} VideoActionType;

typedef enum _MBActionType {
   VM_MB_NULL,
   VM_MB_VOTE_OPEN,
   VM_MB_VOTE_ACTIVE,
   VM_MB_VOTE_RESULTS,
   VM_MB_SET_HEADER
} MBActionType;

typedef enum _SockVMResponse {
   VM_VOTE_RESET_ACK,
   VM_VOTE_RESET_NAK,
   VM_VOTE_NAY_ACK,
   VM_VOTE_NAY_NAK,
   VM_VOTE_YEA_ACK,
   VM_VOTE_YEA_NAK,
   VM_VOTE_PAGE_ACK,
   VM_VOTE_PAGE_NAK,
   VM_VOTE_RTS_ACK,
   VM_VOTE_RTS_NAK
} SockVMResponse;

typedef enum _SockRemoteResponse {
   VM_VOTE_DISCONNECT_ACK,
   VM_VOTE_DISCONNECT_NAK,
   VM_VOTE_SETINDEX_ACK,
   VM_VOTE_SETINDEX_NAK
} SockRemoteResponse;

typedef struct _SockPacket {
   SockPacketType packetType;
   union {
      SockVMAction     VMAction;
      SockRemoteAction remoteAction;
      VideoActionType  VideoAction;
	   MBActionType     MBAction;
      SockVMResponse   response;
   };
   union {
      unsigned char dataByte[256];
      unsigned int  dataWord[128];
      char          dataChar[256];
      char          header[4][64];
      int           index;
   };
} SockPacket;

typedef struct _SockVMPacket {
   SockPacketType packetType;
   union {
      SockVMAction     VMAction;
      SockRemoteAction remoteAction;
      SockVMResponse   response;
   };
   int index;
} SockVMPacket;

typedef struct _SockRemotePacket {
   SockPacketType packetType;
   union {
      SockVMAction     VMAction;
      SockRemoteAction remoteAction;
      SockVMResponse   response;
   };
   int index;
} SockRemotePacket;


#endif