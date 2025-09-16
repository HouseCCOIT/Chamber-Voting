//-----------------------------------------------------------------------------
//
// O22SIOST.h
// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
// Copyright (c) 2000-2018 by Opto 22
//
// Header for the O22SnapIoStream C++ class. 
//
// This implementation only supports IPv4.
// 
// While this code was developed on Microsoft Windows targeting x86 operating 
// systems, it is intended to be as generic as possible.  For Windows specific
// code, search for "_WIN32".  For Linux specific code, search for "_LINUX".
//
//-----------------------------------------------------------------------------

#ifndef __O22SIOST_H_
#define __O22SIOST_H_


#ifndef __O22SIOUT_H
#include "O22SIOUT.h"
#endif


#ifndef __O22STRCT_H
#include "O22STRCT.h"
#endif


// These type #defines are used in OpenStreaming()
#define SIOMM_STREAM_TYPE_STANDARD           1
#define SIOMM_STREAM_TYPE_CUSTOM             2

// These callback functions definitions are used in StartStreamListening()
typedef int(*STREAM_CALLBACK_PROC)(void * pUserParam);
typedef int(*STREAM_EVENT_CALLBACK_PROC)(int nTCPIPAddress, void * pUserParam, int nResult);


/**
 * The O22StreamItem is used to create a linked list of I/O units that
 * will be listened to.
 */
struct O22StreamItem
{
	uint32_t        nIpAddress;           ///< IP address of brain
	uint32_t        nTimeout;             ///< Timeout duration
	uint32_t        nLastPacketTickCount; ///< For tracking timeouts
	bool            bTimeoutSent;         ///< Flag for if a timeout error was sent already
	O22StreamItem * pNext;                ///< Next item in the list
};

/**
 * The O22SnapIoStream C++ class is used to listen to UDP stream packets
 * from multiple Opto 22 SNAP OptoMMP devices streaming to the same port on
 * the computer running this code. Please note that not all OptoMMP devices
 * support streaming.
 *
 * The basic procedure for using this class is:
 *
 *   1. Create an instance of the O22SnapIoStream class
 *   2. Call SetCallbackFuntions() to initialize several callback functions
 *   3. Call OpenStreaming() to initialize the stream type, length, and port
 *   4. Call StartStreamListening() for each I/O unit.  A second thread will
 *      be created to listen for incoming UDP packets.
 *   5. The Stream Event callback function will be called every time a stream
 *      packet from a registered I/O unit is received. Every time this callback
 *      is called, the function GetLastStreamStandardBlockEx() or
 *      GetLastStreamCustomBlockEx() should be called, depending on the type
 *      set in step #3.
 *   6. StopStreamListening() may be called at any time to stop listening
 *      for a specified I/O unit.
 *   7. StartStreamListening() may be called at any time to add I/O units.
 *   8. When done, call CloseStreaming()
 *
 * This class will consume all UDP packets sent to the port specified in
 * OpenStreaming(). No other program on the same computer can listen to UDP
 * packets on the same port while this code is running.
 */
class O22SnapIoStream {

public:
	// Public data

	  // Public Construction/Destruction
	O22SnapIoStream();
	~O22SnapIoStream();

	// Public Members

	// Stream functions

	  /** Opens and initializes the computer's port ot start listening for stream packets.
	   *
	   * \param nType    SIOMM_STREAM_TYPE_STANDARD for standard streams,
	   *                 SIOMM_STREAM_TYPE_CUSTOM for custom streams.
	   *                 The type is set in the I/O unit's stream configuration. This must match
	   *                 it and all I/O units streaming to the same port should be of the same type.
	   * \param nLength  If using custom streams, the size of a single custom block.
	   * \param nPort    The port being streamed to
	   * \return SIOMM_OK if everything initialized correctly.
	   */
	int OpenStreaming(long nType, long nLength, long nPort);

	/** Stops all listening and closes the port.
	 *
	 * \return SIOMM_OK if everything is OK, an error otherwise.
	 */
	int CloseStreaming();

	/** Sets the three callback functions used by the stream listening thread.
	 *
	 * \param pStartThreadCallbackFunc This callback function is called whenever the stream
	 *                                 listening thread is started. Can be NULL if the callback
	 *                                 function is not needed.
	 * \param pStartThreadParam        User data that will be passed back to pStartThreadCallbackFunc.
	 *                                 Can be NULL.
	 * \param pStreamEventCallbackFunc This callback funciton is called whenver a stream packet from
	 *                                 an I/O unit is received.
	 * \param pStreamEventParam        User data that will be passed back to pStreamEventCallbackFunc.
	 *                                 Can be NULL.
	 * \param pStopThreadCallbackFunc  This callback function is called when the stream listening thread
	 *                                 is stopped. Can be NULL.
	 * \param pStopThreadParam         User data that will be passed back to pStopThreadCallbackFunc.
	 *                                 Can be NULL.
	 * \return SIOMM_OK
	 */
	int SetCallbackFunctions(STREAM_CALLBACK_PROC pStartThreadCallbackFunc,
		void * pStartThreadParam,
		STREAM_EVENT_CALLBACK_PROC pStreamEventCallbackFunc,
		void * pStreamEventParam,
		STREAM_CALLBACK_PROC pStopThreadCallbackFunc,
		void * pStopThreadParam);

