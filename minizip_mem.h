#pragma once
#include "windows.h"

#include "minizip/unzip.h"

#ifdef _MINIZIP_MEM_SUPPORT

typedef struct
{
	void* data;
	size_t size;
	size_t pos;

} ZipMemoryStreamFile;

extern ZEXPORT unzFile unzOpenMemoryFile(ZipMemoryStreamFile& memFileStruct, const void* data, size_t len);

#endif

