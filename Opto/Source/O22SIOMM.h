//-----------------------------------------------------------------------------
//
// O22SIOMM.h
// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
// Copyright (c) 1999 - 2018 by Opto 22
//
// Header for the O22SnapIoMemMap C++ class.  
// 
// The O22SnapIoMemMap C++ class is used to communicate from a computer to an
// Opto 22 OptoMMP device.
//
// The basic procedure for using this class is:
//
//   1. Create an instance of the O22SnapIoMemMap class
//   2. Call OpenEnet() or OpenEnet2() to start connecting to an I / O unit
//   3. Call IsOpenDone() to complete the connection  process
//   4. Call SetCommOptions() to set the desired timeout value
//   5. Configure the I / O(scaling, point types, ranges, baud rates, etc) using
//      like ConfigurePt().
//   6. Call any of the memory map functions, such as GetAnaPtValue(),
//      SetDigBankPointStates(), and ReadBlock(), to communicate with the I / O unit.
//      Repeat as neccessary.
//   7. Call Close() when completely fnished messaging the device or a fault
//      occurred.Applications typically operate in a loop(step 6),
//      regularly accessing the device.
//
// While this code was developed on Microsoft Windows targeting x86 operating 
// systems, it is intended to be as generic as possible.  For Windows specific
// code, search for "_WIN32".  For Linux specific code, search for "_LINUX".
// We validated this implementation on Ubuntu 16.04 desktop 64-bit with
// the gnu compiler collection.
//-----------------------------------------------------------------------------

#ifndef __O22SIOMM_H_
#define __O22SIOMM_H_


#ifndef __O22SIOUT_H
#include "O22SIOUT.h"
#endif


#ifndef __O22STRCT_H
#include "O22STRCT.h"
#endif

#ifdef _WIN32
#include <WS2tcpip.h>
#endif

#pragma pack(push)
#pragma pack(1)

/** Serial MMP header for transmitting. */
typedef struct SERIAL_HEADER_X_T // x for x-mit 
{
	unsigned char  m_u8Address;    ///< Device's serial protocol address
	unsigned char  m_u8Type;       ///< Protocol type (2 for kernel mode)
	unsigned short m_u16Crc;       ///< CRC16R, initialize to zero before doing the crc
	unsigned short m_u16Length;    ///< Length of the data payload in bytes (not including the length of this header)
	// data payload bytes follow
} SERIAL_HEADER_X_T;

/** Serial MMP header for receiving. */
typedef struct SERIAL_HEADER_R_T // r for receive
{
	unsigned char  m_u8Type;       ///< Protocol type (2 for kernel mode)
	unsigned short m_u16Crc;       ///< CRC16R, initialize to zero before doing the crc
	unsigned short m_u16Length;    ///< Length of the data payload in bytes (not including the length of this header)
	// data payload bytes follow
} SERIAL_HEADER_R_T;

#pragma pack(pop)


typedef struct SERIAL_PASS_THRU_CONFIG
{
	uint32_t Enable;   ///< Enables or Disables the Serial Pass Through feature.
	uint32_t Bps;      ///< Bits per second (Data Rate).
	uint32_t DataBits; ///< Number of data bits.
	uint32_t Parity;   ///< Parity mode.
	uint32_t StopBits; ///< Number of stop bits.
	uint32_t Duplex;   ///< Full-Duplex = ASCII "F", Half-Duplex = ASCII "H"
	uint32_t Timeout;  ///< Timeout in milliseconds to wait for the response.
} SERIAL_PASS_THRU_CONFIG;

typedef struct SERIAL_PASS_THRU_RW
// follow this header with the data bytes.
{
	uint32_t Address;  ///< The 8-bit multidrop address of the serial brain you want to talk to.
	uint32_t Type;     ///< The 8-bit packet type identifier.  Tells you what kind of packet it is.
	uint32_t Length;   ///< The 16-bit length of the Data field.
} SERIAL_PASS_THRU_RW;

enum ConnectionMethod
{
	ethernet,             ///< Connect with a normal ip address to an tcp/ip based device
	serial,               ///< Connect from the PC to a serial device
	passthru              ///< Connect to a serial device through an SNAP-PAC-S series controller
};

/**
 * The O22SnapIoMemMap C++ class is used to communicate from a computer to an
 * Opto 22 OptoMMP device.
 *
 * The basic procedure for using this class is:
 *
 *   1. Create an instance of the O22SnapIoMemMap class
 *   2. Call OpenEnet() or OpenEnet2() to start connecting to an I/O unit
 *   3. Call IsOpenDone() to complete the connection  process
 *   4. Call SetCommOptions() to set the desired timeout value
 *   5. Configure the I/O (scaling, point types, ranges, baud rates, etc) using
 *      like ConfigurePt().
 *   6. Call any of the memory map functions, such as GetAnaPtValue(),
 *      SetDigBankPointStates(), and ReadBlock(), to communicate with the I/O unit.
 *      Repeat as neccessary.
 *   7. Call Close() when completely fnished messaging the device or a fault
 *      occurred. Applications typically operate in a loop (step 6),
 *      regularly accessing the device.
 */
class O22SnapIoMemMap {

public:
	O22SnapIoMemMap();
	~O22SnapIoMemMap();

	// Connection functions

	/** Starts an asynchronous (unblocked) connection attempt to an OptoMMP device.
	 *
	 * Use the IsOpenDone() method to check if the open connection
	 * is completed.
	 *
	 * This method always connects via TCP. If you wish to use a UDP connection, call
	 * OpenEnet2 instead. Use TCP where network packet drops are expected.
	 *
	 * @see OpenEnet2
	 * @see IsOpenDone
	 *
	 * \param pchIpAddressArg The IPv4 address, IPV6 address or hostname of the OptoMMP device as a null terminated string.
	 * \param nPort The port that the Ethernet brain is listening (usually 2001)
	 * \param nOpenTimeOutMS Time to wait for a connection before giving up (milliseconds). The minimum timeout is 100 milliseconds.
	 * \param nAutoPUC If non-zero, clear the brain's Powerup Clear flag automatically.
	 * \return SIOMM_OK if everything is OK, an error otherwise.
	 */
	int OpenEnet(const char * pchIpAddressArg, int nPort, int nOpenTimeOutMS,
		int nAutoPUC);

	/** Starts the connection process to an OptoMMP device.
	 *
	 * Use the IsOpenDone() method to check if the open connection
	 * is completed.
	 *
	 * @see IsOpenDone
	 *
	 * \param pchIpAddressArg The IPv4 address, IPV6 address or hostname of the OptoMMP device as a null terminated string.
	 * \param nPort The port that the Ethernet brain is listening (usually 2001)
	 * \param nOpenTimeOutMS Time to wait for a connection before giving up (milliseconds). The minimum timeout is 100 milliseconds.
	 * \param nAutoPUC If non-zero, clear the brain's Powerup Clear flag automatically.
	 * \param nConnectionType Use define SIOMM_TCP or SIOMM_UDP for a TCP or UDP connection. These values coorelate to the equivalents
	 *        of SOCK_STREAM or SOCK_DGRAM (respectively). In other words, the numeric constants may change on different platforms.
	 * \return SIOMM_OK if everything is OK, an error otherwise.
	 */
	int OpenEnet2(const char * pchIpAddressArg, int nPort, int nOpenTimeOutMS,
		int nAutoPUC, int nConnectionType);

	/** Connects to a serial OptoMMP device (SNAP-PAC-SB1 or SNAP-PAC-SB2).
	 *
	 * There is no need to call IsOpenDone() when using this connection method: either
	 * the connection will succeed or fail to lock the serial port.
	 *
	 * \param nPort Port number from your PC, e.g. 3 for COM3
	 * \param nAddress Address is set on the brain, e.g. 0xAD
	 * \param nBaudRate Also set on the brain, e.g. 230400
	 * \param nOpenTimeOutMS Time to wait for a connection before giving up (milliseconds)
	 * \param nAutoPUC If non-zero, clear the brain's Powerup Clear flag automatically.
	 * \return SIOMM_OK if we connect, an error otherwise.
	 */
	int OpenSerial(int nPort, uint8_t nAddress, int nBaudRate, int nOpenTimeOutMS,
		int nAutoPUC);

	/** Connects to a serial OptoMMP device (SNAP-PAC-SB1 or SNAP-PAC-SB2) through an Ethernet controller. (SNAP-PAC-S1 or SNAP-PAC-S2).
	 *
	 * \param pchIpAddressArg IP address or hostname of the Ethernet controller to connect through, as a string.
	 * \param nPort The port that the Ethernet controller is listening (usually 2001).
	 * \param nConnectionType SIOMM_TCP or SIOMM_UDP for a TCP or UDP connection.
	 * \param nSerialPort Port number from the controller, e.g. 1 for COM1.
	 * \param nSerialAddress Address is set on the brain, e.g. 0xAD
	 * \param nSerialBaudRate Also set on the brain, e.g. 230400
	 * \param bTwoWire
	 * \param nOpenTimeOutMS Time to wait for a connection before giving up (milliseconds)
	 * \param nAutoPUC If non-zero, clear the brain's Powerup Clear flag automatically.
	 * \return SIOMM_OK if connect, an error otherwise.
	 */
	int OpenSerialPassThru(char * pchIpAddressArg, int nPort,
		int nSerialPort, uint8_t nSerialAddress, int nSerialBaudRate, bool bTwoWire,
		int nOpenTimeOutMS, int nAutoPUC, int nConnectionType = SIOMM_TCP);

	/** Called after OpenEnet() or OpenEnet2() to determine if the connection has completed yet.
	 *
	 * This method should be called repeatedly until it returns something other than
	 * SIOMM_ERROR_NOT_CONNECTED_YET.
	 *
	 * If SIOMM_OK is returned, the connection process is complete and there is a connection
	 * to the I/O unit.
	 *
	 * If the nAutoPUC flag in OpenEnet() or OpenEnet2() was set to TRUE, then this method will
	 * attempt to read and clear the I/O units Powerup Clear (PUC) flag after the connection has
	 * been made.
	 *
	 * \return SIOMM_ERROR_NOT_CONNECTED_YET if the connection process isn't complete yet.
	 * \return SIOMM_TIME_OUT if the connection process timed out.
	 * \return SIOMM_OK if the connection is complete and ready to be used.
	 * \return Or possibly another error.
	 */
	int IsOpenDone();

	/** Close the connection to the OptoMMP device.
	 *
	 * \return SIOMM_OK if everything is OK, an error otherwise.
	 */
	int Close();

	/** Set communication options, such as the connection's communication timeout value.
	 *
	 * \param nTimeOutMS Timeout period for normal communications.
	 * \param nReserved Not used at this time. Set to 0.
	 * \return SIOMM_OK if everything is OK, an error otherwise.
	 */
	int SetCommOptions(int nTimeOutMS, int nReserved);

	/** Returns the current timeout period as set by SetCommOptions()
	 *
	 * \return The current timeout period in milliseconds.
	 */
	int GetCommTimeout() { return m_nTimeOutMS; }


	// The following functions are for building and unpacking read/write 
	// requests for the OptoMMP protocol.

	/** Builds a write block request packet.
	 *
	 * This is an internally used utility method to build a write request. Client code
	 * isn't likely to need it.
	 *
	 * \param pbyWriteBlockRequest User supplied pointer to a block of memory to hold the write request. Must not be NULL, must be at least 16 bytes + wDataLength in size.
	 * \param byTransactionLabel Transaction label to embed in the write request. Lets the caller identify response packets.
	 * \param dwDestinationOffset Target offset to write data to
	 * \param wDataLength Length of the data in pbyBlockData
	 * \param pbyBlockData Data to be written, this is appended to the end of the request.
	 * \return SIOMM_ERROR if pbyWriteBlockRequest or pbyBlockData are NULL, SIOMM_OK otherwise.
	 */
	int BuildWriteBlockRequest(uint8_t  * pbyWriteBlockRequest,
		uint8_t    byTransactionLabel,
		uint32_t   dwDestinationOffset,
		uint16_t    wDataLength,
		uint8_t  * pbyBlockData);

