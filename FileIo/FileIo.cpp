// FileIo.cpp : Defines the functions for the static library.
//

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <atlstr.h>
#include <strsafe.h>
#include "pch.h"
#include "framework.h"
#include "trace.h"
#include "fileio.h"

// TODO: This is an example of a library function
void fnFileIo()
{
}

bool JaReadFile(char *fileName, unsigned char *data)
{
   FILE* file;
   int count;
   char fileStr[80];

   sprintf(fileStr, "Open %s", fileName);

   TRACE((char *)fileStr);

   file = fopen(fileName, "rb");

   if (file != NULL) {
      count = fread(data, 1, sizeof(data), file);
      fclose(file);
   }
   else {
      sprintf(fileStr, "Unable to open %s!", fileName);
      TRACE(fileStr);
      return false;
   }

   if (count != sizeof(data)) {
      sprintf(fileStr, "Unable to read %s!", fileName);
      TRACE(fileStr);
      return false;
   }

   return true;
}