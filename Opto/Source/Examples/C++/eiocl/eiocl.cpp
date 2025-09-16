//-----------------------------------------------------------------------------
//
// eiocon.cpp
// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
//
// This example demonstrates use of the OptoSnapIoMemMap C++ class for 
// communicating to an Opto 22 OptoMMP Device.  It creates a small
// shell environment that lets a user issues commands to read and write the
// device.  The Brain object, an instance of the OptoSnapIoMemMap C++ class,
// is used for connecting and communicating to the I/O unit.
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string>

#ifdef _WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include "O22SIOMM.h"

#ifdef _LINUX
#define _stricmp strcasecmp
#endif

#define EIOCL_ERROR     0
#define EIOCL_MAIN_RESULT_SUCCESS  0
#define EIOCL_MAIN_RESULT_ERROR    1

#define MAX_FILESIZE    200000

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

int SendImageToBrain(O22SnapIoMemMap BrainBoard, std::string strImage);

void PrintHelp()
{
	printf("  Opto 22 SNAP Ethernet I/O Command Line Tool B4.0a\n"
		"\n" //   0     1       2         3            4        5   6    7        8
		"  usage: eiocl ip-addr (or host) port timeout(sec) addr/pt r/w type [length] [data]\n"
		"         eiocl ip-addr (or host) port timeout(sec) -f filename\n"
		"         eiocl ip-addr (or host) port timeout(sec) -i image_filename\n"
		"\n"
		"  Required Read/Write parameters for the data types:\n"
		"\n"
		"     Type        Description            Read Mode     Write Mode\n"
		"    ==========================================================================\n"
		"     d,x,f    Decimal, Hex, Float                     value\n"
		"       b      Binary                    length        length  data\n"
		"       s      String                    length        \"string\"\n"
		"      ip      IP Address                              IP-Address\n"
		"      dp      Digital Point                           state\n"
		"      ap      Analog  Point                           value\n"
		"\n"
		"  Examples:\n"
		"    eiocl 10.20.30.40 2001 10.0 F0B00040 w f  12.34\n"
		"    eiocl 10.20.30.40 2001 10.0 F03B0000 w b  3 4A 69 6D\n"
		"    eiocl 10.20.30.40 2001 10.0 F03B0000 w s  \"Opto\"\n"
		"    eiocl 10.20.30.40 2001 10.0 F03B0000 w ip 123.45.67.89\n"
		"    eiocl 10.20.30.40 2001 10.0 12       w dp 1\n"
		"    eiocl 10.20.30.40 2001 10.0 F0500008 r d\n"
		"    eiocl 10.20.30.40 2001 10.0 F03B0000 r b  3\n"
		"    eiocl 10.20.30.40 2001 10.0 -f test.txt\n"
		"    eiocl 10.20.30.40 2001 10.0 -i imagetest.txt\n"
		"    eiocl nocompanyname.com 2001 5.0 -i imagetest.txt\n"
		"    eiocl 2001:db8::1234:5678:fdcb:1 2001 0.25 -i imagetest.txt\n"
		"    eiocl ::1 2001 0.25 F0B00040 w f  12.34\n");
}

void PrintError(int nResult)
{
	if (nResult == SIOMM_ERROR)
		printf("General error");
	else if (nResult == SIOMM_TIME_OUT)
		printf("Timeout");
	else if (nResult == SIOMM_ERROR_NO_SOCKETS)
		printf("No sockets");
	else if (nResult == SIOMM_ERROR_CREATING_SOCKET)
		printf("Could not create socket to SNAP I/O");
	else if (nResult == SIOMM_ERROR_CONNECTING_SOCKET)
		printf("Could not connect socket to SNAP I/O");
	else if (nResult == SIOMM_ERROR_RESPONSE_BAD)
		printf("Bad response");
	else if (nResult == SIOMM_ERROR_NOT_CONNECTED_YET)
		printf("Not connected yet");
	else if (nResult == SIOMM_ERROR_OUT_OF_MEMORY)
		printf("Out of memory");
	else if (nResult == SIOMM_ERROR_NOT_CONNECTED)
		printf("Not connected to SNAP I/O");
	else if (nResult == SIOMM_BRAIN_ERROR_UNDEFINED_CMD)
		printf("Undefined Command");
	else if (nResult == SIOMM_BRAIN_ERROR_INVALID_PT_TYPE)
		printf("Invalid point type");
	else if (nResult == SIOMM_BRAIN_ERROR_INVALID_FLOAT)
		printf("Invalid float");
	else if (nResult == SIOMM_BRAIN_ERROR_PUC_EXPECTED)
		printf("Powerup Clear expected");
	else if (nResult == SIOMM_BRAIN_ERROR_INVALID_ADDRESS)
		printf("Invalid memory address");
	else if (nResult == SIOMM_BRAIN_ERROR_INVALID_CMD_LENGTH)
		printf("Invalid command length");
	else if (nResult == SIOMM_BRAIN_ERROR_RESERVED)
		printf("Reserved");
	else if (nResult == SIOMM_BRAIN_ERROR_BUSY)
		printf("Busy");
	else if (nResult == SIOMM_BRAIN_ERROR_CANT_ERASE_FLASH)
		printf("Cannot erase flash");
	else if (nResult == SIOMM_BRAIN_ERROR_CANT_PROG_FLASH)
		printf("Cannot program flash");
	else if (nResult == SIOMM_BRAIN_ERROR_IMAGE_TOO_SMALL)
		printf("Downloaded imaged too small");
	else if (nResult == SIOMM_BRAIN_ERROR_IMAGE_CRC_MISMATCH)
		printf("Image CRC mismatch");
	else if (nResult == SIOMM_BRAIN_ERROR_IMAGE_LEN_MISMATCH)
		printf("Image length mismatch");
	else
		printf("Unknown error %d", nResult);

	printf("\n\n");
}