	/** Unpacks a write response packet.
	 *
	 * This is an internally used utility method; client code isn't likely to need it.
	 *
	 * \param pbyWriteQuadletResponse Response packet from the server.
	 * \param pbyTransactionLabel User supplied pointer to a uint8_t to hold the transaction label from the packet
	 * \param pbyResponseCode User supplied pointer to a uint8_t to hold the response code from the packet.
	 * \return SIOMM_ERROR if any of the input parameters are NULL, or if this is not a write response packet. SIOMM_OK otherwise.
	 */
	int UnpackWriteResponse(uint8_t * pbyWriteQuadletResponse,
		uint8_t * pbyTransactionLabel,
		uint8_t * pbyResponseCode);

	/** Builds a read block request packet.
	 *
	 * This is an internally used utility method to build a read request. Client code
	 * isn't likely to need it.
	 *
	 * \param pbyReadBlockRequest User supplied pointer to a block of memory to hold the read request. Must not be NULL, must be at least 16 bytes in size.
	 * \param byTransactionLabel Transaction label to embed in the read request. Lets the caller identify response packets.
	 * \param dwDestinationOffset The target offset to read from
	 * \param wDataLength The amount of data to read, in bytes.
	 * \return SIOMM_ERROR if pbyReadBlockRequest is NULL, SIOMM_OK otherwise.
	 */
	int BuildReadBlockRequest(uint8_t * pbyReadBlockRequest,
		uint8_t   byTransactionLabel,
		uint32_t  dwDestinationOffset,
		uint16_t   wDataLength);

	/** Unpacks a read response.
	 *
	 * This is an internally used utility method to build a read request. Client code
	 * isn't likely to need it.
	 *
	 * \param pbyReadBlockResponse Response packet from the server
	 * \param pbyTransactionLabel User supplied pointer to a uint8_t to hold the transaction label from the packet
	 * \param pbyResponseCode User supplied pointer to a uint8_t to hold the response code from the packet.
	 * \param pwDataLength User supplied pointer to a uint16_t to hold the length of the data in the response packet
	 * \param pbyBlockData User supplied pointer to hold the data from the read request.
	 * \return SIOMM_ERROR if any inputs are NULL, SIOMM_ERROR_INCORRECT_TCODE_RECIEVED if the packet isn't valid, SIOMM_OK otherwise.
	 */
	int UnpackReadBlockResponse(uint8_t  * pbyReadBlockResponse,
		uint8_t  * pbyTransactionLabel,
		uint8_t  * pbyResponseCode,
		uint16_t  * pwDataLength,
		uint8_t  * pbyBlockData);

	/** Reads a quad (4 bytes) of data from the given offset on the brain.
	 *
	 * This is a wrapper for ReadBlock.
	 *
	 * \param dwDestOffset Memory map offset to read data from.
	 * \param pdwQuadlet User supplied pointer to contain the results of the read. Must not be NULL, must be at least 4 bytes in length.
	 * \return SIOMM_ERROR if pwdQuadlet is NULL, otherwise the result from the underlying ReadBlock call.
	 */
	int ReadQuad(uint32_t dwDestOffset, uint32_t * pdwQuadlet);

	/** Writes a quad (4 bytes) of data to the given offset on the brain.
	 *
	 * This is a wrapper for WriteBlock
	 *
	 * \param dwDestOffset Memory map offset to write data to.
	 * \param dwQuadlet Data to be written to the device.
	 * \return The result from the underlying WriteBlock call.
	 */
	int WriteQuad(uint32_t dwDestOffset, uint32_t dwQuadlet);

	/** Reads a float (4 bytes interpreted as a float) of data from the given offset on the brain.
	 *
	 * This is a wrapper for ReadQuad.
	 *
	 * \param dwDestOffset Memory map offset to read data from.
	 * \param pfValue User supplied pointer to contain the results of the read. Must not be NULL, must be at least 1 float in length. (e.g. 4 bytes)
	 * \return SIOMM_ERROR if pfValue is NULL, otherwise the result from the underlying ReadQuad call.
	 */
	int ReadFloat(uint32_t dwDestOffset, float * pfValue);

	/** Writes a float (4 bytes) of data to the given offset on the brain.
	 *
	 * This is a wrapper for WriteQuad
	 *
	 * \param dwDestOffset Memory map offset to write data to.
	 * \param fValue Data to be written to the device.
	 * \return The result from the underlying WriteQuad call.
	 */
	int WriteFloat(uint32_t dwDestOffset, float fValue);

	/** Reads a block of data from the I/O unit.
	 *
	 * \param dwDestOffset Memory map offset to read data from
	 * \param wDataLength Length of memory to read, in bytes
	 * \param pbyData User supplied pointer to store data read from the brain. Must not be NULL, must be at least wDataLength bytes in size.
	 * \return SIOMM_OK if data was read successfully.
	 * \return SIOMM_ERROR if pbyData is NULL.
	 * \return Other errors depending on what sort of connection you're using. (Ethernet, Serial, etc.)
	 */
	int ReadBlock(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);

	/** Writes a block of data to the I/O unit.
	 *
	 * \param dwDestOffset Memory map offset to write data to
	 * \param wDataLength Length of memory to write (e.g. pbyData), in bytes
	 * \param pbyData Data to write to the brain.
	 * \return SIOMM_OK if data was written successfully.
	 * \return SIOMM_ERROR if pbyData is NULL.
	 * \return Other errors depending on what sort of connection you're using. (Ethernet, Serial, etc.)
	 */
	int WriteBlock(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);

	/** Read a chunk of memory from the brain as an array of integers.
	 *
	 * This is a wrapper for ReadBlock.
	 *
	 * @see ReadBlock
	 * \param dwDestOffset Memory map offset to read data from
	 * \param wDataLength Length of memory to read, in integers (not bytes).
	 * \param pnData User supplied pointer to store data read from the I/O unit. Must not be NULL, must be at least wDataLength * sizeof(int) in length.
	 * \return SIOMM_OK if data was read successfully.
	 * \return SIOMM_ERROR if pnData is NULLL.
	 * \return Other errors depending on what sort of connection you're using. (e.g. Ethernet, Serial, etc.)
	 */
	int ReadBlockAsIntegers(uint32_t dwDestOffset, uint16_t wDataLength,
		int  * pnData);

	/** Write an array of integers to the I/O unit.
	 *
	 * This is a wrapper for WriteBlock
	 *
	 * @see WriteBlock
	 * \param dwDestOffset Memory map offset to write data to
	 * \param wDataLength Length of the array of integers in pnData. (Not length in bytes).
	 * \param pnData Array of integers to write to the I/O unit.
	 * \return SIOMM_OK if data was written successfully.
	 * \return SIOMM_ERROR if pnData is NULL.
	 * \return Other errors depending on what sort of connection you're using. (e.g. Ethernet, Serial, etc.)
	 */
	int WriteBlockAsIntegers(uint32_t dwDestOffset, uint16_t wDataLength,
		int  * pnData);

	/** Read a chunk of memory from the brain as an array of integers.
	 *
	 * This is a wrapper for ReadBlock.
	 *
	 * @see ReadBlock
	 * \param dwDestOffset Memory map offset to read data from
	 * \param wDataLength Length of memory to read, in floats (not bytes).
	 * \param pfData User supplied pointer to store data read from the I/O unit. Must not be NULL, must be at least wDataLength * sizeof(float) in length.
	 * \return SIOMM_OK if data was read successfully.
	 * \return SIOMM_ERROR if pnData is NULLL.
	 * \return Other errors depending on what sort of connection you're using. (e.g. Ethernet, Serial, etc.)
	 */
	int ReadBlockAsFloats(uint32_t dwDestOffset, uint16_t wDataLength,
		float * pfData);

	/** Write an array of floats to the I/O unit.
	 *
	 * This is a wrapper for WriteBlock
	 *
	 * @see WriteBlock
	 * \param dwDestOffset Memory map offset to write data to
	 * \param wDataLength Length of the array of float in pnData. (Not length in bytes).
	 * \param pfData Array of floats to write to the I/O unit.
	 * \return SIOMM_OK if data was written successfully.
	 * \return SIOMM_ERROR if pnData is NULL.
	 * \return Other errors depending on what sort of connection you're using. (e.g. Ethernet, Serial, etc.)
	 */
	int WriteBlockAsFloats(uint32_t dwDestOffset, uint16_t wDataLength,
		float * pfData);

	/** Read the Powerup Clear flag from the I/O units status area.
	 *
	 * A Powerup Clear Flag set to 0 means everything is OK. Any other value means that a
	 * Powerup Clear is needed.
	 * 
	 * Normally, set the autoPUC parameter of OpenEnet() or OpenEnet2() to true. For TCP connections
	 * you should never receive a PUC expected NACK. Only with serial and UDP (connectionless
	 * protocols) could you receive such a response.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pnPUCFlag Pointer to an integer to store the returned powerup clear flag.
	 * \return SIOMM_OK if the flag was read successfully.
	 * \return SIOMM_ERROR if pnPUCFlag is NULL.
	 * \return Other errors possibly resulting from ReadQuad.
	 */
	int GetStatusPUC(int *pnPUCFlag);

	/** Read the last error code from the I/O units status area.
	 *
	 * If this is the only application communicating with the OptoMMP device, this value
	 * might be meaninful. Other messaging clients may be messaging the device and thus
	 * their faults will cause this field to update.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pnErrorCode Pointer to an integer to store the returned error code.
	 * \return SIOMM_OK if the error code was read successfully.
	 * \return SIOMM_ERROR if pnInteger is NULL.
	 * \return Other errors possibly resulting from ReadQuad.
	 */
	int GetStatusLastError(int *pnErrorCode);

	/** Read the "BootP always" flag from the I/O units status area.
	 *
	 * If the BootP always flag is 0, the I/O unit will send a BootP (UIO, EIO, SIO, LCE) or DHCP (E1, E2)
	 * request only when it is configured to use a static IP address of 0.0.0.0.
	 *
	 * If the flag is 1, the I/O unit will send a BootP or DHCP request whenever it is turned on.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pnBootpAlways Pointer to an integer to store the returned flag.
	 * \return SIOMM_OK if the flag was read successfully.
	 * \return SIOMM_ERROR if pnInteger is NULL.
	 * \return Other errors possibly resulting from ReadQuad.
	 */
	int GetStatusBootpAlways(int *pnBootpAlways);

	/** Read the value of the Degrees flag in the I/O units status area.
	 *
	 * A value of 0 means temperature values are returned in degrees Celsius, 1 in degrees Fahrenheit.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pnDegrees Pointer to an integer to store the returned flag.
	 * \return SIOMM_OK if the flag was read successfully.
	 * \return SIOMM_ERROR if pnInteger is NULL.
	 * \return Other errors possibly resulting from ReadQuad.
	 */
	int GetStatusDegrees(int *pnDegrees);

	/** Read the communication watchdog time setting from the I/O units status area.
	 *
	 * Returns the configured watchdog time in milliseconds. A value of 0 means watchdog is disabled.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pnTimeMS Pointer to an integer to store the returned watchdog time.
	 * \return SIOMM_OK if the flag was read successfully.
	 * \return SIOMM_ERROR if pnInteger is NULL.
	 * \return Other errors possibly resulting from ReadQuad.
	 */
	int GetStatusWatchdogTime(int *pnTimeMS);

