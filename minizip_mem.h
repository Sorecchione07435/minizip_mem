#pragma once
#include "windows.h"
#include "minizip/unzip.h"
#include "minizip/zip.h"

#ifdef _MINIZIP_MEM_SUPPORT

typedef struct
{
	void* data;
	size_t size;
	size_t pos;

} ZipMemoryStreamFile;

extern unzFile unzOpenMemoryFile(ZipMemoryStreamFile& memFileStruct, const void* data, size_t len);

#endif


