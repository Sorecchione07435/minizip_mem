#include "windows.h"*
#include "minizip_mem.h"

voidpf ZCALLBACK ZipOpen(voidpf opaque, LPCSTR fileName, int mode)
{
	ZipMemoryStreamFile* p = (ZipMemoryStreamFile*)opaque;
	p->pos = 0;
	return p;
}

uLong ZCALLBACK ZipRead(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	ZipMemoryStreamFile* p = (ZipMemoryStreamFile*)stream;

	if (p->pos + size > p->size)
	{
		size = p->size - p->pos;
	}

	memcpy(buf, (const unsigned char*)p->data + p->pos, size);

	p->pos += size;

	return size;
}

uLong ZCALLBACK ZipWrite(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	ZipMemoryStreamFile* p = (ZipMemoryStreamFile*)stream;

	if (p->pos + size > p->size)
	{
		return 0;
	}

	const void* dest = (const unsigned char*)p->data + p->pos;
	memcpy((void*)dest, buf, size);

	p->pos += size;

	if (p->pos > p->size)
	{
		p->size = p->pos;
	}

	return size;
}

long ZCALLBACK ZipTell(voidpf opaque, voidpf stream)
{
	ZipMemoryStreamFile* p = (ZipMemoryStreamFile*)stream;

	return (long)p->pos;
}

long ZCALLBACK ZipSeek(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	ZipMemoryStreamFile* p = (ZipMemoryStreamFile*)stream;

	size_t new_pos = 0;

	switch (origin)
	{
	case SEEK_SET: new_pos = offset; break;
	case SEEK_CUR: new_pos = p->pos + offset; break;
	case SEEK_END: new_pos = p->size + offset; break;
	default: return -1;
	}

	if (new_pos > p->size)
	{
		return -1;
	}

	p->pos = new_pos;
	return 0;
}

int ZCALLBACK ZipClose(voidpf opaque, voidpf stream)
{
	return 0;
}

int ZCALLBACK ZipError(voidpf opaque, voidpf stream)
{
	return 0;
}

void fill_memory_file_func(zlib_filefunc_def* pzlib_filefunc_def, voidpf opaque)
{
	pzlib_filefunc_def->zopen_file = ZipOpen;
	pzlib_filefunc_def->zread_file = ZipRead;
	pzlib_filefunc_def->zwrite_file = ZipWrite;
	pzlib_filefunc_def->ztell_file = ZipTell;
	pzlib_filefunc_def->zseek_file = ZipSeek;
	pzlib_filefunc_def->zclose_file = ZipClose;
	pzlib_filefunc_def->zerror_file = ZipError;
	pzlib_filefunc_def->opaque = opaque;
}

unzFile unzOpenMemoryFile(ZipMemoryStreamFile& memFileStruct, const void* data, size_t len)
{
	zlib_filefunc_def memFunc;

	memFileStruct.data = data;
	memFileStruct.size = len;
	memFileStruct.pos = 0;

	fill_memory_file_func(&memFunc, &memFileStruct);

	unzFile unzMemFile = unzOpen2("file.zip", &memFunc);
	return unzMemFile;
}