	/** Read version information from the I/O unit status area.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pVersionData Pointer to a SIOMM_StatusVersion structure to hold the returned version information. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pVersionData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetStatusVersionEx(SIOMM_StatusVersion *pVersionData);

	/** Read hardware details from the I/O unit status area.
	 *
	 * NOTE: This method has been superseded by GetStatusHardwareEx2. You should probably use that one instead.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * @see GetStatusHardwareEx2
	 * \param pHardwareData Pointer to a SIOMM_StatusHardware structure to hold the returned hardware details. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pHardwareData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetStatusHardwareEx(SIOMM_StatusHardware *pHardwareData);

	/** Read hardware details from the I/O unit status area.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pHardwareData Pointer to a SIOMM_StatusHardware2 structure to hold the returned hardware details. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pHardwareData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetStatusHardwareEx2(SIOMM_StatusHardware2 *pHardwareData);

	/** Read current IPv4 networking details from the I/O unit status area.
	 *
	 * NOTE: This method has been superseded by GetStatusNetworkEx2. You should probably use that one instead.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * @see GetStatusNetworkEx2
	 * \param pNetworkData Pointer to a SIOMM_StatusNetwork structure to hold the returned network details. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pNetworkData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetStatusNetworkEx(SIOMM_StatusNetwork *pNetworkData);

	/** Read current networking details from the I/O units status area.
	 *
	 * See Appendix A - Status Area Read in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pNetworkData Pointer to a SIOMM_StatusNetwork2 structure to hold the returned network details. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pNetworkData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetStatusNetworkEx2(SIOMM_StatusNetwork2 *pNetworkData);

	/** Read the current contents of the Status Read/Write area of the I/O unit.
	 *
	 * See Appendix A - Status Area Write in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param pStatusWriteData Pointer to a SIOMM_StatusWrite structure to hold the returned network details. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pStatusWriteData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetStatusWriteEx(SIOMM_StatusWrite * pStatusWriteData);

	/** Write new data to the Status Read/Write area of the I/O unit.
	 *
	 * See Appendix A - Status Area Write in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param StatusWriteData A SIOMM_StatusWrite structure holding the data to be written to the Status Write area.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetStatusWriteEx(SIOMM_StatusWrite    StatusWriteData);

	/** Writes the given opcode to the SIOMM_STATUS_WRITE_OPERATION location in the I/O units memory map.
	 *
	 * Some example opcodes are:
	 *  - 0x00000001 - Send Powerup Clear
	 *  - 0x00000002 - Reset to defaults
	 *  - 0x00000003 - Store all configuration data to flash.
	 *
	 * See Appendix A - Status Area Write in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param nOpCode Opcode to write to the SIOMM_STATUS_WRITE_OPERATION location.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetStatusOperation(int nOpCode);

	/** Set the I/O unit to send BootP or DHCP request when turned on.
	 *
	 * Possible values for nFlag:
	 *  - 0 Send BootP or DHCP request only if device's IP address is 0.0.0.0.
	 *  - 1 Always send BootP or DHCP request when device is turned on.
	 *
	 * See Appendix A - Status Area Write in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param nFlag BootP request flag.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetStatusBootpRequest(int nFlag);

	/** Configure the I/O unit to report temperatures in degrees Celsius or degrees Fahrenheit
	 *
	 * Possible values for nDegFlag:
	 *  - 0 Report temperatures in degrees Celsius. (this is the default)
	 *  - 1 Report temperatures in degrees Fahrenheit.
	 *
	 * See Appendix A - Status Area Write in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param nDegFlag Option to set for reporting temperatures.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetStatusDegrees(int nDegFlag);

	/** Set watchdog time in milliseconds.
	 *
	 * A value of 0 disables the watchdog. Any other positive value sets the watchdog time
	 * in milliseconds. Negative values are an error.
	 *
	 * See Appendix A - Status Area Write in the OptoMMP Protocol Guide for additional details.
	 *
	 * \param nTimeMS Watchdog time in milliseconds. (Must be >= 0)
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetStatusWatchdogTime(int nTimeMS);

	/** Read the current date and time time from the I/O unit.
	 *
	 * Times are returned in a string in the format YYYY-MM-DD HH:MM:SS:00.
	 *
	 * \param pchDateTime User supplied pointer to a string. Must not be NULL, must be at least 23 characters in length (with null byte).
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pchDateTime is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDateTime(char * pchDateTime);

	/** Set the I/O units date and time.
	 *
	 * The date and time is provided as a string with format: YYYY-MM-DD HH:MM:SS:00
	 *
	 * \param pchDateTime String providing the date and time to set on the I/O unit. Must not be NULL.
	 * \return SIOMM_OK if the date and time were set successfully.
	 * \return SIOMM_ERROR if pchDateTime is NULL.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDateTime(char * pchDateTime);

	/** Read the configuration of the SNAP serial communication modules on the I/O unit.
	 *
	 * See Appendix A - Serial Module Configuration - Read/Write of the Opto MMP Protocol Guide for details.
	 *
	 * \param nSerialPort Which serial port to configure. See Appendix A - Serial Module Configuration - Read/Write for how port numbers are determined.
	 * \param pConfigData Pointer to a SIOMM_SerialModuleConfigArea structure instance to hold the data read.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pConfigData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetSerialModuleConfigurationEx(int nSerialPort,
		SIOMM_SerialModuleConfigArea * pConfigData);

	/** Write configuration data for a SNAP serial communication module.
	 *
	 * See Appendix A - Serial Module Configuration - Read/Write of the Opto MMP Protocol Guide for details.
	 *
	 * \param nSerialPort Which serial port to configure. See Appendix A - Serial Module Configuration - Read/Write for how port numbers are determined.
	 * \param ConfigData SIOMM_SerialModuleConfigArea structure instance with the data to be written.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetSerialModuleConfigurationEx(int nSerialPort,
		SIOMM_SerialModuleConfigArea ConfigData);

	/** Tell the I/O unit what kind of point is at a given location.
	 *
	 * High density digital modules and analog modules are all recognized automatically
	 * by the I/O unit when it boots up. 4-channel digital outputs are not, and need to be configured before use.
	 *
	 * This method is applicable to the old 64-point memory-map implementation (B3000-ENET, UIO, and SNAP-PAC when only 4-channel or less modules are used).
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param nPointType Point type value. A digital output is type 0x180, an input is 0x100.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int ConfigurePoint(int nPoint, int nPointType);

	/** Read the point type of a given point back from the I/O unit.
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * This method is applicable to the 64-point memory-map implementation (B3000-ENET, UIO, and SNAP-PAC when only 4-channel or less modules are used).
	 *
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnModuleType Pointer to an integer to return the point type for the point. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pnModuleType is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetModuleType(int nPoint, int * pnModuleType);

	/** Read the full configuration data for a given point.
	 *
	 * Valid for the 64-point memory model.
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pData Pointer to a SIOMM_PointConfigArea3 instance. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetPtConfigurationEx3(int nPoint, SIOMM_PointConfigArea3 * pData);

	/** Read the full configuration data for a given point.
	 *
	 * NOTE: This method is deprecated, use GetPtConfigurationEx3 instead.
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * @see GetPtConfigurationEx3
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pData Pointer to a SIOMM_PointConfigArea2 instance. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetPtConfigurationEx2(int nPoint, SIOMM_PointConfigArea2 * pData);

	/** Read the full configuration data for a given point.
	 *
	 * NOTE: This method is deprecated, use GetPtConfigurationEx3 instead.
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * @see GetPtConfigurationEx3
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pData Pointer to a SIOMM_PointConfigArea instance. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetPtConfigurationEx(int nPoint, SIOMM_PointConfigArea * pData);

	/** Set the full configuration for a given point.
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param Data SIOMM_PointConfigArea3 instance filled out with the point's full configuration details.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int SetPtConfigurationEx3(int nPoint, SIOMM_PointConfigArea3 Data);

	/** Set the full configuration for a given point.
	 *
	 * NOTE: This method is deprecated, use SetPtConfigurationEx3 instead.
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * @see SetPtConfigurationEx3
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param Data SIOMM_PointConfigArea2 instance filled out with the point's full configuration details.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int SetPtConfigurationEx2(int nPoint, SIOMM_PointConfigArea2 Data);

	/** Set the full configuration for a given point.
	 *
	 * NOTE: This method is deprecated, use SetPtConfigurationEx3 instead.
	 *
	 * See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * @see SetPtConfigurationEx3
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param Data SIOMM_PointConfigArea instance filled out with the point's full configuration details.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int SetPtConfigurationEx(int nPoint, SIOMM_PointConfigArea Data);

	/** Configure a digital point.
	 *
	 * This method does two things:
	 *  - Configures the point's type, the same thing that ConfigurePoint does.
	 *  - Sets point features. See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for feature values.
	 *
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param nPointType Point type value. A digital output is type 0x180, an input is 0x100.
	 * \param nFeature Feature to configure.
	 * \return SIOMM_OK if the point was configured successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigPtConfiguration(int nPoint, int nPointType, int nFeature);

	/** Configure an analog point.
	 *
	 * This method does two things:
	 *  - Configures the point's type, the same thing that ConfigurePoint does.
	 *  - Sets point features. See Appendix A - (Old) Analog and Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for feature values.
	 *
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param nPointType Point type value. For analog point types, see the tables in the Configuring I/O Points section of Chapter 2 of the OptoMMP Protocol Guide.
	 * \param fOffset Point offsets
	 * \param fGain Point gains
	 * \param fHiScale Point high scaling factors
	 * \param fLoScale Point low scaling factors
	 * \return SIOMM_OK if the point was configured successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnaPtConfiguration(int nPoint, int nPointType, float fOffset,
		float fGain, float fHiScale, float fLoScale);

	/** Sets a watchdog value for a point.
	 *
	 * A watchdog monitors communication on the OptoMMP port. If nothing accesses the port for the length of time set
	 * in the watchdog, the I/O unit automatically sets designated digital and analog I/O points to the values you have
	 * determined.
	 *
	 * A watchdog helps make sure that a communication failure doesn’t result in disaster. If communication fails between
	 * the host and the I/O unit controlling a process, the watchdog makes sure the process is automatically brought to a
	 * safe state. For example, a valve could automatically close to avoid completely emptying a tank.
	 *
	 * \param nPoint Point number on the I/O unit to configure. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param fValue Value to set the point to if communications fail
	 * \param nEnabled Turn the watchdog on or off. 0 means off, non-zero means on.
	 * \return SIOMM_OK if the point was configured successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetPtWatchdog(int nPoint, float fValue, int nEnabled);

	/* Standard density digital point methods. See the Digital Point sections
	 * in Appendix A of the OptoMMP Protocol Guide for details.
	 *
	 * NOTE: These methods work only for standard density digital points.
	 * High-Density digital points must use the HDDigital methods defined
	 * below this section.
	 */
	 // Digital point read

	 /** Get the current state of a digital point.
	  *
	  * Note: This method works only for standard density digital points. High density digital points
	  * must use GetHDDigitalPointState.
	  *
	  * @see GetHDDigitalPointState
	  * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	  * \param pnState Pointer to an integer to hold the current state of the point.
	  * \return SIOMM_OK if the point state was read successfully.
	  * \return SIOMM_ERROR if pnState is NULL.
	  * \return Other errors possibly resulting from ReadBlock.
	  */
	int GetDigPtState(int nPoint, int *pnState);

