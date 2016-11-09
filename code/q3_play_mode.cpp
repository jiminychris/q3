/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Christopher LaBauve $
   $Notice: $
   ======================================================================== */

inline collision_shape
CollisionTriangle(v2 PointA, v2 PointB, v2 PointC)
{
    collision_shape Result;
    Result.Type = CollisionShapeType_Triangle;
    Result.A = PointA;
    Result.B = PointB;
    Result.C = PointC;
    return(Result);
}

// TODO(chris): Allow a special shape for triangle strips?
#define CollisionTriangleStrip(Name, Strip)                             \
    collision_shape Name[ArrayCount(Strip)-2];                          \
    for(u32 i=0;i<ArrayCount(Name);++i)                                 \
        Name[i]=(i&1)?CollisionTriangle(Strip[i],Strip[i+2],Strip[i+1]) : \
            CollisionTriangle(Strip[i],Strip[i+1],Strip[i+2]);

// TODO(chris): Allow a special shape for triangle fans?
#define CollisionTriangleFan(Name, Fan)                             \
    collision_shape Name[ArrayCount(Fan)-1];                        \
    Name[0]=CollisionTriangle(Fan[0],Fan[ArrayCount(Name)],Fan[1]); \
    for(u32 i=1;i<ArrayCount(Name);++i)                             \
        Name[i]=CollisionTriangle(Fan[0],Fan[i],Fan[i+1]);          \

#define CombineShapes(Name, Shapes1, Shapes2)                           \
    collision_shape Name[ArrayCount(Shapes1)+ArrayCount(Shapes2)];      \
    for(u32 i=0;i<ArrayCount(Shapes1);++i)Name[i]=Shapes1[i];           \
    for(u32 i=0;i<ArrayCount(Shapes2);++i)Name[ArrayCount(Shapes1)+i]=Shapes2[i];

// TODO(chris): Allow a special shape for rectangles?
#define CollisionRectangle(Rect) \
    CollisionTriangle((Rect).Min, V2((Rect).Max.x, (Rect).Min.y), (Rect).Max), \
    CollisionTriangle((Rect).Max, V2((Rect).Min.x, (Rect).Max.y), (Rect).Min)

inline collision_shape
CollisionCircle(r32 Radius, v2 Center = V2(0, 0))
{
    collision_shape Result;
    Result.Type = CollisionShapeType_Circle;
    Result.Center = Center;
    Result.Radius = Radius;
    return(Result);
}

inline collision_shape
CollisionCircleDiameter(r32 Diameter, v2 Center = V2(0, 0))
{
    collision_shape Result = CollisionCircle(0.5f*Diameter, Center);
}

inline entity *
CreateGuard(play_state *State, v2 P, facing_direction Facing)
{
    entity *Result = CreateEntity(State);
    Result->BrainType = BrainType_Player;
    Result->RenderType = EntityRenderType_Guard;
    Result->ColliderType = ColliderType_None;
    Result->P = P;
    Result->Facing = Facing;

    return(Result);
}

inline entity *
CreateCaptain(play_state *State, v2 P, facing_direction Facing)
{
    entity *Result = CreateEntity(State);
    Result->BrainType = BrainType_None;
    Result->RenderType = EntityRenderType_Captain;
    Result->ColliderType = ColliderType_None;
    Result->P = P;
    Result->Facing = Facing;

    return(Result);
}