int ProcessCommand(char * pchCommand, O22SnapIoMemMap * pBrain, const char * pchPreErrorMsg,
	bool bCalledFromCommandLine)
{
	int            nResult;
	int             nAddress;
	int             nPoint;
	int             nDataLength;
	uint8_t *          pbyData;
	bool            bReadFlag;
	char *          pchMemAddress;
	char *          pchReadWriteType;
	char *          pchDataType;
	char *          pchDataLength;
	char *          pchDataByte;
	char *          pchString;

	nPoint = 0;

	pchMemAddress = strtok(pchCommand, " \n");
	if (!pchMemAddress)
	{
		printf("%sNo memory address given.\n\n", pchPreErrorMsg);
		return EIOCL_ERROR;
	}

	pchReadWriteType = strtok(NULL, " \n");
	if (!pchReadWriteType)
	{
		printf("%sNo read/write type given.\n\n", pchPreErrorMsg);
		return EIOCL_ERROR;
	}

	pchDataType = strtok(NULL, " \n");
	if (!pchDataType)
	{
		printf("%sNo data type given.\n\n", pchPreErrorMsg);
		return EIOCL_ERROR;
	}


	// Check and process read/write flag
	if (!_stricmp(pchReadWriteType, "R"))
	{
		bReadFlag = true;
	}
	else if (!_stricmp(pchReadWriteType, "W"))
	{
		bReadFlag = false;
	}
	else
	{
		printf("%sUnknown read/write flag. Should be R or W.\n\n", pchPreErrorMsg);
		return EIOCL_ERROR;
	}

	// Get the memory address or point number
	if ((0 == _stricmp(pchDataType, "dp")) || (0 == _stricmp(pchDataType, "ap")))
	{
		// Get the address. Not much to check
		sscanf(pchMemAddress, "%d", &nPoint);
	}
	else
	{
		// Get the address. Not much to check
		sscanf(pchMemAddress, "%x", &nAddress);
	}


	// Check and process data types
	if (!_stricmp(pchDataType, "B"))
	{
		// Get the next token, which should be the data length
		pchDataLength = strtok(NULL, " \n");
		if (!pchDataLength)
		{
			printf("%sNo data length given.\n\n", pchPreErrorMsg);
			return EIOCL_ERROR;
		}

		// Get and check the data length
		sscanf(pchDataLength, "%d", &nDataLength);
		if ((nDataLength < 1) || (nDataLength > 1000)) // 1000 is arbitrary
		{
			printf("%sInvalid data length. Should be between 1 and 1000.\n", pchPreErrorMsg);
			return EIOCL_ERROR;
		}

		if (bReadFlag)
		{
			pbyData = new uint8_t[nDataLength];
		}
		else
		{
			// Copy the data into the byte array
			pbyData = new uint8_t[nDataLength];
			for (int i = 0; i < nDataLength; i++)
			{
				int nDataByte;

				// Get the next token
				pchDataByte = strtok(NULL, " \n");
				if (!pchDataByte)
				{
					printf("%sInvalid amount of binary data. Expecting %d items.\n\n", pchPreErrorMsg, nDataLength);
					return EIOCL_ERROR;
				}

				// Check the data
				if (0 == sscanf(pchDataByte, "%x", &nDataByte))
				{
					printf("%sInvalid binary data. '%s' is not a hexadecimal number.\n\n", pchPreErrorMsg, pchDataByte);

					delete[] pbyData;
					return EIOCL_ERROR;
				}

				// Set this byte of data
				pbyData[i] = nDataByte;
			}
		}
	}
	else if (!_stricmp(pchDataType, "D"))
	{
		nDataLength = 4;
		pbyData = new uint8_t[nDataLength];

		if (bReadFlag)
		{
			// nothing to do yet!
		}
		else
		{
			char * pchData;
			int nData;

			pchData = strtok(NULL, " \n");

			if (pchData)
			{
				sscanf(pchData, "%d", &nData);

				pbyData[0] = O22BYTE0(nData);
				pbyData[1] = O22BYTE1(nData);
				pbyData[2] = O22BYTE2(nData);
				pbyData[3] = O22BYTE3(nData);
			}
			else
			{
				printf("%sNo data given.\n\n", pchPreErrorMsg);
				delete[] pbyData;
				return EIOCL_ERROR;
			}
		}
	}
	else if (!_stricmp(pchDataType, "DP"))
	{
		nDataLength = 4;
		pbyData = new uint8_t[nDataLength];

		// Check the point number
		if ((nPoint < 0) || (nPoint > 63))
		{
			printf("%sPoint number must be between 0 and 63.\n\n", pchPreErrorMsg);
			delete[] pbyData;
			return EIOCL_ERROR;
		}

		if (bReadFlag)
		{
			// Figure the memory map address from the digital point given
			nAddress = SIOMM_DPOINT_READ_STATE + (SIOMM_DPOINT_READ_BOUNDARY * nPoint);
		}
		else
		{
			char * pchData;
			int    nData = 0;

			pchData = strtok(NULL, " \n");

			if (pchData)
			{
				sscanf(pchData, "%d", &nData);

				// Figure the memory map address from the digital point given
				if (nData)
					nAddress = SIOMM_DPOINT_WRITE_TURN_ON_BASE + (SIOMM_DPOINT_WRITE_BOUNDARY * nPoint);
				else
					nAddress = SIOMM_DPOINT_WRITE_TURN_OFF_BASE + (SIOMM_DPOINT_WRITE_BOUNDARY * nPoint);

				pbyData[0] = O22BYTE0(1);
				pbyData[1] = O22BYTE1(1);
				pbyData[2] = O22BYTE2(1);
				pbyData[3] = O22BYTE3(1);
			}
			else
			{
				printf("%sNo data given.\n\n", pchPreErrorMsg);
				delete[] pbyData;
				return EIOCL_ERROR;
			}
		}
	}
	else if (!_stricmp(pchDataType, "AP"))
	{
		nDataLength = 4;
		pbyData = new uint8_t[nDataLength];

		if ((nPoint < 0) || (nPoint > 63))
		{
			printf("%sPoint number must be between 0 and 63.\n\n", pchPreErrorMsg);
			delete[] pbyData;
			return EIOCL_ERROR;
		}

		if (bReadFlag)
		{
			// Figure the memory map address from the digital point given
			nAddress = SIOMM_APOINT_READ_VALUE_BASE + (SIOMM_APOINT_READ_BOUNDARY * nPoint);
		}
		else
		{
			char * pchData;
			float  fData = 0.0;
			int    nTemp;

			nAddress = SIOMM_APOINT_WRITE_VALUE_BASE + (SIOMM_APOINT_WRITE_BOUNDARY * nPoint);

			pchData = strtok(NULL, " \n");

			if (pchData)
			{
				sscanf(pchData, "%f", &fData);

				memcpy(&nTemp, &fData, 4);

				pbyData[0] = O22BYTE0(nTemp);
				pbyData[1] = O22BYTE1(nTemp);
				pbyData[2] = O22BYTE2(nTemp);
				pbyData[3] = O22BYTE3(nTemp);
			}
			else
			{
				printf("%sNo data given.\n\n", pchPreErrorMsg);
				delete[] pbyData;
				return EIOCL_ERROR;
			}
		}
	}
	else if (!_stricmp(pchDataType, "X"))
	{
		nDataLength = 4;
		pbyData = new uint8_t[nDataLength];

		if (bReadFlag)
		{
			// nothing to do yet!
		}
		else
		{
			char * pchData;
			int nData;

			pchData = strtok(NULL, " \n");

			if (pchData)
			{
				sscanf(pchData, "%x", &nData);

				pbyData[0] = O22BYTE0(nData);
				pbyData[1] = O22BYTE1(nData);
				pbyData[2] = O22BYTE2(nData);
				pbyData[3] = O22BYTE3(nData);
			}
			else
			{
				printf("%sNo data given.\n\n", pchPreErrorMsg);
				delete[] pbyData;
				return EIOCL_ERROR;
			}
		}
	}
	else if (!_stricmp(pchDataType, "IP"))
	{
		nDataLength = 4;
		pbyData = new uint8_t[nDataLength];

		if (bReadFlag)
		{
			// nothing to do yet!
		}
		else
		{
			char * pchIpAddress;
			uint32_t nIpAddress;

			pchIpAddress = strtok(NULL, " \n");

			if (pchIpAddress)
			{
#ifdef _WIN32
				int i32PtonResult = InetPton(AF_INET, (PCTSTR)pchIpAddress, (PVOID)&nIpAddress);
#elif _LINUX
				int i32PtonResult = inet_pton(AF_INET, (const char *)pchIpAddress, (void *)&nIpAddress);
#else
				??
#endif
				if (i32PtonResult != 1)
				{
					printf("%sInvalid IP address. Should be in #.#.#.# form.\n\n", pchPreErrorMsg);
					delete[] pbyData;
					return EIOCL_ERROR;
				}

				// nIpAddress is already in the correct byte order so just copy
				// it into the data byte array.
				pbyData[0] = O22BYTE3(nIpAddress);
				pbyData[1] = O22BYTE2(nIpAddress);
				pbyData[2] = O22BYTE1(nIpAddress);
				pbyData[3] = O22BYTE0(nIpAddress);
			}
			else
			{
				printf("%sNo IP address given.\n\n", pchPreErrorMsg);
				return EIOCL_ERROR;
			}
		}
	}
	else if (!_stricmp(pchDataType, "F"))
	{
		float fData;
		int   nTemp;

		nDataLength = 4;
		pbyData = new uint8_t[nDataLength];

		if (bReadFlag)
		{
			// nothing to do yet!
		}
		else
		{
			char * pchData;

			pchData = strtok(NULL, " \n");

			if (pchData)
			{
				sscanf(pchData, "%f", &fData);

				memcpy(&nTemp, &fData, 4);

				pbyData[0] = O22BYTE0(nTemp);
				pbyData[1] = O22BYTE1(nTemp);
				pbyData[2] = O22BYTE2(nTemp);
				pbyData[3] = O22BYTE3(nTemp);
			}
			else
			{
				printf("%sNo data given.\n\n", pchPreErrorMsg);
				delete[] pbyData;
				return EIOCL_ERROR;
			}
		}
	}
	else if (0 == _stricmp(pchDataType, "S"))
	{
		if (bReadFlag)
		{
			pchDataLength = strtok(NULL, " \n");
			if (!pchDataLength)
			{
				printf("%sNo string length given.\n\n", pchPreErrorMsg);
				return EIOCL_ERROR;
			}

			// Get and check the data length
			sscanf(pchDataLength, "%d", &nDataLength);
			if ((nDataLength < 1) || (nDataLength > 1000)) // 1000 is arbitrary
			{
				printf("%sInvalid string length. Should be between 1 and 1000.\n\n", pchPreErrorMsg);
				return EIOCL_ERROR;
			}

			pbyData = new uint8_t[nDataLength + 1];
		}
		else
		{
			// Read 'til the next "
			pchString = strtok(NULL, "\n");

			if (!pchString)
			{
				printf("%sNo string given.\n\n", pchPreErrorMsg);
				return EIOCL_ERROR;
			}

			char * pchStringTemp;
			pchStringTemp = new char[strlen(pchString) + 1]; // make a new string(might be too int; oh well)
			pchStringTemp[0] = 0;

			// The strings may in a different depending if it is from a file or the command line.
			// Commands from the command line have any \" turned into just "
			if (bCalledFromCommandLine)
			{
				strcpy(pchStringTemp, pchString);
			}
			else
			{
				// Convert any \" into "
				for (unsigned int i = 0; i < strlen(pchString); i++)
				{
					if ((pchString[i] == '\\') && (pchString[i + 1]))
					{
						strncat(pchStringTemp, "\"", 1);
						i++;
					}
					else
					{
						strncat(pchStringTemp, pchString + i, 1);
					}
				}

				// Trim the right hand side of whitespace
				while (isspace(pchStringTemp[strlen(pchStringTemp) - 1]))
				{
					pchStringTemp[strlen(pchStringTemp) - 1] = 0;
				}

			}

			// Remove the quotes around the string
			pchStringTemp++;
			pchStringTemp[strlen(pchStringTemp) - 1] = 0;

			nDataLength = strlen(pchStringTemp) + 1;

			// Copy the data into the byte array
			pbyData = new uint8_t[nDataLength];

			strcpy((char*)pbyData, pchStringTemp);

			// Reset the temp pointer and clean up
			pchStringTemp--;
			delete[] pchStringTemp;
		}
	}
	else
	{
		printf("%sUnknown data type. Should be B, D, X, F, S, or IP.\n\n", pchPreErrorMsg);
		return EIOCL_ERROR;
	}

	if (bReadFlag)
	{
		// NULL out the data byte array
		memset(pbyData, 0, nDataLength);

		nResult = pBrain->ReadBlock(nAddress, nDataLength, pbyData);

		// Check the result
		if (nResult == SIOMM_OK)
		{
			if (!_stricmp(pchDataType, "B"))
			{
				printf("  Binary data at %s is ", pchMemAddress);
				for (int i = 0; i < nDataLength; i++)
				{
					printf("%02X ", pbyData[i]);
				}
				printf("\n\n");
			}
			else if (!_stricmp(pchDataType, "D"))
			{
				int   nValue;

				nValue = O22MAKELONG(pbyData[0], pbyData[1], pbyData[2], pbyData[3]);

				printf("  Integer value at %s is %d\n\n", pchMemAddress, nValue);
			}
			else if (!_stricmp(pchDataType, "X"))
			{
				int nValue;

				nValue = O22MAKELONG(pbyData[0], pbyData[1], pbyData[2], pbyData[3]);

				printf("  Integer value at %s is 0x%08X\n\n", pchMemAddress, nValue);
			}
			else if (!_stricmp(pchDataType, "DP"))
			{
				int   nValue;

				nValue = O22MAKELONG(pbyData[0], pbyData[1], pbyData[2], pbyData[3]);

				printf("  Digital point #%d is %s\n\n", nPoint, nValue ? "ON" : "OFF");
			}
			else if (!_stricmp(pchDataType, "AP"))
			{
				float fValue;
				int   nTemp;

				nTemp = O22MAKELONG(pbyData[0], pbyData[1], pbyData[2], pbyData[3]);

				memcpy(&fValue, &nTemp, 4);

				printf("  Analog point #%d is %f\n\n", nPoint, fValue);
			}
			else if (!_stricmp(pchDataType, "IP"))
			{
				printf("  IP address at %s is %d.%d.%d.%d\n\n", pchMemAddress,
					pbyData[0], pbyData[1], pbyData[2], pbyData[3]);
			}
			else if (!_stricmp(pchDataType, "F"))
			{
				float fValue;
				int   nTemp;

				nTemp = O22MAKELONG(pbyData[0], pbyData[1], pbyData[2], pbyData[3]);

				memcpy(&fValue, &nTemp, 4);

				printf("  Float value at %s is %f\n\n", pchMemAddress, fValue);
			}
			else if (!_stricmp(pchDataType, "S"))
			{
				pbyData[nDataLength] = 0;
				printf("  String value at %s is \"%s\"\n\n", pchMemAddress, pbyData);
			}
		}

	}
	else
	{
		nResult = pBrain->WriteBlock(nAddress, nDataLength, pbyData);
	}

	delete[] pbyData;

	if (nResult != SIOMM_OK)
	{
		printf("%s", pchPreErrorMsg);
		PrintError(nResult);
	}

	return nResult;
}