	/** Get the on-latch state of a digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use GetHDDigitalPointOnLatch.
	 *
	 * @see GetHDDigitalPointOnLatch
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnState Pointer to an integer to hold the current on-latch state of the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnState is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigPtOnLatch(int nPoint, int *pnState);

	/** Get the off-latch state of a digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use GetHDDigitalPointOffLatch.
	 *
	 * @see GetHDDigitalPointOffLatch
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnState Pointer to an integer to hold the current off-latch state of the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnState is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigPtOffLatch(int nPoint, int *pnState);

	/** Get the active counter state of a digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use GetHDDigitalPointCounterState.
	 *
	 * @see GetHDDigitalPointCounterState
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnState Pointer to an integer to hold the counter state (e.g. on/off) for the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnState is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigPtCounterState(int nPoint, int *pnState);

	/** Get the counters of a digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use GetHDDigitalPointCounts.
	 *
	 * @see GetHDDigitalPointCounters
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnValue Pointer to an integer to hold the counter value for the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigPtCounts(int nPoint, int *pnValue);

	/** Get the full state of a digital point.
	 *
	 * Note: This method works only for standard density digital points.
	 *
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pData Pointer to a SIOMM_DigPointReadArea instance to hold the state of the digital point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigPtReadAreaEx(int nPoint, SIOMM_DigPointReadArea * pData);

	/** Set the state of a digital point. (e.g. on/off)
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use SetHDDigitalPointState.
	 *
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param nState Value to set for the digital point's state.
	 * \return SIOMM_OK if the point state was set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigPtState(int nPoint, int nState);

	/** Turn counters on or off for a digital point.
	 *
	 * Note: This method works only for standard density digital points.
	 *
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param nState 0 to turn counters off, non-zero to turn them on.
	 * \return SIOMM_OK if the point state was set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigPtCounterState(int nPoint, int nState);

	/** Read and clear the counts value for a digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use ReadClearHDDigitalPointCounts.
	 *
	 * @see ReadClearHDDigitalPointCounts
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnState Pointer to an integer to hold the counter state for the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearDigPtCounts(int nPoint, int * pnState);

	/** Read and clear the on-latch state for a digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use ReadClearHDDigitalPointOnLatch.
	 *
	 * @see GetHDDigitalPointOnLatch
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnState Pointer to an integer to hold the current on-latch state of the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnState is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearDigPtOnLatch(int nPoint, int * pnState);

	/** Read and clear the off-latch state of a digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use ReadClearHDDigitalPointOffLatch.
	 *
	 * @see ReadClearHDDigitalPointOffLatch
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pnState Pointer to an integer to hold the current off-latch state of the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnState is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearDigPtOffLatch(int nPoint, int * pnState);

	/** Read the status of the entire bank of standard density digital points.
	 *
	 * Point states are returned as a pair of 32-bit bitmask values: one bit for each of the 64 possible
	 * standard density digital points on an I/O unit.
	 *
	 * \param pnPts63to32 Pointer to an integer to return a bitmask representing points 32 through 63
	 * \param pnPts31to0 Pointer to an integer to return a bitmask representing points 0 through 31
	 * \return SIOMM_OK if the bank was read successfully.
	 * \return SIOMM_ERROR if either parameter was NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigBankPointStates(int *pnPts63to32, int *pnPts31to0);

	/** Read the on-latch states of tge entire bank of standard density digital points.
	 *
	 * Point on-latch states are returned as a pair of 32-bit bitmask values: one bit for each of the 64 possible
	 * standard density digital points on an I/O unit.
	 *
	 * \param pnPts63to32 Pointer to an integer to return a bitmask representing points 32 through 63
	 * \param pnPts31to0 Pointer to an integer to return a bitmask representing points 0 through 31
	 * \return SIOMM_OK if the bank was read successfully.
	 * \return SIOMM_ERROR if either parameter was NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigBankOnLatchStates(int *pnPts63to32, int *pnPts31to0);

	/** Read the off-latch states of tge entire bank of standard density digital points.
	 *
	 * Point off-latch states are returned as a pair of 32-bit bitmask values: one bit for each of the 64 possible
	 * standard density digital points on an I/O unit.
	 *
	 * \param pnPts63to32 Pointer to an integer to return a bitmask representing points 32 through 63
	 * \param pnPts31to0 Pointer to an integer to return a bitmask representing points 0 through 31
	 * \return SIOMM_OK if the bank was read successfully.
	 * \return SIOMM_ERROR if either parameter was NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigBankOffLatchStates(int *pnPts63to32, int *pnPts31to0);

	/** Read the active counter states of tge entire bank of standard density digital points.
	 *
	 * Point active counter states are returned as a pair of 32-bit bitmask values: one bit for each of the 64 possible
	 * standard density digital points on an I/O unit.
	 *
	 * \param pnPts63to32 Pointer to an integer to return a bitmask representing points 32 through 63
	 * \param pnPts31to0 Pointer to an integer to return a bitmask representing points 0 through 31
	 * \return SIOMM_OK if the bank was read successfully.
	 * \return SIOMM_ERROR if either parameter was NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigBankActCounterStates(int *pnPts63to32, int *pnPts31to0);

	/** Read the entire standard density digital point bank in one shot.
	 *
	 * \param pData Pointer to a SIOMM_DigBankReadArea instance to hold the results of the read.
	 * \return SIOMM_OK if the bank was read successfully.
	 * \return SIOMM_ERROR if pData is NULL
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetDigBankReadAreaEx(SIOMM_DigBankReadArea * pData);

	/** Set the states of (potentially) all standard density digital points in one call.
	 *
	 * The two mask parameters determine which points to actually set: only points with 1 values in the
	 * masks will have their states changed. The Pts values determine which states to set.
	 *
	 * \param nPts63to32 Integer representing the states to set for points 32 through 63
	 * \param nPts31to0 Integer representing the states to set for points 0 through 31
	 * \param nMask63to32 Bitmask to choose which points to set states for. (Points 32 through 63)
	 * \param nMask31to0 Bitmask to choose which points to set states for. (Points 0 through 31)
	 * \return SIOMM_OK if the point states were set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigBankPointStates(int nPts63to32, int nPts31to0,
		int nMask63to32, int nMask31to0);

	/** Set the state of a subset of all standard density digital points on at once.
	 *
	 * For any digital point with a 1 in the provided mask parameters, set that point's state to on.
	 *
	 * \param nPts63to32 Bitmask to choose which points to set states for. (Points 32 through 63)
	 * \param nPts31to0 Bitmask to choose which points to set states for. (Points 0 through 31)
	 * \return SIOMM_OK if the point states were set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigBankOnMask(int nPts63to32, int nPts31to0);

	/** Set the state of a subset of all standard density digital points off at once.
	 *
	 * For any digital point with a 1 in the provided mask parameters, set that point's state to off.
	 *
	 * \param nPts63to32 Bitmask to choose which points to set states for. (Points 32 through 63)
	 * \param nPts31to0 Bitmask to choose which points to set states for. (Points 0 through 31)
	 * \return SIOMM_OK if the point states were set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigBankOffMask(int nPts63to32, int nPts31to0);

	/** Turn on the counters for a subset of all standard density digital points on at once.
	 *
	 * For any digital point with a 1 in the provided mask parameters, turn that point's counter on.
	 *
	 * \param nPts63to32 Bitmask to choose which points to set states for. (Points 32 through 63)
	 * \param nPts31to0 Bitmask to choose which points to set states for. (Points 0 through 31)
	 * \return SIOMM_OK if the point states were set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigBankActCounterMask(int nPts63to32, int nPts31to0);

	/** Turn on the counters for a subset of all standard density digital points off at once.
	 *
	 * For any digital point with a 1 in the provided mask parameters, turn that point's counter off.
	 *
	 * \param nPts63to32 Bitmask to choose which points to set states for. (Points 32 through 63)
	 * \param nPts31to0 Bitmask to choose which points to set states for. (Points 0 through 31)
	 * \return SIOMM_OK if the point states were set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigBankDeactCounterMask(int nPts63to32, int nPts31to0);

	/* High density digital point reads. See Appendix A - SNAP High-Density
	 * Digital sections in the OptoMMP Protocol Guide for details.
	 *
	 * NOTE: These methods work only for High Density digital modules; standard
	 * digital modules must use the standard density methods defined above.
	 */

	 /** Get the current state of a high-density digital point.
	  *
	  * Note: This method works only for high density digital points. High density digital points
	  * must use GetDigPtState.
	  *
	  * @see GetDigPtState
	  * \param nModule Which module to read a point's state from.
	  * \param nPoint Point number on the module to read.
	  * \param pnState Pointer to an integer to hold the current state of the point.
	  * \return SIOMM_OK if the point state was read successfully.
	  * \return SIOMM_ERROR if pnState is NULL.
	  * \return Other errors possibly resulting from ReadBlock.
	  */
	int GetHDDigitalPointState(int nModule, int nPoint, int * pnState);

	/** Get the on-latch state of a high-density digital point.
	 *
	 * Note: This method works only for high density digital points. High density digital points
	 * must use GetDigPtOnLatch.
	 *
	 * @see GetDigPtOnLatch
	 * \param nModule Which module to read a point's state from.
	 * \param nPoint Point number on the module to read.
	 * \param pnState Pointer to an integer to hold the current on-latch state of the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnState is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetHDDigitalPointOnLatch(int nModule, int nPoint, int * pnState);

	/** Get the off-latch state of a high-density digital point.
	 *
	 * Note: This method works only for high density digital points. High density digital points
	 * must use GetDigPtOffLatch.
	 *
	 * @see GetDigPtOffLatch
	 * \param nModule Which module to read a point's state from.
	 * \param nPoint Point number on the module to read.
	 * \param pnState Pointer to an integer to hold the current off-latch state of the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnState is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetHDDigitalPointOffLatch(int nModule, int nPoint, int * pnState);

	/** Get the current counter of a high-density digital point.
	 *
	 * Note: This method works only for standard density digital points. High density digital points
	 * must use GetDigPtCounts.
	 *
	 * @see GetDigPtCounts
	 * \param nModule Which module to read a point's state from.
	 * \param nPoint Point number on the module to read.
	 * \param pnCounts Pointer to an integer to hold the counter value for the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetHDDigitalPointCounts(int nModule, int nPoint, int * pnCounts);

	/** Set the state of a high-density digital point. (e.g. on/off)
	 *
	 * Note: This method works only for high density digital points. High density digital points
	 * must use SetDigPtState.
	 *
	 * @see SetDigPtState
	 * \param nModule Which module the point to be written resides on.
	 * \param nPoint Point number on the module to written to.
	 * \param nState Value to set for the digital point's state.
	 * \return SIOMM_OK if the point state was set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetHDDigitalPointState(int nModule, int nPoint, int nState);

	/** Read and clear the counts value for a high density digital point.
	 *
	 * Note: This method works only for high density digital points. High density digital points
	 * must use ReadClearDigPtCounts.
	 *
	 * @see ReadClearDigPtCounts
	 * \param nModule Which module to read and clear a point's counter from.
	 * \param nPoint Point number on the module to read and clear.
	 * \param pnCounts Pointer to an integer to hold the counter value for the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearHDDigitalPointCounts(int nModule, int nPoint, int * pnCounts);

	/** Read and clear the on-latch value for a high density digital point.
	 *
	 * Note: This method works only for high density digital points. High density digital points
	 * must use ReadClearDigPtOnLatch.
	 *
	 * @see ReadClearDigPtOnLatch
	 * \param nModule Which module to read and clear a point's on-latch state from.
	 * \param nPoint Point number on the module to read and clear.
	 * \param pnState Pointer to an integer to hold the on-latch state for the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearHDDigitalPointOnLatch(int nModule, int nPoint, int * pnState);

	/** Read and clear the off-latch value for a high density digital point.
	 *
	 * Note: This method works only for high density digital points. High density digital points
	 * must use ReadClearDigPtOffLatch.
	 *
	 * @see ReadClearDigPtOffLatch
	 * \param nModule Which module to read and clear a point's off-latch state from.
	 * \param nPoint Point number on the module to read and clear.
	 * \param pnState Pointer to an integer to hold the off-latch state for the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pnValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearHDDigitalPointOffLatch(int nModule, int nPoint, int * pnState);

	/* Expanded Analog point methods. Supported on I/O units using firmware 8.0 or
	 * greater, these methods allow access to a greater number of modules and points.
	 */
	 // Analog point read

