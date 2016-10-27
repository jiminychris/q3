/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Christopher LaBauve $
   $Notice: $
   ======================================================================== */

#include "q3_platform.h"
#include <stdio.h>

#define OUT_PATH "w:\\build\\q3.exe"
#define EXE_PATH "w:\\build\\win32_q3.exe"
#define ASSET_PATH "w:\\q3\\data\\"

int main(int ArgCount, char **Args)
{
    FILE *EXEFile, *OutFile;
    errno_t EXEFileOpenResult = fopen_s(&EXEFile, EXE_PATH, "rb");
    errno_t OutFileOpenResult = fopen_s(&OutFile, OUT_PATH, "wb");
    // TODO(chris): If there are more assets, this needs to be more systematic.

    Assert(EXEFileOpenResult == 0);
    Assert(OutFileOpenResult == 0);

    char Buffer[4096];

    size_t BytesRead;
    do
    {
        BytesRead = fread(Buffer, 1, sizeof(Buffer), EXEFile);
        fwrite(Buffer, 1, BytesRead, OutFile);
    } while(BytesRead);
    u32 AssetSize = 0;
#if 0
    FILE *LaserFile;
    errno_t LaserFileOpenResult = fopen_s(&LaserFile, ASSET_PATH"laser.bmp", "rb");
    Assert(LaserFileOpenResult == 0);
    do
    {
        BytesRead = fread(Buffer, 1, sizeof(Buffer), LaserFile);
        fwrite(Buffer, 1, BytesRead, OutFile);
    } while(BytesRead);
    AssetSize += ftell(LaserFile);
#endif
    fwrite(&AssetSize, sizeof(u32), 1, OutFile);
    
    return 0;
}