	/** Starts listening for packets from an I/O unit at the specified IP address.
	 *
	 * More than one I/O unit may be listened to by multiple calls to this function.
	 *
	 * \param pchIpAddressArg IP address of I/O unit in string form, e.g. "1.2.3.4"
	 * \param nTimeoutMS Timeout value used to generate error events if the specified length
	 *                   of time has passed since a stream packet has been received. Different
	 *                   I/O units can have different timeout values.
	 * \return SIOMM_OK if everything is OK, an error otherwise.
	 */
	int StartStreamListening(char * pchIpAddressArg, long nTimeoutMS);

	/** Stops listening to the I/O unit at the specified address.
	 *
	 * \param pchIpAddressArg IP address of I/O unit in string form, e.g. "1.2.3.4"
	 * \return SIOMM_OK if everything is OK, an error otherwise.
	 */
	int StopStreamListening(char * pchIpAddressArg);

	/** Returns the last stream packet received.
	 *
	 * This method should be called immediately after receiving a packet event via
	 * the pStreamEventCallbackFunc callback function set in SetCallbackFunctions.
	 *
	 * This method is used if you're using standard streams. If you're using custom
	 * streams, use GetLastStreamCustomBlockEx.
	 *
	 * \param pStreamData Pointer to a SIOMM_StreamStandardBlock instance to hold the received data.
	 */
	int GetLastStreamStandardBlockEx(SIOMM_StreamStandardBlock *pStreamData);

	/** Returns the last stream packet received.
	 *
	 * This method should be called immediately after receiving a packet event via
	 * the pStreamEventCallbackFunc callback function set in SetCallbackFunctions.
	 *
	 * This method is used if you're using custom streams. If you're using standard
	 * streams, use GetLastStreamStandardBlockEx.
	 *
	 * \param pStreamData Pointer to a SIOMM_StreamCustomBlock instance to hold the received data.
	 */
	int GetLastStreamCustomBlockEx(SIOMM_StreamCustomBlock *pStreamData);


	/** Main worker method.
	 *
	 * NOTE: This method is not considered a public API and should not be called by user programs.
	 * It's public in the class sense only so that the StreadThread() function in O22SIOST.cpp
	 * can call it.
	 *
	 * Called repeatedly by the StreamThread() function. This method checks the socket
	 * for new packets and determines if the user is interested in them.
	 */
	int StreamHandler();


	/** Checks if any stream items have timed out.
	 *
	 * NOTE: This method is not considered a public API and should not be called by user programs.
	 * It's public in the class sense only so that the StreadThread() function in O22SIOST.cpp
	 * can call it.
	 */
	int CheckStreamTimeouts();



	bool m_bListenToStreaming; ///< A flag used in StreamThread() to know when to stop listening

	uint8_t * m_pbyLastStreamBlock; ///< Byte array containing the last block received

	SIOMM_StreamCustomBlock m_LastStreamBlock; ///< Structure containing the last stream block received.
											   ///< It can be converted to a SIOMM_StreadStandardBlock
											   ///< instance in GetLastStreamStandardBlockEx


	// The following members are used to store the callback functions and 
	// user parameters set in the SetCallbackFuntions() function.
	STREAM_CALLBACK_PROC         m_pStartThreadCallbackFunc; ///< User callback function
	STREAM_EVENT_CALLBACK_PROC   m_pStreamEventCallbackFunc; ///< User callback function 
	STREAM_CALLBACK_PROC         m_pStopThreadCallbackFunc;  ///< User callback functio
	void                       * m_pStartThreadParam;        ///< User callback parameter
	void                       * m_pStreamEventParam;        ///< User callback parameter
	void                       * m_pStopThreadParam;         ///< User callback parameter

protected:

	// Protected data

	// Member data for streaming
	SOCKET      m_StreamSocket;  ///< The handle to the UDP socket

	long        m_nStreamType;   ///< The type set in OpenStreaming()
	long        m_nStreamLength; ///< The length set in OpenStreaming()

#ifdef _WIN32
	uintptr_t        m_hStreamThread; ///< Handle to the stream listening thread
	CRITICAL_SECTION m_StreamCriticalSection;
#endif
#ifdef _LINUX
	pthread_t        m_hStreamThread; ///< Handle to the stream listening thread
	pthread_mutex_t  m_StreamCriticalSection;
#endif


	// This class keeps a linked list of I/O units to listen for. 
	// The O22StreamItem structure keeps information on each I/O unit and has 
	// a pNext pointer to let us make the list structure. 
	O22StreamItem * pStreamList;      ///< Head of a list of I/O units to listen for
	long            nStreamListCount; ///< The number of items in pStreamList
};


#endif // __O22SIOST_H_