	 /** Read the current value of an analog point.
	  *
	  * This method uses the Expanded Analog point memory map, and is only usable on I/O
	  * units running firmware 8.0 or greater.
	  *
	  * \param nModule Module number of the point to be read.
	  * \param nPoint Point number on the module to read.
	  * \param pfValue Pointer to a float to hold the result of the read.
	  * \return SIOMM_OK if the point state was read successfully.
	  * \return SIOMM_ERROR if pfValue is NULL.
	  * \return Other errors possibly resulting from ReadBlock.
	  */
	int GetAnalogPointValueEx(int nModule, int nPoint, float *pfValue);

	/** Read the current counts of an analog point.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnalogPointCountsEx(int nModule, int nPoint, float *pfValue);

	/** Read the lowest recorded value of an analog point.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnalogPointMinValueEx(int nModule, int nPoint, float *pfValue);

	/** Read the highest recorded value of an analog point.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnalogPointMaxValueEx(int nModule, int nPoint, float *pfValue);

	/** Read an analog point's TPO period.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnalogPointTpoPeriodEx(int nModule, int nPoint, float *pfValue);

	/** Read the entirety of an analog point's read area in one shot.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param pData Pointer to an instance of a SIOMM_AnaPointReadArea structure to hold the results from the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnalogPointReadAreaEx(int nModule, int nPoint, SIOMM_AnaPointReadArea *pData);

	/** Set the value of an analog point.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param fValue Value to set on the point.
	 * \return SIOMM_OK if the point state was set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnalogPointValueEx(int nModule, int nPoint, float fValue);

	/** Set the counts value of an analog point.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param fValue Value to set on the point.
	 * \return SIOMM_OK if the point state was set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnalogPointCountsEx(int nModule, int nPoint, float fValue);

	/** Set the TPO period of an analog point.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * The TPO period is set in units of time in seconds. Valid range: 0.25 to 64.0 seconds, in 0.25 steps.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param fValue Value to set on the point.
	 * \return SIOMM_OK if the point state was set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnalogPointTpoPeriodEx(int nModule, int nPoint, float fValue);

	/** Read and clear the minimum value recorded on an analog point.
	 *
	 * NOTE: It's safe to pass NULL for pfValue if you simply want to clear the minimum value.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearAnalogPointMinValueEx(int nModule, int nPoint, float *pfValue);

	/** Read and clear the maximum value recorded on an analog point.
	 *
	 * NOTE: It's safe to pass NULL for pfValue if you simply want to clear the maximum value.
	 *
	 * This method uses the Expanded Analog point memory map, and is only usable on I/O
	 * units running firmware 8.0 or greater.
	 *
	 * \param nModule Module number of the point to be read.
	 * \param nPoint Point number on the module to read.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearAnalogPointMaxValueEx(int nModule, int nPoint, float *pfValue);

	/* Legacy Analog point methods. If you're using firmware 8.0 or newer on your
	 * I/O units, it is recommended that you use the newer (Expanded) methods
	 * defined above. If you're using a firmware older than 8.0, use these methods.
	 */

	 /** Read the current value of an analog point.
	  *
	  * This method is meant to be used for I/O units running a firmware version older than 8.0.
	  * If your I/O unit is running 8.0 or above, it's recommended that you use GetAnalogPointValueEx
	  * instead.
	  *
	  * @see GetAnalogPointValueEx
	  * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	  * \param pfValue Pointer to a float to hold the result of the read.
	  * \return SIOMM_OK if the point state was read successfully.
	  * \return SIOMM_ERROR if pfValue is NULL.
	  * \return Other errors possibly resulting from ReadBlock.
	  */
	int GetAnaPtValue(int nPoint, float *pfValue);

	/** Read the current counts value of an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use GetAnalogPointCountsEx
	 * instead.
	 *
	 * @see GetAnalogPointCountsEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaPtCounts(int nPoint, float *pfValue);

	/** Read the lowest recorded value of an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use GetAnalogPointMinValueEx
	 * instead.
	 *
	 * @see GetAnalogPointMinValueEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaPtMinValue(int nPoint, float *pfValue);

	/** Read the highest recorded value of an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use GetAnalogPointMaxValueEx
	 * instead.
	 *
	 * @see GetAnalogPointMaxValueEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaPtMaxValue(int nPoint, float *pfValue);

	/** Read an analog point's TPO period.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use GetAnalogPointTpoPeriodEx
	 * instead.
	 *
	 * @see GetAnalogPointTpoPeriodEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaPtTpoPeriod(int nPoint, float *pfValue);

	/** Read the entirety of an analog point's read area in one shot.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use GetAnalogPointReadAreaEx
	 * instead.
	 *
	 * @see GetAnalogPointReadAreaEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pData Pointer to a SIOMM_AnaPointReadArea instance to hold the results of the read operation.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaPtReadAreaEx(int nPoint, SIOMM_AnaPointReadArea * pData);

	/** Sets the value of an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use SetAnalogPointValueEx
	 * instead.
	 *
	 * @see SetAnalogPointValueEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param fValue Value to set on the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnaPtValue(int nPoint, float fValue);

	/** Sets the counts value of an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use SetAnalogPointCountsEx
	 * instead.
	 *
	 * @see SetAnalogPointCountsEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param fValue Value to set on the point.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnaPtCounts(int nPoint, float fValue);

	/** Set the TPO period of an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use SetAnalogPointTpoPeriodEx
	 * instead.
	 *
	 * The TPO period is set in units of time in seconds. Valid range: 0.25 to 64.0 seconds, in 0.25 steps.
	 *
	 * @see SetAnalogPointTpoPeriodEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param fValue Value to set on the point.
	 * \return SIOMM_OK if the point state was set successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnaPtTpoPeriod(int nPoint, float fValue);

	/** Read and clear the minimum value recorded on an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use ReadClearAnalogPointMinValueEx
	 * instead.
	 *
	 * @see ReadClearAnalogPointMinValueEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL;
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearAnaPtMinValue(int nPoint, float *pfValue);

	/** Read and clear the maximum value recorded on an analog point.
	 *
	 * This method is meant to be used for I/O units running a firmware version older than 8.0.
	 * If your I/O unit is running 8.0 or above, it's recommended that you use ReadClearAnalogPointMaxValueEx
	 * instead.
	 *
	 * @see ReadClearAnalogPointMaxValueEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read.
	 * \return SIOMM_OK if the point state was read successfully.
	 * \return SIOMM_ERROR if pfValue is NULL;
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int ReadClearAnaPtMaxValue(int nPoint, float *pfValue);

	/** Read the value of all 64 possible analog points at once.
	 *
	 * \param pBankData Pointer to a SIOMM_AnaBank instance to hold the results of the call.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pBankData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaBankValuesEx(SIOMM_AnaBank * pBankData);

	/** Read the counts value of all 64 possible analog points at once.
	 *
	 * \param pBankData Pointer to a SIOMM_AnaBank instance to hold the results of the call.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pBankData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaBankCountsEx(SIOMM_AnaBank * pBankData);

	/** Read the minimum recorded value of all 64 possible analog points at once.
	 *
	 * \param pBankData Pointer to a SIOMM_AnaBank instance to hold the results of the call.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pBankData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaBankMinValuesEx(SIOMM_AnaBank * pBankData);

	/** Read the maximum recorded value of all 64 possible analog points at once.
	 *
	 * \param pBankData Pointer to a SIOMM_AnaBank instance to hold the results of the call.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pBankData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaBankMaxValuesEx(SIOMM_AnaBank * pBankData);

	/** Read the entire analog bank at once. Compatible with 4-channel (or less) analog modules.
	 *
	 * \param pBankData Pointer to a SIOMM_AnaBanks instance to hold the results of the call.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pBankData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetAnaBanksEx(SIOMM_AnaBanks * pBankData);

	/** Write all analog point values at once. Compatible with 4-channel (or less) analog modules.
	 *
	 * \param BankData SIOMM_AnaBank instance holding values to be written to the analog points.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pBankData is NULL.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnaBankValuesEx(SIOMM_AnaBank BankData);

	/** Write all analog point counter values at once. Compatible with 4-channel (or less) analog modules.
	 *  Does not work with EPIC analog points.
	 *
	 * \param BankData SIOMM_AnaBank instance holding values to be written to the analog point counters.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pBankData is NULL.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAnaBankCountsEx(SIOMM_AnaBank BankData);

	/** Calculate and set an analog point's offset.
	 *
	 * Memory-mapped I/O units with analog capability can calculate offset and gain for analog input points.
	 * Calculate offset first, and then calculate gain. See the PAC Manager User’s Guide for instructions.
	 *
	 * Use this method for I/O units with firmware versions 7.1 or lower. For units with firmware version 8.0
	 * or newer, use CalcSetAnalogPointOffsetEx.
	 *
	 * @see CalcSetAnalogPointOffsetEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read operation.
	 * \return SIOMM_OK if everything worked correctly.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock
	 */
	int CalcSetAnaPtOffset(int nPoint, float *pfValue);

	/** Calculate and set an analog point's gain.
	 *
	 * Memory-mapped I/O units with analog capability can calculate offset and gain for analog input points.
	 * Calculate offset first, and then calculate gain. See the PAC Manager User’s Guide for instructions.
	 *
	 * Use this method for I/O units with firmware versions 7.1 or lower. For units with firmware version 8.0
	 * or newer, use CalcSetAnalogPointGainEx.
	 *
	 * @see CalcSetAnalogPointGainEx
	 * \param nPoint Point number on the I/O unit to read. Typically, this will be the module slot on the rack multiplied by 4, plus the point number on that module.
	 * \param pfValue Pointer to a float to hold the result of the read operation.
	 * \return SIOMM_OK if everything worked correctly.
	 * \return SIOMM_ERROR if pfValue is NULL.
	 * \return Other errors possibly resulting from ReadBlock
	 */
	int CalcSetAnaPtGain(int nPoint, float *pfValue);

	/** Configure a PID (proportional/integral/derivative) loop to run on the I/O unit.
	 *
	 * @see SIOMM_PidLoopConfigArea for more details on how to configure PID loops. See KB82058 at www.opto22.com. Firmware 9.4c and later have updated algorithms.
	 * \param nPid Which PID loop to configure. The number of available PID loops depends on the I/O unit.
	 * \param Data A SIOMM_PidLoopConfigArea instance containing the PID loop configuration data you wish to set.
	 * \return SIOMM_OK if everything worked correctly.
	 * \return Errors possibly resulting from WriteBlock
	 */
	int SetPidLoopConfigurationEx(int nPid, SIOMM_PidLoopConfigArea Data);

	// TPO (Pulsed Output)
	int SetTpoConfigurationEx(int nTpo, SIOMM_TpoConfigArea Data);
	int GetTpoConfigurationEx(int nTpo, SIOMM_TpoConfigArea * pTpoData);

	/** Configure an event message.
	 *
	 * Some OptoMMP devices may be configured to send a message via email, data streaming, SNMP or a serial
	 * module when a specific event occurs.
	 *
	 * See Chapter 2 - Setting Up Event Messages in the OptoMMP Protocol Guide for details.
	 *
	 * \param nMsg Integer between 0 and 127 (inclusive) indicating which message slot to configure
	 * \param Data An SIOMM_EventMsgConfigArea instance with the data needed to configure the event message
	 * \return SIOMM_OK if the message configuration write completed successfully
	 * \return Other values possibly resulting from a call to WriteBlock.
	 */
	int SetEventMsgConfigurationEx(int nMsg, SIOMM_EventMsgConfigArea Data);

