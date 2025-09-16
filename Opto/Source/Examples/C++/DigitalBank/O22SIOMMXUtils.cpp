#include "O22SIOMMXUtils.h"


#ifdef __AFX_H__ // for MFC only

BOOL GetBitmask64FromString(CString strBitmask, int * n63to32Part, int * n31to0Part)
//---------------------------------------------------------------------------------------
// Convert a string bitmask to an actual bitmask.  Assumes the string is in a 64-bit 
// integer format, such as "0x0000000000000000"
//---------------------------------------------------------------------------------------
{
#ifdef _UNICODE
  *n63to32Part = (int) wcstoul(strBitmask.Mid(2,8), NULL, 16);
  *n31to0Part  = (int) wcstoul(strBitmask.Mid(10,8), NULL, 16);
#else
  *n63to32Part = (int) strtoul(strBitmask.Mid(2,8), NULL, 16);
  *n31to0Part  = (int) strtoul(strBitmask.Mid(10,8), NULL, 16);
#endif

  return TRUE;
}


CString GetResultAsString(int nResult)
//---------------------------------------------------------------------------------------
// Returns a string describing a result value.
//---------------------------------------------------------------------------------------
{
  CString strResult;

  if (nResult == SIOMM_OK)
    strResult = "Ok";
  else if (nResult == SIOMM_ERROR)
    strResult = "General error";
  else if (nResult == SIOMM_TIME_OUT) 
    strResult = "Timeout";
  else if (nResult == SIOMM_ERROR_NO_SOCKETS) 
    strResult = "No sockets";
  else if (nResult == SIOMM_ERROR_CREATING_SOCKET) 
    strResult = "Could not create socket to SNAP I/O";
  else if (nResult == SIOMM_ERROR_CONNECTING_SOCKET) 
    strResult = "Could not connect socket to SNAP I/O";
  else if (nResult == SIOMM_ERROR_RESPONSE_BAD) 
    strResult = "Bad response";
  else if (nResult == SIOMM_ERROR_NOT_CONNECTED_YET) 
    strResult = "Not connected yet";
  else if (nResult == SIOMM_ERROR_OUT_OF_MEMORY)  
    strResult = "ActiveX out of memory";
  else if (nResult == SIOMM_ERROR_NOT_CONNECTED)
    strResult = "Not connected to SNAP I/O";
  else if (nResult == SIOMM_BRAIN_ERROR_UNDEFINED_CMD) 
    strResult = "SNAP I/O Error: Undefined Command";
  else if (nResult == SIOMM_BRAIN_ERROR_INVALID_PT_TYPE) 
    strResult = "SNAP I/O Error: Invalid point type";
  else if (nResult == SIOMM_BRAIN_ERROR_INVALID_FLOAT) 
    strResult = "SNAP I/O Error: Invalid float";
  else if (nResult == SIOMM_BRAIN_ERROR_PUC_EXPECTED) 
    strResult = "SNAP I/O Error: Powerup Clear expected";
  else if (nResult == SIOMM_BRAIN_ERROR_INVALID_ADDRESS) 
    strResult = "SNAP I/O Error: Invalid memory address";
  else if (nResult == SIOMM_BRAIN_ERROR_INVALID_CMD_LENGTH) 
    strResult = "SNAP I/O Error: Invalid command length";
  else if (nResult == SIOMM_BRAIN_ERROR_RESERVED) 
    strResult = "SNAP I/O Error: Reserved";
  else if (nResult == SIOMM_BRAIN_ERROR_BUSY) 
    strResult = "SNAP I/O Error: Busy";
  else if (nResult == SIOMM_BRAIN_ERROR_CANT_ERASE_FLASH) 
    strResult = "SNAP I/O Error: Cannot erase flash";
  else if (nResult == SIOMM_BRAIN_ERROR_CANT_PROG_FLASH) 
    strResult = "SNAP I/O Error: Cannot program flash";
  else if (nResult == SIOMM_BRAIN_ERROR_IMAGE_TOO_SMALL) 
    strResult = "SNAP I/O Error: Downloaded imaged too small";
  else if (nResult == SIOMM_BRAIN_ERROR_IMAGE_CRC_MISMATCH) 
    strResult = "SNAP I/O Error: Image CRC mismatch";
  else if (nResult == SIOMM_BRAIN_ERROR_IMAGE_LEN_MISMATCH) 
    strResult = "SNAP I/O Error: Image length mismatch";
  else
    strResult = "Unknown error";

  return strResult;  
}

#endif // __AFX_H__
