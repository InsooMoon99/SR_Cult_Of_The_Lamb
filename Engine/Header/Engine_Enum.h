#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
// 윈도우 생성 시 사용할 화면 모드
enum WINMODE { MODE_FULL, MODE_WIN };

/// @brief 컴포넌트 업데이트에 따른 인덱스
///	@details
///	- ID_DYNAMIC : 다이나믹 컴포넌트. 매 프레임마다 갱신할 경우
/// - ID_STATIC : 스태틱 컴포넌트. 갱신할 필요가 없을 경우
enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

// 게임 오브젝트의 방향 및 위치 벡터 인덱스
enum INFO { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };

// 회전 축 인덱스
enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

// 텍스처 자원의 타입 구분 인덱스
enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

// 렌더링 우선순위별 렌더링 그룹 인덱스
enum RENDERID
{
    RENDER_PRIORITY,
    RENDER_PRIORITY_ALPHA,
    RENDER_NONALPHA,
    RENDER_ALPHA,
    RENDER_UI,

    RENDER_END
};

// DirectInput 마우스 버튼 상태 인덱스
enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };

// DirectInput 마우스 이동 축 인덱스
enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

enum class COLLIDERTYPE { RECT, CIRCLE, BOX, SPHERE };

enum class ANIMSTATE
{
    STOP,		    // 정지
    LOOP,			// 계속 반복
    ONCE,			// 한번만 재생
    REVERSE_LOOP,	// 거꾸로 계속 반복
    REVERSE_ONCE,	// 거꾸로 한번만 재생

    ANIM_END
};

enum class LAYERTYPE
{
    ENVIRONMENT,
    GAMELOGIC,
    UI,

    LAYER_END
};

enum class COLLISION_LAYERTYPE
{
    DEFAULT, // 모든 충돌
    PLAYER,
    ENEMY,
    WALL,
    PLAYERPROJECTILE,
    BOSSPROJECTILE,
    ITEM,
    ATTACK,
    TRIGGER,
    INTERACTIVEOBJ,
    BUILDING,       // 건물

    COL_LAYER_END
};

enum class OBJTYPE
{
    PLAYER,
    MONSTER,
    NPC,
    CAMERA,
    ITEM,
    /* 오브젝트 타입 종류 추가 */
    LOGIC,
    UI,
    HUMANMONSTER,
    BOSS1,
    BOSS2,
    INTERACTIVE,
    TEMPLE,             // 신전
    KITCHEN,            // 요리용 화덕

    ATTACK_COL,
    PARTICLE,
    TRIGGER,
    BULLET,
    TERRAIN,
    PLACEMENT,
    WALL,

    OBJ_END
};

// 만들어지는 컴포넌트마다 부여하는 고유 타입
enum class COMPONENTTYPE
{
    TRI_COLOR,
    RC_COLOR,
    BOX_COLOR,

    RC_TEX,
    BOX_TEX,
    TERRAIN_TEX,

    TRANSFORM,
    STATE,
    COMBATSTAT,
    INVENTORY,
    RIGIDBODY,
    TERRAIN_RENDER,
    WALL_RENDERER,
    MONSTER_SPAWNER,
    PROJECTILE,
    SPRITEEFFECT,
    GATHERING,

    WEAPON_EQUIP,           // 플레이어 무기
    PL_INPUT,               // 플레이어 방향 입력
    PL_CONTROLLER,          // 플레이어 FSM 컨트롤러
    PL_MOVEMENT,            // 플레이어 움직임
    PL_ATTACK,              // 플레이어 공격
    PL_SKILL,

    TEXTURE,
    TEX_PLAYER,
    TEX_MAINBG,
    TEX_MONSTER,
    TEX_HUMAN_MONSTER,
    TEX_FLOATER_MONSTER,
    TEX_UI_CIRCLE,
    TEX_UI_CELL,
    TEX_UI_COLOR,
    TEX_UI_HEART,
    TEX_UI_WEAPON,
    TEX_UI_SKILLCOUNT,
    TEX_UI_HIGHLIGHT,
    TEX_UI_INVEN,
    TEX_UI_RITUALCARD,
    TEX_UI_HEARTCARD,
    TEX_UI_EXPLAIN,
    TEX_UI_TAROT,
    TEX_UI_MONSTERHP,
    TEX_ITEM,
    TEX_MENU_CHAR,
    TEX_UI_ICON,
    TEX_UI_BUTTON,
    TEX_BOSSPHASE1,
    TEX_BULLET,
    TEX_TEMPLE,
    TEX_OUT,
    TEX_BERRY,
    TEX_SKILL,

