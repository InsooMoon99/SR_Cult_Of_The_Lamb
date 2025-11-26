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
        CAMERA,
        UI,

        LAYER_END
    };

    enum class OBJTYPE
    {
        PLAYER,
        MONSTER,
        CAMERA,
        ITEM,
        /* 오브젝트 타입 종류 추가 */
        UI,
        BOSS2,
        BOSS1,
        HUMANMONSTER,
        TERRAIN,
        PLACEMENT,
        ATTACK_COL,

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
        TERRAIN_RENDER,
        CONTROLLER_PLAYER,
        WEAPON_EQUIP,
        RIGIDBODY,

        TEXTURE,
        TEX_PLAYER,
        TEX_MAINBG,
        TEX_MONSTER,
        TEX_HUMAN_MONSTER,
        TEX_UI_CIRCLE,
        TEX_UI_CELL,
        TEX_UI_COLOR,
        TEX_UI_HEART,
        TEX_UI_WEAPON,
        TEX_UI_SKILLCOUNT,
        TEX_UI_HIGHLIGHT,
        TEX_UI_INVEN,
        TEX_ITEM,
        TEX_MENU_CHAR,
        TEX_UI_ICON,
        TEX_UI_BUTTON,
        TEX_BOSSPHASE1,

        // TODO : Test Tile -> 은수 Texture 버전이랑 합치면 고치기
        TEX_TILE_284,
        TEX_TILE_1024,
        TEX_TILE_OUTLINE,

        ANIMATOR,
        CAMERA,

        RECT_COLL,
        CIRCLE_COLL,
        BOX_COLL,
        SPHERE_COLL,

        ////////////////
        // MapObject ///
        ////////////////
        TEX_GRASS,
        TEX_RUBBLE,
        TEX_DECO,

        COMPONENT_END
    };

    enum class ACTORSTATE
    {
        // 여기 추가할때마다 EnumHelper에 추가해야 Editor에 정상적으로 나옴
        IDLE,
        RUN,
        ATTACK,
        ROLL,
        HIT,
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
        SC_MAINMENU,
        SC_STAGE,
        SC_STAGE1,
        SC_STAGE2,
        SC_BOSSSTAGE,
        SC_EDIT,
        SC_END
    };

}
#endif // Engine_Enum_h__
