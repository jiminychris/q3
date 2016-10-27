#if !defined(Q3_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Christopher LaBauve $
   $Notice: $
   ======================================================================== */

#define DEBUG_CAMERA 0

#include "q3_platform.h"
platform_read_file *PlatformReadFile;
platform_push_thread_work *PlatformPushThreadWork;
platform_wait_for_all_thread_work *PlatformWaitForAllThreadWork;

#include "q3_intrinsics.h"
#include "q3_math.h"
#include "q3_debug.h"
#include "q3_assets.h"
#include "q3_render.h"
#include "q3_random.h"
#include "q3_physics.h"

enum game_mode
{
    GameMode_TitleScreen,
    GameMode_Play,
};

enum title_screen_selection
{
    TitleScreenSelection_Journey,
    TitleScreenSelection_Arcade,
    TitleScreenSelection_Quit,

    PlayType_Terminator,
};

enum play_type
{
    PlayType_Journey,
    PlayType_Arcade,
};

#include "q3_play_mode.h"
#include "q3_title_screen_mode.h"

struct game_state
{
    b32 IsInitialized;

    r32 MetersToPixels;
    u32 RunningSampleCount;

    game_mode Mode;
    game_mode NextMode;
    union
    {
        play_state PlayState;
        title_screen_state TitleScreenState;
    };

    assets Assets;

#if 0
    loaded_bitmap StructuredArt;
#endif

    memory_arena Arena;
};

struct transient_state
{
    b32 IsInitialized;

    memory_arena Arena;
    render_buffer RenderBuffer;
};

#define Q3_H
#endif