	/** Configure digital events and reactions.
	 *
	 * Note: This method works with I/O units running firmware versions 8.0 or lower.
	 *
	 * In a digital event, the I/O unit monitors one or more inputs, outputs, and Scratch Pad bits for a match
	 * to a specific pattern (the event). When the pattern is matched, the I/O unit reacts in a predetermined
	 * way. The reaction can turn digital points on or off and can also set bits in the Scratch Pad. You can
	 * configure up to 128 digital events and reactions.
	 *
	 * See Chapter 2 - Using Digital Events and Reactions in the OptoMMP Protocol Guide for details.
	 *
	 * \param nEvent Integer from 0 to 127 (inclusive) identifying which event reaction to configure.
	 * \param Data An SIOMM_DigitalEventConfigArea instance to configure the event reaction.
	 * \return SIOMM_OK if the event configuration was written successfully
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetDigitalEventConfigurationEx(int nEvent, SIOMM_DigitalEventConfigArea Data);

	/** Configure reactions in response to alarms.
	 *
	 * Similar to the reactions configured in SetEventMsgConfigurationEx, reactions can be
	 * triggered in response to alarms as well.
	 *
	 * See Chapter 2 - Using Alarms and Reactions in the OptoMMP Protocol Guide for details.
	 *
	 * @see SetEventMsgConfigurationEx
	 * \param nEvent Integer from 0 to 63 (inclusive) identifying which alarm reaction to configure
	 * \param Data An SIOMM_AlarmEventConfigData instance to configure the alarm reaction.
	 * \return SIOMM_OK if the alarm configuration was written successfully
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetAlarmEventConfigurationEx(int nEvent, SIOMM_AlarmEventConfigArea Data);

	/** Configure reactions in response to serial events.
	 *
	 * See Chapter 2 - Using Serial Events and Reactions of the OptoMMP Protocol Guide for details.
	 *
	 * \param nEvent Index of the serial event to configure
	 * \param Data An SIOMM_SerialEventConfigArea instance to use to configure the serial reaction
	 * \return SIOMM_OK if the serial event configuration was written successfully
	 * \return Other errors possibly resulting from WriteBlock
	 */
	int SetSerialEventConfigurationEx(int nEvent, SIOMM_SerialEventConfigArea Data);

	/** Configure a timer event.
	 *
	 * Note: Only supported on firmware versions 8.0 and older.
	 *
	 * See Appendix A - Digital Events - Expanded (Formerly Timers) of the OptoMMP Protocol Guide for more details.
	 *
	 * \param nEvent Index of the timer event to configure
	 * \param Data An SIOMM_TimerEventConfigArea instance used to configure the timer event
	 * \return SIOMM_ if the timer event configuration was written successfully
	 * \return Errors possibly resulting from WriteBlock
	 */
	int SetTimerEventConfigurationEx(int nEvent, SIOMM_TimerEventConfigArea Data);


	// Stream functions
	/** Read the device's current streaming configuration. Only some OptoMMP devices support this function.
	 *
	 * Streaming allows your application to get continuous information from some OptoMMP devices
	 * without the need to continually poll it.
	 *
	 * See Chapter 2 - Streaming Data in the OptoMMP Protocol Guide for details.
	 *
	 * \param pnOnFlag Whether or not streaming is enabled. 0 is off, non-zero is on.
	 * \param pnIntervalMS Streaming interval, in milliseconds
	 * \param pnPort IP port number to stream to
	 * \param pnIoMirroringEnabled Whether I/O mirroring is enabled. 0 is off, non-zero is on.
	 * \param pnStartAddress Beginning address of the data being streamed
	 * \param pnDataSize Size of data being streamed. Maximum size is 1480 bytes.
	 * \return SIOMM_OK if the streaming configuration was read successfully.
	 * \return Errors possibly resulting from a call to ReadBlock.
	 */
	int GetStreamConfiguration(int * pnOnFlag, int * pnIntervalMS,
		int * pnPort,
		int * pnIoMirroringEnabled,
		int * pnStartAddress,
		int * pnDataSize);

	/** Set the IO units current streaming configuration. Only some OptoMMP devices support this function.
	 *
	 * Streaming allows your application to get continuous information from a OptoMMP device
	 * without the need to continually poll it.
	 *
	 * See Chapter 2 - Streaming Data in the OptoMMP Protocol Guide for details.
	 *
	 * \param nOnFlag Whether or not streaming is enabled. 0 is off, non-zero is on.
	 * \param nIntervalMS Streaming interval, in milliseconds
	 * \param nPort IP port number to stream to
	 * \param nIoMirroringEnabled Whether I/O mirroring is enabled. 0 is off, non-zero is on.
	 * \param nStartAddress Beginning address of the data to be streamed
	 * \param nDataSize Size of data to be streamed. Maximum size is 1480 bytes.
	 * \return SIOMM_OK if the streaming configuration was read successfully.
	 * \return Errors possibly resulting from a call to ReadBlock.
	 */
	int SetStreamConfiguration(int nOnFlag, int nIntervalMS, int nPort,
		int nIoMirroringEnabled, int nStartAddress,
		int nDataSize);

	/** Retrieve the IP address of a streaming target
	 *
	 * Returns the IP address of the target as a single 4-byte integer.
	 *
	 * \param nTarget Index of the streaming target to retrieve the IP address for. (1-8)
	 * \param pnIpAddressArg Pointer to an integer to hold the returned address. Must not be NULL.
	 * \return SIOMM_OK if the IP address was read successfully
	 * \return SIOMM_ERROR if pnIpAddressArg is NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int GetStreamTarget(int nTarget, int * pnIpAddressArg);

	/** Set the IP address for a streaming target
	 *
	 * The IP address is passed as a string in dotted number notation, e.g. "1.2.3.4". The passed string must
	 * be an IP adddress; the method does not perform hostname lookups.
	 *
	 * \param nTarget Index of the streaming target to set the IP address for. (1-8)
	 * \param pchIpAddressArg IP address to set as a dotted string (e.g. "1.2.3.4")
	 * \return SIOMM_OK if the IP address was set successfully
	 * \return SIOMM_ERROR if pchIpAddressArg is NULL
	 * \return Other errors possibly resulting from a call to WriteBlock
	 */
	int SetStreamTarget(int nTarget, char * pchIpAddressArg);

	/** Read the streaming data area of the I/O units memory map.
	 *
	 * Returns the current state of the I/O unit's streaming area.
	 *
	 * \param pStreamData Pointer to an SIOMM_StreamStandardBlock instance to hold the results of the read.
	 * \return SIOMM_OK if the streaming data area was read successfully.
	 * \return SIOMM_ERROR if pStreamData is NULL
	 * \return Other errors possibly resulting from ReadBlock
	 */
	int GetStreamReadAreaEx(SIOMM_StreamStandardBlock *pStreamData);


	/** Read the I/O unit's scratchpad bits area
	 *
	 * \param pnBits63to32 Pointer to hold bits 32-63 of the scratchpad area
	 * \param pnBits31to0 Pointer to hold bits 0-31 of the scratchpad area
	 * \return SIOMM_OK if the scratchpad area was read successfully.
	 * \return SIOMM_ERROR if either argument is NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int GetScratchPadBitArea(int *pnBits63to32, int *pnBits31to0);

	/** Set the I/O unit's scratchpad bits area
	 *
	 * \param nBits63to32 Bitmask to set for bits 32-63
	 * \param nBits31to0 Bitmask to set for bits 0-31
	 * \return SIOMM_OK if the scratpchpad area was set successfully
	 * \return Errors possibly resulting from a call to WriteBlock
	 */

	int SetScratchPadBitArea(int   nBits63to32, int   nBits31to0);

	/** Set the I/O unit's scratchpad area.
	 *
	 * This method allows the caller to conditionally set bits in the scratchpad area. Any bit location
	 * set to 1 in nOnMask* arguments will be set ON, others will be left untouched. Any bit location set
	 * to 1 in the nOffMask* arguments will be set OFF, others will be left untouched.
	 *
	 * \param nOnMask63to32 Bitmask of locations to set ON, bits 32-63
	 * \param nOnMask31to0 Bitmask of locations to set ON, bits 0-31
	 * \param nOffMask63to32 Bitmask of locations to set OFF, bits 32-63
	 * \param nOffMask31to0 Bitmask of locations to set OFF, bits 0-31
	 * \return SIOMM_OK if the scratchpad area was written to successfully
	 * \return Errors possibly resulting from a call to WriteBlock
	 */
	int SetScratchPadBitAreaMask(int nOnMask63to32, int nOnMask31to0,
		int nOffMask63to32, int nOffMask31to0);

	/** Read from the I/O unit's integer scratch pad area
	 *
	 * \param nStartIndex Index of the integer scratchpad location to begin reading from
	 * \param nLength Length (in integer locations) of the data to read
	 * \param pnData Pointer to a user-supplied buffer to write the data read to. Must be nLength * 4 bytes in length.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pnData was NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int GetScratchPadIntegerArea(int nStartIndex, int nLength,
		int * pnData);

	/** Write to the I/O unit's integer scratch pad area
	 *
	 * \param nStartIndex Index of the integer scratchpad location to begin writing to
	 * \param nLength Length (in integer locations) of the data to write
	 * \param pnData Pointer to a user-supplied buffer of data to write. Must be nLength * 4 bytes in length.
	 * \return SIOMM_OK if the data was written successfully
	 * \return SIOMM_ERROR if pnData was NULL
	 * \return Other errors possibly resulting from a call to WriteBlock
	 */
	int SetScratchPadIntegerArea(int nStartIndex, int nLength,
		int * pnData);

	/** Read from the I/O unit's float scratch pad area
	 *
	 * \param nStartIndex Index of the float scratchpad location to begin reading from
	 * \param nLength Length (in float locations) of the data to read
	 * \param pfData Pointer to a user-supplied buffer to write the data read to. Must be nLength * 4 bytes in length.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pnData was NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int GetScratchPadFloatArea(int nStartIndex, int nLength,
		float * pfData);

	/** Write to the I/O unit's float scratch pad area
	 *
	 * \param nStartIndex Index of the float scratchpad location to begin writing to
	 * \param nLength Length (in float locations) of the data to write
	 * \param pfData Pointer to a user-supplied buffer of data to write. Must be nLength * 4 bytes in length.
	 * \return SIOMM_OK if the data was written successfully
	 * \return SIOMM_ERROR if pnData was NULL
	 * \return Other errors possibly resulting from a call to WriteBlock
	 */
	int SetScratchPadFloatArea(int nStartIndex, int nLength,
		float * pfData);