internal void
PlayMode(game_memory *GameMemory, game_input *Input, renderer_state *RendererState)
{
    game_state *GameState = (game_state *)GameMemory->PermanentMemory;
    transient_state *TranState = (transient_state *)GameMemory->TemporaryMemory;
    play_state *State = &GameState->PlayState;

    render_buffer *RenderBuffer = &TranState->RenderBuffer;
    RenderBuffer->Projection = RenderBuffer->DefaultProjection;

    if(!State->IsInitialized)
    {
        State->IsInitialized = true;
        RenderBuffer->CameraP = V3(0.0f, 0.0f, 150.0f);

        State->EntityCount = 1;

        CreateGuard(State, V2(0, 0), FacingDirection_Up);
        CreateCaptain(State, V2(-8.0f, 0), FacingDirection_Down);
        State->TestPaletteIndex = 0;

        State->TestColorIndex = 0;
    }

    game_controller *PlayerController = Input->Controllers + Input->MostRecentlyUsedController;
    
    temporary_memory RenderMemory = BeginTemporaryMemory(&RenderBuffer->Arena);
    PushClear(RendererState, RenderBuffer, V3(0.0f, 0.0f, 0.0f));
    v2 ScreenCenter = 0.5f*V2i(RenderBuffer->Width, RenderBuffer->Height);

    u32 NESPalette[] =
    {
        0xFF7C7C7C, 0xFF0000FC, 0xFF0000BC, 0xFF4428BC, 0xFF940084, 0xFFA80020, 0xFFA81000, 0xFF881400,
        0xFF503000, 0xFF007800, 0xFF006800, 0xFF005800, 0xFF004058, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFFBCBCBC, 0xFF0078F8, 0xFF0058F8, 0xFF6844FC, 0xFFD800CC, 0xFFE40058, 0xFFF83800, 0xFFE45C10,
        0xFFAC7C00, 0xFF00B800, 0xFF00A800, 0xFF00A844, 0xFF008888, 0xFF000000, 0xFF000000, 0xFF000000,
        0xFFF8F8F8, 0xFF3CBCFC, 0xFF6888FC, 0xFF9878F8, 0xFFF878F8, 0xFFF85898, 0xFFF87858, 0xFFFCA044,
        0xFFF8B800, 0xFFB8F818, 0xFF58D854, 0xFF58F898, 0xFF00E8D8, 0xFF787878, 0xFF000000, 0xFF000000,
        0xFFFCFCFC, 0xFFA4E4FC, 0xFFB8B8F8, 0xFFD8B8F8, 0xFFF8B8F8, 0xFFF8A4C0, 0xFFF0D0B0, 0xFFFCE0A8,
        0xFFF8D878, 0xFFD8F878, 0xFFB8F8B8, 0xFFB8F8D8, 0xFF00FCFC, 0xFFF8D8F8, 0xFF000000, 0xFF000000,
    };

    for(u32 AttributeByteIndex = 0;
        AttributeByteIndex < ArrayCount(RenderBuffer->AttributeTable);
        ++AttributeByteIndex)
    {
        RenderBuffer->AttributeTable[AttributeByteIndex] = (1 << 6) | (1 << 4) | (1 << 2) | (1 << 0);
//        RenderBuffer->AttributeTable[AttributeByteIndex] = 0;
    }
    RenderBuffer->Palettes[0] =
    {
        NESPalette[63],
        NESPalette[26],
        NESPalette[19],
        NESPalette[55],
    };
    RenderBuffer->Palettes[1] =
    {
        NESPalette[63],
        NESPalette[23],
        NESPalette[19],
        NESPalette[55],
    };
    RenderBuffer->Palettes[2] =
    {
        NESPalette[1],
        NESPalette[2],
        NESPalette[3],
        NESPalette[4],
    };
    RenderBuffer->Palettes[3] =
    {
        NESPalette[5],
        NESPalette[6],
        NESPalette[7],
        NESPalette[8],
    };

    palette ThiefPalettes[] =
    {
        {
            0x00000000,
            NESPalette[13],
            NESPalette[8],
            NESPalette[40],
        },
        {
            0x00000000,
            NESPalette[8],
            NESPalette[11],
            NESPalette[40],
        },
        {
            0x00000000,
            NESPalette[8],
            NESPalette[13],
            NESPalette[40],
        },
    };
    palette KnightPalettes[] =
    {
        {
            0x00000000,
            NESPalette[22],
            NESPalette[39],
            NESPalette[32],
        },
        {
            0x00000000,
            NESPalette[2],
            NESPalette[34],
            NESPalette[32],
        },
    };
    palette GuardPalettes[] =
    {
        {
            0x00000000,
            NESPalette[0],
            NESPalette[40],
            NESPalette[32],
        },
    };
    palette CaptainPalettes[] =
    {
        {
            0x00000000,
            NESPalette[7],
            NESPalette[40],
            NESPalette[32],
        },
    };

    palette_cycle PaletteCycles[256];
    PaletteCycles[EntityRenderType_Thief].Count = ArrayCount(ThiefPalettes);
    PaletteCycles[EntityRenderType_Thief].Palettes = ThiefPalettes;
    PaletteCycles[EntityRenderType_Knight].Count = ArrayCount(KnightPalettes);
    PaletteCycles[EntityRenderType_Knight].Palettes = KnightPalettes;
    PaletteCycles[EntityRenderType_Magician].Count = ArrayCount(KnightPalettes);
    PaletteCycles[EntityRenderType_Magician].Palettes = KnightPalettes;
    PaletteCycles[EntityRenderType_Demon].Count = ArrayCount(KnightPalettes);
    PaletteCycles[EntityRenderType_Demon].Palettes = KnightPalettes;
    PaletteCycles[EntityRenderType_Guard].Count = ArrayCount(GuardPalettes);
    PaletteCycles[EntityRenderType_Guard].Palettes = GuardPalettes;
    PaletteCycles[EntityRenderType_Captain].Count = ArrayCount(CaptainPalettes);
    PaletteCycles[EntityRenderType_Captain].Palettes = CaptainPalettes;

    if(!State->Paused)
    {
        for(u32 EntityIndex = 1;
            EntityIndex < State->EntityCount;
            )
        {
            u32 NextIndex = EntityIndex + 1;
            entity *Entity = State->Entities + EntityIndex;

            switch(Entity->BrainType)
            {
                case BrainType_Player:
                {
                    if(WentDown(PlayerController->ActionUp))
                    {
                        State->TestPaletteIndex = ((State->TestPaletteIndex + 1) %
                                                   PaletteCycles[Entity->RenderType].Count);
                    }

                    if(WentDown(PlayerController->ActionRight))
                    {
                        State->TestColorIndex = (State->TestColorIndex + 1) & (ArrayCount(NESPalette) - 1);
                    }

                    if(WentDown(PlayerController->ActionLeft))
                    {
                        State->TestPaletteIndex = 0;
                        Entity->RenderType = (entity_render_type)((Entity->RenderType + 1) %
                                                                  EntityRenderType_Terminator);
                    }
                    
                    v2 dP = {};
                    facing_direction NewFacing = Entity->Facing;
                    if(PlayerController->LeftStick.y >= 0.5f)
                    {
                        dP.y += 1;
                        NewFacing = FacingDirection_Up;
                    }
                    else if(PlayerController->LeftStick.y <= -0.5f)
                    {
                        dP.y -= 1;
                        NewFacing = FacingDirection_Down;
                    }
                    else if(PlayerController->LeftStick.x >= 0.5f)
                    {
                        dP.x += 1;
                        NewFacing = FacingDirection_Right;
                    }
                    else if(PlayerController->LeftStick.x <= -0.5f)
                    {
                        dP.x -= 1;
                        NewFacing = FacingDirection_Left;
                    }

                    if(Entity->Facing != NewFacing)
                    {
                        Entity->LastFacing = Entity->Facing;
                        Entity->Facing = NewFacing;
                    }
                    if(dP.x != 0.0f || dP.y != 0.0f)
                    {
                        Entity->MoveTimer += Input->dtForFrame;
                        Entity->P += dP;
                    }
                } break;
            }
            
            EntityIndex = NextIndex;
        }
    }

    if(WentDown(PlayerController->Select))
    {
        GameState->NextMode = GameMode_TitleScreen;
    }

    loaded_tile *RockFace = GameState->Assets.Tiles + TileType_RockFace;
    loaded_tile *RockTop = GameState->Assets.Tiles + TileType_RockTop;
    loaded_tile *RockCorner1 = GameState->Assets.Tiles + TileType_RockCorner1;
    loaded_tile *RockCorner3 = GameState->Assets.Tiles + TileType_RockCorner3;
    loaded_tile *RockCorner4 = GameState->Assets.Tiles + TileType_RockCorner4;
    loaded_tile *Ground = GameState->Assets.Tiles + TileType_Solid3;
    loaded_tile *Cave = GameState->Assets.Tiles + TileType_Solid0;
    loaded_tile *Water = GameState->Assets.Tiles + TileType_Water;
    loaded_tile *Bridge = GameState->Assets.Tiles + TileType_Bridge;
    loaded_tile *TreeFace = GameState->Assets.Tiles + TileType_TreeFace;
    loaded_tile *TreeTopLeft = GameState->Assets.Tiles + TileType_TreeTopLeft;
    loaded_tile *TreeTopRight = GameState->Assets.Tiles + TileType_TreeTopRight;
    loaded_tile *TreeBottomLeft = GameState->Assets.Tiles + TileType_TreeBottomLeft;
    loaded_tile *TreeBottomRight = GameState->Assets.Tiles + TileType_TreeBottomRight;

    loaded_tile *Tiles[] =
    {
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        Water, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, Water,
        Water, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, Water,
        Water, RockFace, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockFace, Water,
        Water, RockFace, Ground, Ground, Ground, Ground, TreeTopLeft, TreeFace, TreeTopRight, Ground, Ground, Ground, Ground, Ground, RockFace, Water,
        Water, RockFace, Ground, Ground, Ground, Ground, TreeBottomLeft, Cave, TreeBottomRight, Ground, Ground, Ground, Ground, Ground, RockFace, Water,
        Water, RockFace, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Bridge,
        Water, RockFace, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockTop, Water,
        Water, RockFace, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockFace, Water,
        Water, RockFace, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockFace, Water,
        Water, RockFace, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockFace, Water,
        Water, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, Water,
    };
    PushTiles(RenderBuffer, ArrayCount(Tiles), Tiles);

    for(u32 EntityIndex = 1;
        EntityIndex < State->EntityCount;
        )
    {
        u32 NextIndex = EntityIndex + 1;
        entity *Entity = State->Entities + EntityIndex;

        switch(Entity->RenderType)
        {
            case EntityRenderType_Thief:
            {
                loaded_tile *Tile = 0;
                r32 MoveTimerMax = 0.5f;
                r32 MoveTimerHalf = 0.5f*MoveTimerMax;
                if(Entity->MoveTimer >= MoveTimerMax)
                {
                    Entity->MoveTimer -= MoveTimerMax;
                }
                if(Entity->MoveTimer < MoveTimerHalf)
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefLeft0;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefFront0;
                        } break;
                    }
                }
                else
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefLeft1;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_ThiefFront1;
                        } break;
                    }
                }
                palette_cycle *Cycle = PaletteCycles + Entity->RenderType;
                PushSprite(RenderBuffer, Tile, Entity->P, Cycle->Palettes + State->TestPaletteIndex);
            } break;

            case EntityRenderType_Knight:
            {
                loaded_tile *Tile = 0;
                r32 MoveTimerMax = 0.5f;
                r32 MoveTimerHalf = 0.5f*MoveTimerMax;
                if(Entity->MoveTimer >= MoveTimerMax)
                {
                    Entity->MoveTimer -= MoveTimerMax;
                }
                if(Entity->MoveTimer < MoveTimerHalf)
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightLeft0;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightFront0;
                        } break;
                    }
                }
                else
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightLeft1;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_KnightFront1;
                        } break;
                    }
                }
                palette_cycle *Cycle = PaletteCycles + Entity->RenderType;
                PushSprite(RenderBuffer, Tile, Entity->P, Cycle->Palettes + State->TestPaletteIndex);
            } break;

            case EntityRenderType_Magician:
            {
                loaded_tile *Tile = 0;
                r32 MoveTimerMax = 0.5f;
                r32 MoveTimerHalf = 0.5f*MoveTimerMax;
                if(Entity->MoveTimer >= MoveTimerMax)
                {
                    Entity->MoveTimer -= MoveTimerMax;
                }
                facing_direction Facing = Entity->Facing;
                if(Facing == FacingDirection_Down)
                {
                    Facing = Entity->LastFacing;
                    if(Facing == FacingDirection_Up)
                    {
                        Facing = FacingDirection_Right;
                    }
                }
                if(Entity->MoveTimer < MoveTimerHalf)
                {
                    switch(Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_MagicianRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_MagicianBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_MagicianLeft0;
                        } break;
                    }
                }
                else
                {
                    switch(Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_MagicianRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_MagicianBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_MagicianLeft1;
                        } break;
                    }
                }
                palette_cycle *Cycle = PaletteCycles + Entity->RenderType;
                PushSprite(RenderBuffer, Tile, Entity->P, Cycle->Palettes + State->TestPaletteIndex);
            } break;
            
            case EntityRenderType_Demon:
            {
                loaded_tile *Tile = 0;
                r32 MoveTimerMax = 0.5f;
                r32 MoveTimerHalf = 0.5f*MoveTimerMax;
                if(Entity->MoveTimer >= MoveTimerMax)
                {
                    Entity->MoveTimer -= MoveTimerMax;
                }
                if(Entity->MoveTimer < MoveTimerHalf)
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonLeft0;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonFront0;
                        } break;
                    }
                }
                else
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonLeft1;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_DemonFront1;
                        } break;
                    }
                }
                palette_cycle *Cycle = PaletteCycles + Entity->RenderType;
                PushSprite(RenderBuffer, Tile, Entity->P, Cycle->Palettes + State->TestPaletteIndex);
            } break;
            
            case EntityRenderType_Guard:
            {
                loaded_tile *Tile = 0;
                r32 MoveTimerMax = 0.5f;
                r32 MoveTimerHalf = 0.5f*MoveTimerMax;
                if(Entity->MoveTimer >= MoveTimerMax)
                {
                    Entity->MoveTimer -= MoveTimerMax;
                }
                if(Entity->MoveTimer < MoveTimerHalf)
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardLeft0;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardFront0;
                        } break;
                    }
                }
                else
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardLeft1;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_GuardFront1;
                        } break;
                    }
                }
                palette_cycle *Cycle = PaletteCycles + Entity->RenderType;
                PushSprite(RenderBuffer, Tile, Entity->P, Cycle->Palettes + State->TestPaletteIndex);
            } break;
            
            case EntityRenderType_Captain:
            {
                loaded_tile *Tile = 0;
                r32 MoveTimerMax = 0.5f;
                r32 MoveTimerHalf = 0.5f*MoveTimerMax;
                if(Entity->MoveTimer >= MoveTimerMax)
                {
                    Entity->MoveTimer -= MoveTimerMax;
                }
                if(Entity->MoveTimer < MoveTimerHalf)
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainLeft0;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainFront0;
                        } break;
                    }
                }
                else
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainLeft1;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Tile = GameState->Assets.Tiles + TileType_CaptainFront1;
                        } break;
                    }
                }
                palette_cycle *Cycle = PaletteCycles + Entity->RenderType;
                PushSprite(RenderBuffer, Tile, Entity->P, Cycle->Palettes + State->TestPaletteIndex);
            } break;
        }
            
        EntityIndex = NextIndex;
    }

    {
        TIMED_BLOCK("Render Game");
        RenderBufferToBackBuffer(RendererState, RenderBuffer, RenderFlags_UsePipeline);
    }
    
    {DEBUG_GROUP("Play Mode");
        DEBUG_FILL_BAR("Entities", State->EntityCount, ArrayCount(State->Entities));
        DEBUG_FILL_BAR("Particles", State->ParticleCount, ArrayCount(State->Particles));
        DEBUG_VALUE("Shape Arena", State->PhysicsState.ShapeArena);
        DEBUG_VALUE("Shape Freelist", (b32)State->PhysicsState.FirstFreeShape);
        DEBUG_VALUE("Render Arena", TranState->RenderBuffer.Arena);
        DEBUG_VALUE("Test Color Index", State->TestColorIndex);
    }
    EndTemporaryMemory(RenderMemory);
}
