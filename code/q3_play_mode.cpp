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
    }

    game_controller *LinkController = Input->Controllers + Input->MostRecentlyUsedController;
    
    temporary_memory RenderMemory = BeginTemporaryMemory(&RenderBuffer->Arena);
    PushClear(RendererState, RenderBuffer, V3(0.0f, 0.0f, 0.0f));
    v2 ScreenCenter = 0.5f*V2i(RenderBuffer->Width, RenderBuffer->Height);

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

    palette LinkPalette =
    {
        0x00000000,
        0xFFC84C0C,
        0xFF80D010,
        0xFFFC9838,
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
                            Link = GameState->Assets.Tiles + TileType_LinkRight0;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Link = GameState->Assets.Tiles + TileType_LinkBack0;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Link = GameState->Assets.Tiles + TileType_LinkLeft0;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Link = GameState->Assets.Tiles + TileType_LinkFront0;
                        } break;
                    }
                }
                else
                {
                    switch(Entity->Facing)
                    {
                        case FacingDirection_Right:
                        {
                            Link = GameState->Assets.Tiles + TileType_LinkRight1;
                        } break;
                    
                        case FacingDirection_Up:
                        {
                            Link = GameState->Assets.Tiles + TileType_LinkBack1;
                        } break;
                    
                        case FacingDirection_Left:
                        {
                            Link = GameState->Assets.Tiles + TileType_LinkLeft1;
                        } break;
                    
                        case FacingDirection_Down:
                        {
                            Link = GameState->Assets.Tiles + TileType_LinkFront1;
                        } break;
                    }
                }
                PushSprite(RenderBuffer, Link, Entity->P, &LinkPalette);
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
    }
    EndTemporaryMemory(RenderMemory);
}
