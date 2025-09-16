//-----------------------------------------------------------------------------
//
// O22SIOMM.cpp
// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
// Copyright (c) 1999 - 2018 by Opto 22
//
// Source for the O22SnapIoMemMap C++ class.  
// 
// The O22SnapIoMemMap C++ class is used to communicate from a computer to an
// Opto 22 SNAP Ethernet I/O unit.
//
// See the header file for more information on using this class.
//
// While this class was developed on Microsoft Windows operating systems, it
// is intended to be as generic as possible.  For Windows specific code,
// search for "_WIN32".  For Linux specific code, search for "_LINUX".
//-----------------------------------------------------------------------------

#ifndef __O22SIOMM_H_
#include "O22SIOMM.h"
#endif

#ifdef _WIN32
#define WINSOCK_VERSION_REQUIRED_MAJ 2
#define WINSOCK_VERSION_REQUIRED_MIN 0

#include <tchar.h>
#endif // _WIN32

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#ifdef _LINUX
#include <netinet/tcp.h>
#endif

// Handy little macro to cut down on the number of lines needed for input validation. Call
// as "check(condition, error_code)". If condition evaluates to false, the function will
// immediately return with the given error code
#define check(condition, error_code) if (!(condition)) { return (error_code); }

static uint32_t ExpandedAnalogPointReadBase(int module, int point)
//-------------------------------------------------------------------------------------------------
// Returns the base address to the proper read location for an analog point in the expanded memory
// map.
//-------------------------------------------------------------------------------------------------
{
	return SIOMM4096_APOINT_READ_AREA_BASE +
		(SIOMM4096_APOINT_READ_MODULE_OFFSET * module) +
		(SIOMM4096_APOINT_READ_POINT_OFFSET * point);
}

static uint32_t ExpandedAnalogPointWriteBase(int module, int point)
//-------------------------------------------------------------------------------------------------
// Returns the base address to the proper read/write location for an analog point in the expanded
// memory map
//-------------------------------------------------------------------------------------------------
{
	return SIOMM4096_APOINT_WRITE_AREA_BASE +
		(SIOMM4096_APOINT_WRITE_MODULE_OFFSET * module) +
		(SIOMM4096_APOINT_WRITE_POINT_OFFSET * point);
}

static uint32_t ExpandedAnalogPointReadClearBase(int module, int point)
//-------------------------------------------------------------------------------------------------
// Returns the base address to the read & clear location for an analog point in the expanded 
// memory map.
//-------------------------------------------------------------------------------------------------
{
	return SIOMM4096_APOINT_READ_CLEAR_BASE +
		(SIOMM4096_APOINT_READ_CLEAR_MODULE_OFFSET * module) +
		(SIOMM4096_APOINT_READ_CLEAR_POINT_OFFSET * point);
}

O22SnapIoMemMap::O22SnapIoMemMap()
//-------------------------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------------------------
{
	// Set defaults
	m_Socket = INVALID_SOCKET;
	m_byTransactionLabel = 0;
	m_nRetries = 0;
	m_nOpenTime = 0;
	m_nOpenTimeOutMS = 0;
	m_nTimeOutMS = 1000;
	m_tvTimeOut.tv_sec = m_nTimeOutMS / 1000;
	m_tvTimeOut.tv_usec = (m_nTimeOutMS % 1000) * 1000;
	m_method = ethernet;
	m_serialHandle = 0;
}


O22SnapIoMemMap::~O22SnapIoMemMap()
//-------------------------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------------------------
{
	Close();

#ifdef _WIN32
	WSACleanup();
#endif

}


int O22SnapIoMemMap::OpenEnet(const char * pchIpAddressArg, int nPort, int nOpenTimeOutMS, int nAutoPUC)
//-------------------------------------------------------------------------------------------------
// Open a connection to a SNAP Ethernet I/O unit
//-------------------------------------------------------------------------------------------------
{
	m_nAutoPUCFlag = nAutoPUC;
	m_nConnectionType = SIOMM_TCP;

	return OpenSockets(pchIpAddressArg, nPort, nOpenTimeOutMS);
}


int O22SnapIoMemMap::OpenEnet2(const char * pchIpAddressArg, int nPort, int nOpenTimeOutMS, int nAutoPUC, int nConnectionType)
//-------------------------------------------------------------------------------------------------
// Open a connection to a SNAP Ethernet I/O unit
//-------------------------------------------------------------------------------------------------
{
	m_nAutoPUCFlag = nAutoPUC;
	m_nConnectionType = nConnectionType;

	return OpenSockets(pchIpAddressArg, nPort, nOpenTimeOutMS);
}


int O22SnapIoMemMap::OpenSerial(int nPort, uint8_t nAddress, int nBaudRate, int nOpenTimeOutMS, int nAutoPUC)
//-------------------------------------------------------------------------------------------------
// Open a connection to a Serial I/O unit directly from a PC comm port.
//-------------------------------------------------------------------------------------------------
{
#ifndef _WIN32
	/* Serial connections are only implemented on Windows at the moment. */
	return SIOMM_ERROR_NOT_IMPLEMENTED;

#else

	if (nPort < 0 || nPort >= 256)
		return SIOMM_ERROR_INVALID_SERIAL_ADDRESS;

	Close();
	m_method = serial;
	m_serialAddress = nAddress;
	m_serialBaudRate = nBaudRate;
	m_nAutoPUCFlag = nAutoPUC;
	m_nOpenTimeOutMS = nOpenTimeOutMS;

	// Open COM port using the address parameter
	TCHAR comstring[14];
	_sntprintf(comstring, 14, TEXT("\\\\.\\COM%d"), nPort);

	m_serialHandle = ::CreateFile(comstring, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (m_serialHandle == INVALID_HANDLE_VALUE)
		return SIOMM_ERROR_NOT_CONNECTED;

	// Configure Comm Timeouts TODO: this is windows specific
	COMMTIMEOUTS commTimeouts;
	commTimeouts.ReadIntervalTimeout = MAXDWORD;
	commTimeouts.ReadTotalTimeoutConstant = 0;
	commTimeouts.ReadTotalTimeoutMultiplier = 0;
	commTimeouts.WriteTotalTimeoutConstant = 1000;
	commTimeouts.WriteTotalTimeoutMultiplier = 0;
	::SetCommTimeouts(m_serialHandle, &commTimeouts);

	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	::GetCommState(m_serialHandle, &dcb);
	dcb.fBinary = TRUE;             // binary mode, no EOF check
	dcb.fParity = FALSE;            // parity checking
	dcb.fErrorChar = FALSE;            // enable error replacement
	dcb.fNull = FALSE;            // don't enable null stripping - we need to receive binary data.
	dcb.fAbortOnError = FALSE;            // abort reads/writes on error
	dcb.ErrorChar = 0;                // error replacement character
	dcb.EofChar = 0;                // end of input character
	dcb.EvtChar = 0;                // received event character
	dcb.BaudRate = m_serialBaudRate; // baud rate parameter
	dcb.ByteSize = 8;                // number of bits/byte, 4-8
	dcb.Parity = SPACEPARITY;      // 0-4=no,odd,even,mark,space
	dcb.StopBits = 0;                // 0,1,2 = 1, 1.5, 2
	::SetCommState(m_serialHandle, (DCB *)&(dcb));

	return SIOMM_OK;
#endif // ifndef _WIN32
}


int O22SnapIoMemMap::OpenSerialPassThru(char * pchIpAddressArg, int nPort,
	int nSerialPort, uint8_t nSerialAddress, int nSerialBaudRate, bool bTwoWire,
	int nOpenTimeOutMS, int nAutoPUC, int nConnectionType)
{
	check(pchIpAddressArg != NULL, SIOMM_ERROR);

	Close();

	m_nConnectionType = nConnectionType;

	OpenSockets(pchIpAddressArg, nPort, nOpenTimeOutMS);

	m_method = ethernet;   // we want to see if opening the controller connection is done
	m_nAutoPUCFlag = true; // need to clear the controller puc.
	int hr = IsOpenDone();
	while ((hr != SIOMM_OK) && (hr != SIOMM_TIME_OUT))
	{
#ifdef _WIN32
		Sleep(100);
#else
		usleep(10000);
#endif
		hr = IsOpenDone();
	}

	if (hr == SIOMM_TIME_OUT)
		return hr;

	m_method = passthru;
	m_serialAddress = nSerialAddress;
	m_serialBaudRate = nSerialBaudRate;
	m_serialPort = nSerialPort;
	m_serialTwoWire = bTwoWire;
	m_nAutoPUCFlag = nAutoPUC;

	return SIOMM_OK;
}



int O22SnapIoMemMap::OpenSockets(const char * pchIpAddressArg, int nPort, int nOpenTimeOutMS)
//-------------------------------------------------------------------------------------------------
// Use sockets to open a connection to the SNAP I/O unit
//-------------------------------------------------------------------------------------------------
{
	check(pchIpAddressArg != NULL, SIOMM_ERROR);

	int       nResult; // for checking the return values of functions

#ifdef _WIN32
  // Initialize WinSock.dll
	WSADATA   wsaData; // for checking WinSock

	nResult = WSAStartup(O22MAKEWORD(WINSOCK_VERSION_REQUIRED_MIN,
		WINSOCK_VERSION_REQUIRED_MAJ), &wsaData);
	if (nResult != 0)
	{
		// We couldn't find a socket interface. 
		return SIOMM_ERROR_NO_SOCKETS;
	}

	// Confirm that the WinSock DLL supports WINSOCK_VERSION 
	if ((O22LOBYTE(wsaData.wVersion) != WINSOCK_VERSION_REQUIRED_MAJ) ||
		(O22HIBYTE(wsaData.wVersion) != WINSOCK_VERSION_REQUIRED_MIN))
	{
		// We couldn't find an acceptable socket interface. 
		WSACleanup();
		return SIOMM_ERROR_NO_SOCKETS;
	}
#endif


	// If a socket or other connection is open, close it now.
	Close();

	if (nOpenTimeOutMS < 100)
	{
		m_nOpenTimeOutMS = 100;
	}

	// Setup the socket address structure
	char szPortString[64];
	sprintf(szPortString, "%d", nPort);
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = m_nConnectionType;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	struct addrinfo *pstAddresses;

	int i32Status = getaddrinfo((const char *)pchIpAddressArg, (const char *)szPortString, &hints, &pstAddresses);
	if (i32Status != 0)
	{
		// couldn't resolve the string to an address
#ifdef _WIN32
		WSACleanup();
#endif
		return SIOMM_ERROR_CREATING_SOCKET;
	}

	// Create the socket via TCP or UDP
	// currently, only try the first resolved address
	m_Socket = socket(pstAddresses->ai_family, m_nConnectionType, pstAddresses->ai_protocol);
	if (m_Socket == INVALID_SOCKET)
	{
		// Couldn't create the socket
#ifdef _WIN32
		WSACleanup();
#endif

		return SIOMM_ERROR_CREATING_SOCKET;
	}

	// Make the socket non-blocking
#ifdef _WIN32
  // Windows uses ioctlsocket() to set the socket as non-blocking.
  // Other systems may use ioctl() or fcntl()
	u_long nNonBlocking = 1;
	if (SOCKET_ERROR == ioctlsocket(m_Socket, FIONBIO, &nNonBlocking))
#elif defined _LINUX
	if (-1 == fcntl(m_Socket, F_SETFL, O_NONBLOCK))
#else
	??
#endif
	{
		CloseSockets();

		return SIOMM_ERROR_CREATING_SOCKET;
	}

	// enable TCP_NODELAY, only applies to TCP sockets, UDP ignores this option
#ifdef _WIN32
	int i32TrueFlag = 1;
	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (char *)&i32TrueFlag, sizeof(i32TrueFlag));
#elif defined _LINUX
	int i32TrueFlag = ~0;
	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (char *)&i32TrueFlag, sizeof(i32TrueFlag));
#else
	??
#endif

	// store the address and length
	memcpy(&m_SocketAddressStore, pstAddresses->ai_addr, pstAddresses->ai_addrlen);
	m_SocketAddrLength = pstAddresses->ai_addrlen;
	freeaddrinfo(pstAddresses);
	nResult = connect(m_Socket, (sockaddr*)&m_SocketAddressStore, m_SocketAddrLength);

	if (0 == nResult)
	{
		nResult = SIOMM_OK;
	}
	else
	{
		nResult = GET_LAST_SOCKET_ERROR();

#ifdef _WIN32
		// Getting a WSAEWOULDBLOCK error is expected. See MSDN article on connect()
		if (WSAEWOULDBLOCK == nResult)
			nResult = SIOMM_OK;

#else
		// A non-blocking socket may well return EINPROGRESS, this is normal.
		if (EINPROGRESS == nResult) {
			nResult = SIOMM_OK;
		}
#endif
	}

	// Get the time for the timeout logic in IsOpenDone()
	m_nOpenTime = GetTickCount();

	return nResult;
}


int O22SnapIoMemMap::CloseSockets()
//-------------------------------------------------------------------------------------------------
// Close the sockets connection
//-------------------------------------------------------------------------------------------------
{
	int nResult = 0;

	// Close up everything
	if (m_Socket != INVALID_SOCKET)
	{
#ifdef _WIN32
		nResult = closesocket(m_Socket);
#elif defined _LINUX
		nResult = close(m_Socket);
#else
		? ?
#endif
	}

	if (0 == nResult)
	{
		nResult = SIOMM_OK;
	}
	else if (SOCKET_ERROR == nResult)
	{
		nResult = GET_LAST_SOCKET_ERROR();
	}
	else
	{
		nResult = SIOMM_ERROR_UNEXPECTED_CLOSE_RESULT;
	}

	// Reset our data members
	memset(&m_SocketAddressStore, 0, sizeof(m_SocketAddressStore));
	m_SocketAddrLength = 0;
	m_Socket = INVALID_SOCKET;
	m_nOpenTimeOutMS = 0;
	m_nOpenTime = 0;
	m_nRetries = 0;


	return SIOMM_OK;
}

int O22SnapIoMemMap::IsOpenDone()
//-------------------------------------------------------------------------------------------------
// Called after an OpenEnet() function to determine if the open process is completed yet.
//-------------------------------------------------------------------------------------------------
{
	int    nResult;     // for checking the return values of functions

	if (m_method == ethernet)
	{
		fd_set  fds;
		timeval tvTimeOut;
		int    nSelectResult;

		// Check the open timeout
		uint32_t nOpenTimeOutTemp = GetTickCount();

		// Check for overflow of the system timer
		if (m_nOpenTime > nOpenTimeOutTemp)
			m_nOpenTime = 0;

		// Check for timeout
		if (m_nOpenTimeOutMS < (nOpenTimeOutTemp - m_nOpenTime))
		{
			// Timeout has occured.
			CloseSockets();
			return SIOMM_TIME_OUT;
		}

		FD_ZERO(&fds);
		FD_SET(m_Socket, &fds);

		// We want the select to return immediately, so set the timeout to zero
		tvTimeOut.tv_sec = 0;
		tvTimeOut.tv_usec = 100;

		// Use select() to check if the socket is connected and ready
		nSelectResult = select((int)m_Socket + 1, NULL, &fds, NULL, &tvTimeOut);

		// nSelectResult should be 1
		if (1 != nSelectResult)
		{
			if (0 == nSelectResult)
			{
				nResult = SIOMM_ERROR_NOT_CONNECTED_YET;
			}
			else if (SOCKET_ERROR == nSelectResult)
			{
				// Get a specific error (if possible)
				nResult = GET_LAST_SOCKET_ERROR();
			}
			else
			{
				nResult = SIOMM_ERROR_UNEXPECTED_SELECT_RESULT;
			}

			return nResult;
		}
	}

	// Okay, we must be connected if we get past the select() above, or if we're using serial.
	uint32_t   dwPUCFlag;   // a flag for checking the status of PowerUp Clear on the I/O unit

	if (m_nAutoPUCFlag)
	{
		// Now, check the PowerUp Clear flag of the brain.  It must be cleared 
		// before can do anything with it (other than read the status area.)

		// Read PowerUp Clear flag
		nResult = ReadQuad(SIOMM_STATUS_READ_PUC_FLAG, &dwPUCFlag);

		// Check for good result from the ReadQuad()
		if (SIOMM_OK == nResult)
		{
			// the PUC flag will be TRUE if PUC is needed
			if (dwPUCFlag)
			{
				// Send PUC
				nResult = WriteQuad(SIOMM_STATUS_WRITE_OPERATION, 1);

				return nResult;
			}
			else
			{
				// Everything must be okay
				return SIOMM_OK;
			}
		}
		else // the ReadQuad() had an error
		{
			return nResult;
		}
	}
	else
	{
		// Everything must be okay
		return SIOMM_OK;
	}
}


int O22SnapIoMemMap::SetCommOptions(int nTimeOutMS, int nReserved)
//-------------------------------------------------------------------------------------------------
// Set communication options
//-------------------------------------------------------------------------------------------------
{
	// m_nRetries   = nReserved;
	m_nTimeOutMS = nTimeOutMS;

	// Set the timeout that sockets will use
	m_tvTimeOut.tv_sec = m_nTimeOutMS / 1000;
	m_tvTimeOut.tv_usec = (m_nTimeOutMS % 1000) * 1000;

	return SIOMM_OK;
}


int O22SnapIoMemMap::BuildReadBlockRequest(uint8_t * pbyReadBlockRequest,
	uint8_t   byTransactionLabel,
	uint32_t  dwDestinationOffset,
	uint16_t   wDataLength)
	//-------------------------------------------------------------------------------------------------
	// Build a read block request packet
	//-------------------------------------------------------------------------------------------------
{
	check(pbyReadBlockRequest != NULL, SIOMM_ERROR);

	// Destination Id
	pbyReadBlockRequest[0] = 0x00;
	pbyReadBlockRequest[1] = 0x00;

	// Transaction Label
	pbyReadBlockRequest[2] = byTransactionLabel << 2;

	// Transaction Code
	pbyReadBlockRequest[3] = SIOMM_TCODE_READ_BLOCK_REQUEST << 4;

	// Source Id
	pbyReadBlockRequest[4] = 0x00;
	pbyReadBlockRequest[5] = 0x00;

	// Destination Offset
	pbyReadBlockRequest[6] = 0xFF;
	pbyReadBlockRequest[7] = 0xFF;
	pbyReadBlockRequest[8] = O22BYTE0(dwDestinationOffset);
	pbyReadBlockRequest[9] = O22BYTE1(dwDestinationOffset);
	pbyReadBlockRequest[10] = O22BYTE2(dwDestinationOffset);
	pbyReadBlockRequest[11] = O22BYTE3(dwDestinationOffset);

	// Data length
	pbyReadBlockRequest[12] = O22HIBYTE(wDataLength);
	pbyReadBlockRequest[13] = O22LOBYTE(wDataLength);

	// Extended Transaction Code
	pbyReadBlockRequest[14] = 0x00;
	pbyReadBlockRequest[15] = 0x00;

	return SIOMM_OK;
}


int O22SnapIoMemMap::BuildWriteBlockRequest(uint8_t *  pbyWriteBlockRequest,
	uint8_t    byTransactionLabel,
	uint32_t   dwDestinationOffset,
	uint16_t    wDataLength,
	uint8_t  * pbyBlockData)
	//-------------------------------------------------------------------------------------------------
	// Build a write block request packet
	//-------------------------------------------------------------------------------------------------
{
	check(pbyWriteBlockRequest != NULL, SIOMM_ERROR);
	check(pbyBlockData != NULL, SIOMM_ERROR);

	// Destination Id
	pbyWriteBlockRequest[0] = 0x00;
	pbyWriteBlockRequest[1] = 0x00;

	// Transaction Label
	pbyWriteBlockRequest[2] = byTransactionLabel << 2;

	// Transaction Code
	pbyWriteBlockRequest[3] = SIOMM_TCODE_WRITE_BLOCK_REQUEST << 4;

	// Source Id
	pbyWriteBlockRequest[4] = 0x00;
	pbyWriteBlockRequest[5] = 0x00;

	// Destination Offset
	pbyWriteBlockRequest[6] = 0xFF;
	pbyWriteBlockRequest[7] = 0xFF;
	pbyWriteBlockRequest[8] = O22BYTE0(dwDestinationOffset);
	pbyWriteBlockRequest[9] = O22BYTE1(dwDestinationOffset);
	pbyWriteBlockRequest[10] = O22BYTE2(dwDestinationOffset);
	pbyWriteBlockRequest[11] = O22BYTE3(dwDestinationOffset);

	// Data length
	pbyWriteBlockRequest[12] = O22HIBYTE(wDataLength);
	pbyWriteBlockRequest[13] = O22LOBYTE(wDataLength);

	// Extended Transaction Code
	pbyWriteBlockRequest[14] = 0x00;
	pbyWriteBlockRequest[15] = 0x00;

	// Block data to write
	memcpy(&(pbyWriteBlockRequest[16]), pbyBlockData, wDataLength);


	return SIOMM_OK;
}



