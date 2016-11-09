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
CreateLink(play_state *State, v2 P, facing_direction Facing)
{
    entity *Result = CreateEntity(State);
    Result->Type = EntityType_Link;
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

        CreateLink(State, V2(0, 0), FacingDirection_Up);
        State->LinkPaletteIndex = 0;
        State->DarknutPaletteIndex = 0;

        State->TestColorIndex = 0;
    }

    game_controller *LinkController = Input->Controllers + Input->MostRecentlyUsedController;
    
    temporary_memory RenderMemory = BeginTemporaryMemory(&RenderBuffer->Arena);
    PushClear(RendererState, RenderBuffer, V3(0.0f, 0.0f, 0.0f));
    v2 ScreenCenter = 0.5f*V2i(RenderBuffer->Width, RenderBuffer->Height);

    u32 NESPalette[] =
    {
        0xFF7C7C7C,
        0xFF0000FC,
        0xFF0000BC,
        0xFF4428BC,
        0xFF940084,
        0xFFA80020,
        0xFFA81000,
        0xFF881400,
        0xFF503000,
        0xFF007800,
        0xFF006800,
        0xFF005800,
        0xFF004058,
        0xFF000000,
        0xFF000000,
        0xFF000000,
        0xFFBCBCBC,
        0xFF0078F8,
        0xFF0058F8,
        0xFF6844FC,
        0xFFD800CC,
        0xFFE40058,
        0xFFF83800,
        0xFFE45C10,
        0xFFAC7C00,
        0xFF00B800,
        0xFF00A800,
        0xFF00A844,
        0xFF008888,
        0xFF000000,
        0xFF000000,
        0xFF000000,
        0xFFF8F8F8,
        0xFF3CBCFC,
        0xFF6888FC,
        0xFF9878F8,
        0xFFF878F8,
        0xFFF85898,
        0xFFF87858,
        0xFFFCA044,
        0xFFF8B800,
        0xFFB8F818,
        0xFF58D854,
        0xFF58F898,
        0xFF00E8D8,
        0xFF787878,
        0xFF000000,
        0xFF000000,
        0xFFFCFCFC,
        0xFFA4E4FC,
        0xFFB8B8F8,
        0xFFD8B8F8,
        0xFFF8B8F8,
        0xFFF8A4C0,
        0xFFF0D0B0,
        0xFFFCE0A8,
        0xFFF8D878,
        0xFFD8F878,
        0xFFB8F8B8,
        0xFFB8F8D8,
        0xFF00FCFC,
        0xFFF8D8F8,
        0xFF000000,
        0xFF000000,
    };

    palette LinkPalettes[] =
    {
        {
            0x00000000,
            NESPalette[23],
            NESPalette[41],
            NESPalette[40],
        },
        {
            0x00000000,
            NESPalette[23],
            NESPalette[50],
            NESPalette[40],
        },
        {
            0x00000000,
            NESPalette[23],
            NESPalette[22],
            NESPalette[40],
        },
    };

    palette DarknutPalettes[] =
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

    if(!State->Paused)
    {
        for(u32 EntityIndex = 1;
            EntityIndex < State->EntityCount;
            )
        {
            u32 NextIndex = EntityIndex + 1;
            entity *Entity = State->Entities + EntityIndex;

            switch(Entity->Type)
            {
                case EntityType_Link:
                {
                    if(WentDown(LinkController->ActionUp))
                    {
                        ++State->DarknutPaletteIndex;
                        if(State->DarknutPaletteIndex == ArrayCount(DarknutPalettes))
                        {
                            State->DarknutPaletteIndex = 0;
                        }
                    }

                    if(WentDown(LinkController->ActionRight))
                    {
                        State->TestColorIndex = (State->TestColorIndex + 1) & (ArrayCount(NESPalette) - 1);
                    }
                    
                    v2 dP = {};
                    if(LinkController->LeftStick.y >= 0.5f)
                    {
                        dP.y += 1;
                        Entity->Facing = FacingDirection_Up;
                    }
                    else if(LinkController->LeftStick.y <= -0.5f)
                    {
                        dP.y -= 1;
                        Entity->Facing = FacingDirection_Down;
                    }
                    else if(LinkController->LeftStick.x >= 0.5f)
                    {
                        dP.x += 1;
                        Entity->Facing = FacingDirection_Right;
                    }
                    else if(LinkController->LeftStick.x <= -0.5f)
                    {
                        dP.x -= 1;
                        Entity->Facing = FacingDirection_Left;
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

    if(WentDown(LinkController->Select))
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

    loaded_tile *Tiles[] =
    {
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave, Cave,
        RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, Ground, Ground, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace,
        RockFace, RockFace, RockFace, RockFace, Cave, RockFace, RockCorner4, Ground, Ground, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace,
        RockFace, RockFace, RockFace, RockCorner4, Ground, Ground, Ground, Ground, Ground, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace,
        RockFace, RockFace, RockCorner4, Ground, Ground, Ground, Ground, Ground, Ground, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace,
        RockFace, RockCorner4, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockCorner3, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace,
        Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground,
        RockTop, RockCorner1, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockTop, RockTop,
        RockFace, RockFace, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockFace, RockFace,
        RockFace, RockFace, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, Ground, RockFace, RockFace,
        RockFace, RockFace, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockTop, RockFace, RockFace,
        RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace, RockFace,
    };
    PushTiles(RenderBuffer, ArrayCount(Tiles), Tiles);

    for(u32 EntityIndex = 1;
        EntityIndex < State->EntityCount;
        )
    {
        u32 NextIndex = EntityIndex + 1;
        entity *Entity = State->Entities + EntityIndex;

        switch(Entity->Type)
        {
            case EntityType_Link:
            {
                loaded_tile *Link = 0;
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
                            Link = GameState->Assets.Tiles + TileType_DarknutRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Link = GameState->Assets.Tiles + TileType_DarknutBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Link = GameState->Assets.Tiles + TileType_DarknutLeft0;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Link = GameState->Assets.Tiles + TileType_DarknutFront0;
                        } break;
                    }
                }
                else
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Link = GameState->Assets.Tiles + TileType_DarknutRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Link = GameState->Assets.Tiles + TileType_DarknutBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Link = GameState->Assets.Tiles + TileType_DarknutLeft1;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Link = GameState->Assets.Tiles + TileType_DarknutFront1;
                        } break;
                    }
                }
                PushSprite(RenderBuffer, Link, Entity->P, DarknutPalettes + State->DarknutPaletteIndex);
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
