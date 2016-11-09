#if !defined(Q3_ASSETS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Christopher LaBauve $
   $Notice: $
   ======================================================================== */

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

enum tile_type
{
    TileType_Rock,
    TileType_RockCorner1,
    TileType_RockCorner2,
    TileType_RockCorner3,
    TileType_RockCorner4,
    TileType_RockFace,
    TileType_RockTop,
    TileType_Solid0,
    TileType_Solid1,
    TileType_Solid2,
    TileType_Solid3,
    TileType_Tree,
    TileType_TreeBottomLeft,
    TileType_TreeBottomRight,
    TileType_TreeFace,
    TileType_TreeTopLeft,
    TileType_TreeTopRight,

    TileType_LinkFront0,
    TileType_LinkFront1,
    TileType_LinkFrontStab,
    TileType_LinkBack0,
    TileType_LinkBack1,
    TileType_LinkBackStab,
    TileType_LinkLeft0,
    TileType_LinkLeft1,
    TileType_LinkLeftStab,
    TileType_LinkRight0,
    TileType_LinkRight1,
    TileType_LinkRightStab,
    TileType_LinkGrab0,
    TileType_LinkGrab1,

    TileType_DarknutFront0,
    TileType_DarknutFront1,
    TileType_DarknutBack0,
    TileType_DarknutBack1,
    TileType_DarknutLeft0,
    TileType_DarknutLeft1,
    TileType_DarknutRight0,
    TileType_DarknutRight1,
};

struct loaded_tile
{
    // NOTE(chris): A tile is 16x16 indices between 0 and 3. So 256*2/8 = 64 bytes.
    u8 ColorIndices[64];
};

struct palette
{
    u32 Colors[4];
};

struct assets
{
    u32 TileCount;
    loaded_tile *Tiles;
};

#define Q3_ASSETS_H
#endif
