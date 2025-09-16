//-----------------------------------------------------------------------------
//
// DigitalBankConsoleApp.cpp
// Copyright (c) 1999-2017 by Opto 22
//
// This example demonstrates use of the OptoSnapIoMemMap C++ class for 
// communicating to an Opto 22 SNAP Ethernet I/O unit 
//-----------------------------------------------------------------------------

#include <stdio.h>
#include "O22SIOMM.h"

int main( int argc, char **argv)
{
  O22SnapIoMemMap        Brain;
  SIOMM_DigBankReadArea  DigBankData;
  long                   nResult;

  // this app originally worked with the SNAP-PAC family of I/O
  // it is modified to operate with the EPIC platform.
  fprintf(stderr, "dbankcon example application for EPIC\n");
  // Make sure there's at least one command-line parameter.  
  if (argc < 2)
  {
    fprintf(stderr, "Must have IP address in command line.\n\nExample: dbankcon.exe 10.192.54.0\n");
    return 1;
  }

  // Assume that the command-line parameter is a valid IP address.
  // Use port 2001 and a timeout of 10 seconds. Set for TCP use
  // 2 for UDP.
  nResult = Brain.OpenEnet2(argv[1], 2001, 10000, 1, 1);

  // Check the result of the open function
  if (SIOMM_OK == nResult)
  {
    // Keep calling IsOpenDone() until we connect or error
    nResult = Brain.IsOpenDone();
    while (SIOMM_ERROR_NOT_CONNECTED_YET == nResult)
    {
      nResult = Brain.IsOpenDone();
    }

    // Check the final result from IsOpenDone()
    if (SIOMM_OK != nResult)
    {
      fprintf(stderr, "Error %ld while connecting to Ethernet I/O brain.\n", nResult);
      return 1;
    }
  } else
  {
    fprintf(stderr, "Error %ld while connecting to Ethernet I/O brain.\n", nResult);
  }

  // Read the digital bank area
  nResult = Brain.GetDigBankReadAreaEx(&DigBankData);

  // Check the result
  if (SIOMM_OK == nResult)
  {
    // If everything is okay, then print the state of the digital bank read area.
    printf("\n  DIGITAL BANK READ AREA\n");
    printf("    Point States:     0x%08X%08X\n", DigBankData.nStatePts63to32, DigBankData.nStatePts31to0);
    printf("    On-Latch States:  0x%08X%08X\n", DigBankData.nOnLatchStatePts63to32, DigBankData.nOnLatchStatePts31to0);
    printf("    Off-Latch States: 0x%08X%08X\n", DigBankData.nOffLatchStatePts63to32, DigBankData.nOffLatchStatePts31to0);
  }
  else
  {
    // Print error message and exit.
    fprintf(stderr, "Unable to read digital bank data.\n");
    Brain.Close();
    return 1;
  }

  // Close the connection to the brain
  Brain.Close();

  return 0;
}

