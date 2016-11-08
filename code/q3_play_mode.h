#if !defined(Q3_PLAY_MODE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Christopher LaBauve $
   $Notice: $
   ======================================================================== */

enum facing_direction
{
    FacingDirection_Right,
    FacingDirection_Up,
    FacingDirection_Left,
    FacingDirection_Down,
};

enum entity_type
{
    EntityType_Link = 1<<0,
};

struct entity
{
    entity_type Type;
    collider_type ColliderType;
    v2 P;
    r32 MoveTimer;
    facing_direction Facing;
};

struct particle
{
    v3 P;
    v3 dP;
    r32 Yaw;
    r32 dYaw;
    v2 A;
    v2 B;
    v2 C;
    r32 Timer;
    r32 Duration;
    v4 Color;
};

struct play_state
{
    b32 IsInitialized;

    u32 LinkPaletteIndex;

    u32 TunicIndex;
    
    physics_state PhysicsState;

    b32 Paused;
    s32 Lives;

    u32 EntityCount;
    entity Entities[256];

    u32 ParticleCount;
    particle Particles[256];
};

inline particle *
CreateParticle(play_state *State)
{
    particle *Result = State->Particles;
    if(State->ParticleCount < ArrayCount(State->Particles))
    {
        Result = State->Particles + State->ParticleCount++;
    }
    else
    {
        Assert(!"Created too many particles");
    }
    return(Result);
}

inline void
DestroyParticle(play_state *State, particle *Particle)
{
    *Particle = State->Particles[--State->ParticleCount];
}

inline entity *
CreateEntity(play_state *State)
{
    entity *Result = State->Entities;
    physics_state *PhysicsState = &State->PhysicsState;
    memory_arena *ShapeArena = &PhysicsState->ShapeArena;
    if(State->EntityCount < ArrayCount(State->Entities))
    {
#if 0
        if(HasRoomForArray(ShapeArena, ShapeCount, collision_shape))
        {
#endif
            Result = State->Entities + State->EntityCount++;
            *Result = {};
#if 0
            for(u32 ShapeIndex = 0;
                ShapeIndex < ShapeCount;
                ++ShapeIndex)
            {
                collision_shape *Shape = PhysicsState->FirstFreeShape;
                if(Shape)
                {
                    PhysicsState->FirstFreeShape = Shape->NextFree;
                }
                else
                {
                    Shape = PushStruct(ShapeArena, collision_shape);
                }
                *Shape = *(Shapes + ShapeIndex);
                Shape->Next = Result->CollisionShapes;
                Result->CollisionShapes = Shape;
                Result->CollisionShapeCount = ShapeCount;
            }
            Result->BoundingRadius = CalculateBoundingRadius(Result);
        }
        else
        {
            Assert(!"Created too many collision shapes.");
        }
#endif
    }
    else
    {
        Assert(!"Created too many entities.");
    }
    return(Result);
}

inline void
DestroyEntity(play_state *State, entity *Entity)
{
#if 0
    physics_state *PhysicsState = &State->PhysicsState;
    for(collision_shape *Shape = Entity->CollisionShapes;
        Shape;
        )
    {
        collision_shape *Next = Shape->Next;

        Shape->NextFree = PhysicsState->FirstFreeShape;
        PhysicsState->FirstFreeShape = Shape;
        
        Shape = Next;
    }
#endif
    *Entity = State->Entities[--State->EntityCount];
}

#if 0
inline b32
IsDestroyed(entity *Entity)
{
    b32 Result = Entity->DestroyedBy;
    return(Result);
}

inline void
SetNotDestroyed(entity *Entity)
{
    Entity->DestroyedBy = (collider_type)0;
}

inline b32
CanCollide(entity *Entity)
{
    b32 Result = !IsDestroyed(Entity) && (Entity->ColliderType != ColliderType_None);
    return(Result);
}

inline b32
CanCollide(play_state *State, entity *A, entity *B)
{
    b32 Result = !IsDestroyed(A) && !IsDestroyed(B) &&
        CanCollide(&State->PhysicsState, A->ColliderType, B->ColliderType);
    return(Result);
}
#endif

#define Q3_PLAY_MODE_H
#endif