int main(int argc, char *argv[])
{
	O22SnapIoMemMap Brain;
	int             nResult;
	int             nPort;

	printf("\n");

	// Make sure there's at least five command-line parameters.  
	if (argc < 6)
	{
		PrintHelp();
		return EIOCL_MAIN_RESULT_ERROR;
	}

	// Check the host argument
	// create enough "hints" just to try and resolve a host
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	struct addrinfo *pstAddresses;
	// this will validate ipv4 or ipv6 or attempt to look up a host
	int i32Status = getaddrinfo((const char *)argv[1], (const char *)"0", &hints, &pstAddresses);
	if (i32Status != 0)
	{
		printf("  Improper IPv4 address, IPv6 address or a unresolvable hostname.\n");
		return EIOCL_MAIN_RESULT_ERROR;
	}

	// Get the port number and check it
	sscanf(argv[2], "%d", &nPort);
	if ((nPort < 1) || (nPort > 65535))
	{
		printf("  Invalid IP port. Should be between 1 and 65535.\n"
			"  Example: 2001\n");
		return EIOCL_MAIN_RESULT_ERROR;
	}

	float f32TimeoutSeconds = 0.0;
	sscanf(argv[3], "%f", &f32TimeoutSeconds);
	if (f32TimeoutSeconds < 0.25)
	{
		printf("  Invalid timeout value. Should be at least 0.25 seconds.\n"
			"  Example: 10\n");
		return EIOCL_MAIN_RESULT_ERROR;
	}

	// Set the send/recv timeout value
	Brain.SetCommOptions((int)(f32TimeoutSeconds * 1000.0), 0);

	// Open a connection. Use the same timeout for the open.
	nResult = Brain.OpenEnet(argv[1], nPort, (int)(f32TimeoutSeconds * 1000.0), 1);

	// Print a status message
  //  printf("\n  Attemping to connect to Ethernet I/O unit...");

	// Check the result of the open function
	if (SIOMM_OK == nResult)
	{
		// Keep calling IsOpenDone() until we connect or error
		nResult = Brain.IsOpenDone();
		while (SIOMM_ERROR_NOT_CONNECTED_YET == nResult)
		{
			// Try again
			nResult = Brain.IsOpenDone();
		}

	}

	// Check for error on OpenEnet() and IsOpenDone()
	if (SIOMM_OK != nResult)
	{
		Brain.Close();

		printf("Error - Unable to open a connection to the device: ");
		PrintError(nResult);
		return EIOCL_MAIN_RESULT_ERROR;
	}

	// Check if an image file is being specified or not - 
	if (0 == _stricmp(argv[4], "-i"))
	{
		int    nError = 0;
		FILE * pImageFile;

		// Try to open the given file
		pImageFile = fopen(argv[5], "r");

		// Check the file
		if (pImageFile)
		{
			char    pchLineTemp[15]; // for reading in each line from the file
			unsigned int dwNumBytes, dwSize;
			std::string strImage;
			char    pchWholeFile[MAX_FILESIZE];

			// read in the first part of the file--should look something like this: 
			// "FFFFF0360000: 0000E7B8E63435"
			// (mem map address of where to start writing, followed by ": ", 4 bytes of size,
			// and 4 bytes of CRC). We'll use this info to see if this file looks even 
			// close to a valid image file.
			//
			// SendImageToBrain then parses the rest of the data and sends it off.
			// The data that follows the header info repeats this pattern:
			// (for example: "F03800040000000400000000")
			// 4 bytes of mem map address (in this case: "F0380004")
			// 4 bytes of length (here it's "00000004")
			// X bytes of data (the length from the previous four bytes tell use 4 bytes 
			//    for this data as well, in this case, all zeros: "00000000"
			if (fgets(pchLineTemp, 14, pImageFile) == NULL)
			{
				printf("fgets error\n");
			}
			else  // opened the file ok
			{
				printf("Reading image file header...\n");

				// Get the # of bytes total
				fscanf(pImageFile, "%8x", &dwNumBytes);

				// Calculate the size of the file
				if (0 == fseek(pImageFile, 0L, SEEK_END))
				{
					dwSize = ftell(pImageFile);

					// put the pointer in the file back to beginning
					fseek(pImageFile, 0, SEEK_SET);

					if (MAX_FILESIZE < dwSize)
					{
						printf("Image file larger than this program can handle. Modify MAX_FILESIZE.");
					}
					else if ((dwSize - (dwNumBytes * 2 + 30)) > 2)
					{
						// something's wrong with this file, the file size doesn't match # of bytes 
						// the file specifies 
						printf("%s is not a valid image file. Num bytes file says: %d, size of file: %d", argv[5],
							dwNumBytes, dwSize);
					}
					else // most likely we have an okay image file
					{
						if (fgets(pchWholeFile, (dwNumBytes * 2), pImageFile) == NULL)
						{
							printf("fgets error reading the whole file.\n");
						}
						else
						{
							printf("Writing image file information to: %s\n", argv[1]);

							strImage.assign(pchWholeFile, MAX_FILESIZE);

							// Send Image to I/O Unit
							nError = SendImageToBrain(Brain, strImage);
							if (nError != SIOMM_OK) {
								printf("Writing image to device failed with error code %d\n", nError);
							}
							else {
								printf("Done writing image.\n");
							}
						}
					}
				}
			}
			fclose(pImageFile);
		}
		else
		{
			printf("Error opening file: %s", argv[5]);
		}

	}
	// Check if a file is being specified or not
	else if (0 == _stricmp(argv[4], "-f"))
	{
		FILE * pCommandFile;

		// Try to open the given file
		pCommandFile = fopen(argv[5], "rt");

		// Check the file
		if (pCommandFile)
		{
			char   pchLineCommandTemp[1000]; // for reading in each line from the file
			char * pchLineCommand;           // for parsing through each line
			bool   bContinue = true;
			int    nLine = 1;

			// Get the first line
			pchLineCommand = fgets(pchLineCommandTemp, 1000, pCommandFile);

			while (pchLineCommand && bContinue)
			{
				// Trim the left hand side of whitespace
				while (isspace(pchLineCommand[0]))
				{
					pchLineCommand++;
				}

				// Filter out empty lines and comments
				if ((strlen(pchLineCommand) > 1) &&
					(!((pchLineCommand[0] == '/') && (pchLineCommand[1] == '/'))))
				{
					char pchPreErrorMsg[50];
					sprintf(pchPreErrorMsg, "  ERROR on line #%d: ", nLine);

					// Pass this line on.
					nResult = ProcessCommand(pchLineCommand, &Brain, pchPreErrorMsg, false);

					if (nResult != SIOMM_OK)
					{
						bContinue = false; // this will get us out of the loop
					}

					// Sleep for a moment so the device won't be flooded
#ifdef _WIN32
					Sleep(100000); // this delay is arbitrary
#endif
#ifdef _LINUX
					usleep(100); // this delay is arbitrary
#endif
				}

				nLine++;

				// Get the next line
				pchLineCommand = fgets(pchLineCommand, 1000, pCommandFile);
			}

			fclose(pCommandFile);

		}
		else
		{
			// Unable to open the file.
			printf("  Unable to open file \"%s\".\n", argv[5]);
			nResult = EIOCL_ERROR;
		}
	}
	else
	{

		char * pchCommand;
		pchCommand = new char[1000]; // what should this be?
		pchCommand[0] = 0;

		for (int i = 4; i < argc; i++)
		{
			if ((7 == i) && (0 == _stricmp(argv[6], "S")) && (0 == _stricmp(argv[5], "W")))
			{
				strcat(pchCommand, "\"");
				strcat(pchCommand, argv[i]);
				strcat(pchCommand, "\"");
			}
			else
			{
				strcat(pchCommand, argv[i]);
				strcat(pchCommand, " ");
			}
		}

		strcat(pchCommand, "\n");

		nResult = ProcessCommand(pchCommand, &Brain, "  ERROR: ", true);
	}

	// Close the connection to the device
	Brain.Close();

	if (nResult == SIOMM_OK)
		return EIOCL_MAIN_RESULT_SUCCESS;
	else
		return EIOCL_MAIN_RESULT_ERROR;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/// FUNCTIONS FOR PERFORMING store image to flash
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void ConvertHexStringToBytes(const std::string &strHexString, unsigned int nNumChars, uint8_t *pBytes)
//-------------------------------------------------------------------------------------------------
// Converts a string of hex characters to an array of bytes. For each 2 characters
// there is one byte.
//-------------------------------------------------------------------------------------------------
{
	unsigned int  nCharPos = 0, nBytePos = 0;
	std::string   strPiece;
	unsigned int ulValue;
	char          *pChar;

	while (nCharPos < nNumChars)
	{
		// Get 2 characters
		if (nCharPos + 2 <= strHexString.length())
		{
			strPiece = strHexString.substr(nCharPos, 2);

			// Convert the piece to a decimal value
			ulValue = strtoul(strPiece.c_str(), &pChar, 16);
		}
		else
		{
			ulValue = 0;
		}


		// Store in byte array
		pBytes[nBytePos] = (uint8_t)ulValue;

		// Increment the indexes
		nCharPos += 2;
		nBytePos++;
	}
}

#define MAX_MEMMAP_READ_SIZE  1024

int SendImageToBrain(O22SnapIoMemMap BrainBoard, std::string strImage)
//-------------------------------------------------------------------------------------------------
// Send an device image. Only the items explicitly configured
// are in the image.
//-------------------------------------------------------------------------------------------------
{
	int            nRetVal = 1;
	bool           bErrorInPointConfigArea = false;
	std::string    strPiece, strPieceData;
	int           nResult = SIOMM_ERROR_NOT_CONNECTED_YET;
	uint8_t           bytes[4];
	uint32_t          dwTotalLengthInBytes, dwTotalLengthInChars, dwPosition;
	uint32_t          dwAddress, dwDataLengthInBytes, dwDataLengthInChars;
	uint8_t           DataBlock[MAX_MEMMAP_READ_SIZE + 1];
	int            nAmountToWrite, nStartChar, nSizeOfStringPiece;

	nRetVal = 0;
	// Get the total length out of the image string
	strPiece = strImage.substr(14, 8);
	ConvertHexStringToBytes(strPiece, 8, bytes);
	dwTotalLengthInBytes = O22MAKELONG(bytes[0], bytes[1], bytes[2], bytes[3]);

	// Calculate the total number of chars in the file
	dwTotalLengthInChars = dwTotalLengthInBytes * 2 + 30;

	// Read each address-length-data chunk and send to the I/O Unit
	dwPosition = 30;
	while (dwPosition < dwTotalLengthInChars)
	{
		nRetVal = 2; // preset to "error while writing"

		// Get the address
		strPiece = strImage.substr(dwPosition, 8);
		if (strPiece.empty())
			break;
		ConvertHexStringToBytes(strPiece, 8, bytes);
		dwAddress = O22MAKELONG(bytes[0], bytes[1], bytes[2], bytes[3]);
		dwPosition += 8;

		// Get the length of the data
		strPiece = strImage.substr(dwPosition, 8);
		if (strPiece.empty())
			break;
		ConvertHexStringToBytes(strPiece, 8, bytes);
		dwDataLengthInBytes = O22MAKELONG(bytes[0], bytes[1], bytes[2], bytes[3]);
		dwDataLengthInChars = dwDataLengthInBytes * 2; // account for 2 chars for each byte of data
		dwPosition += 8;

		// Get the data
		strPiece = strImage.substr(dwPosition, dwDataLengthInChars);
		if (strPiece.empty())
			break;
		// Send the information to the I/O Unit
		dwPosition += dwDataLengthInChars; // increment string position ahead of time
		nStartChar = 0;
		while (dwDataLengthInChars > 0)
		{
			// Prepare the DataBlock for writing
			nAmountToWrite = min(dwDataLengthInBytes, MAX_MEMMAP_READ_SIZE);
			nSizeOfStringPiece = nAmountToWrite * 2; // number of chars is twice as much as bytes
			strPieceData = strPiece.substr(nStartChar, nSizeOfStringPiece);
			ConvertHexStringToBytes(strPieceData, nSizeOfStringPiece, DataBlock);

			// Write the block
			nResult = BrainBoard.WriteBlock(dwAddress, nAmountToWrite, DataBlock);
			if (SIOMM_OK != nResult)
			{
				if (dwAddress == 0xf03b0000)
				{
					// Web security was removed at one point, so display a message but keep on going.
					printf("- SNAP-ENET Web Security is not supported on the destination I/O Unit.");
					nResult = SIOMM_OK; // reset the result so we don't break out of the outer loop
					break;
				}
				else if (dwAddress >= 0xf0c00000 && dwAddress <= 0xf0c00fff)
				{
					if (!bErrorInPointConfigArea)
					{
						// Error while writing the point configuration area. This can only happen
						// when writing to an Ultimate I/O brain where the source image contains
						// modules that do not exist on the Ultimate I/O brain. Ethernet I/O brains
						// do not report this "error". In fact, it is not an error since the data
						// is actually stored in the brain memory map. It should be a warning, but
						// the brain kernel only returns one error code (uh, didn't work). It was
						// decided to not put a message in the message list since all it could 
						// possibly result in is confusing the users, and they're already confused
						// enough. (4-5-2002)
						bErrorInPointConfigArea = true;
					}
					nResult = SIOMM_OK; // reset the result so we don't break out of the outer loop
				}
				else
				{
					printf("- An error occurred while writing to address 0x%X. A common reason for this type of error is incompatible kernel versions.", dwAddress);
					nRetVal = 4;
					break;
				}
			}

			// Increment/decrement and continue
			dwAddress += nAmountToWrite;
			nStartChar += nSizeOfStringPiece;
			dwDataLengthInChars -= nSizeOfStringPiece;
			dwDataLengthInBytes -= nAmountToWrite;
		}
		if (SIOMM_OK != nResult)
			break;

		nRetVal = 0; // all ok for this address-length-data chunk
	}

	// If no error, store the image to flash
	if (nRetVal == 0)
	{
		nResult = BrainBoard.SetStatusOperation(3);
		if (SIOMM_OK != nResult)
		{
			printf("- An error occurred while saving to flash memory.");
			nRetVal = 3;
		}
	}
	else if (nRetVal == 2)
	{
		// The file is not formatted correctly
		printf("- The source image was not formatted correctly or has been corrupted. Image transfer aborted.");
	}

	return nRetVal;
}