int O22SnapIoMemMap::UnpackReadBlockResponse(uint8_t  * pbyReadBlockResponse,
	uint8_t  * pbyTransactionLabel,
	uint8_t  * pbyResponseCode,
	uint16_t  * pwDataLength,
	uint8_t  * pbyBlockData)
	//-------------------------------------------------------------------------------------------------
	// Unpack a read block response from the brain
	//-------------------------------------------------------------------------------------------------
{
	check(pbyReadBlockResponse != NULL, SIOMM_ERROR);
	check(pbyTransactionLabel != NULL, SIOMM_ERROR);
	check(pbyResponseCode != NULL, SIOMM_ERROR);
	check(pwDataLength != NULL, SIOMM_ERROR);
	check(pbyBlockData != NULL, SIOMM_ERROR);

	// Check for correct Transaction Code
	uint8_t byTransactionCode = pbyReadBlockResponse[3] >> 4;
	if (SIOMM_TCODE_READ_BLOCK_RESPONSE != byTransactionCode)
	{
		return SIOMM_ERROR_INCORRECT_TCODE_RECIEVED;
	}

	*pbyTransactionLabel = pbyReadBlockResponse[2];
	*pbyTransactionLabel >>= 2;

	*pbyResponseCode = pbyReadBlockResponse[6];
	*pbyResponseCode >>= 4;

	if (pwDataLength != NULL)
	{
		*pwDataLength = O22MAKEWORD(pbyReadBlockResponse[12], pbyReadBlockResponse[13]);

		if (*pwDataLength > 0)
			memcpy(pbyBlockData, &(pbyReadBlockResponse[16]), *pwDataLength);
	}

	return SIOMM_OK;
}



int O22SnapIoMemMap::UnpackWriteResponse(uint8_t  * pbyWriteResponse,
	uint8_t  * pbyTransactionLabel,
	uint8_t  * pbyResponseCode)
	//-------------------------------------------------------------------------------------------------
	// Unpack a write response from the brain
	//-------------------------------------------------------------------------------------------------
{
	check(pbyWriteResponse != NULL, SIOMM_ERROR);
	check(pbyTransactionLabel != NULL, SIOMM_ERROR);
	check(pbyResponseCode != NULL, SIOMM_ERROR);

	// Check for correct Transaction Code
	uint8_t byTransactionCode = pbyWriteResponse[3] >> 4;
	if (SIOMM_TCODE_WRITE_RESPONSE != byTransactionCode)
	{
		return SIOMM_ERROR;
	}

	// Unpack Transaction Label
	*pbyTransactionLabel = pbyWriteResponse[2];
	*pbyTransactionLabel >>= 2;

	// Unpack Response Code
	*pbyResponseCode = pbyWriteResponse[6];
	*pbyResponseCode >>= 4;

	return SIOMM_OK;
}


int O22SnapIoMemMap::ReadFloat(uint32_t dwDestOffset, float * pfValue)
//-------------------------------------------------------------------------------------------------
// Read a float value from a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	uint32_t dwQuadlet; // A temp for getting the read value
	int  nResult;   // for checking the return values of functions

	nResult = ReadQuad(dwDestOffset, &dwQuadlet);

	// Check the result
	if (nResult == SIOMM_OK)
	{
		// If the ReadQuad was OK, copy the data
		memcpy(pfValue, &dwQuadlet, 4);
	}

	return nResult;
}


int O22SnapIoMemMap::WriteFloat(uint32_t dwDestOffset, float fValue)
//-------------------------------------------------------------------------------------------------
// Write a float value to a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	uint32_t dwQuadlet; // A temp for setting the write value

	// Copy the float into a uint32_t for easy manipulation
	memcpy(&dwQuadlet, &fValue, 4);

	return WriteQuad(dwDestOffset, dwQuadlet);
}


int O22SnapIoMemMap::ReadBlock(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
//-------------------------------------------------------------------------------------------------
// Read a block of data from a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	check(pbyData != NULL, SIOMM_ERROR);

	if (m_method == ethernet)
		return ReadBlockEthernet(dwDestOffset, wDataLength, pbyData);
	else if (m_method == serial)
		return ReadBlockSerial(dwDestOffset, wDataLength, pbyData);
	else if (m_method == passthru)
		return ReadBlockPassThru(dwDestOffset, wDataLength, pbyData);
	else
		return SIOMM_ERROR_NOT_CONNECTED;
}


