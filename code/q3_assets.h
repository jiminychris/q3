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
    TileType_Water,
    TileType_ShoreBottom,
    TileType_ShoreBottomRight,
    TileType_ShoreRight,
    TileType_ShoreTopLeft,
    TileType_ShoreTopRight,
    TileType_Bridge,

    TileType_ThiefFront0,
    TileType_ThiefFront1,
    TileType_ThiefFrontStab,
    TileType_ThiefBack0,
    TileType_ThiefBack1,
    TileType_ThiefBackStab,
    TileType_ThiefLeft0,
    TileType_ThiefLeft1,
    TileType_ThiefLeftStab,
    TileType_ThiefRight0,
    TileType_ThiefRight1,
    TileType_ThiefRightStab,
    TileType_ThiefGrab0,
    TileType_ThiefGrab1,

    TileType_KnightFront0,
    TileType_KnightFront1,
    TileType_KnightBack0,
    TileType_KnightBack1,
    TileType_KnightLeft0,
    TileType_KnightLeft1,
    TileType_KnightRight0,
    TileType_KnightRight1,

    TileType_MagicianBack0,
    TileType_MagicianBack1,
    TileType_MagicianLeft0,
    TileType_MagicianLeft1,
    TileType_MagicianRight0,
    TileType_MagicianRight1,

    TileType_DemonFront0,
    TileType_DemonFront1,
    TileType_DemonBack0,
    TileType_DemonBack1,
    TileType_DemonLeft0,
    TileType_DemonLeft1,
    TileType_DemonRight0,
    TileType_DemonRight1,

    TileType_GuardFront0,
    TileType_GuardFront1,
    TileType_GuardBack0,
    TileType_GuardBack1,
    TileType_GuardLeft0,
    TileType_GuardLeft1,
    TileType_GuardRight0,
    TileType_GuardRight1,

    TileType_CaptainFront0,
    TileType_CaptainFront1,
    TileType_CaptainBack0,
    TileType_CaptainBack1,
    TileType_CaptainLeft0,
    TileType_CaptainLeft1,
    TileType_CaptainRight0,
    TileType_CaptainRight1,
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
