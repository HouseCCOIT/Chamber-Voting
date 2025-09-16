//-----------------------------------------------------------------------------
//
// O22SIOUT.cpp
// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
// Copyright (c) 1999 - 2018 by Opto 22
//
// Utility functions for the OptoMMP Communication Toolkit C++ classes.  
//
// While this code was developed on Microsoft Windows targeting x86 operating 
// systems, it is intended to be as generic as possible.  For Windows specific
// code, search for "_WIN32".  For Linux specific code, search for "_LINUX".
//-----------------------------------------------------------------------------

#ifndef __O22SIOUT_H
#include "O22SIOUT.h"
#endif


float O22MAKEFLOAT2(uint8_t * a, int o)
//-------------------------------------------------------------------------------------------------
// This is similar to the O22MAKELONG2 #define, but it had to be put in a function.
//-------------------------------------------------------------------------------------------------
{
  float f;
  int  i = O22MAKELONG2(a, o); 
  
  memcpy(&f, &i, 4);
  
  return f;
}

void O22FILL_ARRAY_FROM_FLOATX(uint8_t * a, int i, float f)
{
  uint32_t dwt; 
  memcpy(&dwt, &f, 4); 
  O22FILL_ARRAY_FROM_INT32(a, i, dwt);
}


void O22FILL_ARRAY_FROM_FLOAT_LIT(uint8_t * a, int i, float f)
{
  uint32_t dwt; 
  memcpy(&dwt, &f, 4); 
  O22FILL_ARRAY_FROM_INT32(a, i, dwt);
}

uint16_t Crc16R(const unsigned char  *pu8aryStr, uint32_t dwLength)
{
  register uint16_t                  wCrc;
  register uint32_t                 dwIndex;
  register const unsigned char  *pu8aryPtr;
  register uint32_t                 dwCounter;

  pu8aryPtr   = pu8aryStr;
  wCrc        = 0;
  dwCounter   = dwLength;

  while(dwCounter) 
  {
    dwCounter--;
    dwIndex = (wCrc ^ *pu8aryPtr++) & 0xFF;
    wCrc   = (wCrc >> 8) ^ *(waryCrc16r_Table + dwIndex);
  }

  return wCrc;
}

// Helpers to get the memory addresses for serial passthru
unsigned int GetPassThruConfigAddress(int port)
{
  switch (port)
  {
  case 0: return MEMORY_SERIALPASSTHRU_CFG0;
  case 1: return MEMORY_SERIALPASSTHRU_CFG1;
  case 2: return MEMORY_SERIALPASSTHRU_CFG2;
  case 3: return MEMORY_SERIALPASSTHRU_CFG3;
  case 4: return MEMORY_SERIALPASSTHRU_CFG4;
  case 5: return MEMORY_SERIALPASSTHRU_CFG5;
  default: return MEMORY_SERIALPASSTHRU_CFG0;
  }
}

unsigned int GetPassThruReadWriteAddress(int port)
{
  switch (port)
  {
  case 0: return MEMORY_SERIALPASSTHRU_RW0;
  case 1: return MEMORY_SERIALPASSTHRU_RW1;
  case 2: return MEMORY_SERIALPASSTHRU_RW2;
  case 3: return MEMORY_SERIALPASSTHRU_RW3;
  case 4: return MEMORY_SERIALPASSTHRU_RW4;
  case 5: return MEMORY_SERIALPASSTHRU_RW5;
  default: return MEMORY_SERIALPASSTHRU_RW0;
  }
}