int O22SnapIoMemMap::ReadBlockEthernet(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
{
	check(pbyData != NULL, SIOMM_ERROR);

	// check that we have a valid socket
	check(m_Socket != INVALID_SOCKET, SIOMM_ERROR_NOT_CONNECTED);

	uint8_t  byReadBlockRequest[SIOMM_SIZE_READ_BLOCK_REQUEST];
	uint8_t *pbyReadBlockResponse;
	uint8_t  byTransactionLabel;
	uint8_t  byResponseCode;
	uint8_t *pbyDataTemp;
	int  nResult;
	int  nSendResult;
	int  nSelectResult;
	int  nRecvResult;
	uint16_t  wDataLengthRead;
	fd_set fds;
	uint32_t   nStartTime, nCurrentTime, nTimeOutLeft;
#ifdef _LINUX
	tms   DummyTime;
#endif

	// Allocate the response buffer
	pbyReadBlockResponse = new uint8_t[SIOMM_BLOCK_MAX_BYTES + SIOMM_SIZE_READ_BLOCK_RESPONSE];
	if (pbyReadBlockResponse == NULL)
		return SIOMM_ERROR_OUT_OF_MEMORY; // Couldn't allocate memory!

	  // Allocate a temporary data buffer
	pbyDataTemp = new uint8_t[wDataLength];
	if (pbyDataTemp == NULL)
	{
		delete[] pbyReadBlockResponse;

		return SIOMM_ERROR_OUT_OF_MEMORY; // Couldn't allocate memory!
	}


	FD_ZERO(&fds);
	FD_SET(m_Socket, &fds);

	uint32_t dwCurrentDestOffset = dwDestOffset;
	int   nReadPartialLength = SIOMM_BLOCK_MAX_BYTES; // This is the largest read size. 
													   // The last (or only) one might be smaller
	int   nReadTotalLength = wDataLength;           // The number of bytes to read
	int   nBytesRead = 0;                     // The number of bytes read so far
	int   nTimeOutLeftMS = m_nTimeOutMS;          // The total time for the read(s)

	// Get the start time
	nStartTime = GetTickCount();

	while (dwCurrentDestOffset < (dwDestOffset + nReadTotalLength))
	{
		// Check the timeout
		nCurrentTime = GetTickCount();
		nTimeOutLeft = m_nTimeOutMS - (nCurrentTime - nStartTime);
		if (nTimeOutLeft > m_nTimeOutMS)
		{
			// Timeout has occured.
			nResult = SIOMM_TIME_OUT;
			break;
		}

		// Get the correct length to read
		if (nReadPartialLength > (nReadTotalLength - nBytesRead))
			nReadPartialLength = nReadTotalLength - nBytesRead;

		// Increment the transaction label
		UpdateTransactionLabel();

		// Build the request packet
		BuildReadBlockRequest(byReadBlockRequest, m_byTransactionLabel, dwCurrentDestOffset, nReadPartialLength);

		// Send the packet to the Snap I/O unit
		nSendResult = send(m_Socket, (char*)byReadBlockRequest, SIOMM_SIZE_READ_BLOCK_REQUEST, 0 /*??*/);
		if (SOCKET_ERROR == nSendResult)
		{
			// This probably means we're not connected.
			// Get a specific error 
			nResult = GET_LAST_SOCKET_ERROR();
			break;
		}
		else if (nSendResult != SIOMM_SIZE_READ_BLOCK_REQUEST)
		{
			// The wrong number of bytes was sent by send()...
			nResult = SIOMM_ERROR_INCORRECT_LENGTH_SENT;
			break;
		}

		// Reset the timeout values
		m_tvTimeOut.tv_sec = nTimeOutLeft / 1000;
		m_tvTimeOut.tv_usec = (nTimeOutLeft % 1000) * 1000;

		// Is the recv ready?
		nSelectResult = select((int)m_Socket + 1, &fds, NULL, NULL, &m_tvTimeOut);

		// nSelectResult should be 1
		if (1 != nSelectResult)
		{
			if (0 == nSelectResult)
			{
				nResult = SIOMM_TIME_OUT;
			}
			else if (SOCKET_ERROR == nSelectResult)
			{
				// Get a specific error (if possible)
				nResult = GET_LAST_SOCKET_ERROR();
			}
			else
			{
				nResult = SIOMM_ERROR_UNEXPECTED_SELECT_RESULT;
			}

			break;
		}

		// The response is ready, so recv it.
		nRecvResult = recv(m_Socket, (char*)pbyReadBlockResponse,
			nReadPartialLength + SIOMM_SIZE_READ_BLOCK_RESPONSE, 0);


		// Check for errors from recv()
		if (nRecvResult <= 0)
		{
			if (SOCKET_ERROR == nRecvResult)
			{
				// Get a specific error
				nResult = GET_LAST_SOCKET_ERROR();
			}
			else if (0 == nRecvResult)
			{
				nResult = SIOMM_ERROR_GRACEFULLY_CLOSED;
			}
			else
			{
				nResult = SIOMM_ERROR_UNEXPECTED_RECV_RESULT;
			}

			break;
		}


		if ((nReadPartialLength + SIOMM_SIZE_READ_BLOCK_RESPONSE) != nRecvResult)
		{
			// we got the wrong number of bytes back!

			// Check for a valid error response
			if (nRecvResult >= SIOMM_SIZE_READ_BLOCK_RESPONSE)
			{
				// We have enough bytes to try to get some useful information back.

				// The most likely normal situation that would get here is requesting
				// an invalid memory location. The brain will send back a response
				// packet but no read data. The response code will be 5, or error
				// code 0xE005.

				nResult = UnpackReadBlockResponse(pbyReadBlockResponse,
					&byTransactionLabel, &byResponseCode,
					NULL, NULL);

				if (SIOMM_OK == nResult)
				{
					// Make sure that the received transaction label matches the
					// sent one.
					if (m_byTransactionLabel == byTransactionLabel)
					{
						if (SIOMM_RESPONSE_CODE_ACK == byResponseCode)
							// We've received what looks like a good response along with
							// an ACK, but since the length is wrong, we need to return
							// an error.
							nResult = SIOMM_ERROR_WRONG_NUM_BYTES_RETURNED;
						else
							// We've received what looks like a good response and we have 
							// what appears to be an error from the brain, so return that
							// error.
							nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
					}
					else
					{
						nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
					}
				}
			}
			else
			{
				nResult = SIOMM_ERROR_WRONG_NUM_BYTES_RETURNED;
			}

			break;
		}


		// Unpack the response and get the data
		nResult = UnpackReadBlockResponse(pbyReadBlockResponse,
			&byTransactionLabel, &byResponseCode,
			&wDataLengthRead, pbyDataTemp + nBytesRead);

		// Check that the response was okay
		if (SIOMM_OK == nResult)
		{
			if (m_byTransactionLabel != byTransactionLabel)
			{
				nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
				break;
			}
			else if (SIOMM_RESPONSE_CODE_ACK != byResponseCode)
			{
				nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
				break;
			}
		}
		else
		{
			// Something went wrong with UnpackReadBlockResponse(). 
			// Just leave its result in nResult and break out of the loop.
			break;
		}

		dwCurrentDestOffset += nReadPartialLength;
		nBytesRead += nReadPartialLength;
	}

	if (SIOMM_OK == nResult)
	{
		// All the reads were successful, so now copy all the data that was read
		memcpy(pbyData, pbyDataTemp, wDataLength);
	}

	// Clean up memory
	delete[] pbyReadBlockResponse;
	delete[] pbyDataTemp;

	return nResult;
}


int O22SnapIoMemMap::ReadBlockSerial(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
{
#ifndef _WIN32
	/* Serial support is only implemented on Windows systems at the moment. */
	return SIOMM_ERROR_NOT_IMPLEMENTED;

#else
	check(pbyData != NULL, SIOMM_ERROR);

	// Check that we have a valid serial handle.
	check(m_serialHandle != 0, SIOMM_ERROR_NOT_CONNECTED);

	// Build the request packet
	uint8_t  byReadBlockRequest[SIOMM_SIZE_READ_BLOCK_REQUEST];
	UpdateTransactionLabel();  // Increment the transaction label
	BuildReadBlockRequest(byReadBlockRequest, m_byTransactionLabel, dwDestOffset, wDataLength);

	SendSerialMessage(byReadBlockRequest, SIOMM_SIZE_READ_BLOCK_REQUEST);

	// receive
	uint8_t bySerialResponse[sizeof(SERIAL_HEADER_R_T) + SIOMM_BLOCK_MAX_BYTES + SIOMM_SIZE_READ_BLOCK_RESPONSE];
	uint32_t dwResponseLength = SIOMM_SIZE_READ_BLOCK_RESPONSE + wDataLength + sizeof(SERIAL_HEADER_R_T);
	uint32_t dwResponseReceived = 0;
	ULONG readSize = 0;
	SERIAL_HEADER_R_T * pReceiveHeader = (SERIAL_HEADER_R_T *)bySerialResponse;

	// timeout setup
	uint32_t dwStopTime = GetTickCount() + m_nTimeOutMS;

	while (dwResponseLength > 0)
	{
		if (::ReadFile(m_serialHandle, bySerialResponse + dwResponseReceived, dwResponseLength, &readSize, 0) <= 0)
			return GetLastError();

		dwResponseReceived += readSize;
		dwResponseLength -= readSize;

		if (dwResponseReceived > sizeof(SERIAL_HEADER_R_T))
		{
			if (ntohs(pReceiveHeader->m_u16Length) <= (dwResponseReceived - sizeof(SERIAL_HEADER_R_T)))
			{
				break;
			}
		}

		// Check if we timed out.
		if (dwStopTime < GetTickCount())
			return SIOMM_ERROR_SERIAL_TIMEOUT;
	}

	// Make sure the CRC matches
	uint16_t u16MessageCrc = ntohs(pReceiveHeader->m_u16Crc);
	pReceiveHeader->m_u16Crc = 0;
	if (u16MessageCrc != Crc16R(bySerialResponse, ntohs(pReceiveHeader->m_u16Length) + sizeof(SERIAL_HEADER_R_T)))
		return SIOMM_ERROR_SERIAL_MISMATCHED_CRC;

	uint8_t * pbyReadData = bySerialResponse + sizeof(SERIAL_HEADER_R_T);
	int nResult = 0;
	if (ntohs(pReceiveHeader->m_u16Length) != dwResponseReceived - sizeof(SERIAL_HEADER_R_T))
	{
		if (SIOMM_SIZE_READ_BLOCK_RESPONSE < dwResponseReceived - sizeof(SERIAL_HEADER_R_T))
		{
			// we have enough data to get a real return code
			uint8_t byTransactionLabel;
			uint8_t byResponseCode;
			nResult = UnpackReadBlockResponse(pbyReadData, &byTransactionLabel, &byResponseCode, NULL, NULL);

			if (SIOMM_OK == nResult)
			{
				if (m_byTransactionLabel != byTransactionLabel)
					nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
				else if (SIOMM_RESPONSE_CODE_ACK != byResponseCode)
					// We've received what looks like a good response and we have 
					// what appears to be an error from the brain, so return that
					// error.
					nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
			}
		}
		else
			nResult = SIOMM_ERROR_WRONG_NUM_BYTES_RETURNED;

		return nResult;
	}

	uint8_t byTransactionLabel;
	uint8_t byResponseCode;
	uint16_t dwDataLength;
	nResult = UnpackReadBlockResponse(pbyReadData, &byTransactionLabel,
		&byResponseCode, &dwDataLength, pbyData);

	if (SIOMM_OK == nResult)
	{
		if (m_byTransactionLabel != byTransactionLabel)
			nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
		else if (SIOMM_RESPONSE_CODE_ACK != byResponseCode)
			nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
	}

	return nResult;
#endif
}


int O22SnapIoMemMap::ReadBlockPassThru(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
{
	check(pbyData != NULL, SIOMM_ERROR);

	short passThruBufferSize = SIOMM_BLOCK_MAX_BYTES - SIOMM_SIZE_READ_BLOCK_RESPONSE - 12;
	// if the data requested is too int, break it up into smaller pieces
	if (wDataLength > passThruBufferSize)
	{
		uint32_t subOffset = dwDestOffset;
		uint8_t * pbySubData = pbyData;
		uint16_t remainingLength = wDataLength;

		int nResult;
		while ((remainingLength != 0) && (remainingLength <= wDataLength))
		{
			if (remainingLength > passThruBufferSize)
			{
				nResult = ReadBlockPassThru(subOffset, passThruBufferSize, pbySubData);
				subOffset += passThruBufferSize;
				remainingLength -= passThruBufferSize;
				pbySubData += passThruBufferSize;
			}
			else
			{
				nResult = ReadBlockPassThru(subOffset, remainingLength, pbySubData);
				subOffset += remainingLength;
				remainingLength -= remainingLength;
				pbySubData += remainingLength;
			}

			if (nResult != SIOMM_OK)
				return nResult;
		}

		return nResult;
	}

	SERIAL_PASS_THRU_CONFIG config;
	config.Enable = htonl(1);
	config.Bps = htonl(m_serialBaudRate);
	config.DataBits = htonl(8);
	config.Parity = htonl(0x00004D44);
	config.StopBits = htonl(1);
	config.Duplex = htonl(m_serialTwoWire ? 0x00000048 : 0x00000046);
	config.Timeout = htonl(m_nTimeOutMS);

	// Build the request packet
	uint8_t  byReadBlockRequest[SIOMM_SIZE_READ_BLOCK_REQUEST];
	UpdateTransactionLabel();  // Increment the transaction label
	uint8_t expectedTransactionLabel = m_byTransactionLabel;
	BuildReadBlockRequest(byReadBlockRequest, m_byTransactionLabel, dwDestOffset, wDataLength);

	uint8_t * pRequest = new uint8_t[sizeof(SERIAL_PASS_THRU_RW) + SIOMM_SIZE_READ_BLOCK_REQUEST];
	memcpy(pRequest + sizeof(SERIAL_PASS_THRU_RW), byReadBlockRequest, SIOMM_SIZE_READ_BLOCK_REQUEST);

	SERIAL_PASS_THRU_RW * pReadWrite = (SERIAL_PASS_THRU_RW *)pRequest;
	pReadWrite->Address = htonl(m_serialAddress);
	pReadWrite->Type = htonl(2);
	pReadWrite->Length = htonl(SIOMM_SIZE_READ_BLOCK_REQUEST);

	// tell the controller that we're doing a pass through packet
	int nResult = WriteBlockEthernet(GetPassThruConfigAddress(m_serialPort), sizeof(SERIAL_PASS_THRU_CONFIG), (uint8_t *)&config);

	if (nResult == SIOMM_OK)
	{
		// send the read request
		nResult = WriteBlockEthernet(GetPassThruReadWriteAddress(m_serialPort), sizeof(SERIAL_PASS_THRU_RW) + SIOMM_SIZE_READ_BLOCK_REQUEST, pRequest);

		if (nResult == SIOMM_OK)
		{
			uint8_t byReadBlockResponse[SIOMM_BLOCK_MAX_BYTES + SIOMM_SIZE_READ_BLOCK_RESPONSE];
			uint16_t wResponseLength = SIOMM_SIZE_READ_BLOCK_RESPONSE + wDataLength + 4;

			nResult = ReadBlockEthernet(GetPassThruReadWriteAddress(m_serialPort), wResponseLength, byReadBlockResponse);

			if (nResult == SIOMM_OK)
			{
				uint8_t byTransactionLabel;
				uint8_t byResponseCode;
				uint16_t dwDataLength;
				nResult = UnpackReadBlockResponse(byReadBlockResponse + 4, &byTransactionLabel,
					&byResponseCode, &dwDataLength, pbyData);

				if (nResult == SIOMM_OK)
				{
					if (byTransactionLabel != expectedTransactionLabel)
						nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
					else if (byResponseCode != SIOMM_RESPONSE_CODE_ACK)
						nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
				}
			}
		}
	}

	// tell the controller that we're done
	config.Enable = 0;
	WriteBlockEthernet(GetPassThruConfigAddress(m_serialPort), sizeof(SERIAL_PASS_THRU_CONFIG), (uint8_t *)&config);

	delete[] pRequest;
	return nResult;
}


int O22SnapIoMemMap::ReadQuad(uint32_t dwDestOffset, uint32_t * pdwQuadlet)
//-------------------------------------------------------------------------------------------------
// Read a quadlet of data from a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	check(pdwQuadlet != NULL, SIOMM_ERROR);

	uint8_t arrbyData[sizeof(uint32_t)];
	int nResult;

	nResult = ReadBlock(dwDestOffset, sizeof(uint32_t), arrbyData);

	if (SIOMM_OK == nResult)
	{
		*pdwQuadlet = O22MAKELONG2(arrbyData, 0);
	}

	return nResult;
}


int O22SnapIoMemMap::WriteBlock(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
//-------------------------------------------------------------------------------------------------
// Write a block of data to a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	check(pbyData != NULL, SIOMM_ERROR);

	if (m_method == ethernet)
		return WriteBlockEthernet(dwDestOffset, wDataLength, pbyData);
	else if (m_method == serial)
		return WriteBlockSerial(dwDestOffset, wDataLength, pbyData);
	else if (m_method == passthru)
		return WriteBlockPassThru(dwDestOffset, wDataLength, pbyData);
	else
		return SIOMM_ERROR_NOT_CONNECTED;
}


int O22SnapIoMemMap::WriteBlockEthernet(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
{
	check(pbyData != NULL, SIOMM_ERROR);

	// Check that we have a valid socket
	check(m_Socket != INVALID_SOCKET, SIOMM_ERROR_NOT_CONNECTED);

	uint8_t  byWriteBlockResponse[SIOMM_SIZE_WRITE_RESPONSE];
	uint8_t  byWriteBlockRequest[SIOMM_SIZE_WRITE_BLOCK_REQUEST + SIOMM_BLOCK_MAX_BYTES];
	uint8_t  byTransactionLabel;
	uint8_t  byResponseCode;
	int  nResult;
	int  nSendResult;
	int  nSelectResult;
	int  nRecvResult;
	fd_set fds;
	uint32_t   nStartTime, nCurrentTime, nTimeOutLeft;
#ifdef _LINUX
	tms   DummyTime;
#endif

	FD_ZERO(&fds);
	FD_SET(m_Socket, &fds);

	uint32_t dwCurrentDestOffset = dwDestOffset;
	int   nWritePartialLength = SIOMM_BLOCK_MAX_BYTES; // This is the largest write size. 
													   // The last (or only) one might be smaller
	int   nWriteTotalLength = wDataLength;           // The number of bytes to write
	int   nBytesWritten = 0;                     // The number of bytes written so far
	int   nTimeOutLeftMS = m_nTimeOutMS;          // The total time for the write(s)

	// Get the start time
	nStartTime = GetTickCount();

	while (dwCurrentDestOffset < (dwDestOffset + nWriteTotalLength))
	{
		// Check the timeout
		nCurrentTime = GetTickCount();
		nTimeOutLeft = m_nTimeOutMS - (nCurrentTime - nStartTime);
		if (nTimeOutLeft > m_nTimeOutMS)
		{
			// Timeout has occured.
			nResult = SIOMM_TIME_OUT;
			break;
		}


		// Get the correct length to write
		if (nWritePartialLength > (nWriteTotalLength - nBytesWritten))
			nWritePartialLength = nWriteTotalLength - nBytesWritten;

		// Increment the transaction label
		UpdateTransactionLabel();

		// Build the write request packet
		BuildWriteBlockRequest(byWriteBlockRequest, m_byTransactionLabel,
			dwCurrentDestOffset, nWritePartialLength, pbyData + nBytesWritten);

		// Send the packet to the Snap I/O unit
		nSendResult = send(m_Socket, (char*)byWriteBlockRequest, SIOMM_SIZE_WRITE_BLOCK_REQUEST + nWritePartialLength, 0 /*??*/);

		if (SOCKET_ERROR == nSendResult)
		{
			// This probably means we're not connected.
			// Get a specific error 
			nResult = GET_LAST_SOCKET_ERROR();

			break;
		}
		else
		{
			if (nSendResult != (SIOMM_SIZE_WRITE_BLOCK_REQUEST + nWritePartialLength))
			{
				// The wrong number of bytes was sent by send()...
				nResult = SIOMM_ERROR_INCORRECT_LENGTH_SENT;

				break;
			}
		}

		// Reset the timeout values
		m_tvTimeOut.tv_sec = nTimeOutLeft / 1000;
		m_tvTimeOut.tv_usec = (nTimeOutLeft % 1000) * 1000;

		// Is the recv ready?
		nSelectResult = select((int)m_Socket + 1, &fds, NULL, NULL, &m_tvTimeOut);

		// nSelectResult should be 1
		if (1 != nSelectResult)
		{
			if (0 == nSelectResult)
			{
				nResult = SIOMM_TIME_OUT;
			}
			else if (SOCKET_ERROR == nSelectResult)
			{
				// Get a specific error (if possible)
				nResult = GET_LAST_SOCKET_ERROR();
			}
			else
			{
				nResult = SIOMM_ERROR_UNEXPECTED_SELECT_RESULT;
			}

			break;
		}


		// The response is ready, so recv it.
		nRecvResult = recv(m_Socket, (char*)byWriteBlockResponse, SIOMM_SIZE_WRITE_RESPONSE, 0 /*??*/);

		// Check for errors from recv()
		if (nRecvResult <= 0)
		{
			if (SOCKET_ERROR == nRecvResult)
			{
				// Get a specific error (if possible)
				nResult = GET_LAST_SOCKET_ERROR();
			}
			else if (0 == nRecvResult)
			{
				nResult = SIOMM_ERROR_GRACEFULLY_CLOSED;
			}
			else
			{
				nResult = SIOMM_ERROR_UNEXPECTED_RECV_RESULT;
			}

			break;
		}


		if (SIOMM_SIZE_WRITE_RESPONSE != nRecvResult)
		{
			// we got the wrong number of bytes back!

			// Check for a valid error response
			if (nRecvResult > SIOMM_SIZE_WRITE_RESPONSE)
			{
				// We have enough bytes to try to get some useful information back.

				// The most likely normal situation that would get here is requesting
				// an invalid memory location. The brain will send back a response
				// packet but no read data. The response code will be 5, or error
				// code 0xE005.

				nResult = UnpackWriteResponse(byWriteBlockResponse,
					&byTransactionLabel, &byResponseCode);

				if (SIOMM_OK == nResult)
				{
					// Make sure that the received transaction label matches the
					// sent one.
					if (m_byTransactionLabel == byTransactionLabel)
					{
						if (SIOMM_RESPONSE_CODE_ACK == byResponseCode)
							// We've received what looks like a good response along with
							// an ACK, but since the length is wrong, we need to return
							// an error.
							nResult = SIOMM_ERROR_WRONG_NUM_BYTES_RETURNED;
						else
							// We've received what looks like a good response and we have 
							// what appears to be an error from the brain, so return that
							// error.
							nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
					}
					else
					{
						nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
					}
				}
			}
			else
			{
				nResult = SIOMM_ERROR_WRONG_NUM_BYTES_RETURNED;
			}

			break;
		}


		// Unpack the response
		nResult = UnpackWriteResponse(byWriteBlockResponse, &byTransactionLabel,
			&byResponseCode);

		// Check that the response was okay
		if (SIOMM_OK == nResult)
		{
			if (m_byTransactionLabel != byTransactionLabel)
			{
				nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
				break;
			}
			else if (SIOMM_RESPONSE_CODE_ACK != byResponseCode)
			{
				nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
				break;
			}
		}
		else
		{
			// Something went wrong with UnpackWriteResponse(). 
			// Just leave its result in nResult and break out of the loop.
			break;
		}

		dwCurrentDestOffset += nWritePartialLength;
		nBytesWritten += nWritePartialLength;
	}

	return nResult;
}


int O22SnapIoMemMap::SendSerialMessage(uint8_t * pMessage, const uint16_t wMessageLength)
{
#ifndef _WIN32
	/* Serial support is only implemented on Windows at the moment. */
	return SIOMM_ERROR_NOT_IMPLEMENTED;
#else
	check(pMessage != NULL, SIOMM_ERROR);

	// Create the request packet.
	uint8_t * pbySerialRequest = new uint8_t[sizeof(SERIAL_HEADER_X_T) + wMessageLength];
	memcpy(pbySerialRequest + sizeof(SERIAL_HEADER_X_T), pMessage, wMessageLength);

	// Fill in the serial header.
	SERIAL_HEADER_X_T * pTransferHeader = (SERIAL_HEADER_X_T *)pbySerialRequest;
	pTransferHeader->m_u16Length = htons(wMessageLength);     // only includes data length, not the header
	pTransferHeader->m_u16Crc = htons(0);                  // set to 0 before doing the calculation
	pTransferHeader->m_u8Address = m_serialAddress;
	pTransferHeader->m_u8Type = 2;                         // kernel mode
	pTransferHeader->m_u16Crc = htons(Crc16R(pbySerialRequest, sizeof(SERIAL_HEADER_X_T) + wMessageLength));

	// Set mark parity for the first byte, which is special
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	::GetCommState(m_serialHandle, &dcb);
	dcb.Parity = MARKPARITY;
	::SetCommState(m_serialHandle, &dcb);
	::SetCommMask(m_serialHandle, EV_TXEMPTY);

	ULONG sentSize = 0;
	if (0 == ::WriteFile(m_serialHandle, pbySerialRequest, 1, &sentSize, 0))
	{
		delete pbySerialRequest;
		return GetLastError();
	}
	if (1 != sentSize)
	{
		delete pbySerialRequest;
		return SIOMM_ERROR_INCORRECT_LENGTH_SENT;
	}

	Sleep(BaudRateToSleepTime(m_serialBaudRate));                 // Wait until the parity is transferred

	DWORD dwEventMask = 0;
	while (!(dwEventMask & EV_TXEMPTY))
	{
		::WaitCommEvent(m_serialHandle, &dwEventMask, NULL);

		if (dwEventMask & EV_TXEMPTY)
			break;
		else
			Sleep(1);
	}

	// reset the comm port parity
	::GetCommState(m_serialHandle, &dcb);
	dcb.Parity = SPACEPARITY;
	::SetCommState(m_serialHandle, &dcb);

	// transmit the remaining buffer
	uint32_t dwRemainingLength = wMessageLength + sizeof(SERIAL_HEADER_X_T) - 1;
	uint32_t dwStartingPlace = 1;

	while (dwRemainingLength > 0)
	{
		if (0 == ::WriteFile(m_serialHandle, pbySerialRequest + dwStartingPlace, dwRemainingLength, &sentSize, 0))
		{
			delete pbySerialRequest;
			return GetLastError();
		}

		dwRemainingLength -= sentSize;
		dwStartingPlace += sentSize;

		if (dwRemainingLength <= 0)
			break;
		else
			Sleep(1);
	}

	delete pbySerialRequest;
	return SIOMM_OK;
#endif
}


int O22SnapIoMemMap::WriteBlockSerial(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
{
#ifndef _WIN32
	/* Serial supported is only implemented on Windows systems at the moment. */
	return SIOMM_ERROR_NOT_IMPLEMENTED;

#else
	check(pbyData != NULL, SIOMM_ERROR);

	// Check that we have a valid serial handle.
	check(m_serialHandle != 0, SIOMM_ERROR_NOT_CONNECTED);

	// Build the request packet
	uint8_t * pbyWriteBlockRequest = new uint8_t[SIOMM_SIZE_WRITE_BLOCK_REQUEST + wDataLength];
	UpdateTransactionLabel();
	BuildWriteBlockRequest(pbyWriteBlockRequest, m_byTransactionLabel, dwDestOffset, wDataLength, pbyData);

	SendSerialMessage(pbyWriteBlockRequest, SIOMM_SIZE_WRITE_BLOCK_REQUEST + wDataLength);

	// receive
	uint8_t bySerialResponse[sizeof(SERIAL_HEADER_R_T) + SIOMM_SIZE_WRITE_RESPONSE];
	uint32_t dwResponseLength = SIOMM_SIZE_WRITE_RESPONSE + sizeof(SERIAL_HEADER_R_T);
	uint32_t dwResponseReceived = 0;
	ULONG readSize = 0;
	SERIAL_HEADER_R_T * pReceiveHeader = (SERIAL_HEADER_R_T *)bySerialResponse;

	// timeout setup
	uint32_t dwStopTime = GetTickCount() + m_nTimeOutMS;

	while (dwResponseLength > 0)
	{
		if (0 >= ::ReadFile(m_serialHandle, bySerialResponse + dwResponseReceived, dwResponseLength, &readSize, 0))
		{
			delete pbyWriteBlockRequest;
			return GetLastError();
		}

		dwResponseReceived += readSize;
		dwResponseLength -= readSize;

		if (dwResponseReceived > sizeof(SERIAL_HEADER_R_T))
		{
			if (ntohs(pReceiveHeader->m_u16Length) <= (dwResponseReceived - sizeof(SERIAL_HEADER_R_T)))
			{
				break;
			}
		}

		// Check if we timed out.
		if (dwStopTime < GetTickCount())
		{
			delete pbyWriteBlockRequest;
			return SIOMM_ERROR_SERIAL_TIMEOUT;
		}
	}

	// Make sure the CRC matches
	uint16_t u16MessageCrc = ntohs(pReceiveHeader->m_u16Crc);
	pReceiveHeader->m_u16Crc = 0;
	if (u16MessageCrc != Crc16R(bySerialResponse, ntohs(pReceiveHeader->m_u16Length) + sizeof(SERIAL_HEADER_R_T)))
	{
		delete pbyWriteBlockRequest;
		return SIOMM_ERROR_SERIAL_MISMATCHED_CRC;
	}

	uint8_t * pbyWriteData = bySerialResponse + sizeof(SERIAL_HEADER_R_T);
	int nResult = 0;
	if (SIOMM_SIZE_WRITE_RESPONSE <= dwResponseReceived)
	{
		uint8_t byTransactionLabel;
		uint8_t byResponseCode;
		nResult = UnpackWriteResponse(pbyWriteData, &byTransactionLabel, &byResponseCode);

		if (SIOMM_OK == nResult)
		{
			if (m_byTransactionLabel != byTransactionLabel)
				nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
			else if (SIOMM_RESPONSE_CODE_ACK != byResponseCode)
				// We've received what looks like a good response and we have 
				// what appears to be an error from the brain, so return that
				// error.
				nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
		}
	}
	else // SIOMM_SIZE_WRITE_RESPONSE > readSize
		nResult = SIOMM_ERROR_WRONG_NUM_BYTES_RETURNED;

	delete pbyWriteBlockRequest;
	return nResult;
#endif
}


int O22SnapIoMemMap::BaudRateToSleepTime(int baudRate)
//-------------------------------------------------------------------------------------------------
// Figure out how int to sleep (persist the parity mark) for a given baud rate.
//-------------------------------------------------------------------------------------------------
{
	if (baudRate > 9600)
		return 1;
	if (baudRate > 1200)
		return 10;
	if (baudRate > 600)
		return 20;
	return 40;
}


int O22SnapIoMemMap::WriteBlockPassThru(const uint32_t dwDestOffset, const uint16_t wDataLength, uint8_t * pbyData)
{
	check(pbyData != NULL, SIOMM_ERROR);

	short passThruBufferSize = SIOMM_BLOCK_MAX_BYTES - SIOMM_SIZE_WRITE_BLOCK_REQUEST - 12;
	// if the data to write is too int, break it up into smaller pieces
	if (wDataLength > passThruBufferSize)
	{
		uint32_t subOffset = dwDestOffset;
		uint8_t * pbySubData = pbyData;
		uint16_t remainingLength = wDataLength;

		int nResult;
		while ((remainingLength != 0) && (remainingLength <= wDataLength))
		{
			if (remainingLength > passThruBufferSize)
			{
				nResult = WriteBlockPassThru(subOffset, passThruBufferSize, pbySubData);
				subOffset += passThruBufferSize;
				remainingLength -= passThruBufferSize;
				pbySubData += passThruBufferSize;
			}
			else
			{
				nResult = WriteBlockPassThru(subOffset, remainingLength, pbySubData);
				subOffset += remainingLength;
				remainingLength -= remainingLength;
				pbySubData += remainingLength;
			}

			if (nResult != SIOMM_OK)
				return nResult;
		}

		return nResult;
	}

	SERIAL_PASS_THRU_CONFIG config;
	config.Enable = htonl(1);
	config.Bps = htonl(m_serialBaudRate);
	config.DataBits = htonl(8);
	config.Parity = htonl(0x00004D44);
	config.StopBits = htonl(1);
	config.Duplex = htonl(m_serialTwoWire ? 0x00000048 : 0x00000046);
	config.Timeout = htonl(m_nTimeOutMS);

	// Build the request packet
	uint8_t byWriteBlockRequest[SIOMM_SIZE_WRITE_BLOCK_REQUEST + SIOMM_BLOCK_MAX_BYTES];
	UpdateTransactionLabel();
	uint8_t expectedTransactionLabel = m_byTransactionLabel;
	BuildWriteBlockRequest(byWriteBlockRequest, m_byTransactionLabel, dwDestOffset, wDataLength, pbyData);
	int requestSize = SIOMM_SIZE_WRITE_BLOCK_REQUEST + wDataLength;

	uint8_t * pRequest = new uint8_t[sizeof(SERIAL_PASS_THRU_RW) + requestSize];
	memcpy(pRequest + sizeof(SERIAL_PASS_THRU_RW), byWriteBlockRequest, requestSize);

	SERIAL_PASS_THRU_RW * pReadWrite = (SERIAL_PASS_THRU_RW *)pRequest;
	pReadWrite->Address = htonl(m_serialAddress);
	pReadWrite->Type = htonl(2);
	pReadWrite->Length = htonl(requestSize);

	// tell the controller that we're doing a pass through packet
	int nResult = WriteBlockEthernet(GetPassThruConfigAddress(m_serialPort), sizeof(SERIAL_PASS_THRU_CONFIG), (uint8_t *)&config);

	if (nResult == SIOMM_OK)
	{
		// send the write request
		uint16_t dataLength = (uint16_t)(sizeof(SERIAL_PASS_THRU_RW) + requestSize);
		nResult = WriteBlockEthernet(GetPassThruReadWriteAddress(m_serialPort), dataLength, pRequest);

		if (nResult == SIOMM_OK)
		{
			uint16_t wResponseLength = SIOMM_SIZE_WRITE_RESPONSE + wDataLength + 4;
			uint8_t * pbyWriteBlockResponse = new uint8_t[wResponseLength];

			nResult = ReadBlockEthernet(GetPassThruReadWriteAddress(m_serialPort), wResponseLength, pbyWriteBlockResponse);

			if (nResult == SIOMM_OK)
			{
				uint8_t byTransactionLabel;
				uint8_t byResponseCode;

				nResult = UnpackWriteResponse(pbyWriteBlockResponse + 4, &byTransactionLabel, &byResponseCode);

				if (nResult == SIOMM_OK)
				{
					if (byTransactionLabel != expectedTransactionLabel)
						nResult = SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED;
					else if (byResponseCode != SIOMM_RESPONSE_CODE_ACK)
						nResult = (SIOMM_BRAIN_ERROR_BASE | (int)byResponseCode);
				}
			}

			delete pbyWriteBlockResponse;
		}
	}

	// tell the controller that we're done
	config.Enable = 0;
	WriteBlockEthernet(GetPassThruConfigAddress(m_serialPort), sizeof(SERIAL_PASS_THRU_CONFIG), (uint8_t *)&config);

	delete pRequest;
	return nResult;
}


int O22SnapIoMemMap::WriteQuad(uint32_t dwDestOffset, uint32_t dwQuadlet)
//-------------------------------------------------------------------------------------------------
// Write a quadlet of data to a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[sizeof(uint32_t)];

	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, dwQuadlet);

	return WriteBlock(dwDestOffset, sizeof(uint32_t), arrbyData);
}


int O22SnapIoMemMap::Close()
//-------------------------------------------------------------------------------------------------
// Close the connection to the I/O unit
//-------------------------------------------------------------------------------------------------
{
	if (m_method != serial)
		return CloseSockets();
	else
	{
#ifdef _WIN32
		::CloseHandle(m_serialHandle);
#endif
		m_serialHandle = 0;
		return SIOMM_OK;
	}
}


int O22SnapIoMemMap::GetDigPtState(int nPoint, int *pnState)
//-------------------------------------------------------------------------------------------------
// Get the state of the specified digital point.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_STATE + (SIOMM_DPOINT_READ_BOUNDARY * nPoint),
		(uint32_t*)pnState);
}


int O22SnapIoMemMap::GetDigPtOnLatch(int nPoint, int *pnState)
//-------------------------------------------------------------------------------------------------
// Get the on-latch state of the specified digital point.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_ONLATCH_STATE + (SIOMM_DPOINT_READ_BOUNDARY * nPoint),
		(uint32_t*)pnState);

}

int O22SnapIoMemMap::GetDigPtOffLatch(int nPoint, int *pnState)
//-------------------------------------------------------------------------------------------------
// Get the off-latch state of the specified digital point.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_OFFLATCH_STATE + (SIOMM_DPOINT_READ_BOUNDARY * nPoint),
		(uint32_t*)pnState);

}

int O22SnapIoMemMap::GetDigPtCounterState(int nPoint, int *pnState)
//-------------------------------------------------------------------------------------------------
// Get the active counter state of the specified digital point.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_ACTIVE_COUNTER + (SIOMM_DPOINT_READ_BOUNDARY * nPoint),
		(uint32_t*)pnState);

}

int O22SnapIoMemMap::GetDigPtCounts(int nPoint, int *pnValue)
//-------------------------------------------------------------------------------------------------
// Get the counters of the specified digital point.
//-------------------------------------------------------------------------------------------------
{
	check(pnValue != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_COUNTER_DATA + (SIOMM_DPOINT_READ_BOUNDARY * nPoint),
		(uint32_t*)pnValue);

}

int O22SnapIoMemMap::GetDigPtReadAreaEx(int nPoint, SIOMM_DigPointReadArea * pData)
//-------------------------------------------------------------------------------------------------
// Get the read area for the specified digital point
//-------------------------------------------------------------------------------------------------
{
	check(pData != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[20]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_DPOINT_READ_AREA_BASE + (SIOMM_DPOINT_READ_BOUNDARY * nPoint),
		20, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pData->nState = O22MAKELONG2(arrbyData, 0);
		pData->nOnLatch = O22MAKELONG2(arrbyData, 4);
		pData->nOffLatch = O22MAKELONG2(arrbyData, 8);
		pData->nCounterState = O22MAKELONG2(arrbyData, 12);
		pData->nCounts = O22MAKELONG2(arrbyData, 16);
	}

	return nResult;
}


int O22SnapIoMemMap::SetDigPtState(int nPoint, int nState)
//-------------------------------------------------------------------------------------------------
// Set the state of the specified digital point
//-------------------------------------------------------------------------------------------------
{
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	if (nState)
		return  WriteQuad(SIOMM_DPOINT_WRITE_TURN_ON_BASE + (SIOMM_DPOINT_WRITE_BOUNDARY * nPoint), 1);
	else
		return  WriteQuad(SIOMM_DPOINT_WRITE_TURN_OFF_BASE + (SIOMM_DPOINT_WRITE_BOUNDARY * nPoint), 1);
}