	/** Read from the I/O unit's string scratch pad area
	 *
	 * \param nStartIndex Index of the string scratchpad location to begin reading from
	 * \param nLength Length (in string locations) of the data to read
	 * \param pStringData Pointer to an array of SIOMM_ScratchPadString instances to hold the read string data. Must be sizeof(SIOMM_ScratchPadString) * nLength bytes in length.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pStringData was NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int GetScratchPadStringArea(int nStartIndex, int nLength,
		SIOMM_ScratchPadString * pStringData);

	/** Write to the I/O unit's string scratch pad area
	 *
	 * \param nStartIndex Index of the string scratchpad location to begin reading from
	 * \param nLength Length (in string locations) of the data to read
	 * \param pStringData Array of SIOMM_ScratchPadString instances to write to the I/O units string scratchpad. Must be sizeof(SIOMM_ScratchPadString) * nLength bytes in length.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pStringData was NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int SetScratchPadStringArea(int nStartIndex, int nLength,
		SIOMM_ScratchPadString * pStringData);

	/** Read from the I/O unit's 64-bit integer scratch pad area
	 *
	 * \param nStartIndex Index of the 64-bit integer scratchpad location to begin reading from
	 * \param nLength Length (in integer locations) of the data to read
	 * \param pnData Pointer to a user-supplied buffer to write the data read to. Must be nLength * 8 bytes in length.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pnData was NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int GetScratchPadInteger64Area(int nStartIndex, int nLength,
		int * pnData);

	/** Write to the I/O unit's 64-bit integer scratch pad area
	 *
	 * \param nStartIndex Index of the 64-bit integer scratchpad location to begin writing to
	 * \param nLength Length (in integer locations) of the data to write
	 * \param pnData Pointer to a user-supplied buffer of data to write. Must be nLength * 8 bytes in length.
	 * \return SIOMM_OK if the data was written successfully
	 * \return SIOMM_ERROR if pnData was NULL
	 * \return Other errors possibly resulting from a call to WriteBlock
	 */
	int SetScratchPadInteger64Area(int nStartIndex, int nLength,
		int * pnData);

	/** Read from the I/O Unit's data log.
	 *
	 * The SNAP-PAC-R Series, SNAP-PAC-EB or SNAP-PAC-SB, SNAP Ultimate, or SNAP Ethernet devices includes a
	 * feature that allows data from memory map addresses to be recorded in a log file. The data from
	 * up to 64 memory map addresses can be logged, and all logged data is recorded in one file. The
	 * log file holds up to 300 lines of data; when it is filled, new entries replace the oldest ones.
	 *
	 * See Chapter 2 - Logging Data in the OptoMMP Protocol Guide for more details.
	 *
	 * \param nStartIndex Sample location to start reading from
	 * \param nLength Number of samples to read.
	 * \param pDataSamples Array of SIOMM_DataLogSample instances to hold the read data. Must be sizeof(SIOMM_DataLogSample) * nLength bytes in length.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pDataSamples is NULL
	 * \return Other errors possibly resulting from a call to ReadBlock
	 */
	int GetDataLoggingSamplesEx(int nStartIndex, int nLength,
		SIOMM_DataLogSample * pDataSamples);

	/** Tell the I/O unit what kind of point is at a given location.
	 *
	 * Digital inputs, high density digital modules, and analog modules are all recognized automatically
	 * by the I/O unit when it boots up. Digital outputs are not, and so need to be configured before use.
	 *
	 * NOTE: This method only works with I/O units running firmware 8.0 or newer. For I/O units running older
	 * firmware, use ConfigurePoint(nPoint, nPointType)
	 *
	 * See Appendix A - (Expanded) Analog & Digital Point Configuration - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * \param nModule Which module the point to be configured resides on
	 * \param nPoint Point number on the module to configure.
	 * \param nPointType Point type value. A digital output is type 0x180, an input is 0x100.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int ConfigurePoint(int nModule, int nPoint, int nPointType);

	/** Set a point's feature.
	 * Works with devices with firmware version 8.0 or newer.
	 * \param nModule Module position of where the point resides.
	 * \param nPoint Point index (starting from zero) on the module.
	 * \param nPointFeature Feature to set the point. For volatile features, this value turns to zero when complete.
	 * \return SIOMM_OK if the data was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetPointFeatureEx(int nModule, int nPoint, int nPointFeature);

	/** Get a point's feature enumeration.
	* Works with devices with firmware version 8.0 or newer.
	* \param nModule Module position of where the point resides.
	* \param nPoint Point index (starting from zero) on the module.
	* \param nPointFeature Feature retrieved from the point. For volatile features, this value turns to zero when complete.
	* \return SIOMM_OK if the data was written successfully.
	* \return Other errors possibly resulting from WriteBlock.
	*/
	int GetPointFeatureEx(int nModule, int nPoint, int * pnPointFeature);

	/** Determine the type of a point on a given module.
	 *
	 * NOTE: This method only works with I/O units running firmware 8.0 or newer. For I/O units running older
	 * firmware, use GetModuleType(nPoint, pnModuleType).
	 *
	 * \param nModule Which module the targetted point resides on
	 * \param nPoint Which point to check the type for
	 * \param pnModuleType Pointer to an integer to return the point type for the point. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pnModuleType is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetModuleType(int nModule, int nPoint, int *pnModuleType);

	/** Read the full configuration data for a given point.
	 *
	 * See Appendix A - (Expanded) Analog & Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to read configuration for.
	 * \param pData Pointer to a SIOMM_PointConfigArea4 instance. Must not be NULL.
	 * \return SIOMM_OK if the data was read successfully.
	 * \return SIOMM_ERROR if pData is NULL.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int GetPointConfigurationEx4(int nModule, int nPoint, SIOMM_PointConfigArea4 *pData);

	/** Read the current gain value of an analog point.
	 *
	 * NOTE: This method only works with I/O units running firmware 8.0 or newer.
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to read the gain value for
	 * \param pfGain Pointer to a float value to return the point's gain value.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pfGain is NULL
	 * \return Other errors possibly resulting from ReadBlock
	 */
	int GetPointGain(int nModule, int nPoint, float *pfGain);

	/** Read the current offset value of an analog point
	 *
	 * NOTE: This method only works with I/O units running firmware 8.0 or newer
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to read the offset value for
	 * \param pfOffset Pointer to a float value to return the point's offset value.
	 * \return SIOMM_OK if the data was read successfully
	 * \return SIOMM_ERROR if pfOffsetis NULL
	 * \return Other errors possibly resulting from ReadBlock
	 */
	int GetPointOffset(int nModule, int nPoint, float *pfOffset);

	/** Set the full configuration data for a given point.
	 *
	 * See Appendix A - (Expanded) Analog & Digital Point Configuration Information - Read/Write of the OptoMMP Protocol Guide for details.
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to set configuration for.
	 * \param PtConfigData SIOMM_PointConfigArea4 instance holding the configuration for this point
	 * \return SIOMM_OK if the configuration was written successfully.
	 * \return Other errors possibly resulting from WriteBlock.
	 */
	int SetPointConfigurationEx4(int nModule, int nPoint, SIOMM_PointConfigArea4 PtConfigData);

	/** Set an analog point's filter weight.
	 *
	 * SNAP Analog I/O channels may use a filter weight to smooth analog input signals
	 * that are erratic or change suddenly. The formula used for filtering is
	 * Y = ( X - Y )/W + Y, where Y is the filtered value, X is the new unfiltered value,
	 * and W is the filter weight.
	 *
	 * A filter weight of 0 turns off the calculation. Values less than or equal to 0.5 are changed
	 * to 0, since those values would cause an unstable signal.
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to set configuration for.
	 * \param fFilterWeight Filter weight value to set for the point.
	 * \return SIOMM_OK if the point's configuration was written successfully
	 * \return Other errors possibly resulting from WriteBlock
	 */
	int SetPointFilterWeight(int nModule, int nPoint, float fFilterWeight);

	/** Set an analog point's gain value.
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to set configuration for.
	 * \param fGain Filter weight value to set for the point.
	 * \return SIOMM_OK if the point's configuration was written successfully
	 * \return Other errors possibly resulting from WriteBlock
	 */
	int SetPointGain(int nModule, int nPoint, float fGain);

	/** Set an analog point's offset value
	 *
	 * \param nModule Which module the targeted resides on
	 * \param nPoint Point number on the module to set the offset value for
	 * \param fOffset Offset value to set on the point
	 * \return SIOMM_OK if the point's configuration was written successfully
	 * \return Errors possibly resulting from WriteBlock
	 */
	int SetPointOffset(int nModule, int nPoint, float fOffset);

	/** Set scaling values for an analog point.
	 *
	 * You can scale analog input or output points to match your needs. For example, you
	 * can scale a -5 V to +5 V input point to reflect 0% to 100%. Point types may be
	 * unipolar or bipolar.
	 *
	 * See Chapter 2 - Scaling (Analog Points) of the OptoMMP Protocol Guide for details.
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to set scaling values for
	 * \param fHiScale High value to use for scaling
	 * \param fLoScale Low value to use for scaling
	 * \return SIOMM_OK if the point's scaling values were written successfully
	 * \return Errors possibly resulting from WriteBlock
	 */
	int SetPointScale(int nModule, int nPoint, float fHiScale, float fLoScale);

	/** Configure a watchdog for a point
	 *
	 * A watchdog monitors communication on the OptoMMP port. If nothing accesses the port for the length of time set
	 * in the watchdog, the I/O unit automatically sets designated digital and analog I/O points to the values you have
	 * determined.
	 *
	 * A watchdog helps make sure that a communication failure doesn’t result in disaster. If communication fails between
	 * the host and the I/O unit controlling a process, the watchdog makes sure the process is automatically brought to a
	 * safe state. For example, a valve could automatically close to avoid completely emptying a tank.
	 *
	 * \param nModule Which module the targeted point resides on
	 * \param nPoint Point number on the module to set watchdog parameters for
	 * \param fValue Value to set the point to if communications fail
	 * \param nEnabled Turn the watchdog on or off. 0 means off, non-zero means on.
	 * \return SIOMM_OK if the point was configured successfully.
	 * \return Errors possibly resulting from WriteBlock.
	 */
	int SetPointWatchdog(int nModule, int nPoint, float fValue, int nEnabled);

	/** Sets a user-friendly name for a point.
	 *
	 * A point's name is limited to 51 characters, including a NULL byte.
	 *
	 * \param nModule Which module the point to be named resides on
	 * \param nPoint Point number on the module to set a name for
	 * \param pchPointName C-string holding the point's name
	 * \return SIOMM_OK if the point's name was written successfully
	 * \return SIOMM_ERROR if pchPointName is NULL
	 * \return Other errors possibly resulting from WriteBlock
	 */
	int SetPointName(int nModule, int nPoint, char *pchPointName);

	/** Get the read area for a high density digital point
	 *
	 * \param nModule Which module the point to be read resides on
	 * \param nPoint Point number on the module to read
	 * \param pData Pointer to a SIOMM_DigPointReadArea instance to hold the results of the read operation
	 * \return SIOMM_OK if the point was read successfully
	 * \return SIOMM_ERROR if pData is NULL
	 * \return Other values possibly resulting from ReadBlock.
	 */
	int GetDigitalPointReadAreaEx(int nModule, int nPoint, SIOMM_DigPointReadArea *pData);

	/** Calculate and set an analog point's offset.
	 *
	 * Memory-mapped I/O units with analog capability can calculate offset and gain for analog input points.
	 * Calculate offset first, and then calculate gain. See the PAC Manager User's Guide for instructions.
	 *
	 * Use this method for I/O units with firmware versions 8.0 or newer. For units with firmware version 7.1
	 * or older, use CalcSetAnaPtOffset.
	 *
	 * @see CalcSetAnaPtOffset
	 * \param nModule Integer between 0 and 63 (inclusive) indicating which module the target point is on.
	 * \param nPoint Integer between 0 and 63 (inclusive) indicating which point on the target module to target
	 * \param pfValue Pointer to a float holding the result from the read operation.
	 * \return SIOMM_OK if everything worked correctly.
	 * \return SIOMM_ERROR if pfValue is NULL, or if either nModule or nPoint are out of range.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int CalcSetAnalogPointOffsetEx(int nModule, int nPoint, float *pfValue);

	/** Calculate and set an analog point's gain.
	 *
	 * Memory-mapped I/O units with analog capability can calculate offset and gain for analog input points.
	 * Calculate offset first, and then calculate gain. See the PAC Manager User's Guide for instructions.
	 *
	 * Use this method for I/O units with firmware versions 8.0 or newer. For units with firmware version 7.1
	 * or older, use CalcSetAnaPtGain.
	 *
	 * @see CalcSetAnaPtGain
	 * \param nModule Integer between 0 and 63 (inclusive) indicating which module the target point is on.
	 * \param nPoint Integer between 0 and 63 (inclusive) indicating which point on the target module to target
	 * \param pfValue Pointer to a float holding the result from the read operation.
	 * \return SIOMM_OK if everything worked correctly.
	 * \return SIOMM_ERROR if pfValue is NULL, or if either nModule or nPoint are out of range.
	 * \return Other errors possibly resulting from ReadBlock.
	 */
	int CalcSetAnalogPointGainEx(int nModule, int nPoint, float *pfValue);

