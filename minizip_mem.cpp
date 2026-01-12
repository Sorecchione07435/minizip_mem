#include "minizip_mem.h"

voidpf ZCALLBACK ZipOpen(voidpf opaque, LPCSTR, int)
{
	if (!opaque) return 0;

	auto* mf = (ZipMemoryStreamFile*)opaque;

	mf->pos = 0;
	return mf;
}

uLong ZCALLBACK ZipRead(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	if (!stream || !buf || size == 0) return 0;
	
	auto* mf = (ZipMemoryStreamFile*)stream;

	if (mf->pos + size > mf->size)
	{
		size = mf->size - mf->pos;
	}

	memcpy(buf, (unsigned char*)mf->data + mf->pos, size);

	mf->pos += size;

	return size;
}

long ZCALLBACK ZipTell(voidpf opaque, voidpf stream)
{
	if (!stream) return -1;

	auto* mf = (ZipMemoryStreamFile*)stream;
	return (long)mf->pos;
}

long ZCALLBACK ZipSeek(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	if (!stream) return -1;

	auto* mf = (ZipMemoryStreamFile*)stream;

	size_t new_pos = 0;

	switch (origin)
	{
		case SEEK_SET: new_pos = offset; break;
		case SEEK_CUR: new_pos = mf->pos + offset; break;
		case SEEK_END: new_pos = mf->size + offset; break;

		default: return -1;
	}

	if (new_pos > mf->size)
	{
		return -1;
	}

	mf->pos = new_pos;
	return 0;
}

uLong ZCALLBACK ZipWrite(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	return (uLong)-1;
}

int ZCALLBACK ZipClose(voidpf opaque, voidpf stream)
{
	return 0;
}

int ZCALLBACK ZipError(voidpf opaque, voidpf stream)
{
	return 0;
}

void fill_memory_file_funcs(zlib_filefunc_def* pzlib_filefunc_def, voidpf opaque)
{
	pzlib_filefunc_def->zopen_file = ZipOpen;
	pzlib_filefunc_def->zread_file = ZipRead;
	pzlib_filefunc_def->ztell_file = ZipTell;
	pzlib_filefunc_def->zseek_file = ZipSeek;
	pzlib_filefunc_def->zwrite_file = ZipWrite;
	pzlib_filefunc_def->zclose_file = ZipClose;
	pzlib_filefunc_def->zerror_file = ZipError;
	pzlib_filefunc_def->opaque = opaque;
}

unzFile unzOpenMemoryFile(ZipMemoryStreamFile& memoryStreamFile, const void* data, size_t len)
{
	if (!data || len == 0) return nullptr;

	zlib_filefunc_def memFuncs = {};

	memoryStreamFile.data = (void*)data;
	memoryStreamFile.size = len;
	memoryStreamFile.pos = 0;

	fill_memory_file_funcs(&memFuncs, &memoryStreamFile);

	return unzOpen2("file.zip", &memFuncs);
}