int O22SnapIoMemMap::SetDigPtCounterState(int nPoint, int nState)
//-------------------------------------------------------------------------------------------------
// Set the active counter state of the specified digital point
//-------------------------------------------------------------------------------------------------
{
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	if (nState)
		return  WriteQuad(SIOMM_DPOINT_WRITE_ACTIVATE_COUNTER + (SIOMM_DPOINT_WRITE_BOUNDARY * nPoint), 1);
	else
		return  WriteQuad(SIOMM_DPOINT_WRITE_DEACTIVATE_COUNTER + (SIOMM_DPOINT_WRITE_BOUNDARY * nPoint), 1);
}


int O22SnapIoMemMap::GetBitmask64(uint32_t dwDestOffset, int *pnPts63to32, int *pnPts31to0)
//-------------------------------------------------------------------------------------------------
// Get a 64-bit bitmask from a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	check(pnPts63to32 != NULL, SIOMM_ERROR);
	check(pnPts31to0 != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[8]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(dwDestOffset, 8, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and make the longs
		*pnPts63to32 = O22MAKELONG2(arrbyData, 0);
		*pnPts31to0 = O22MAKELONG2(arrbyData, 4);
	}

	return nResult;
}


int O22SnapIoMemMap::SetBitmask64(uint32_t dwDestOffset, int nPts63to32, int nPts31to0)
//-------------------------------------------------------------------------------------------------
// Set a 64-bit bitmask to a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[8]; // buffer for the data to be read

	// Unpack the bytes
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, nPts63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, nPts31to0);

	// Write the data
	nResult = WriteBlock(dwDestOffset, 8, (uint8_t*)arrbyData);

	return nResult;
}


int O22SnapIoMemMap::GetDigBankPointStates(int *pnPts63to32, int *pnPts31to0)
//-------------------------------------------------------------------------------------------------
// Get the digital point states in the digital bank
//-------------------------------------------------------------------------------------------------
{
	check(pnPts63to32 != NULL, SIOMM_ERROR);
	check(pnPts31to0 != NULL, SIOMM_ERROR);

	return GetBitmask64(SIOMM_DBANK_READ_POINT_STATES, pnPts63to32, pnPts31to0);
}


int O22SnapIoMemMap::GetDigBankOnLatchStates(int *pnPts63to32, int *pnPts31to0)
//-------------------------------------------------------------------------------------------------
// Get the digital point on-latch states in the digital bank
//-------------------------------------------------------------------------------------------------
{
	check(pnPts63to32 != NULL, SIOMM_ERROR);
	check(pnPts31to0 != NULL, SIOMM_ERROR);

	return GetBitmask64(SIOMM_DBANK_READ_ON_LATCH_STATES, pnPts63to32, pnPts31to0);
}


int O22SnapIoMemMap::GetDigBankOffLatchStates(int *pnPts63to32, int *pnPts31to0)
//-------------------------------------------------------------------------------------------------
// Get the digital point off-latch states in the digital bank
//-------------------------------------------------------------------------------------------------
{
	check(pnPts63to32 != NULL, SIOMM_ERROR);
	check(pnPts31to0 != NULL, SIOMM_ERROR);

	return GetBitmask64(SIOMM_DBANK_READ_OFF_LATCH_STATES, pnPts63to32, pnPts31to0);
}


int O22SnapIoMemMap::GetDigBankActCounterStates(int *pnPts63to32, int *pnPts31to0)
//-------------------------------------------------------------------------------------------------
// Get the digital point active counter states in the digital bank
//-------------------------------------------------------------------------------------------------
{
	check(pnPts63to32 != NULL, SIOMM_ERROR);
	check(pnPts31to0 != NULL, SIOMM_ERROR);

	return GetBitmask64(SIOMM_DBANK_READ_ACTIVE_COUNTERS, pnPts63to32, pnPts31to0);
}


int O22SnapIoMemMap::SetDigBankPointStates(int nPts63to32, int nPts31to0,
	int nMask63to32, int nMask31to0)
	//-------------------------------------------------------------------------------------------------
	// Set the digital point states in the digital bank.  Only those points set in the mask parameters
	// are set.
	//-------------------------------------------------------------------------------------------------
{
	int nOnMask63to32;
	int nOnMask31to0;
	int nOffMask63to32;
	int nOffMask31to0;
	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[16]; // buffer for the data to be read

	// Figure out the On Mask
	nOnMask63to32 = nPts63to32 & nMask63to32;
	nOnMask31to0 = nPts31to0 & nMask31to0;

	// Figure out the Off Mask
	nOffMask63to32 = (nPts63to32 ^ 0xFFFFFFFF) & nMask63to32;
	nOffMask31to0 = (nPts31to0 ^ 0xFFFFFFFF) & nMask31to0;

	// Pack the bytes
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, nOnMask63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, nOnMask31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 8, nOffMask63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, nOffMask31to0);

	// Write the data
	nResult = WriteBlock(SIOMM_DBANK_WRITE_TURN_ON_MASK, 16, (uint8_t*)arrbyData);

	return nResult;

}


int O22SnapIoMemMap::SetDigBankOnMask(int nPts63to32, int nPts31to0)
//-------------------------------------------------------------------------------------------------
// Turn on the given digital points
//-------------------------------------------------------------------------------------------------
{
	return SetBitmask64(SIOMM_DBANK_WRITE_TURN_ON_MASK, nPts63to32, nPts31to0);
}

int O22SnapIoMemMap::SetDigBankOffMask(int nPts63to32, int nPts31to0)
//-------------------------------------------------------------------------------------------------
// Turn off the given digital points
//-------------------------------------------------------------------------------------------------
{
	return SetBitmask64(SIOMM_DBANK_WRITE_TURN_OFF_MASK, nPts63to32, nPts31to0);
}

int O22SnapIoMemMap::SetDigBankActCounterMask(int nPts63to32, int nPts31to0)
//-------------------------------------------------------------------------------------------------
// Active counters for the given digital points
//-------------------------------------------------------------------------------------------------
{
	return SetBitmask64(SIOMM_DBANK_WRITE_ACT_COUNTERS_MASK, nPts63to32, nPts31to0);
}

int O22SnapIoMemMap::SetDigBankDeactCounterMask(int nPts63to32, int nPts31to0)
//-------------------------------------------------------------------------------------------------
// Deactive counters for the given digital points
//-------------------------------------------------------------------------------------------------
{
	return SetBitmask64(SIOMM_DBANK_WRITE_DEACT_COUNTERS_MASK, nPts63to32, nPts31to0);
}


int O22SnapIoMemMap::GetDigBankReadAreaEx(SIOMM_DigBankReadArea * pData)
//-------------------------------------------------------------------------------------------------
// Get the read area for the digital bank
//-------------------------------------------------------------------------------------------------
{
	check(pData != NULL, SIOMM_ERROR);

	int nResult;       // for checking the return values of functions
	uint8_t arrbyData[32]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_DBANK_READ_AREA_BASE, 32, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pData->nStatePts63to32 = O22MAKELONG2(arrbyData, 0);
		pData->nStatePts31to0 = O22MAKELONG2(arrbyData, 4);
		pData->nOnLatchStatePts63to32 = O22MAKELONG2(arrbyData, 8);
		pData->nOnLatchStatePts31to0 = O22MAKELONG2(arrbyData, 12);
		pData->nOffLatchStatePts63to32 = O22MAKELONG2(arrbyData, 16);
		pData->nOffLatchStatePts31to0 = O22MAKELONG2(arrbyData, 20);
		pData->nActiveCountersPts63to32 = O22MAKELONG2(arrbyData, 24);
		pData->nActiveCountersPts31to0 = O22MAKELONG2(arrbyData, 28);
	}

	return nResult;
}


int O22SnapIoMemMap::ReadClearDigPtCounts(int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Read and clear counts for the specified digital point
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_CLEAR_COUNTS_BASE + (SIOMM_DPOINT_READ_CLEAR_BOUNDARY * nPoint),
		(uint32_t*)pnState);
}


int O22SnapIoMemMap::ReadClearDigPtOnLatch(int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Read and clear the on-latch state for the specified digital point
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_CLEAR_ON_LATCH_BASE + (SIOMM_DPOINT_READ_CLEAR_BOUNDARY * nPoint),
		(uint32_t*)pnState);
}


int O22SnapIoMemMap::ReadClearDigPtOffLatch(int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Read and clear the off-latch state for the specified digital point
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < 64, SIOMM_ERROR_INDEX_BAD);

	return  ReadQuad(SIOMM_DPOINT_READ_CLEAR_OFF_LATCH_BASE + (SIOMM_DPOINT_READ_CLEAR_BOUNDARY * nPoint),
		(uint32_t*)pnState);
}


int O22SnapIoMemMap::GetHDDigitalPointState(int nModule, int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Gets the state of a digital point.  Point is specified by module number and point location 
// within that module.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	uint32_t result = 0;
	int readResult = ReadQuad(SIOMM4096_DPOINT_READ_STATE + ((nPoint < 32) ? 4 : 0) +
		(SIOMM4096_DPOINT_READ_BOUNDARY * nModule),
		&result);

	*pnState = result & (1 << (nPoint % 32));

	return readResult;
}


int O22SnapIoMemMap::GetHDDigitalPointOnLatch(int nModule, int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Gets the on-latch state of a digital point.  Point is specified by module number and point 
// location within that module.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	uint32_t result = 0;
	int readResult = ReadQuad(SIOMM4096_DPOINT_READ_ONLATCH_STATE + ((nPoint < 32) ? 4 : 0) +
		(SIOMM4096_DPOINT_READ_BOUNDARY * nModule),
		&result);

	*pnState = result & (1 << (nPoint % 32));

	return readResult;
}


int O22SnapIoMemMap::GetHDDigitalPointOffLatch(int nModule, int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Gets the off-latch state of a digital point.  Point is specified by module number and point 
// location within that module.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	uint32_t result = 0;
	int readResult = ReadQuad(SIOMM4096_DPOINT_READ_OFFLATCH_STATE + ((nPoint < 32) ? 4 : 0) +
		(SIOMM4096_DPOINT_READ_BOUNDARY * nModule),
		&result);

	*pnState = result & (1 << (nPoint % 32));

	return readResult;
}


int O22SnapIoMemMap::GetHDDigitalPointCounts(int nModule, int nPoint, int * pnCounts)
//-------------------------------------------------------------------------------------------------
// Gets the counts value of a digital point.  Point is specified by module number and point 
// location within that module.
//-------------------------------------------------------------------------------------------------
{
	check(pnCounts != NULL, SIOMM_ERROR);
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	uint32_t result = 0;
	int readResult = ReadQuad(SIOMM4096_DPOINT_READ_COUNTER_DATA +
		(SIOMM4096_DPOINT_READ_COUNTER_BOUNDARY * nModule) +
		(4 * nPoint),
		&result);
	*pnCounts = result;

	return readResult;
}


int O22SnapIoMemMap::SetHDDigitalPointState(int nModule, int nPoint, int nState)
//-------------------------------------------------------------------------------------------------
// Set digital point state.  Point is specified by module number and point location within that
// module.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	uint32_t addressBase = (nState == 0) ? SIOMM4096_DPOINT_WRITE_TURN_OFF_BASE : SIOMM4096_DPOINT_WRITE_TURN_ON_BASE;

	return WriteQuad(addressBase + ((nPoint < 32) ? 4 : 0) +
		(SIOMM4096_DPOINT_WRITE_BOUNDARY * nModule),
		(1 << (nPoint % 32)));
}


int O22SnapIoMemMap::ReadClearHDDigitalPointCounts(int nModule, int nPoint, int * pnCounts)
//-------------------------------------------------------------------------------------------------
// Read and clear digital point counts.  Point is specified by module number and point location
// within that module.
//-------------------------------------------------------------------------------------------------
{
	check(pnCounts != NULL, SIOMM_ERROR);
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	uint32_t result = 0;
	int readResult = ReadQuad(SIOMM4096_DPOINT_READCLEAR_COUNTER +
		(SIOMM4096_DPOINT_READCLEAR_COUNTER_BOUNDARY * nModule) +
		(4 * nPoint),
		&result);
	*pnCounts = result;

	return readResult;
}


int O22SnapIoMemMap::ReadClearHDDigitalPointOnLatch(int nModule, int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Read and clear digital point on-latch.  Point is specified by module number and point location
// within that module.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	// There is no easy way to do this to a single point on a module.  First, we read the state that
	// we're interested in.
	int hr = GetHDDigitalPointOnLatch(nModule, nPoint, pnState);
	if (hr != SIOMM_OK)
		return hr;

	// Now we write a bitmask to clear the latch for this point.
	return WriteQuad(SIOMM4096_DPOINT_READCLEAR_ONLATCH + ((nPoint < 32) ? 4 : 0) +
		(SIOMM4096_DPOINT_READCLEAR_LATCH_BOUNDARY * nModule),
		(1 << (nPoint % 32)));
}


int O22SnapIoMemMap::ReadClearHDDigitalPointOffLatch(int nModule, int nPoint, int * pnState)
//-------------------------------------------------------------------------------------------------
// Read and clear digital point off-latch.  Point is specified by module number and point location
// within that module.
//-------------------------------------------------------------------------------------------------
{
	check(pnState != NULL, SIOMM_ERROR);
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR_INDEX_BAD);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR_INDEX_BAD);

	// There is no easy way to do this to a single point on a module.  First, we read the state that
	// we're interested in.
	int hr = GetHDDigitalPointOffLatch(nModule, nPoint, pnState);
	if (hr != SIOMM_OK)
		return hr;

	// Now we write a bitmask to clear the latch for this point.
	return WriteQuad(SIOMM4096_DPOINT_READCLEAR_OFFLATCH + ((nPoint < 32) ? 4 : 0) +
		(SIOMM4096_DPOINT_READCLEAR_LATCH_BOUNDARY * nModule),
		(1 << (nPoint % 32)));
}


int O22SnapIoMemMap::GetAnaPtValue(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the value of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_VALUE_BASE + (SIOMM_APOINT_READ_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::GetAnaPtCounts(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the raw counts of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_COUNTS_BASE + (SIOMM_APOINT_READ_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::GetAnaPtMinValue(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the minimum value of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_MIN_VALUE_BASE + (SIOMM_APOINT_READ_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::GetAnaPtMaxValue(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the maximum value of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_MAX_VALUE_BASE + (SIOMM_APOINT_READ_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::GetAnaPtTpoPeriod(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the TPO period of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_TPO_PERIOD_BASE + (SIOMM_APOINT_READ_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::GetAnaPtReadAreaEx(int nPoint, SIOMM_AnaPointReadArea * pData)
//-------------------------------------------------------------------------------------------------
// Get the read area for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(pData != NULL, SIOMM_ERROR);

	int  nResult;       // for checking the return values of functions
	uint8_t  arrbyData[16]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_APOINT_READ_AREA_BASE + (SIOMM_APOINT_READ_BOUNDARY * nPoint),
		16, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pData->fValue = O22MAKEFLOAT2(arrbyData, 0);
		pData->fCounts = O22MAKEFLOAT2(arrbyData, 4);
		pData->fMinValue = O22MAKEFLOAT2(arrbyData, 8);
		pData->fMaxValue = O22MAKEFLOAT2(arrbyData, 12);
	}

	return nResult;
}


int O22SnapIoMemMap::SetAnaPtValue(int nPoint, float fValue)
//-------------------------------------------------------------------------------------------------
// Set the value for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	return WriteFloat(SIOMM_APOINT_WRITE_VALUE_BASE + (SIOMM_APOINT_WRITE_BOUNDARY * nPoint),
		fValue);
}


int O22SnapIoMemMap::SetAnaPtCounts(int nPoint, float fValue)
//-------------------------------------------------------------------------------------------------
// Set the raw counts for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	return WriteFloat(SIOMM_APOINT_WRITE_COUNTS_BASE + (SIOMM_APOINT_WRITE_BOUNDARY * nPoint),
		fValue);
}


int O22SnapIoMemMap::SetAnaPtTpoPeriod(int nPoint, float fValue)
//-------------------------------------------------------------------------------------------------
// Set the TPO period for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	return WriteFloat(SIOMM_APOINT_WRITE_TPO_PERIOD_BASE + (SIOMM_APOINT_WRITE_BOUNDARY * nPoint),
		fValue);
}


int O22SnapIoMemMap::ReadClearAnaPtMinValue(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Read and clear the minimum value for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_CLEAR_MIN_VALUE_BASE + (SIOMM_APOINT_READ_CLEAR_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::ReadClearAnaPtMaxValue(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Read and clear the maximum value for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_CLEAR_MAX_VALUE_BASE + (SIOMM_APOINT_READ_CLEAR_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::ConfigurePoint(int nPoint, int nPointType)
//-------------------------------------------------------------------------------------------------
// Configure the specified point
//-------------------------------------------------------------------------------------------------
{
	return WriteQuad(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		nPointType);
}


int O22SnapIoMemMap::GetModuleType(int nPoint, int * pnModuleType)
//-------------------------------------------------------------------------------------------------
// Get the module type at the specified point position
//-------------------------------------------------------------------------------------------------
{
	check(pnModuleType != NULL, SIOMM_ERROR);

	return  ReadQuad(SIOMM_POINT_CONFIG_READ_MOD_TYPE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		(uint32_t*)pnModuleType);
}


int O22SnapIoMemMap::SetDigPtConfiguration(int nPoint, int nPointType, int nFeature)
//-------------------------------------------------------------------------------------------------
// Configure a digital point
//-------------------------------------------------------------------------------------------------
{
	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[8]; // buffer for the data to be read

	// Build the data area
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, nPointType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, nFeature);

	// Write the data
	nResult = WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		8, (uint8_t*)arrbyData);

	return nResult;
}


int O22SnapIoMemMap::SetAnaPtConfiguration(int nPoint, int nPointType,
	float fOffset, float fGain,
	float fHiScale, float fLoScale)
	//-------------------------------------------------------------------------------------------------
	// Configure an analog point
	//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[24]; // buffer for the data to be read

	// Build the data area
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, nPointType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, 0);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, fOffset);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 12, fGain);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 16, fHiScale);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, fLoScale);

	// Write the data
	return WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		24, (uint8_t*)arrbyData);
}


int O22SnapIoMemMap::SetPtConfigurationEx3(int nPoint, SIOMM_PointConfigArea3 PtConfigData)
//-------------------------------------------------------------------------------------------------
// Configure a digital or analog point
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_POINT_CONFIG_BOUNDARY]; // buffer for the data to be read
	uint8_t arrbyDataClamp[SIOMM_POINT_CONFIG_CLAMP_BOUNDARY]; // buffer for the clamping area
	int  nResult;

	// Build the data area
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, PtConfigData.nPointType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, PtConfigData.nFeature);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, PtConfigData.fOffset);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 12, PtConfigData.fGain);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 16, PtConfigData.fHiScale);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, PtConfigData.fLoScale);
	// Bytes 24-27 are not used at this time. 
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 28, PtConfigData.fFilterWeight);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 32, PtConfigData.fWatchdogValue);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, PtConfigData.nWatchdogEnabled);

	memcpy(arrbyData + 44, PtConfigData.byName, SIOMM_POINT_CONFIG_NAME_SIZE);

	O22FILL_ARRAY_FROM_FLOAT(arrbyDataClamp, 0, PtConfigData.fLoClamp);
	O22FILL_ARRAY_FROM_FLOAT(arrbyDataClamp, 4, PtConfigData.fHiClamp);


	// Write the first section
	nResult = WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		24, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section
	nResult = WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + 28 + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		12, (uint8_t*)(&(arrbyData[28])));

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the third section
	nResult = WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + 44 + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		SIOMM_POINT_CONFIG_NAME_SIZE, (uint8_t*)(&(arrbyData[44])));

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the fourth section (clamping)
	return WriteBlock(SIOMM_POINT_CONFIG_WRITE_LOCLAMP_BASE + (SIOMM_POINT_CONFIG_CLAMP_BOUNDARY * nPoint),
		SIOMM_POINT_CONFIG_CLAMP_BOUNDARY, (uint8_t*)arrbyDataClamp);
}


int O22SnapIoMemMap::SetPtConfigurationEx2(int nPoint, SIOMM_PointConfigArea2 PtConfigData)
//-------------------------------------------------------------------------------------------------
// Configure a digital or analog point
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_POINT_CONFIG_BOUNDARY]; // buffer for the data to be read
	int  nResult;

	// Build the data area
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, PtConfigData.nPointType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, PtConfigData.nFeature);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, PtConfigData.fOffset);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 12, PtConfigData.fGain);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 16, PtConfigData.fHiScale);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, PtConfigData.fLoScale);
	// Bytes 24-27 are not used at this time. 
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 28, PtConfigData.fFilterWeight);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 32, PtConfigData.fWatchdogValue);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, PtConfigData.nWatchdogEnabled);

	memcpy(arrbyData + 44, PtConfigData.byName, SIOMM_POINT_CONFIG_NAME_SIZE);

	// Write the first section
	nResult = WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		24, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section
	nResult = WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + 28 + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		12, (uint8_t*)(&(arrbyData[28])));

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the third section
	return WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + 44 + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		SIOMM_POINT_CONFIG_NAME_SIZE, (uint8_t*)(&(arrbyData[44])));

}


