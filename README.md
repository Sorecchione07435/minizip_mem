
# minizip_mem

A small static library for minizip that adds support for extracting ZIP files directly into memory, bypassing the file system.

This is ideal for applications that need high-performance, diskless extraction, or embedding archives in executables.

## Features

-   Extract ZIP files directly from memory buffers
-   No temporary files required
-   Compatible with x86 and x64 architectures
-   Simple API: `unzOpenMemoryFile()`

## Installation

1.  Include the library header in your project:

```c
#include "minizip_mem.h"

```

2.  Link the precompiled `.lib` file matching your architecture (`x86` or `x64`).
    
3.  Define the following preprocessor constant to enable memory support:
    

```cpp
#define _MINIZIP_MEM_SUPPORT

```

## Usage Example

```cpp
#include "windows.h"

#include "minizip/unzip.h"
#include "minizip/zip.h"

#include "minizip_mem.h" // <-- minizip_mem header

bool ReadZipFromMemory(const BYTE* archiveData, size_t archiveSize)
{
    // Initialize memory stream structure
    ZipMemoryStreamFile memFileStream = {};

    // Open the ZIP archive directly from memory
    unzFile memZipFile = unzOpenMemoryFile(memFileStream, archiveData, archiveSize);
    if (!memZipFile)
    {
        // Failed to open archive
        return false;
    }

    // Go to the first file in the archive
    if (unzGoToFirstFile(memZipFile) != UNZ_OK)
    {
        unzClose(memZipFile);
        return false;
    }

    // Example: read the first file into memory
    char buffer[1024];
    int bytesRead = unzReadCurrentFile(memZipFile, buffer, sizeof(buffer));
    if (bytesRead < 0)
    {
        unzClose(memZipFile);
        return false;
    }

    // Close the archive when done
    unzClose(memZipFile);
    return true;
}

```

### ⚠️ Note
 - `ZipMemoryStreamFile` keeps track of the memory buffer, its total size, and the current read/write position.  
 - For large files, consider allocating a dynamic buffer or reading in chunks until the file is completely extracted, unless the entire archive is already mapped to virtual memory.  
 - This example reads only the first file; to process multiple files, loop using `unzGoToNextFile()`.

## Advantages of Memory Extraction

-   **Performance**: Avoids disk I/O for faster extraction.
-   **Security**: Data never touches disk, reducing the risk of exposure.
-   **Flexible Data Sources**: Works with embedded resources, network streams, or custom buffers.
-   **Cleaner Resource Management**: No temporary files, fewer permissions issues.

## References

-   [minizip GitHub Repository](https://github.com/madler/zlib/tree/master/contrib/minizip)
-   [zlib Official Documentation](https://zlib.net/)
