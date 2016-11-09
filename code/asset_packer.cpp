/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Christopher LaBauve $
   $Notice: $
   ======================================================================== */

#include "q3_platform.h"
#include "q3_intrinsics.h"
#include "q3_assets.h"
#include <windows.h>
#include <stdio.h>

#define ASSET_PATH "w:\\q3\\data\\"
#define OUT_PATH "assets.q3d"

struct named_tile_file
{
    tile_type Type;
    char *FileName;
};

inline u8
GrayToColorIndex(u32 Gray)
{
    u8 Result = 0;
    switch(Gray)
    {
        case 0xFF:
        {
            Result = 3;
        } break;

        case 0xC0:
        {
            Result = 2;
        } break;

        case 0x80:
        {
            Result = 1;
        } break;

        case 0x00:
        {
            Result = 0;
        } break;

        InvalidDefaultCase;
    }

    return(Result);
}

int main(int ArgCount, char **Args)
{
    b32 ChangeDirectorySucceeded = SetCurrentDirectory(ASSET_PATH);
    Assert(ChangeDirectorySucceeded);

    FILE *OutFile;
    errno_t OutFileOpenResult = fopen_s(&OutFile, OUT_PATH, "wb");
    Assert(OutFileOpenResult == 0);

    named_tile_file NamedTileFiles[] =
    {
        {TileType_Rock, "rock.bmp"},
        {TileType_RockCorner1, "rockc1.bmp"},
        {TileType_RockCorner2, "rockc2.bmp"},
        {TileType_RockCorner3, "rockc3.bmp"},
        {TileType_RockCorner4, "rockc4.bmp"},
        {TileType_RockFace, "rockface.bmp"},
        {TileType_RockTop, "rocktop.bmp"},
        {TileType_Solid0, "solid0.bmp"},
        {TileType_Solid1, "solid1.bmp"},
        {TileType_Solid2, "solid2.bmp"},
        {TileType_Solid3, "solid3.bmp"},
        {TileType_Tree, "tree.bmp"},
        {TileType_TreeBottomLeft, "treebl.bmp"},
        {TileType_TreeBottomRight, "treebr.bmp"},
        {TileType_TreeFace, "treeface.bmp"},
        {TileType_TreeTopLeft, "treetl.bmp"},
        {TileType_TreeTopRight, "treetr.bmp"},
        {TileType_Water, "water.bmp"},
        {TileType_ShoreBottom, "shoreb.bmp"},
        {TileType_ShoreBottomRight, "shorebr.bmp"},
        {TileType_ShoreRight, "shorer.bmp"},
        {TileType_ShoreTopLeft, "shoretl.bmp"},
        {TileType_ShoreTopRight, "shoretr.bmp"},
        {TileType_Bridge, "bridge.bmp"},

        {TileType_ThiefFront0, "linkf0.bmp"},
        {TileType_ThiefFront1, "linkf1.bmp"},
        {TileType_ThiefFrontStab, "linkfs.bmp"},
        {TileType_ThiefBack0, "linkb0.bmp"},
        {TileType_ThiefBack1, "linkb1.bmp"},
        {TileType_ThiefBackStab, "linkbs.bmp"},
        {TileType_ThiefLeft0, "linkl0.bmp"},
        {TileType_ThiefLeft1, "linkl1.bmp"},
        {TileType_ThiefLeftStab, "linkls.bmp"},
        {TileType_ThiefRight0, "linkr0.bmp"},
        {TileType_ThiefRight1, "linkr1.bmp"},
        {TileType_ThiefRightStab, "linkrs.bmp"},
        {TileType_ThiefGrab0, "linkg0.bmp"},
        {TileType_ThiefGrab1, "linkg1.bmp"},

        {TileType_KnightFront0, "darknutf0.bmp"},
        {TileType_KnightFront1, "darknutf1.bmp"},
        {TileType_KnightBack0, "darknutb0.bmp"},
        {TileType_KnightBack1, "darknutb1.bmp"},
        {TileType_KnightLeft0, "darknutl0.bmp"},
        {TileType_KnightLeft1, "darknutl1.bmp"},
        {TileType_KnightRight0, "darknutr0.bmp"},
        {TileType_KnightRight1, "darknutr1.bmp"},

        {TileType_MagicianBack0, "wizrobeb0.bmp"},
        {TileType_MagicianBack1, "wizrobeb1.bmp"},
        {TileType_MagicianLeft0, "wizrobel0.bmp"},
        {TileType_MagicianLeft1, "wizrobel1.bmp"},
        {TileType_MagicianRight0, "wizrober0.bmp"},
        {TileType_MagicianRight1, "wizrober1.bmp"},

        {TileType_DemonFront0, "demonf0.bmp"},
        {TileType_DemonFront1, "demonf1.bmp"},
        {TileType_DemonBack0, "demonb0.bmp"},
        {TileType_DemonBack1, "demonb1.bmp"},
        {TileType_DemonLeft0, "demonl0.bmp"},
        {TileType_DemonLeft1, "demonl1.bmp"},
        {TileType_DemonRight0, "demonr0.bmp"},
        {TileType_DemonRight1, "demonr1.bmp"},

        {TileType_GuardFront0, "guardf0.bmp"},
        {TileType_GuardFront1, "guardf1.bmp"},
        {TileType_GuardBack0, "guardb0.bmp"},
        {TileType_GuardBack1, "guardb1.bmp"},
        {TileType_GuardLeft0, "guardl0.bmp"},
        {TileType_GuardLeft1, "guardl1.bmp"},
        {TileType_GuardRight0, "guardr0.bmp"},
        {TileType_GuardRight1, "guardr1.bmp"},

        {TileType_CaptainFront0, "captainf0.bmp"},
        {TileType_CaptainFront1, "captainf1.bmp"},
        {TileType_CaptainBack0, "captainb0.bmp"},
        {TileType_CaptainBack1, "captainb1.bmp"},
        {TileType_CaptainLeft0, "captainl0.bmp"},
        {TileType_CaptainLeft1, "captainl1.bmp"},
        {TileType_CaptainRight0, "captainr0.bmp"},
        {TileType_CaptainRight1, "captainr1.bmp"},
    };

    loaded_tile Tiles[ArrayCount(NamedTileFiles)];
    u32 TileCount = ArrayCount(NamedTileFiles);

    u8 Buffer[4096];
    size_t BytesRead;
    u8 *Memory;
    u32 Pitch = TILE_WIDTH*sizeof(u32);
    for(s32 TileIndex = 0;
        TileIndex < ArrayCount(NamedTileFiles);
        ++TileIndex)
    {
        named_tile_file *NamedTileFile = NamedTileFiles + TileIndex;
        Assert(TileIndex == NamedTileFile->Type);
        FILE *TileFile;
        errno_t TileFileOpenResult = fopen_s(&TileFile, NamedTileFile->FileName, "rb");
        Assert(TileFileOpenResult == 0);

        BytesRead = fread(Buffer, 1, sizeof(Buffer), TileFile);
        
        Assert(BytesRead == 1162);

        bitmap_header *BMPHeader = (bitmap_header *)Buffer;

        Assert(BMPHeader->FileType == (('B' << 0) | ('M' << 8)));
        Assert(BMPHeader->Reserved1 == 0);
        Assert(BMPHeader->Reserved2 == 0);
        Assert(BMPHeader->Planes == 1);
        Assert(BMPHeader->BitsPerPixel == 32);
        Assert(BMPHeader->Compression == 3);
        Assert(BMPHeader->ColorsUsed == 0);
        Assert(BMPHeader->ColorsImportant == 0);

        Assert(BMPHeader->Height == TILE_HEIGHT);
        Assert(BMPHeader->Width == TILE_WIDTH);
        Memory = (u8 *)BMPHeader + BMPHeader->BitmapOffset;

        u32 RedShift = BitScanForward(BMPHeader->RedMask).Index;

        u8 *PixelRow = (u8 *)Memory;
        loaded_tile *Tile = Tiles + TileIndex;
        u8 *OutByte = Tile->ColorIndices;
        r32 Inv255 = 1.0f / 255.0f;
        for(s32 Y = 0;
            Y < TILE_HEIGHT;
            ++Y)
        {
            u32 *Pixel = ((u32 *)PixelRow);
            for(s32 X = 0;
                X < TILE_WIDTH;
                X += 4)
            {
                u32 Gray0 = ((*(Pixel + 0) >> RedShift) & 0xFF);
                u32 Gray1 = ((*(Pixel + 1) >> RedShift) & 0xFF);
                u32 Gray2 = ((*(Pixel + 2) >> RedShift) & 0xFF);
                u32 Gray3 = ((*(Pixel + 3) >> RedShift) & 0xFF);

                *OutByte++ = ((GrayToColorIndex(Gray0) << 0) |
                              (GrayToColorIndex(Gray1) << 2) |
                              (GrayToColorIndex(Gray2) << 4) |
                              (GrayToColorIndex(Gray3) << 6));
                Pixel += 4;
            }
            PixelRow += Pitch;
        }
    }

    u32 AssetSize = 0;
    size_t ElementsWritten;

    ElementsWritten = fwrite(&TileCount, sizeof(u32), 1, OutFile);
    Assert(ElementsWritten == 1);
    AssetSize += sizeof(u32)*1;

    ElementsWritten = fwrite(Tiles, sizeof(loaded_tile), TileCount, OutFile);
    Assert(ElementsWritten == TileCount);
    AssetSize += sizeof(loaded_tile)*TileCount;
    
    fwrite(&AssetSize, sizeof(u32), 1, OutFile);
    
    return 0;
}