int O22SnapIoMemMap::SetPtConfigurationEx(int nPoint, SIOMM_PointConfigArea PtConfigData)
//-------------------------------------------------------------------------------------------------
// Configure a digital or analog point
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[40]; // buffer for the data to be read
	int  nResult;

	// Build the data area
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, PtConfigData.nPointType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, PtConfigData.nFeature);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, PtConfigData.fOffset);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 12, PtConfigData.fGain);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 16, PtConfigData.fHiScale);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, PtConfigData.fLoScale);
	// Bytes 24-31 are not used at this time. 
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 32, PtConfigData.fWatchdogValue);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, PtConfigData.nWatchdogEnabled);

	// Write the first section
	nResult = WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		24, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section
	return WriteBlock(SIOMM_POINT_CONFIG_WRITE_TYPE_BASE + 32 + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		8, (uint8_t*)(&(arrbyData[32])));
}


int O22SnapIoMemMap::GetPtConfigurationEx3(int nPoint, SIOMM_PointConfigArea3 * pPtConfigData)
//-------------------------------------------------------------------------------------------------
// Get the configuration for a point
//-------------------------------------------------------------------------------------------------
{
	check(pPtConfigData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[SIOMM_POINT_CONFIG_BOUNDARY]; // buffer for the data to be read
	uint8_t arrbyDataClamp[SIOMM_POINT_CONFIG_CLAMP_BOUNDARY]; // buffer for the clamp data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_POINT_CONFIG_READ_MOD_TYPE_BASE +
		(SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		SIOMM_POINT_CONFIG_BOUNDARY, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Read the clamping config data
		nResult = ReadBlock(SIOMM_POINT_CONFIG_WRITE_LOCLAMP_BASE +
			(SIOMM_POINT_CONFIG_CLAMP_BOUNDARY * nPoint),
			SIOMM_POINT_CONFIG_CLAMP_BOUNDARY, (uint8_t*)arrbyDataClamp);

		// Check for error
		if (SIOMM_OK == nResult)
		{
			// If everything is okay, go ahead and fill the structure
			pPtConfigData->nModuleType = O22MAKELONG2(arrbyData, 0);
			pPtConfigData->nPointType = O22MAKELONG2(arrbyData, 4);
			pPtConfigData->nFeature = O22MAKELONG2(arrbyData, 8);
			pPtConfigData->fOffset = O22MAKEFLOAT2(arrbyData, 12);
			pPtConfigData->fGain = O22MAKEFLOAT2(arrbyData, 16);
			pPtConfigData->fHiScale = O22MAKEFLOAT2(arrbyData, 20);
			pPtConfigData->fLoScale = O22MAKEFLOAT2(arrbyData, 24);
			// Bytes 28-31 are not used at this time
			pPtConfigData->fFilterWeight = O22MAKEFLOAT2(arrbyData, 32);
			pPtConfigData->fWatchdogValue = O22MAKEFLOAT2(arrbyData, 36);
			pPtConfigData->nWatchdogEnabled = O22MAKELONG2(arrbyData, 40);

			memcpy(pPtConfigData->byName, arrbyData + 48, 16);

			pPtConfigData->fLoClamp = O22MAKEFLOAT2(arrbyDataClamp, 0);
			pPtConfigData->fHiClamp = O22MAKEFLOAT2(arrbyDataClamp, 4);
		}
	}

	return nResult;
}


int O22SnapIoMemMap::GetPtConfigurationEx2(int nPoint, SIOMM_PointConfigArea2 * pPtConfigData)
//-------------------------------------------------------------------------------------------------
// Get the configuration for a point
//-------------------------------------------------------------------------------------------------
{
	check(pPtConfigData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[SIOMM_POINT_CONFIG_BOUNDARY]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_POINT_CONFIG_READ_MOD_TYPE_BASE +
		(SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		SIOMM_POINT_CONFIG_BOUNDARY, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pPtConfigData->nModuleType = O22MAKELONG2(arrbyData, 0);
		pPtConfigData->nPointType = O22MAKELONG2(arrbyData, 4);
		pPtConfigData->nFeature = O22MAKELONG2(arrbyData, 8);
		pPtConfigData->fOffset = O22MAKEFLOAT2(arrbyData, 12);
		pPtConfigData->fGain = O22MAKEFLOAT2(arrbyData, 16);
		pPtConfigData->fHiScale = O22MAKEFLOAT2(arrbyData, 20);
		pPtConfigData->fLoScale = O22MAKEFLOAT2(arrbyData, 24);
		// Bytes 28-31 are not used at this time
		pPtConfigData->fFilterWeight = O22MAKEFLOAT2(arrbyData, 32);
		pPtConfigData->fWatchdogValue = O22MAKEFLOAT2(arrbyData, 36);
		pPtConfigData->nWatchdogEnabled = O22MAKELONG2(arrbyData, 40);

		memcpy(pPtConfigData->byName, arrbyData + 48, 16);
	}

	return nResult;
}


int O22SnapIoMemMap::GetPtConfigurationEx(int nPoint, SIOMM_PointConfigArea * pPtConfigData)
//-------------------------------------------------------------------------------------------------
// Get the configuration for a point
//-------------------------------------------------------------------------------------------------
{
	check(pPtConfigData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[44]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_POINT_CONFIG_READ_MOD_TYPE_BASE +
		(SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		44, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pPtConfigData->nModuleType = O22MAKELONG2(arrbyData, 0);
		pPtConfigData->nPointType = O22MAKELONG2(arrbyData, 4);

		pPtConfigData->nFeature = O22MAKELONG2(arrbyData, 8);
		pPtConfigData->fOffset = O22MAKEFLOAT2(arrbyData, 12);
		pPtConfigData->fGain = O22MAKEFLOAT2(arrbyData, 16);
		pPtConfigData->fHiScale = O22MAKEFLOAT2(arrbyData, 20);
		pPtConfigData->fLoScale = O22MAKEFLOAT2(arrbyData, 24);
		// Bytes 28-35 are not used at this time
		pPtConfigData->fWatchdogValue = O22MAKEFLOAT2(arrbyData, 36);
		pPtConfigData->nWatchdogEnabled = O22MAKELONG2(arrbyData, 40);
	}

	return nResult;
}


int O22SnapIoMemMap::SetPtWatchdog(int nPoint, float fValue, int nEnabled)
//-------------------------------------------------------------------------------------------------
// Set the watchdog values for a point.
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[8]; // buffer for the data to be read

	// Build the data area
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, fValue);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, nEnabled);

	// Write the data
	return WriteBlock(SIOMM_POINT_CONFIG_WRITE_WDOG_VALUE_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPoint),
		8, (uint8_t*)arrbyData);
}



int O22SnapIoMemMap::CalcSetAnaPtOffset(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Calculate and set an analog point's offset
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_CALC_SET_OFFSET_BASE +
		(SIOMM_APOINT_READ_CALC_SET_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::CalcSetAnaPtGain(int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Calculate and set an analog point's gain
//-------------------------------------------------------------------------------------------------
{
	check(pfValue != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM_APOINT_READ_CALC_SET_GAIN_BASE +
		(SIOMM_APOINT_READ_CALC_SET_BOUNDARY * nPoint),
		pfValue);
}


int O22SnapIoMemMap::SetAnalogOutputClamp(int nPoint, float fLowClamp, float fHighClamp)
//-------------------------------------------------------------------------------------------------
// Set low and high clamp values for an analog output point.
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[8]; // buffer for the data to be written

	// Build the data area
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, fLowClamp);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 4, fHighClamp);

	// Write the data
	return WriteBlock(SIOMM_POINT_CONFIG_WRITE_LOCLAMP_BASE +
		(SIOMM_POINT_CONFIG_CLAMP_BOUNDARY * nPoint),
		8, (uint8_t*)arrbyData);
}


int O22SnapIoMemMap::GetAnalogOutputClamp(int nPoint, float * pfLowClamp, float * pfHighClamp)
//-------------------------------------------------------------------------------------------------
// Get low and high clamp values for an analog output point.
//-------------------------------------------------------------------------------------------------
{
	check(pfLowClamp != NULL, SIOMM_ERROR);
	check(pfHighClamp != NULL, SIOMM_ERROR);

	uint8_t arrbyData[8]; // buffer for the data to be read

	int nResult = ReadBlock(SIOMM_POINT_CONFIG_WRITE_LOCLAMP_BASE +
		(SIOMM_POINT_CONFIG_CLAMP_BOUNDARY * nPoint),
		8, (uint8_t*)arrbyData);

	if (SIOMM_OK == nResult)
	{
		*pfLowClamp = O22MAKEFLOAT2(arrbyData, 0);
		*pfHighClamp = O22MAKEFLOAT2(arrbyData, 4);
	}

	return nResult;
}


int O22SnapIoMemMap::GetStatusPUC(int *pnPUCFlag)
//-------------------------------------------------------------------------------------------------
// Get the Powerup Clear flag
//-------------------------------------------------------------------------------------------------
{
	check(pnPUCFlag != NULL, SIOMM_ERROR);
	return ReadQuad(SIOMM_STATUS_READ_PUC_FLAG, (uint32_t*)pnPUCFlag);
}


int O22SnapIoMemMap::GetStatusLastError(int *pnErrorCode)
//-------------------------------------------------------------------------------------------------
// Get the last error
//-------------------------------------------------------------------------------------------------
{
	check(pnErrorCode != NULL, SIOMM_ERROR);
	return ReadQuad(SIOMM_STATUS_READ_LAST_ERROR, (uint32_t*)pnErrorCode);
}


int O22SnapIoMemMap::GetStatusBootpAlways(int *pnBootpAlways)
//-------------------------------------------------------------------------------------------------
// Get the "bootp always" flag
//-------------------------------------------------------------------------------------------------
{
	check(pnBootpAlways != NULL, SIOMM_ERROR);
	return ReadQuad(SIOMM_STATUS_READ_BOOTP_FLAG, (uint32_t*)pnBootpAlways);
}


int O22SnapIoMemMap::GetStatusDegrees(int *pnDegrees)
//-------------------------------------------------------------------------------------------------
// Get the temperature unit.
//-------------------------------------------------------------------------------------------------
{
	check(pnDegrees != NULL, SIOMM_ERROR);
	return ReadQuad(SIOMM_STATUS_READ_DEGREES_FLAG, (uint32_t*)pnDegrees);
}


int O22SnapIoMemMap::GetStatusWatchdogTime(int *pnTimeMS)
//-------------------------------------------------------------------------------------------------
// Get the device's watchdog time setting
//-------------------------------------------------------------------------------------------------
{
	check(pnTimeMS != NULL, SIOMM_ERROR);
	return ReadQuad(SIOMM_STATUS_READ_WATCHDOG_TIME, (uint32_t*)pnTimeMS);
}

int O22SnapIoMemMap::GetStatusVersionEx(SIOMM_StatusVersion *pVersionData)
//-------------------------------------------------------------------------------------------------
// Get version information
//-------------------------------------------------------------------------------------------------
{
	check(pVersionData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[32]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_STATUS_READ_BASE, 32, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pVersionData->nMapVer = O22MAKELONG2(arrbyData, 0);
		pVersionData->nLoaderVersion = O22MAKELONG2(arrbyData, 24);
		pVersionData->nKernelVersion = O22MAKELONG2(arrbyData, 28);
	}

	return nResult;
}


int O22SnapIoMemMap::GetStatusHardwareEx(SIOMM_StatusHardware *pHardwareData)
//-------------------------------------------------------------------------------------------------
// Get hardware information
//-------------------------------------------------------------------------------------------------
{
	check(pHardwareData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[44]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_STATUS_READ_BASE, 44, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pHardwareData->nIoUnitType = O22MAKELONG2(arrbyData, 32);
		pHardwareData->byHwdVerMonth = O22MAKEBYTE2(arrbyData, 36);
		pHardwareData->byHwdVerDay = O22MAKEBYTE2(arrbyData, 37);
		pHardwareData->wHwdVerYear = O22MAKEWORD2(arrbyData, 38);
		pHardwareData->nRamSize = O22MAKELONG2(arrbyData, 40);
	}

	return nResult;
}

int O22SnapIoMemMap::GetStatusHardwareEx2(SIOMM_StatusHardware2 *pHardwareData)
//-------------------------------------------------------------------------------------------------
// Get hardware information
//-------------------------------------------------------------------------------------------------
{
	check(pHardwareData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[SIOMM_STATUS_READ_PART_NUMBER_SIZE]; // buffer for the data to be read
	SIOMM_StatusHardware HardwareData1;

	// Use the original function first
	nResult = GetStatusHardwareEx(&HardwareData1);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Read the part number
		nResult = ReadBlock(SIOMM_STATUS_READ_PART_NUMBER,
			SIOMM_STATUS_READ_PART_NUMBER_SIZE, (uint8_t*)arrbyData);

		// Check for error
		if (SIOMM_OK == nResult)
		{
			// Copy the part number string
			memcpy(pHardwareData->byPartNumber, arrbyData, SIOMM_STATUS_READ_PART_NUMBER_SIZE);

			// If everything is okay, go ahead and fill the structure
			pHardwareData->nIoUnitType = HardwareData1.nIoUnitType;
			pHardwareData->byHwdVerMonth = HardwareData1.byHwdVerMonth;
			pHardwareData->byHwdVerDay = HardwareData1.byHwdVerDay;
			pHardwareData->wHwdVerYear = HardwareData1.wHwdVerYear;
			pHardwareData->nRamSize = HardwareData1.nRamSize;
		}
	}

	return nResult;
}


int O22SnapIoMemMap::GetStatusNetworkEx(SIOMM_StatusNetwork *pNetworkData)
//-------------------------------------------------------------------------------------------------
// Get network information
//-------------------------------------------------------------------------------------------------
{
	check(pNetworkData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[64]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_STATUS_READ_BASE, 64, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pNetworkData->wMACAddress0 = O22MAKEWORD2(arrbyData, 46);
		pNetworkData->wMACAddress1 = O22MAKEWORD2(arrbyData, 48);
		pNetworkData->wMACAddress2 = O22MAKEWORD2(arrbyData, 50);
		pNetworkData->nTCPIPAddress = O22MAKELONG2(arrbyData, 52);
		pNetworkData->nSubnetMask = O22MAKELONG2(arrbyData, 56);
		pNetworkData->nDefGateway = O22MAKELONG2(arrbyData, 60);
	}

	return nResult;
}


int O22SnapIoMemMap::GetStatusNetworkEx2(SIOMM_StatusNetwork2 *pNetworkData)
//-------------------------------------------------------------------------------------------------
// Get network information
//-------------------------------------------------------------------------------------------------
{
	check(pNetworkData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[124]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_STATUS_READ_BASE, 124, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pNetworkData->wMACAddress0 = O22MAKEWORD2(arrbyData, 46);
		pNetworkData->wMACAddress1 = O22MAKEWORD2(arrbyData, 48);
		pNetworkData->wMACAddress2 = O22MAKEWORD2(arrbyData, 50);
		pNetworkData->nTCPIPAddress = O22MAKELONG2(arrbyData, 52);
		pNetworkData->nSubnetMask = O22MAKELONG2(arrbyData, 56);
		pNetworkData->nDefGateway = O22MAKELONG2(arrbyData, 60);
		pNetworkData->nTcpIpMinRtoMS = O22MAKELONG2(arrbyData, 88);
		pNetworkData->nInitialRtoMS = O22MAKELONG2(arrbyData, 100);
		pNetworkData->nTcpRetries = O22MAKELONG2(arrbyData, 104);
		pNetworkData->nTcpIdleTimeout = O22MAKELONG2(arrbyData, 108);
		pNetworkData->nEnetLateCol = O22MAKELONG2(arrbyData, 112);
		pNetworkData->nEnetExcessiveCol = O22MAKELONG2(arrbyData, 116);
		pNetworkData->nEnetOtherErrors = O22MAKELONG2(arrbyData, 120);
	}

	return nResult;
}


int O22SnapIoMemMap::GetStatusWriteEx(SIOMM_StatusWrite * pStatusWriteData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pStatusWriteData != NULL, SIOMM_ERROR);

	// Read the data
	return ReadBlockAsIntegers(SIOMM_STATUS_WRITE_BASE, sizeof(SIOMM_StatusWrite) / 4, (int*)pStatusWriteData);
}


int O22SnapIoMemMap::SetStatusWriteEx(SIOMM_StatusWrite    StatusWriteData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	// Write the data
	return WriteBlockAsIntegers(SIOMM_STATUS_WRITE_BASE, sizeof(SIOMM_StatusWrite) / 4, (int*)(&StatusWriteData));
}


int O22SnapIoMemMap::SetStatusOperation(int nOpCode)
//-------------------------------------------------------------------------------------------------
// Issue an operation code
//-------------------------------------------------------------------------------------------------
{
	return WriteQuad(SIOMM_STATUS_WRITE_OPERATION, nOpCode);
}


int O22SnapIoMemMap::SetStatusBootpRequest(int nFlag)
//-------------------------------------------------------------------------------------------------
// Set the "bootp always" flag
//-------------------------------------------------------------------------------------------------
{
	return WriteQuad(SIOMM_STATUS_WRITE_BOOTP, nFlag);
}


int O22SnapIoMemMap::SetStatusDegrees(int nDegFlag)
//-------------------------------------------------------------------------------------------------
// Set the temperature unit
//-------------------------------------------------------------------------------------------------
{
	return WriteQuad(SIOMM_STATUS_WRITE_TEMP_DEGREES, nDegFlag);
}


int O22SnapIoMemMap::SetStatusWatchdogTime(int nTimeMS)
//-------------------------------------------------------------------------------------------------
// Set the watchdog time
//-------------------------------------------------------------------------------------------------
{
	return WriteQuad(SIOMM_STATUS_WRITE_WATCHDOG_TIME, nTimeMS);
}


int O22SnapIoMemMap::GetDateTime(char * pchDateTime)
//-------------------------------------------------------------------------------------------------
// Get the brain's date and time
//-------------------------------------------------------------------------------------------------
{
	check(pchDateTime != NULL, SIOMM_ERROR);

	int nResult;        // for checking the return values of functions
	uint8_t arrbyData[SIOMM_DATE_AND_TIME_SIZE]; // buffer for the data to be read

	nResult = ReadBlock(SIOMM_DATE_AND_TIME_BASE, SIOMM_DATE_AND_TIME_SIZE, (uint8_t*)arrbyData);

	// Check the result
	if (SIOMM_OK == nResult)
	{
		// Copy the date and time string.
		strncpy(pchDateTime, (char*)arrbyData, SIOMM_DATE_AND_TIME_SIZE);
	}

	return nResult;
}

int O22SnapIoMemMap::SetDateTime(char * pchDateTime)
//-------------------------------------------------------------------------------------------------
// Set the brain's date and time
//-------------------------------------------------------------------------------------------------
{
	check(pchDateTime != NULL, SIOMM_ERROR);
	return WriteBlock(SIOMM_DATE_AND_TIME_BASE, SIOMM_DATE_AND_TIME_SIZE, (uint8_t*)pchDateTime);
}


int O22SnapIoMemMap::GetAnaBank(uint32_t dwDestOffset, SIOMM_AnaBank * pBankData)
//-------------------------------------------------------------------------------------------------
// Get an analog bank section from a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	check(pBankData != NULL, SIOMM_ERROR);

	int nResult;        // for checking the return values of functions
	uint8_t arrbyData[SIOMM_ABANK_MAX_BYTES]; // buffer for the data to be read
	uint32_t dwQuadlet;     // A temp for getting the read value

	// Read the data
	nResult = ReadBlock(dwDestOffset, SIOMM_ABANK_MAX_BYTES, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Unpack the data packet
		for (int i = 0; i < SIOMM_ABANK_MAX_ELEMENTS; i++)
		{
			dwQuadlet = O22MAKELONG2(arrbyData, i * sizeof(int));

			// Copy the data
			memcpy(&(pBankData->fValue[i]), &dwQuadlet, sizeof(float));
		}
	}

	return nResult;
}


int O22SnapIoMemMap::SetAnaBank(uint32_t dwDestOffset, SIOMM_AnaBank BankData)
//-------------------------------------------------------------------------------------------------
// Set an analog bank section to a location in the SNAP I/O memory map.
//-------------------------------------------------------------------------------------------------
{
	int nResult;        // for checking the return values of functions
	uint8_t arrbyData[SIOMM_ABANK_MAX_BYTES]; // buffer for the data to be read
	uint32_t dwTemp;     // A temp for getting the read value

	// Pack the data packet
	for (int i = 0; i < SIOMM_ABANK_MAX_ELEMENTS; i++)
	{
		// Copy the float into a uint32_t for easy manipulation
		memcpy(&dwTemp, &(BankData.fValue[i]), sizeof(int));

		O22FILL_ARRAY_FROM_INT32(arrbyData, i * sizeof(float), dwTemp);
	}

	// Read the data
	nResult = WriteBlock(dwDestOffset, SIOMM_ABANK_MAX_BYTES, (uint8_t*)&arrbyData);

	return nResult;
}



int O22SnapIoMemMap::GetAnaBankValuesEx(SIOMM_AnaBank * pBankData)
//-------------------------------------------------------------------------------------------------
// Get the analog point values in the analog bank
//-------------------------------------------------------------------------------------------------
{
	check(pBankData != NULL, SIOMM_ERROR);
	return GetAnaBank(SIOMM_ABANK_READ_POINT_VALUES, pBankData);
}


int O22SnapIoMemMap::GetAnaBankCountsEx(SIOMM_AnaBank * pBankData)
//-------------------------------------------------------------------------------------------------
// Get the analog point raw counts in the analog bank
//-------------------------------------------------------------------------------------------------
{
	check(pBankData != NULL, SIOMM_ERROR);
	return GetAnaBank(SIOMM_ABANK_READ_POINT_COUNTS, pBankData);
}