    ANIMATOR,

    CAM_CONTROLLER,
    CAM_MOVEMENT,
    CAM_CALC,

    RECT_COLL,
    CIRCLE_COLL,
    BOX_COLL,
    SPHERE_COLL,

    ////////////////
    //   Boss 2   //
    ////////////////
    TEX_SLAVE_TEST,
    TEX_BOSS2_PHASE2,
    TEX_BOSS2_EYEBALL,

    BOSS2_CONTROLLER,
    EYEBALL_CONTROLLER,
    TEX_WARNING_CHAIN,
    TEX_CHAIN,

    ////////////////
    //  MapObject //
    ////////////////
    TEX_TILE_284,
    TEX_TILE_1024,
    TEX_TILE_OUTLINE,

    TEX_GRASS,
    TEX_RUBBLE,
    TEX_DECO,

    TEX_DUNGEON_DECO,
    TEX_ETC,

    ////////////////
    //  Particle  //
    ////////////////
    TEX_PARTICLE,

    COMPONENT_END
};

enum class ACTORSTATE
{
    // 여기 추가할때마다 EnumHelper에 추가해야 Editor에 정상적으로 나옴
    IDLE,
    RUN,
    ATTACK,

    PLAYER_ROLL,                // 구르기
    PLAYER_SWORD_COMBO1,        // 검 일반공격 콤보 1
    PLAYER_SWORD_COMBO2,        // 검 일반공격 콤보 2
    PLAYER_SWORD_COMBO3,        // 검 일반공격 콤보 3
    PLAYER_SWORD_HEAVY,         // 검 강공격
    PLAYER_GAUNTLET_COMBO1,     // 건틀릿 일반공격 콤보 1
    PLAYER_GAUNTLET_COMBO2,     // 건틀릿 일반공격 콤보 2
    PLAYER_GAUNTLET_COMBO3,     // 건틀릿 일반공격 콤보 3
    PLAYER_GAUNTLET_COMBO4,
    PLAYER_GAUNTLET_HEAVY,      // 건틀릿 강공격
    PLAYER_SKILL_CHARGING,
    PLAYER_SKILL_SINGLE,
    PLAYER_SKILL_TRIPLE,
    HIT,

    PLAYER_ACTION,
    PLAYER_WOOD,
    PLAYER_STONE,
    PLAYER_BERRY,
    //GATHERING,

    DEAD,

    STATE_END
};

enum class ACTORDIR
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    L_UP,
    R_UP,
    L_DOWN,
    R_DOWN,

    DIR_END
};

enum class SCENETYPE
{
    SC_LOGO,
    SC_LOGO_BOSS1,
    SC_LOGO_BOSS2,
    SC_MAINMENU,
    SC_STAGE,
    SC_DUNGEON,
    SC_TAROT_TEST,
    SC_BOSS_STAGE1,
    SC_BOSS_STAGE2,

    SC_EDIT,

    SC_END
};

enum SOUNDTYPE
{
    SOUND_BGM,
    SOUND_EFFECT,

    SOUND_END
};

// 카메라 컨트롤 모드
enum CAM_MODE
{
    CAM_FREE,               // 자유 카메라
    CAM_TARGET,             // 타겟 고정 카메라

    CAM_END
};

// 카메라 시점 타입
enum VIEW_TYPE
{
    VIEW_FPS,               // 1인칭
    VIEW_TPS,               // 3인칭

    VIEW_END
};

// 카메라 투영 타입
enum PROJECTION_TYPE
{
    PROJ_PERSPECTIVE,       // 원근 투영
    PROJ_ORTHOGRAPHIC,      // 직교 투영

    PROJ_END
};

// 건축물 종류
enum BUILDING_TYPE
{
    TEMPLE,                 // 신전
    KITCHEN,                // 요리용 화덕

    BUILDING_END
};
}
#endif // Engine_Enum_h__