	// Analog output clamping

	/** Set clamping values for an analog point.
	 *
	 * NOTE: This method works with I/O units running firmware versions 8.0 or newer. For units with
	 * older firmware versions, use the version of SetAnalogOutputClamp without the nModule parameter.
	 *
	 * \param nModule Which module the point to be configured resides on
	 * \param nPoint Point number on the module to configure
	 * \param fLowClamp Lower bound clamping value
	 * \param fHighClamp Upper bound clamping value
	 * \return SIOMM_OK if the point configuration was written successfully
	 * \return Errors possibly resulting from WriteBlock
	 */
	int SetAnalogOutputClamp(int nModule, int nPoint, float fLowClamp, float fHighClamp);

	/** Retrieve clamping values for an analog point.
	 *
	 * NOTE: This method works with I/O units running firmware versions 8.0 or newer. For units with
	 * older firmware versions, use the version of GetAnalogOutputClamp without the nModule parameter.
	 *
	 * \param nModule Which module the point to be configured resides on
	 * \param nPoint Point number on the module to configure
	 * \param pfLowClamp Pointer to a float to hold the lower bound clamping value
	 * \param pfHighClamp Pointer to a float to hold the upper bound clamping value
	 * \return SIOMM_OK if the point configuration was read successfully
	 * \return SIOMM_ERROR if either pfLowClamp or pfHighClamp are NULL
	 * \return Errors possibly resulting from ReadBlock
	 */
	int GetAnalogOutputClamp(int nModule, int nPoint, float * pfLowClamp, float * pfHighClamp);

	/** Set clamping values for an analog point.
	 *
	 * NOTE: This method works with I/O units running firmware versions older than 8.0. For units with
	 * newer firmware versions, use the version of SetAnalogOutputClamp with the nModule parameter.
	 *
	 * \param nPoint Point number on the I/O unit to configure
	 * \param fLowClamp Lower bound clamping value
	 * \param fHighClamp Upper bound clamping value
	 * \return SIOMM_OK if the point configuration was written successfully
	 * \return Errors possibly resulting from WriteBlock
	 */
	int SetAnalogOutputClamp(int nPoint, float fLowClamp, float fHighClamp);

	/** Retrieve clamping values for an analog point.
	 *
	 * NOTE: This method works with I/O units running firmware versions older than 8.0. For units with
	 * newer firmware versions, use the version of SetAnalogOutputClamp with the nModule parameter.
	 *
	 * \param nPoint Point number on the I/O unit to configure
	 * \param pfLowClamp Pointer to a float to hold the lower bound clamping value
	 * \param pfHighClamp Pointer to a float to hold the upper bound clamping value
	 * \return SIOMM_OK if the point configuration was read successfully
	 * \return SIOMM_ERROR if either pfLowClamp or pfHighClamp are NULL
	 * \return Errors possibly resulting from ReadBlock
	 */
	int GetAnalogOutputClamp(int nPoint, float * pfLowClamp, float * pfHighClamp);

	/** Read a EPIC generation discrete channel.
	 * \param nModuleIndex module index from 0 to 63.
	 * \param nChannelIndex module index from 0 to 63.
	 * \param pstructEpicDigital pointer to a EPIC digital channel structure.
	 * \return SIOMM_OK if the read was successful.
	 * \return SIOMM_ERROR if nModuleIndex or nChannelIndex are out of range.
	 * \return Errors resulting from Readblock() such as a timeout or the TCP session was closed.
	 */
	int EpicGetDigitalChannel(int nModuleIndex, int nChannelIndex, SIOMM_EpicDigitalChannelRead * pstructEpicDigital);

	/** Read a EPIC digital packed area. Reads all digital states, on-latches, off-latches, and channel quality for every channel.
	* \param pstructEpicPackedDigital pointer to a EPIC packed digital channel structure, length 16 structures long.
	* \return SIOMM_OK if the read was successful.
	* \return SIOMM_ERROR if nModuleIndex or nChannelIndex are out of range.
	* \return Errors resulting from Readblock() such as a timeout or the TCP session was closed.
	*/
	int EpicGetDigitalPacked(SIOMM_EpicPackedDigitalRead * pstructEpicPackedDigital);

	/** Read a EPIC generation analog channel.
	* \param nModuleIndex module index from 0 to 63.
	* \param nChannelIndex module index from 0 to 63.
	* \param pstructEpicAnalog pointer to a EPIC analog channel structure.
	* \returns SIOMM_OK if the read was successful.
	* \returns SIOMM_ERROR if nModuleIndex or nChannelIndex are out of range.
	* \return Errors resulting from Readblock() such as a timeout or the TCP session was closed.
	*/
	int EpicGetAnalogChannel(int nModuleIndex, int nChannelIndex, SIOMM_EpicAnalogChannelRead * pstructEpicAnalog);

	/** Read the quality status of all EPIC I/O modules.
	 * \param pstructModuleQuality pointer to structure to store the module quality bitmasks.
	 * \returns likely communication errors or invalid memmap address (when not used with EPIC)
	 */
	int EpicGetModuleQuality(SIOMM_ModuleQuality * pstructModuleQuality);

	/** Read the quality status of a module's channels.
 	 * \param nModuleIndex module to read status.
 	 * \param pstructChannelQuality pointer to structure to store the channel quality values.
	 * \returns likely communication errors or invalid memmap address (when not used with EPIC)
	 */
	int EpicGetChannelQuality(int nModuleIndex, SIOMM_ChannelQuality * pstructChannelQuality);

	/** Read custom area addresses. Available for SNAP-PAC.
	 * /param nStartIndex starting index (0 through 1023) to begin reading from.
	 * /param nLength length of the read (0 through 255). Note, do not read past index 1024.
	 * /param pstructCustomAddresses pointer to structure storing the custom area addresses from the device.
	 * /return errors related to communication faults are most likely
	*/
	int GetCustomAreaAddresses(int nStartIndex, int nLength, SIOMM_CustomAreaAddresses * pstructCustomAddresses);

	/** Write custom area addresses. Available for SNAP-PAC.
	* /param nStartIndex starting index (0 through 1023) to begin writing to.
	* /param nLength length of the write (0 through 255). Note, do not read past index 1024.
	* /param pstructCustomAddresses pointer to structure storing the custom area addresses for writing.
	* /return errors related to communication faults are most likely
	*/
	int SetCustomAreaAddresses(int nStartIndex, int nLength, const SIOMM_CustomAreaAddresses * pstructCustomAddresses);

	/** Read custom area data. Available for SNAP-PAC.
	* /param nStartIndex starting index (0 through 1023) to begin reading from.
	* /param nLength length of the read (0 through 255). Note, do not read past index 1024.
	* /param pstructCustomAddresses pointer to structure storing the custom area data read.
	* /return errors related to communication faults are most likely
	*/
	int GetCustomAreaData(int nStartIndex, int nLength, SIOMM_CustomAreaData * pstructCustomData);

	/** Write custom area data. Available for SNAP-PAC.
	* /param nStartIndex starting index (0 through 1023) to begin writing to.
	* /param nLength length of the read (0 through 255). Note, do not write past index 1024.
	* /param pstructCustomAddresses pointer to structure storing the data to be read.
	* /return errors related to communication faults are most likely
	*/
	int SetCustomAreaData(int nStartIndex, int nLength, const SIOMM_CustomAreaData * pstructCustomData);

	/** Write bytes to memory map.
	 * /param dwOptoMMPAddress lower 32-bits of the OptoMMP address to be written.
	 * /param nLength quantity of bytes to write.
	 * /param pbyary byte array of data to write.
	 */
	int WriteBytes(uint32_t dwOptoMMPAddress, int nLength, const uint8_t * pbyary);

	/** Read bytes from memory map.
	* /param dwOptoMMPAddress lower 32-bits of the OptoMMP address to be read.
	* /param nLength quantity of bytes to read.
	* /param pbyary byte array of data to store data.
	*/
	int ReadBytes(uint32_t dwOptoMMPAddress, int nLength, uint8_t * pbyary);

protected:
	SOCKET           m_Socket;
#ifdef _WIN32
	SOCKADDR_STORAGE m_SocketAddressStore;   // generic socket address
#elif defined _LINUX
	sockaddr_storage m_SocketAddressStore;   // generic socket address
#else
	? ?
#endif
	size_t           m_SocketAddrLength;     // length of the address contained in storage
	int              m_nConnectionType;      // TCP or UDP

#ifdef _WIN32
	HANDLE           m_serialHandle;    // handle to our open serial port
#else
	int              m_serialHandle;
#endif

	uint8_t          m_serialAddress;   // Address on the serial brain
	int              m_serialBaudRate;
	int              m_serialPort;
	bool             m_serialTwoWire;   // Two-wire mode or Four-wire mode (half or full duplex)

	ConnectionMethod m_method;         // tcp/ip, serial, or passthru

	timeval     m_tvTimeOut;           // Timeout structure for sockets
	uint32_t    m_nTimeOutMS;          // For holding the user's timeout
	uint32_t    m_nOpenTimeOutMS;      // For holding the open timeout
	uint32_t    m_nOpenTime;           // For testing the open timeout
	int         m_nRetries;            // For holding the user's retries.

	int         m_nAutoPUCFlag;        // For holding the AutoPUC flag 
									   // sent in OpenEnet()

	uint8_t     m_byTransactionLabel;  // The current transaction label

	// Open/Close sockets functions
	int OpenSockets(const char * pchIpAddressArg, int nPort, int nOpenTimeOutMS);
	int CloseSockets();

	// Generic functions for getting/setting 64-bit bitmasks
	int GetBitmask64(uint32_t dwDestOffset, int *pnPts63to32, int *pnPts31to0);
	int SetBitmask64(uint32_t dwDestOffset, int nPts63to32, int nPts31to0);

	// Generic functions for getting/setting analog banks
	int GetAnaBank(uint32_t dwDestOffset, SIOMM_AnaBank * pBankData);
	int SetAnaBank(uint32_t dwDestOffset, SIOMM_AnaBank BankData);

	// Gets the next transaction label
	inline void UpdateTransactionLabel() {
		\
			m_byTransactionLabel++;        \
			if (m_byTransactionLabel >= 64)  \
				m_byTransactionLabel = 0;      \
	}

	inline uint32_t GetTickCount(); // Generic way of getting the time as a 
								 // tick count.

private:
	// Private data

	// Private Members
	int ReadBlockEthernet(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);
	int WriteBlockEthernet(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);
	int ReadBlockSerial(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);
	int WriteBlockSerial(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);
	int ReadBlockPassThru(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);
	int WriteBlockPassThru(const uint32_t dwDestOffset, const uint16_t wDataLength,
		uint8_t * pbyData);

	int BaudRateToSleepTime(int baudRate);
	int SendSerialMessage(uint8_t * pMessage, const uint16_t wMessageLength);
};



#endif // __O22SIOMM_H_