int O22SnapIoMemMap::GetAnaBankMinValuesEx(SIOMM_AnaBank * pBankData)
//-------------------------------------------------------------------------------------------------
// Get the analog point minimum values in the analog bank
//-------------------------------------------------------------------------------------------------
{
	check(pBankData != NULL, SIOMM_ERROR);
	return GetAnaBank(SIOMM_ABANK_READ_POINT_MIN_VALUES, pBankData);
}

int O22SnapIoMemMap::GetAnaBankMaxValuesEx(SIOMM_AnaBank * pBankData)
//-------------------------------------------------------------------------------------------------
// Get the analog point maximum values in the analog bank
//-------------------------------------------------------------------------------------------------
{
	check(pBankData != NULL, SIOMM_ERROR);
	return GetAnaBank(SIOMM_ABANK_READ_POINT_MAX_VALUES, pBankData);
}


int O22SnapIoMemMap::GetAnaBanksEx(SIOMM_AnaBanks * pBankData)
//-------------------------------------------------------------------------------------------------
// Get the analog bank areas in one call
//-------------------------------------------------------------------------------------------------
{
	check(pBankData != NULL, SIOMM_ERROR);

	int nResult;        // for checking the return values of functions
	float arrfData[4 * SIOMM_ABANK_MAX_ELEMENTS]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlockAsFloats(SIOMM_ABANK_READ_POINT_VALUES, 4 * SIOMM_ABANK_MAX_ELEMENTS, (float*)arrfData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Unpack the data packet
		for (int i = 0; i < SIOMM_ABANK_MAX_ELEMENTS; i++)
		{
			// Values
			pBankData->Values.fValue[i] = arrfData[i];

			// Counts
			pBankData->Counts.fValue[i] = arrfData[i + SIOMM_ABANK_MAX_ELEMENTS];

			// Min Values
			pBankData->MinValues.fValue[i] = arrfData[i + 2 * SIOMM_ABANK_MAX_ELEMENTS];

			// Max Values
			pBankData->MaxValues.fValue[i] = arrfData[i + 3 * SIOMM_ABANK_MAX_ELEMENTS];
		}
	}

	return nResult;
}


int O22SnapIoMemMap::SetAnaBankValuesEx(SIOMM_AnaBank BankData)
//-------------------------------------------------------------------------------------------------
// Set the analog point values in the analog bank
//-------------------------------------------------------------------------------------------------
{
	return SetAnaBank(SIOMM_ABANK_WRITE_POINT_VALUES, BankData);
}


int O22SnapIoMemMap::SetAnaBankCountsEx(SIOMM_AnaBank BankData)
//-------------------------------------------------------------------------------------------------
// Set the analog point raw counts in the analog bank
//-------------------------------------------------------------------------------------------------
{
	return SetAnaBank(SIOMM_ABANK_WRITE_POINT_COUNTS, BankData);
}



int O22SnapIoMemMap::SetStreamConfiguration(int nOnFlag, int nIntervalMS, int nPort,
	int nIoMirroringEnabled, int nStartAddress,
	int nDataSize)
	//-------------------------------------------------------------------------------------------------
	// 
	//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[24]; // buffer for the data to be read

	// Build the data area

	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, nIoMirroringEnabled);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, nStartAddress);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 8, nDataSize);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, nOnFlag);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 16, nIntervalMS);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 20, nPort);

	// Write the data
	return WriteBlock(SIOMM_STREAM_CONFIG_BASE, 24, (uint8_t*)arrbyData);
}


int O22SnapIoMemMap::SetStreamTarget(int nTarget, char * pchIpAddressArg)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pchIpAddressArg != NULL, SIOMM_ERROR);

	int nIp1, nIp2, nIp3, nIp4;
	int nTemp;

	// Parse the IP address
	sscanf(pchIpAddressArg, "%d.%d.%d.%d", &nIp1, &nIp2, &nIp3, &nIp4);

	// Make an integer out of the IP address parts
	nTemp = O22MAKELONG(nIp1, nIp2, nIp3, nIp4);

	return WriteQuad(SIOMM_STREAM_TARGET_BASE + (SIOMM_STREAM_TARGET_BOUNDARY * (nTarget - 1)),
		nTemp);
}

int O22SnapIoMemMap::GetStreamConfiguration(int * pnOnFlag, int * pnIntervalMS, int * pnPort,
	int * pnIoMirroringEnabled, int * pnStartAddress,
	int * pnDataSize)
	//-------------------------------------------------------------------------------------------------
	// 
	//-------------------------------------------------------------------------------------------------
{
	check(pnOnFlag != NULL, SIOMM_ERROR);
	check(pnIntervalMS != NULL, SIOMM_ERROR);
	check(pnPort != NULL, SIOMM_ERROR);
	check(pnIoMirroringEnabled != NULL, SIOMM_ERROR);
	check(pnStartAddress != NULL, SIOMM_ERROR);
	check(pnDataSize != NULL, SIOMM_ERROR);

	int nResult;       // for checking the return values of functions
	uint8_t arrbyData[24]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_STREAM_CONFIG_BASE, 24, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		*pnIoMirroringEnabled = O22MAKELONG2(arrbyData, 0);
		*pnStartAddress = O22MAKELONG2(arrbyData, 4);
		*pnDataSize = O22MAKELONG2(arrbyData, 8);
		*pnOnFlag = O22MAKELONG2(arrbyData, 12);
		*pnIntervalMS = O22MAKELONG2(arrbyData, 16);
		*pnPort = O22MAKELONG2(arrbyData, 20);
	}

	return nResult;
}


int O22SnapIoMemMap::GetStreamTarget(int nTarget, int * pnIpAddressArg)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pnIpAddressArg != NULL, SIOMM_ERROR);

	int nResult;       // for checking the return values of functions

	// Read the data
	nResult = ReadQuad(SIOMM_STREAM_TARGET_BASE + (SIOMM_STREAM_TARGET_BOUNDARY * (nTarget - 1)),
		(uint32_t*)pnIpAddressArg);

	return nResult;
}


int O22SnapIoMemMap::GetStreamReadAreaEx(SIOMM_StreamStandardBlock * pStreamData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pStreamData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[SIOMM_STREAM_READ_AREA_SIZE]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_STREAM_READ_AREA_BASE, SIOMM_STREAM_READ_AREA_SIZE, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Flip-flop all the bytes
		int nTemp;
		for (int i = 0; i < SIOMM_STREAM_READ_AREA_SIZE; i += sizeof(int))
		{
			nTemp = O22MAKELONG2(arrbyData, i);

			memcpy((void*)(arrbyData + i), &nTemp, sizeof(int));
		}

		// Update the stream block
		memcpy(&(((char*)pStreamData)[4]), arrbyData, SIOMM_STREAM_READ_AREA_SIZE);
	}

	return nResult;
}

int O22SnapIoMemMap::GetSerialModuleConfigurationEx(int nSerialPort, SIOMM_SerialModuleConfigArea * pConfigData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pConfigData != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t arrbyData[SIOMM_SERIAL_MODULE_CONFIG_BOUNDARY]; // buffer for the data to be read
	uint32_t dwEomData;

	// Read the data
	nResult = ReadBlock(SIOMM_SERIAL_MODULE_CONFIG_BASE +
		(SIOMM_SERIAL_MODULE_CONFIG_BOUNDARY * nSerialPort),
		SIOMM_SERIAL_MODULE_CONFIG_BOUNDARY, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		nResult = ReadQuad(SIOMM_SERIAL_MODULE_EOM_BASE +
			(SIOMM_SERIAL_MODULE_EOM_BOUNDARY * nSerialPort),
			&dwEomData);

		// Check for error
		if (SIOMM_OK == nResult)
		{
			pConfigData->nIpPort = O22MAKELONG2(arrbyData, 0);
			pConfigData->nBaudRate = O22MAKELONG2(arrbyData, 4);
			pConfigData->byParity = arrbyData[8];
			pConfigData->byDataBits = arrbyData[9];
			pConfigData->byStopBits = arrbyData[10];
			// Byte 11 is reserved
			pConfigData->byTestMessage = arrbyData[12];
			pConfigData->byEOM1 = O22BYTE0(dwEomData);
			pConfigData->byEOM2 = O22BYTE1(dwEomData);
			pConfigData->byEOM3 = O22BYTE2(dwEomData);
			pConfigData->byEOM4 = O22BYTE3(dwEomData);
		}
	}

	return nResult;
}

int O22SnapIoMemMap::SetSerialModuleConfigurationEx(int nSerialPort, SIOMM_SerialModuleConfigArea ConfigData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_SERIAL_MODULE_CONFIG_BOUNDARY]; // buffer for the data to be read
	uint32_t dwEomTemp;
	int  nResult;

	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, ConfigData.nBaudRate);
	arrbyData[4] = O22BYTE3(ConfigData.byParity);
	arrbyData[5] = O22BYTE3(ConfigData.byDataBits);
	arrbyData[6] = O22BYTE3(ConfigData.byStopBits);
	// bytes 7 is reserved
	arrbyData[8] = O22BYTE3(ConfigData.byTestMessage);

	dwEomTemp = O22MAKELONG(ConfigData.byEOM1, ConfigData.byEOM2, ConfigData.byEOM3, ConfigData.byEOM4);

	// Write the first section
	nResult = WriteBlock(SIOMM_SERIAL_MODULE_CONFIG_BASE + sizeof(int) +
		(SIOMM_SERIAL_MODULE_CONFIG_BOUNDARY * nSerialPort),
		9, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section (End-of-message characters)
	return WriteQuad(SIOMM_SERIAL_MODULE_EOM_BASE + (SIOMM_SERIAL_MODULE_EOM_BOUNDARY * nSerialPort),
		dwEomTemp);

}


int O22SnapIoMemMap::GetScratchPadBitArea(int *pnBits63to32, int *pnBits31to0)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pnBits63to32 != NULL && pnBits31to0 != NULL, SIOMM_ERROR);
	return GetBitmask64(SIOMM_SCRATCHPAD_BITS_BASE, pnBits63to32, pnBits31to0);
}


int O22SnapIoMemMap::SetScratchPadBitArea(int nBits63to32, int nBits31to0)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	return SetBitmask64(SIOMM_SCRATCHPAD_BITS_BASE, nBits63to32, nBits31to0);
}


int O22SnapIoMemMap::SetScratchPadBitAreaMask(int nOnMask63to32, int nOnMaskPts31to0,
	int nOffMask63to32, int nOffMaskPts31to0)
	//-------------------------------------------------------------------------------------------------
	// 
	//-------------------------------------------------------------------------------------------------
{
	int nResult;       // for checking the return values of functions
	uint8_t arrbyData[16]; // buffer for the data to be read

	// Unpack the bytes
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, nOnMask63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, nOnMaskPts31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 8, nOffMask63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, nOffMaskPts31to0);

	// Write the data
	nResult = WriteBlock(SIOMM_SCRATCHPAD_BITS_ON_MASK_BASE, 16, (uint8_t*)arrbyData);

	return nResult;
}


