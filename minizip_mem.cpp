#include "minizip_mem.h"

voidpf ZCALLBACK ZipOpen(voidpf opaque, const char*, int)
{
    if (!opaque) return nullptr;

    auto* mf = static_cast<ZipMemoryStreamFile*>(opaque);
    mf->pos = 0;

    return mf;
}

uLong ZCALLBACK ZipRead(voidpf, voidpf stream, void* buf, uLong size)
{
    if (!stream || !buf || size == 0) return 0;

    auto* mf = static_cast<ZipMemoryStreamFile*>(stream);

    size_t remain = mf->size - mf->pos;

    if (size > remain) 
    {
        size = static_cast<uLong>(remain);
    }  

    memcpy(buf, (unsigned char*)mf->data + mf->pos, size);

    mf->pos += size;

    return size;
}

long ZCALLBACK ZipTell(voidpf, voidpf stream)
{
    if (!stream) return -1;

    auto* mf = static_cast<ZipMemoryStreamFile*>(stream);

    return static_cast<long>(mf->pos);
}

long ZCALLBACK ZipSeek(voidpf, voidpf stream, uLong offset, int origin)
{
    if (!stream) return -1;
    auto* mf = static_cast<ZipMemoryStreamFile*>(stream);

    long new_pos = 0;

    switch (origin)
    {
        case SEEK_SET: new_pos = static_cast<long>(offset); break;
        case SEEK_CUR: new_pos = static_cast<long>(mf->pos) + static_cast<long>(offset); break;
        case SEEK_END: new_pos = static_cast<long>(mf->size) + static_cast<long>(offset); break;
        default: return -1;
    }

    if (new_pos < 0 || new_pos > static_cast<long>(mf->size))
        return -1;

    mf->pos = static_cast<size_t>(new_pos);
    return 0;
}

uLong ZCALLBACK ZipWrite(voidpf, voidpf, const void*, uLong)
{
    return static_cast<uLong>(-1);
}

int ZCALLBACK ZipClose(voidpf, voidpf)
{
    return 0;
}

int ZCALLBACK ZipError(voidpf, voidpf)
{
    return 0;
}

inline void fill_memory_file_funcs(zlib_filefunc_def* funcs, voidpf opaque)
{
    funcs->zopen_file = ZipOpen;
    funcs->zread_file = ZipRead;
    funcs->ztell_file = ZipTell;
    funcs->zseek_file = ZipSeek;
    funcs->zwrite_file = ZipWrite;
    funcs->zclose_file = ZipClose;
    funcs->zerror_file = ZipError;
    funcs->opaque = opaque;
}

inline unzFile unzOpenMemoryFile(ZipMemoryStreamFile& memStream, const void* data, size_t len)
{
    if (!data || len == 0) return nullptr;

    zlib_filefunc_def memFuncs{};
    
    memStream.data = static_cast<const unsigned char*>(data);
    memStream.size = len;
    memStream.pos = 0;

    fill_memory_file_funcs(&memFuncs, &memStream);

    unzFile uf = unzOpen2(nullptr, &memFuncs);

    if (!uf)
    {
        return nullptr;
    }

    return uf;
}