int O22SnapIoMemMap::GetScratchPadIntegerArea(int nStartIndex, const int nLength, int * pnData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pnData != NULL, SIOMM_ERROR);
	check(nStartIndex >= 0 && nStartIndex < SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS, SIOMM_ERROR_INDEX_BAD);
	check(nLength > 0 && nLength <= (SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS - nStartIndex), SIOMM_ERROR_LENGTH_BAD);

	int nResult;
	int arrnData[SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS]; // max buffer for the data to be read

	// Read from the first scratch pad area
	if (nStartIndex < SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1)
	{
		int nReadIndex = nStartIndex;
		int nReadLength = nLength;

		// Calc the current read length
		if ((nReadIndex + nReadLength) > (SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1 - nReadIndex))
			nReadLength = SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1 - nReadIndex;

		// Read the data
		nResult = ReadBlockAsIntegers(SIOMM_SCRATCHPAD_INTEGER_BASE1 + (nReadIndex * sizeof(int)),
			(uint16_t)nReadLength, &(arrnData[nReadIndex]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	// Read from the second scratch pad area
	if ((nStartIndex > SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1) ||
		((nStartIndex + nLength) > SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1))
	{
		int nReadIndex;
		int nReadLength;

		// Figure out the read index and length
		if (nStartIndex > SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1)
		{
			nReadIndex = nStartIndex - SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1;
			nReadLength = nLength;
		}
		else
		{
			nReadIndex = 0;
			nReadLength = nLength - SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1 + nStartIndex;
		}

		// Read the data
		nResult = ReadBlockAsIntegers(SIOMM_SCRATCHPAD_INTEGER_BASE2 + (nReadIndex * sizeof(int)),
			(uint16_t)nReadLength,
			&(arrnData[nReadIndex + SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	// Check for an error
	if (SIOMM_OK == nResult)
	{
		// Everything was read okay, so fill in the array that was passed in
		for (int i = 0; i < nLength; i++)
		{
			pnData[i] = arrnData[nStartIndex + i];
		}
	}

	return nResult;
}


int O22SnapIoMemMap::SetScratchPadIntegerArea(int nStartIndex, int nLength, int * pnData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pnData != NULL, SIOMM_ERROR);
	check(nStartIndex >= 0 && nStartIndex < SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS, SIOMM_ERROR_INDEX_BAD);
	check(nLength > 0 && nLength <= (SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS - nStartIndex), SIOMM_ERROR_LENGTH_BAD);

	int nResult;

	// Write to the first scratch pad area
	if (nStartIndex < SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1)
	{
		int nWriteIndex = nStartIndex;
		int nWriteLength = nLength;

		// Calc the current write length
		if ((nWriteIndex + nWriteLength) > (SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1 - nWriteIndex))
			nWriteLength = SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1 - nWriteIndex;

		// Write the data
		nResult = WriteBlockAsIntegers(SIOMM_SCRATCHPAD_INTEGER_BASE1 + (nWriteIndex * sizeof(int)),
			(uint16_t)nWriteLength, &(pnData[nWriteIndex]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	// Write to the second scratch pad area
	if ((nStartIndex > SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1) ||
		((nStartIndex + nLength) > SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1))
	{
		int nWriteIndex;
		int nWriteLength;

		// Figure out the write index and length
		if (nStartIndex > SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1)
		{
			nWriteIndex = nStartIndex - SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1;
			nWriteLength = nLength;
		}
		else
		{
			nWriteIndex = 0;
			nWriteLength = nLength - SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1 + nStartIndex;
		}

		// Write the data
		nResult = WriteBlockAsIntegers(SIOMM_SCRATCHPAD_INTEGER_BASE2 + (nWriteIndex * sizeof(int)),
			(uint16_t)nWriteLength,
			&(pnData[nWriteIndex + SIOMM_SCRATCHPAD_INTEGER_MAX_ELEMENTS1]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	return nResult;
}


int O22SnapIoMemMap::GetScratchPadFloatArea(int nStartIndex, int nLength, float * pfData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pfData != NULL, SIOMM_ERROR);
	check(nStartIndex >= 0 && nStartIndex < SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS, SIOMM_ERROR_INDEX_BAD);
	check(nLength > 0 && nLength <= (SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS - nStartIndex), SIOMM_ERROR_LENGTH_BAD);

	int nResult;
	float arrfData[SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS]; // max buffer for the data to be read

	// Read from the first scratch pad area
	if (nStartIndex < SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1)
	{
		int nReadIndex = nStartIndex;
		int nReadLength = nLength;

		// Calc the current read length
		if ((nReadIndex + nReadLength) > (SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1 - nReadIndex))
			nReadLength = SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1 - nReadIndex;

		// Read the data
		nResult = ReadBlockAsFloats(SIOMM_SCRATCHPAD_FLOAT_BASE1 + (nReadIndex * sizeof(int)),
			(uint16_t)nReadLength, &(arrfData[nReadIndex]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	// Read from the second scratch pad area
	if ((nStartIndex > SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1) ||
		((nStartIndex + nLength) > SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1))
	{
		int nReadIndex;
		int nReadLength;

		// Figure out the read index and length
		if (nStartIndex > SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1)
		{
			nReadIndex = nStartIndex - SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1;
			nReadLength = nLength;
		}
		else
		{
			nReadIndex = 0;
			nReadLength = nLength - SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1 + nStartIndex;
		}

		// Read the data
		nResult = ReadBlockAsFloats(SIOMM_SCRATCHPAD_FLOAT_BASE2 + (nReadIndex * sizeof(int)),
			(uint16_t)nReadLength,
			&(arrfData[nReadIndex + SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	// Check for an error
	if (SIOMM_OK == nResult)
	{
		// Everything was read okay, so fill in the array that was passed in
		for (int i = 0; i < nLength; i++)
		{
			pfData[i] = arrfData[nStartIndex + i];
		}
	}

	return nResult;
}


int O22SnapIoMemMap::SetScratchPadFloatArea(int nStartIndex, int nLength, float * pfData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pfData != NULL, SIOMM_ERROR);
	check(nStartIndex >= 0 && nStartIndex < SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS, SIOMM_ERROR_INDEX_BAD);
	check(nLength > 0 && nLength <= (SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS - nStartIndex), SIOMM_ERROR_LENGTH_BAD);

	int nResult;

	// Write to the first scratch pad area
	if (nStartIndex < SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1)
	{
		int nWriteIndex = nStartIndex;
		int nWriteLength = nLength;

		// Calc the current write length
		if ((nWriteIndex + nWriteLength) > (SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1 - nWriteIndex))
			nWriteLength = SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1 - nWriteIndex;

		// Write the data
		nResult = WriteBlockAsFloats(SIOMM_SCRATCHPAD_FLOAT_BASE1 + (nWriteIndex * sizeof(int)),
			(uint16_t)nWriteLength, &(pfData[nWriteIndex]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	// Write to the second scratch pad area
	if ((nStartIndex > SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1) ||
		((nStartIndex + nLength) > SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1))
	{
		int nWriteIndex;
		int nWriteLength;

		// Figure out the write index and length
		if (nStartIndex > SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1)
		{
			nWriteIndex = nStartIndex - SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1;
			nWriteLength = nLength;
		}
		else
		{
			nWriteIndex = 0;
			nWriteLength = nLength - SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1 + nStartIndex;
		}

		// Write the data
		nResult = WriteBlockAsFloats(SIOMM_SCRATCHPAD_FLOAT_BASE2 + (nWriteIndex * sizeof(int)),
			(uint16_t)nWriteLength,
			&(pfData[nWriteIndex + SIOMM_SCRATCHPAD_FLOAT_MAX_ELEMENTS1]));

		// Return if there is an error
		if (SIOMM_OK != nResult)
			return nResult;
	}

	return nResult;
}


int O22SnapIoMemMap::GetScratchPadStringArea(int nStartIndex, int nLength,
	SIOMM_ScratchPadString * pStringData)
	//-------------------------------------------------------------------------------------------------
	// 
	//-------------------------------------------------------------------------------------------------
{
	check(pStringData != NULL, SIOMM_ERROR);

	int nResult;                                       // for checking the return values of functions
	uint8_t arrbyData[SIOMM_SCRATCHPAD_STRING_MAX_BYTES];  // buffer for the data to be read

	// Check the length
	if (nLength > SIOMM_SCRATCHPAD_STRING_MAX_ELEMENTS) // 8
		nLength = SIOMM_SCRATCHPAD_STRING_MAX_ELEMENTS;

	// Read the data
	uint16_t dataLength = (uint16_t)(nLength * SIOMM_SCRATCHPAD_STRING_BOUNDARY);
	nResult = ReadBlock(SIOMM_SCRATCHPAD_STRING_BASE + (nStartIndex * SIOMM_SCRATCHPAD_STRING_BOUNDARY),
		dataLength, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Unpack the data packet
		for (int i = 0; i < nLength; i++)
		{
			pStringData[i].wLength = O22MAKEWORD2(arrbyData, i * SIOMM_SCRATCHPAD_STRING_BOUNDARY);
			memcpy(pStringData[i].byString,
				arrbyData + (i * SIOMM_SCRATCHPAD_STRING_BOUNDARY) + SIOMM_SCRATCHPAD_STRING_LENGTH_SIZE,
				SIOMM_SCRATCHPAD_STRING_DATA_SIZE);
		}
	}

	return nResult;
}


int O22SnapIoMemMap::SetScratchPadStringArea(int nStartIndex, int nLength,
	SIOMM_ScratchPadString * pStringData)
	//-------------------------------------------------------------------------------------------------
	// 
	//-------------------------------------------------------------------------------------------------
{
	check(pStringData != NULL, SIOMM_ERROR);

	uint8_t arrbyData[SIOMM_SCRATCHPAD_STRING_MAX_BYTES]; // buffer 

	// Check the length
	if (nLength > SIOMM_SCRATCHPAD_STRING_MAX_ELEMENTS)
		nLength = SIOMM_SCRATCHPAD_STRING_MAX_ELEMENTS;

	// Pack the data packet
	for (int i = 0; i < nLength; i++)
	{
		O22FILL_ARRAY_FROM_INT16(arrbyData, i * SIOMM_SCRATCHPAD_STRING_BOUNDARY, pStringData[i].wLength);
		memcpy(arrbyData + (i * SIOMM_SCRATCHPAD_STRING_BOUNDARY) + SIOMM_SCRATCHPAD_STRING_LENGTH_SIZE,
			pStringData[i].byString,
			SIOMM_SCRATCHPAD_STRING_DATA_SIZE);
	}

	// Read the data
	uint16_t dataLength = (uint16_t)(nLength * SIOMM_SCRATCHPAD_STRING_BOUNDARY);
	return WriteBlock(SIOMM_SCRATCHPAD_STRING_BASE + (nStartIndex * SIOMM_SCRATCHPAD_STRING_BOUNDARY),
		dataLength, (uint8_t*)&arrbyData);
}


int O22SnapIoMemMap::GetScratchPadInteger64Area(int nStartIndex, int nLength,
	int * pnData)
	//-------------------------------------------------------------------------------------------------
	// pnData is twice the size of nLength, as each 64 bit integer is 2 longs.
	// The pattern is high, low, high, low.
	//-------------------------------------------------------------------------------------------------
{
	check(pnData != NULL, SIOMM_ERROR);
	check(nStartIndex >= 0 && nStartIndex < SIOMM_SCRATCHPAD_INTEGER_64_MAX_ELEMENTS, SIOMM_ERROR_INDEX_BAD);
	check(nLength > 0 && nLength <= (SIOMM_SCRATCHPAD_INTEGER_64_MAX_ELEMENTS - nStartIndex), SIOMM_ERROR_LENGTH_BAD);

	int nResult;
	int arrnData[SIOMM_SCRATCHPAD_INTEGER_64_MAX_ELEMENTS * 2]; // max buffer for the data to be read

	// Read the data from the scratch pad area
	nResult = ReadBlockAsIntegers(SIOMM_SCRATCHPAD_INTEGER_64_BASE + (nStartIndex * 64),
		(uint16_t)nLength * 2, &(arrnData[nStartIndex * 2]));

	// Check for an error
	if (SIOMM_OK == nResult)
	{
		// Everything was read okay, so fill in the array that was passed in
		for (int i = 0; i < nLength * 2; i += 2)
		{
			// Swap the low and high 4 bytes
			pnData[i] = arrnData[nStartIndex * 2 + i + 1];
			pnData[i + 1] = arrnData[nStartIndex * 2 + i];
		}
	}

	return nResult;
}


int O22SnapIoMemMap::SetScratchPadInteger64Area(int nStartIndex, int nLength,
	int * pnData)
	//-------------------------------------------------------------------------------------------------
	// pnData is twice the size of nLength, as each 64 bit integer is 2 longs.
	// The pattern is high, low, high, low.
	//-------------------------------------------------------------------------------------------------
{
	check(pnData != NULL, SIOMM_ERROR);
	check(nStartIndex >= 0 && nStartIndex < SIOMM_SCRATCHPAD_INTEGER_64_MAX_ELEMENTS, SIOMM_ERROR_INDEX_BAD);
	check(nLength > 0 && nLength <= (SIOMM_SCRATCHPAD_INTEGER_64_MAX_ELEMENTS - nStartIndex), SIOMM_ERROR_LENGTH_BAD);

	// Write the data to the scratch pad area
	return WriteBlockAsIntegers(SIOMM_SCRATCHPAD_INTEGER_64_BASE + (nStartIndex * 64),
		(uint16_t)nLength * 2, &(pnData[nStartIndex * 2]));
}


int O22SnapIoMemMap::ReadBlockAsIntegers(uint32_t dwDestOffset, uint16_t wDataLength, int * pnData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pnData != NULL, SIOMM_ERROR);
	check(wDataLength > 0 && wDataLength <= SIOMM_BLOCK_AS_INT_MAX_ELEMENTS, SIOMM_ERROR_LENGTH_BAD);

	int nResult;                            // for checking the return values of functions
	uint16_t nBytesToRead = wDataLength * sizeof(int);
	uint8_t * arrbyData = new uint8_t[nBytesToRead]; // buffer for the data to be read

	if (arrbyData == NULL)
	{
		return SIOMM_ERROR_OUT_OF_MEMORY; // Couldn't allocate memory!
	}

	// Read the data
	nResult = ReadBlock(dwDestOffset, nBytesToRead, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Unpack the data packet
		for (int i = 0; i < wDataLength; i++)
		{
			pnData[i] = O22MAKELONG2(arrbyData, i * sizeof(int));
		}
	}

	// Clean up the buffer
	delete[] arrbyData;

	return nResult;
}


int O22SnapIoMemMap::ReadBlockAsFloats(uint32_t dwDestOffset, uint16_t wDataLength, float * pfData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pfData != NULL, SIOMM_ERROR);
	check(wDataLength > 0 && wDataLength <= SIOMM_BLOCK_AS_FLT_MAX_ELEMENTS, SIOMM_ERROR_LENGTH_BAD);

	int nResult;                                   // for checking the return values of functions
	uint16_t nBytesToRead = wDataLength * sizeof(float);
	uint8_t * arrbyData = new uint8_t[nBytesToRead];      // buffer for the data to be read

	if (arrbyData == NULL)
	{
		return SIOMM_ERROR_OUT_OF_MEMORY; // Couldn't allocate memory!
	}

	// Read the data
	nResult = ReadBlock(dwDestOffset, nBytesToRead, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Unpack the data packet
		for (int i = 0; i < wDataLength; i++)
		{
			pfData[i] = O22MAKEFLOAT2(arrbyData, i * sizeof(float));
		}
	}

	// Clean up the buffer
	delete[] arrbyData;

	return nResult;
}

int O22SnapIoMemMap::WriteBlockAsIntegers(uint32_t dwDestOffset, uint16_t wDataLength, int  * pnData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pnData != NULL, SIOMM_ERROR);
	check(wDataLength > 0 && wDataLength <= SIOMM_BLOCK_AS_INT_MAX_ELEMENTS, SIOMM_ERROR_LENGTH_BAD);

	int nResult;                                  // for checking the return values of functions
	uint16_t nBytesToWrite = wDataLength * sizeof(int);
	uint8_t * arrbyData = new uint8_t[nBytesToWrite];    // buffer for the data to be written

	if (arrbyData == NULL)
	{
		return SIOMM_ERROR_OUT_OF_MEMORY; // Couldn't allocate memory!
	}

	// Pack the data packet
	for (int i = 0; i < wDataLength; i++)
	{
		O22FILL_ARRAY_FROM_INT32(arrbyData, i * sizeof(int), pnData[i]);
	}

	// Read the data
	nResult = WriteBlock(dwDestOffset, nBytesToWrite, arrbyData);

	// Clean up the buffer
	delete[] arrbyData;

	return nResult;
}


int O22SnapIoMemMap::WriteBlockAsFloats(uint32_t dwDestOffset, uint16_t wDataLength, float * pfData)
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
	check(pfData != NULL, SIOMM_ERROR);
	check(wDataLength > 0 && wDataLength <= SIOMM_BLOCK_AS_FLT_MAX_ELEMENTS, SIOMM_ERROR_LENGTH_BAD);

	int nResult;                                  // for checking the return values of functions
	uint16_t nBytesToWrite = wDataLength * sizeof(float);
	uint8_t * arrbyData = new uint8_t[nBytesToWrite];    // buffer for the data to be written

	if (arrbyData == NULL)
	{
		return SIOMM_ERROR_OUT_OF_MEMORY; // Couldn't allocate memory!
	}

	// Pack the data packet
	for (int i = 0; i < wDataLength; i++)
	{
		O22FILL_ARRAY_FROM_FLOAT(arrbyData, i * sizeof(float), pfData[i]);
	}

	// Read the data
	nResult = WriteBlock(dwDestOffset, nBytesToWrite, arrbyData);

	// Clean up the buffer
	delete[] arrbyData;

	return nResult;
}


int O22SnapIoMemMap::GetDataLoggingSamplesEx(int nStartIndex, int nLength,
	SIOMM_DataLogSample * pDataSamples)
	//-------------------------------------------------------------------------------------------------
	// 
	//-------------------------------------------------------------------------------------------------
{
	check(pDataSamples != NULL, SIOMM_ERROR);

	int nResult;      // for checking the return values of functions
	uint8_t * pbyData;
	int i;             // some compilers do not like reusing i if it is declared in a for loop.

	// Allocate a buffer
	pbyData = new uint8_t[nLength * sizeof(SIOMM_DataLogSample)];
	if (pbyData == NULL)
		return SIOMM_ERROR_OUT_OF_MEMORY; // Couldn't allocate memory!

	  // Read the samples in chunks of SIOMM_DATALOG_SAMPLES_READ_BLOCK_SIZE
	for (i = 0; i < nLength; i += SIOMM_DATALOG_SAMPLES_READ_BLOCK_COUNT)
	{
		int nNumSamplesToRead;

		// Check to see if this will be the last time through
		if ((i + SIOMM_DATALOG_SAMPLES_READ_BLOCK_COUNT) <= nLength)
			nNumSamplesToRead = SIOMM_DATALOG_SAMPLES_READ_BLOCK_COUNT;
		else
			nNumSamplesToRead = nLength - i;


		// Read the data
		nResult = ReadBlock(SIOMM_DATALOG_SAMPLES_BASE + SIOMM_DATALOG_SAMPLES_BOUNDARY * nStartIndex +
			SIOMM_DATALOG_SAMPLES_BOUNDARY * i,
			SIOMM_DATALOG_SAMPLES_BOUNDARY * nNumSamplesToRead,
			pbyData + SIOMM_DATALOG_SAMPLES_BOUNDARY * i);

		if (SIOMM_OK != nResult)
		{
			// cleanup and return
			delete[] pbyData;
			return nResult;
		}
	}

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// Flip-flop the bytes, as needed
		for (i = 0; i < nLength; i++)
		{
			// Copy the timestamp
			memcpy(&(pDataSamples[i].Time),
				pbyData + SIOMM_DATALOG_SAMPLES_BOUNDARY * i,
				sizeof(SIOMM_RTC_DATE_TIME));

			// Only the year needs to be flip-flopped
			pDataSamples[i].Time.wYear = O22MAKEWORD2(pbyData,
				SIOMM_DATALOG_SAMPLES_BOUNDARY * i);

			// Memmap address
			pDataSamples[i].nMemMap = O22MAKELONG2(pbyData,
				SIOMM_DATALOG_SAMPLES_BOUNDARY * i +
				SIOMM_DATALOG_SAMPLES_MEMMAP_OFFSET);

			// Data Format
			pDataSamples[i].nFormat = O22MAKELONG2(pbyData,
				SIOMM_DATALOG_SAMPLES_BOUNDARY * i +
				SIOMM_DATALOG_SAMPLES_FORMAT_OFFSET);

			// Data
			pDataSamples[i].unData.nInteger = O22MAKELONG2(pbyData,
				SIOMM_DATALOG_SAMPLES_BOUNDARY * i +
				SIOMM_DATALOG_SAMPLES_DATA_OFFSET);

		}
	}


	return nResult;
}



uint32_t O22SnapIoMemMap::GetTickCount()
//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------
{
#ifdef _WIN32
	return ::GetTickCount(); // GetTickCount returns the number of milliseconds that have
							 // elapsed since the system was started. 
#elif defined _LINUX
	tms DummyTime;
	return times(&DummyTime); // times() returns the number of milliseconds that have
							  // elapsed since the system was started. 
#else
#error "O22SnapIoMemMap::GetTickCount is not implemented on this platform; you'll need to supply it."
#endif
}


int O22SnapIoMemMap::SetPidLoopConfigurationEx(int nPid, SIOMM_PidLoopConfigArea Data)
//-------------------------------------------------------------------------------------------------
// Configure a PID Loop
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_PIDLOOP_CONFIG_BOUNDARY]; // buffer for the data to be written

	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, Data.fGain);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 4, Data.fTuneI);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, Data.fTuneD);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 12, Data.fTuneFF);

	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 16, Data.fOutMaxChange);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, Data.fOutMinChange);

	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 24, Data.fInRangeLo);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 28, Data.fInRangeHi);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 32, Data.fOutClampLo);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 36, Data.fOutClampHi);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 40, Data.fScanSec);

	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 44, Data.fOutWhenInLo);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 48, Data.fOutWhenInHi);

	O22FILL_ARRAY_FROM_INT32(arrbyData, 52, Data.nInputMemMap);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 56, Data.nSetpointMemMap);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 60, Data.nOutputMemMap);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 64, Data.nAlgorithm);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 68, Data.nManual);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 72, Data.nCfgFlags);

	return WriteBlock(SIOMM_PIDLOOP_CONFIG_GAIN_BASE + (SIOMM_POINT_CONFIG_BOUNDARY * nPid),
		76, (uint8_t*)arrbyData);
}


int O22SnapIoMemMap::SetTpoConfigurationEx(int nTpo, SIOMM_TpoConfigArea Data)
//-------------------------------------------------------------------------------------------------
// Configure a TPO or Pulse Train
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_TPO_CONFIG_BOUNDARY]; // buffer for the data to be written
	int  nResult;

	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, Data.fPercent);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 4, Data.fPeriod);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, Data.fDelaySec);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, Data.nPulseQty);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 16, Data.nCfgWord);
	//O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, 0); // nCfgWord Must On
	//O22FILL_ARRAY_FROM_FLOAT(arrbyData, 24, 0); // nCfgWord Must Off
	O22FILL_ARRAY_FROM_INT32(arrbyData, 28, Data.nOutputMemMap);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 32, Data.nOutputMask);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, Data.nOutputMemMapOff);


	// Write the first section
	nResult = WriteBlock(SIOMM_TPO_CONFIG_MEMMAP_BASE + (SIOMM_TPO_CONFIG_BOUNDARY * nTpo),
		20, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section
	return WriteBlock(SIOMM_TPO_CONFIG_OUTPUT_MEMMAP_BASE + (SIOMM_TPO_CONFIG_BOUNDARY * nTpo),
		12, (uint8_t*)(&(arrbyData[28])));
}


int O22SnapIoMemMap::GetTpoConfigurationEx(int nTpo, SIOMM_TpoConfigArea * pTpoData)
//-------------------------------------------------------------------------------------------------
// Get the configuration for a TPO (more accurately, any pulsed output)
//-------------------------------------------------------------------------------------------------
{
	int nResult;
	uint8_t arrbyData[SIOMM_TPO_CONFIG_BOUNDARY]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM_TPO_CONFIG_MEMMAP_BASE + (SIOMM_TPO_CONFIG_BOUNDARY * nTpo),
		SIOMM_TPO_CONFIG_BOUNDARY, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pTpoData->fPercent = O22MAKEFLOAT2(arrbyData, 0);
		pTpoData->fPeriod = O22MAKEFLOAT2(arrbyData, 4);
		pTpoData->fDelaySec = O22MAKEFLOAT2(arrbyData, 8);
		pTpoData->nPulseQty = O22MAKELONG2(arrbyData, 12);
		pTpoData->nCfgWord = O22MAKELONG2(arrbyData, 16);
		pTpoData->nOutputMemMap = O22MAKELONG2(arrbyData, 28);
		pTpoData->nOutputMask = O22MAKELONG2(arrbyData, 32);
		pTpoData->nOutputMemMapOff = O22MAKELONG2(arrbyData, 36);
	}

	return nResult;
}


int O22SnapIoMemMap::SetEventMsgConfigurationEx(int nMsg, SIOMM_EventMsgConfigArea Data)
//-------------------------------------------------------------------------------------------------
// Configure an Event Message
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_EVENT_MSG_BOUNDARY]; // buffer for the data to be written
	int  nResult;

	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, Data.nState);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, Data.nScratchOn63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 8, Data.nScratchOn31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, Data.nScratchOff63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 16, Data.nScratchOff31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 20, Data.nStreamEnabled);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 24, Data.nStreamPeriod);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 28, Data.nEmailEnabled);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 32, Data.nEmailPeriod);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, Data.nSnmpEnabled);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 40, Data.nSnmpStreamPeriod);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 44, Data.nSnmpSpecificType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 48, Data.nPriority);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 52, 0); // reserved
	O22FILL_ARRAY_FROM_INT32(arrbyData, 56, Data.nSerialEnabled);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 60, Data.nSerialStreamPeriod);

	memcpy(arrbyData + 64, Data.byMessage, SIOMM_EVENT_MSG_MSG_LENGTH);

	// Write the first section
	nResult = WriteBlock(SIOMM_EVENT_MSG_MEMMAP_BASE + (SIOMM_EVENT_MSG_BOUNDARY * nMsg),
		52, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section (End-of-message characters)
	return WriteBlock(SIOMM_EVENT_MSG_MEMMAP_BASE + 56 + (SIOMM_EVENT_MSG_BOUNDARY * nMsg),
		134, (uint8_t*)(&(arrbyData[56])));
}


int O22SnapIoMemMap::SetDigitalEventConfigurationEx(int nEvent, SIOMM_DigitalEventConfigArea Data)
//-------------------------------------------------------------------------------------------------
// Configure a Digital Event
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_DIGITAL_EVENT_BOUNDARY]; // buffer for the data to be written

	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, Data.nEventDigitalOn63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, Data.nEventDigitalOn31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 8, Data.nEventDigitalOff63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, Data.nEventDigitalOff31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 16, Data.nEventScratchOn63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 20, Data.nEventScratchOn31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 24, Data.nEventScratchOff63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 28, Data.nEventScratchOff31to0);

	O22FILL_ARRAY_FROM_INT32(arrbyData, 32, Data.nReactionDigitalOn63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, Data.nReactionDigitalOn31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 40, Data.nReactionDigitalOff63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 44, Data.nReactionDigitalOff31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 48, Data.nReactionScratchOn63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 52, Data.nReactionScratchOn31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 56, Data.nReactionScratchOff63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 60, Data.nReactionScratchOff31to0);

	// Write the second section (End-of-message characters)
	return WriteBlock(SIOMM_DIGITAL_EVENT_MEMMAP_BASE + (SIOMM_DIGITAL_EVENT_BOUNDARY * nEvent),
		SIOMM_DIGITAL_EVENT_BOUNDARY, (uint8_t*)(&arrbyData));
}


int O22SnapIoMemMap::SetAlarmEventConfigurationEx(int nEvent, SIOMM_AlarmEventConfigArea Data)
//-------------------------------------------------------------------------------------------------
// Configure an Alarm Event
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_ALARM_EVENT_BOUNDARY]; // buffer for the data to be written

	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, Data.nDeviationInAlarm);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 4, Data.nDeviationEnabled);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, Data.fDeviationMiddleRange);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 12, Data.fDeviationAmount);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 16, Data.nDeviationScratchOn63to32);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, Data.nDeviationScratchOn31to0);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 24, Data.nDeviationScratchOff63to32);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 28, Data.nDeviationScratchOff31to0);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 32, Data.nHiAlarmInAlarm);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 36, Data.nHiAlarmEnabled);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 40, Data.fHiAlarmLimit);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 44, Data.fHiAlarmLimitDeadband);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 48, Data.nHiAlarmScratchOn63to32);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 52, Data.nHiAlarmScratchOn31to0);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 56, Data.nHiAlarmScratchOff63to32);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 60, Data.nHiAlarmScratchOff31to0);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 64, Data.nLoAlarmInAlarm);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 68, Data.nLoAlarmEnabled);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 72, Data.fLoAlarmLimit);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 76, Data.fLoAlarmLimitDeadband);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 80, Data.nLoAlarmScratchOn63to32);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 84, Data.nLoAlarmScratchOn31to0);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 88, Data.nLoAlarmScratchOff63to32);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 92, Data.nLoAlarmScratchOff31to0);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 96, Data.nValueLogicalAdr);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 100, Data.nIsFloat); // Bool

	// Write the second section (End-of-message characters)
	return WriteBlock(SIOMM_ALARM_EVENT_MEMMAP_BASE + (SIOMM_ALARM_EVENT_BOUNDARY * nEvent),
		104, (uint8_t*)(&arrbyData));
}


int O22SnapIoMemMap::SetSerialEventConfigurationEx(int nEvent, SIOMM_SerialEventConfigArea Data)
//-------------------------------------------------------------------------------------------------
// Configure an Alarm Event
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_SERIAL_EVENT_BOUNDARY]; // buffer for the data to be written


	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, Data.nSerialPortsMask);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, 0); // reserved
	O22FILL_ARRAY_FROM_INT32(arrbyData, 8, Data.nTrapType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, Data.nTrapPeriod);

	memcpy(arrbyData + 16, Data.byPattern, 40);
	memcpy(arrbyData + 56, Data.byReplace, 40);

	O22FILL_ARRAY_FROM_INT32(arrbyData, 96, Data.nScratchOn63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 100, Data.nScratchOn31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 104, Data.nScratchOff63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 108, Data.nScratchOff31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 112, Data.nSendEmail);


	// Write the second section (End-of-message characters)
	return WriteBlock(SIOMM_SERIAL_EVENT_MEMMAP_BASE + (SIOMM_SERIAL_EVENT_BOUNDARY * nEvent),
		SIOMM_SERIAL_EVENT_BOUNDARY, (uint8_t*)(&arrbyData));
}



int O22SnapIoMemMap::SetTimerEventConfigurationEx(int nMsg, SIOMM_TimerEventConfigArea Data)
//-------------------------------------------------------------------------------------------------
// Configure an Event Message
//-------------------------------------------------------------------------------------------------
{
	uint8_t arrbyData[SIOMM_TIMER_EVENT_BOUNDARY]; // buffer for the data to be written
	int  nResult;

	uint32_t writeBase = SIOMM_TIMER_EVENT_MEMMAP_BASE + (SIOMM_TIMER_EVENT_BOUNDARY * nMsg);
	check((writeBase + SIOMM_TIMER_EVENT_BOUNDARY) < SIOMM_TIMER_EVENT_MEMMAP_END, SIOMM_ERROR);

	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, Data.nDigitalIn_On63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, Data.nDigitalIn_On31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 8, Data.nDigitalIn_Off63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 12, Data.nDigitalIn_Off31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 16, Data.nScratchIn_On63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 20, Data.nScratchIn_On31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 24, Data.nScratchIn_Off63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 28, Data.nScratchIn_Off31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 32, Data.nDigitalOut_On63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, Data.nDigitalOut_On31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 40, Data.nDigitalOut_Off63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 44, Data.nDigitalOut_Off31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 48, Data.nScratchOut_On63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 52, Data.nScratchOut_On31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 56, Data.nScratchOut_Off63to32);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 60, Data.nScratchOut_Off31to0);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 64, Data.nPresetMsec);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 68, 0); // reserved
	O22FILL_ARRAY_FROM_INT32(arrbyData, 72, Data.nRemainingMsec);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 76, Data.nState);

	// Write the first section
	nResult = WriteBlock(writeBase, 68, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section (End-of-message characters)
	return WriteBlock(writeBase + 72, 8, (uint8_t*)(&(arrbyData[72])));
}


int O22SnapIoMemMap::CalcSetAnalogPointOffsetEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Calculate and set an analog point's offset
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfValue != NULL, SIOMM_ERROR);

	int nResult;

	nResult = ReadFloat(SIOMM4096_APOINT_READ_CALC_SET_OFFSET_BASE +
		(SIOMM4096_APOINT_READ_CALC_SET_BOUNDARY * (nModule * 64 + nPoint)),
		pfValue);

	return nResult;
}


int O22SnapIoMemMap::CalcSetAnalogPointGainEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Calculate and set an analog point's gain
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfValue != NULL, SIOMM_ERROR);

	int nResult;

	nResult = ReadFloat(SIOMM4096_APOINT_READ_CALC_SET_GAIN_BASE +
		(SIOMM4096_APOINT_READ_CALC_SET_BOUNDARY * (nModule * 64 + nPoint)),
		pfValue);

	return nResult;
}


int O22SnapIoMemMap::ConfigurePoint(int nModule, int nPoint, int nPointType)
//-------------------------------------------------------------------------------------------------
// Configure the specified point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	int nResult;

	nResult = WriteQuad(SIOMM4096_POINT_CONFIG_WRITE_TYPE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		nPointType);

	return nResult;
}

int O22SnapIoMemMap::SetPointFeatureEx(int nModule, int nPoint, int nPointFeature)
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	int nResult;

	nResult = WriteQuad(SIOMM4096_POINT_CONFIG_WRITE_FEATURE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		nPointFeature);

	return nResult;
}

int O22SnapIoMemMap::GetPointFeatureEx(int nModule, int nPoint, int * pnPointFeature)
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	int nResult;

	nResult = ReadQuad(SIOMM4096_POINT_CONFIG_WRITE_FEATURE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		(uint32_t *)pnPointFeature);

	return nResult;
}


int O22SnapIoMemMap::GetModuleType(int nModule, int nPoint, int *pnModuleType)
//-------------------------------------------------------------------------------------------------
// Get the module type at the specified point position
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	int nResult;

	nResult = ReadQuad(SIOMM4096_POINT_CONFIG_READ_MOD_TYPE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		(uint32_t*)pnModuleType);

	return nResult;
}

int O22SnapIoMemMap::GetDigitalPointReadAreaEx(int nModule, int nPoint, SIOMM_DigPointReadArea * pData)
//-------------------------------------------------------------------------------------------------
// Get the read area for the specified digital point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pData != NULL, SIOMM_ERROR);

	int nResult;

	uint8_t arrbyData[20]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM4096_DPOINT_READ_AREA_BASE +
		(SIOMM4096_DPOINT_READ_BOUNDARY * (nModule * 64 + nPoint)),
		20, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pData->nState = O22MAKELONG2(arrbyData, 0);
		pData->nOnLatch = O22MAKELONG2(arrbyData, 4);
		pData->nOffLatch = O22MAKELONG2(arrbyData, 8);
		pData->nCounterState = O22MAKELONG2(arrbyData, 12);
		pData->nCounts = O22MAKELONG2(arrbyData, 16);
	}

	return nResult;
}


int O22SnapIoMemMap::GetPointConfigurationEx4(int nModule, int nPoint, SIOMM_PointConfigArea4 *pPtConfigData)
//-------------------------------------------------------------------------------------------------
// Get the configuration for a point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pPtConfigData != NULL, SIOMM_ERROR);

	int nResult;

	uint8_t arrbyData[SIOMM4096_POINT_CONFIG_BOUNDARY]; // buffer for the data to be read

	// Read the data
	nResult = ReadBlock(SIOMM4096_POINT_CONFIG_READ_MOD_TYPE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		SIOMM4096_POINT_CONFIG_BOUNDARY, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK == nResult)
	{
		// If everything is okay, go ahead and fill the structure
		pPtConfigData->nModuleType = O22MAKELONG2(arrbyData, 0);
		pPtConfigData->nPointType = O22MAKELONG2(arrbyData, 4);
		pPtConfigData->nFeature = O22MAKELONG2(arrbyData, 8);
		pPtConfigData->fOffset = O22MAKEFLOAT2(arrbyData, 12);
		pPtConfigData->fGain = O22MAKEFLOAT2(arrbyData, 16);
		pPtConfigData->fHiScale = O22MAKEFLOAT2(arrbyData, 20);
		pPtConfigData->fLoScale = O22MAKEFLOAT2(arrbyData, 24);
		// Bytes 28-31 are not used at this time
		pPtConfigData->fFilterWeight = O22MAKEFLOAT2(arrbyData, 32);
		pPtConfigData->fWatchdogValue = O22MAKEFLOAT2(arrbyData, 36);
		pPtConfigData->nWatchdogEnabled = O22MAKELONG2(arrbyData, 40);

		memcpy(pPtConfigData->byName, arrbyData + 48, SIOMM4096_POINT_CONFIG_NAME_SIZE);
	}

	return nResult;
}


int O22SnapIoMemMap::GetPointGain(int nModule, int nPoint, float *pfGain)
//-------------------------------------------------------------------------------------------------
// Get the configuration for Gain for a point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfGain != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM4096_POINT_CONFIG_WRITE_GAIN_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		pfGain);
}


int O22SnapIoMemMap::GetPointOffset(int nModule, int nPoint, float *pfOffset)
//-------------------------------------------------------------------------------------------------
// Get the configuration for Offset for a point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfOffset != NULL, SIOMM_ERROR);

	return ReadFloat(SIOMM4096_POINT_CONFIG_WRITE_OFFSET_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		pfOffset);
}


int O22SnapIoMemMap::SetPointConfigurationEx4(int nModule, int nPoint, SIOMM_PointConfigArea4 PtConfigData)
//-------------------------------------------------------------------------------------------------
// Configure a digital or analog point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	int nResult;

	uint8_t arrbyData[SIOMM4096_POINT_CONFIG_BOUNDARY]; // buffer for the data to be written

	// Build the data area
	O22FILL_ARRAY_FROM_INT32(arrbyData, 0, PtConfigData.nPointType);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, PtConfigData.nFeature);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 8, PtConfigData.fOffset);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 12, PtConfigData.fGain);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 16, PtConfigData.fHiScale);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 20, PtConfigData.fLoScale);
	// Bytes 24-27 are not used at this time. 
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 28, PtConfigData.fFilterWeight);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 32, PtConfigData.fWatchdogValue);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 36, PtConfigData.nWatchdogEnabled);

	memcpy(arrbyData + 44, PtConfigData.byName, SIOMM4096_POINT_CONFIG_NAME_SIZE_NO_NULL);

	int nPointOffset = nModule * 64 + nPoint;
	// Write the first section
	nResult = WriteBlock(SIOMM4096_POINT_CONFIG_WRITE_TYPE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * nPointOffset),
		24, (uint8_t*)arrbyData);

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the second section
	nResult = WriteBlock(SIOMM4096_POINT_CONFIG_WRITE_TYPE_BASE + 28 +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * nPointOffset),
		12, (uint8_t*)(&(arrbyData[28])));

	// Check for error
	if (SIOMM_OK != nResult)
		return nResult;

	// Write the third section
	return WriteBlock(SIOMM4096_POINT_CONFIG_WRITE_TYPE_BASE + 44 +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * nPointOffset),
		SIOMM4096_POINT_CONFIG_NAME_SIZE_NO_NULL, (uint8_t*)(&(arrbyData[44])));
}


int O22SnapIoMemMap::SetPointFilterWeight(int nModule, int nPoint, float fFilterWeight)
//-------------------------------------------------------------------------------------------------
// Set the filter weight for a point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	return WriteFloat(SIOMM4096_POINT_CONFIG_WRITE_FILTER_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		fFilterWeight);
}


int O22SnapIoMemMap::SetPointGain(int nModule, int nPoint, float fGain)
//-------------------------------------------------------------------------------------------------
// Set the gain for a point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	return WriteFloat(SIOMM4096_POINT_CONFIG_WRITE_GAIN_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		fGain);
}


int O22SnapIoMemMap::SetPointName(int nModule, int nPoint, char *pchPointName)
//-------------------------------------------------------------------------------------------------
// Set the name for a point
// Copy one less than the amount that will be sent to ensure null termination
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pchPointName != NULL, SIOMM_ERROR);

	int nResult;
	uint8_t arrbyData[SIOMM4096_POINT_CONFIG_NAME_SIZE]; // buffer for the data to be written
	int nLength, nSize;

	nLength = (int)strlen(pchPointName);
	if (nLength > 0)
	{
		// Fill the array with nulls
		memset(arrbyData, 0, SIOMM4096_POINT_CONFIG_NAME_SIZE);

		nSize = (nLength < SIOMM4096_POINT_CONFIG_NAME_SIZE_NO_NULL) ? nLength : SIOMM4096_POINT_CONFIG_NAME_SIZE_NO_NULL;
		strncpy((char*)arrbyData, pchPointName, nSize);
		nResult = WriteBlock(SIOMM4096_POINT_CONFIG_WRITE_NAME_BASE +
			(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
			nSize + 1, arrbyData);
	}

	return nResult;
}


int O22SnapIoMemMap::SetPointOffset(int nModule, int nPoint, float fOffset)
//-------------------------------------------------------------------------------------------------
// Set the offset for a point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	return WriteFloat(SIOMM4096_POINT_CONFIG_WRITE_OFFSET_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		fOffset);
}


int O22SnapIoMemMap::SetPointScale(int nModule, int nPoint, float fHiScale, float fLoScale)
//-------------------------------------------------------------------------------------------------
// Set the scaling values for a point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	int nResult;

	uint8_t arrbyData[8]; // buffer for the data to be written

	// Build the data area
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, fHiScale);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 4, fLoScale);

	// Write the data
	nResult = WriteBlock(SIOMM4096_POINT_CONFIG_WRITE_HISCALE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		8, (uint8_t*)arrbyData);

	return nResult;
}


int O22SnapIoMemMap::SetPointWatchdog(int nModule, int nPoint, float fValue, int nEnabled)
//-------------------------------------------------------------------------------------------------
// Set the watchdog values for a point.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	int nResult;

	uint8_t arrbyData[8]; // buffer for the data to be written

	// Build the data area
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, fValue);
	O22FILL_ARRAY_FROM_INT32(arrbyData, 4, nEnabled);

	// Write the data
	nResult = WriteBlock(SIOMM4096_POINT_CONFIG_WRITE_WDOG_VALUE_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		8, (uint8_t*)arrbyData);

	return nResult;
}


int O22SnapIoMemMap::SetAnalogOutputClamp(int nModule, int nPoint, float fLowClamp, float fHighClamp)
//-------------------------------------------------------------------------------------------------
// Set low and high clamp values on an analog output point.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	uint8_t arrbyData[8]; // buffer for the data to be written

	// Build the data area
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 0, fLowClamp);
	O22FILL_ARRAY_FROM_FLOAT(arrbyData, 4, fHighClamp);

	// Write the data
	return WriteBlock(SIOMM4096_POINT_CONFIG_WRITE_LOCLAMP_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		8, (uint8_t*)arrbyData);
}


int O22SnapIoMemMap::GetAnalogOutputClamp(int nModule, int nPoint, float * pfLowClamp, float * pfHighClamp)
//-------------------------------------------------------------------------------------------------
// Get low and high clamp values on an analog output point.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfLowClamp != NULL, SIOMM_ERROR);
	check(pfHighClamp != NULL, SIOMM_ERROR);

	uint8_t arrbyData[8]; // buffer for the data to be read

	int nResult = ReadBlock(SIOMM4096_POINT_CONFIG_WRITE_LOCLAMP_BASE +
		(SIOMM4096_POINT_CONFIG_BOUNDARY * (nModule * 64 + nPoint)),
		8, (uint8_t*)arrbyData);

	if (SIOMM_OK == nResult)
	{
		*pfLowClamp = O22MAKEFLOAT2(arrbyData, 0);
		*pfHighClamp = O22MAKEFLOAT2(arrbyData, 4);
	}

	return nResult;
}

int O22SnapIoMemMap::GetAnalogPointValueEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Gets the value of a high density analog point.  Point is specified by module number and point 
// location within that module.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfValue != NULL, SIOMM_ERROR);

	uint32_t readOffset = ExpandedAnalogPointReadBase(nModule, nPoint);
	return ReadFloat(readOffset, pfValue);
}

int O22SnapIoMemMap::GetAnalogPointCountsEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the raw counts of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfValue != NULL, SIOMM_ERROR);

	uint32_t readOffset = ExpandedAnalogPointReadBase(nModule, nPoint) + 4;
	return ReadFloat(readOffset, pfValue);
}

int O22SnapIoMemMap::GetAnalogPointMinValueEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the minimum value of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfValue != NULL, SIOMM_ERROR);

	uint32_t readOffset = ExpandedAnalogPointReadBase(nModule, nPoint) + 8;
	return ReadFloat(readOffset, pfValue);
}

int O22SnapIoMemMap::GetAnalogPointMaxValueEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the maximum value of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfValue != NULL, SIOMM_ERROR);

	uint32_t readOffset = ExpandedAnalogPointReadBase(nModule, nPoint) + 12;
	return ReadFloat(readOffset, pfValue);
}

int O22SnapIoMemMap::GetAnalogPointReadAreaEx(int nModule, int nPoint, SIOMM_AnaPointReadArea * pData)
//-------------------------------------------------------------------------------------------------
// Get the read area for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pData != NULL, SIOMM_ERROR);

	uint32_t readOffset = ExpandedAnalogPointReadBase(nModule, nPoint);
	uint8_t data[16];
	int result = ReadBlock(readOffset, 16, data);
	if (result == SIOMM_OK) {
		pData->fValue = O22MAKEFLOAT2(data, 0);
		pData->fCounts = O22MAKEFLOAT2(data, 4);
		pData->fMinValue = O22MAKEFLOAT2(data, 8);
		pData->fMaxValue = O22MAKEFLOAT2(data, 12);
	}

	return result;
}

int O22SnapIoMemMap::GetAnalogPointTpoPeriodEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Get the TPO period of the specified analog point.
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);
	check(pfValue != NULL, SIOMM_ERROR);

	uint32_t readOffset = ExpandedAnalogPointWriteBase(nModule, nPoint) + 12;
	return ReadFloat(readOffset, pfValue);
}

int O22SnapIoMemMap::SetAnalogPointValueEx(int nModule, int nPoint, float fValue)
//-------------------------------------------------------------------------------------------------
// Set the value for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	uint32_t writeOffset = ExpandedAnalogPointWriteBase(nModule, nPoint);
	return WriteFloat(writeOffset, fValue);
}

int O22SnapIoMemMap::SetAnalogPointCountsEx(int nModule, int nPoint, float fValue)
//-------------------------------------------------------------------------------------------------
// Set the raw counts for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	uint32_t writeOffset = ExpandedAnalogPointWriteBase(nModule, nPoint) + 4;
	return WriteFloat(writeOffset, fValue);
}

int O22SnapIoMemMap::SetAnalogPointTpoPeriodEx(int nModule, int nPoint, float fValue)
//-------------------------------------------------------------------------------------------------
// Set the TPO period for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	uint32_t writeOffset = ExpandedAnalogPointWriteBase(nModule, nPoint) + 12;
	return WriteFloat(writeOffset, fValue);
}

int O22SnapIoMemMap::ReadClearAnalogPointMinValueEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Read and clear the minimum value for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	uint32_t offset = ExpandedAnalogPointReadClearBase(nModule, nPoint);
	float value;
	int result = ReadFloat(offset, &value);

	// It's considered valid to call a Read and Clear method just to clear a value, so 
	// the caller may just pass in NULL for pfValue.
	if (pfValue != NULL) {
		*pfValue = value;
	}
	return result;
}

int O22SnapIoMemMap::ReadClearAnalogPointMaxValueEx(int nModule, int nPoint, float *pfValue)
//-------------------------------------------------------------------------------------------------
// Read and clear the maximum value for the specified analog point
//-------------------------------------------------------------------------------------------------
{
	check(nModule >= 0 && nModule < SIOMM4096_MAX_MODULES, SIOMM_ERROR);
	check(nPoint >= 0 && nPoint < SIOMM4096_MAX_POINTS, SIOMM_ERROR);

	uint32_t offset = ExpandedAnalogPointReadClearBase(nModule, nPoint) + 4;
	float value;
	int result = ReadFloat(offset, &value);

	// It's considered valid to call a Read and Clear method just to clear a value, so 
	// the caller may just pass in NULL for pfValue.
	if (pfValue != NULL) {
		*pfValue = value;
	}
	return result;
}

int O22SnapIoMemMap::EpicGetDigitalChannel(int nModuleIndex, int nChannelIndex, SIOMM_EpicDigitalChannelRead * pstructEpicDiscrete)
{
	check(nModuleIndex >= 0 && nModuleIndex < 64, SIOMM_ERROR);
	check(nChannelIndex >= 0 && nChannelIndex < 64, SIOMM_ERROR);

	unsigned int u32Address = 0xf01e0000 + (nModuleIndex * 0x1000) + (nChannelIndex * 0x40);
	return ReadBlockAsIntegers(u32Address, sizeof(SIOMM_EpicDigitalChannelRead) / sizeof(int), (int *)pstructEpicDiscrete);
}

int O22SnapIoMemMap::EpicGetDigitalPacked(SIOMM_EpicPackedDigitalRead * pstructEpicPackedDigital)
{
	return ReadBlockAsIntegers(0xf1001900, 16 * (sizeof(SIOMM_EpicPackedDigitalRead) / sizeof(int)), (int *)pstructEpicPackedDigital);
}

int O22SnapIoMemMap::EpicGetAnalogChannel(int nModuleIndex, int nChannelIndex, SIOMM_EpicAnalogChannelRead * pstructEpicAnalog)
{
	check(nModuleIndex >= 0 && nModuleIndex < 64, SIOMM_ERROR);
	check(nChannelIndex >= 0 && nChannelIndex < 64, SIOMM_ERROR);

	unsigned int u32Address = 0xf0260000 + (nModuleIndex * 0x1000) + (nChannelIndex * 0x40);
	return ReadBlockAsIntegers(u32Address, sizeof(SIOMM_EpicDigitalChannelRead) / sizeof(int), (int *)pstructEpicAnalog);
}

int O22SnapIoMemMap::EpicGetModuleQuality(SIOMM_ModuleQuality * pstructModuleQuality)
{
	return ReadBlockAsIntegers(0xf1002000, sizeof(SIOMM_ModuleQuality) / sizeof(int), (int *)pstructModuleQuality);
}

int O22SnapIoMemMap::EpicGetChannelQuality(int nModuleIndex, SIOMM_ChannelQuality * pstructChannelQuality)
{
	check(nModuleIndex >= 0 && nModuleIndex < 64, SIOMM_ERROR);

	unsigned int u32Address = 0xf1002080 + (nModuleIndex * 64 * 4);
	return ReadBlockAsIntegers(u32Address, sizeof(SIOMM_ChannelQuality) / sizeof(int), (int *)pstructChannelQuality);
}

int O22SnapIoMemMap::GetCustomAreaAddresses(int nStartIndex, int nLength, SIOMM_CustomAreaAddresses * pstructCustomAddresses)
{
	unsigned int u32Address = 0xf0d50000 + (nStartIndex * 4);
	return ReadBlockAsIntegers(u32Address, nLength, (int *)pstructCustomAddresses);
}

int O22SnapIoMemMap::SetCustomAreaAddresses(int nStartIndex, int nLength, const SIOMM_CustomAreaAddresses * pstructCustomAddresses)
{
	unsigned int u32Address = 0xf0d50000 + (nStartIndex * 4);
	return WriteBlockAsIntegers(u32Address, nLength, (int *)pstructCustomAddresses);
}

int O22SnapIoMemMap::GetCustomAreaData(int nStartIndex, int nLength, SIOMM_CustomAreaData * pstructCustomData)
{
	unsigned int u32Address = 0xf0d60000 + (nStartIndex * 4);
	return ReadBlockAsIntegers(u32Address, nLength, (int *)pstructCustomData);
}

int O22SnapIoMemMap::SetCustomAreaData(int nStartIndex, int nLength, const SIOMM_CustomAreaData * pstructCustomData)
{
	unsigned int u32Address = 0xf0d60000 + (nStartIndex * 4);
	return WriteBlockAsIntegers(u32Address, nLength, (int *)pstructCustomData);
}

int O22SnapIoMemMap::WriteBytes(uint32_t dwOptoMMPAddress, int nLength, const uint8_t * pbyary)
{
	return WriteBlock(dwOptoMMPAddress, (uint16_t)nLength, (uint8_t *)pbyary);
}

int O22SnapIoMemMap::ReadBytes(uint32_t dwOptoMMPAddress, int nLength, uint8_t * pbyary)
{
	return ReadBlock(dwOptoMMPAddress, (uint16_t)nLength, (uint8_t *)pbyary);
}

// readblock/writeblock
