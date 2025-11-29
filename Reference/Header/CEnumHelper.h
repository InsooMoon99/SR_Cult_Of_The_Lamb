#pragma once
#include "Engine_Define.h"

#include "CGameObject.h"

BEGIN(Engine)
// const char* -> wstring 변환
static wstring CharToWString(const char* cstr)
{
    if (cstr == nullptr)
        return L"";

    int srcLen = static_cast<int>(strlen(cstr));
    if (srcLen == 0)
        return L"";

    int required = MultiByteToWideChar(CP_UTF8, 0, cstr, srcLen, nullptr, 0);
    if (required <= 0)
        return L"";    // 변환 실패

    wstring result;
    result.resize(required);

    // 실제 변환
    int converted = MultiByteToWideChar(
        CP_UTF8,
        0,
        cstr,
        srcLen,
        &result[0],
        required);

    if (converted <= 0)
        return L"";    // 변환 실패

    return result;
}

// wstring -> Utf8로 변환 [ImGui 띄울때 사용]
static string WStringToUtf8(const wstring& wstr)
{
    if (wstr.empty())
        return {};

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        static_cast<int>(wstr.size()),
        nullptr,
        0,
        nullptr,
        nullptr);

    if (sizeNeeded <= 0)
        return {};

    string result(sizeNeeded, 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        static_cast<int>(wstr.size()),
        result.data(),
        sizeNeeded,
        nullptr,
        nullptr);

    return result;
}

// ImGui와의 호환성때문에 기본적으로 const char*로 반환
static const char* ToString(ACTORSTATE state)
{
    switch (state)
    {
    case ACTORSTATE::IDLE:                  return "Idle";
    case ACTORSTATE::RUN:                   return "Run";
    case ACTORSTATE::ATTACK:                return "Attack";
    case ACTORSTATE::HIT:                   return "Hit";
    case ACTORSTATE::DEAD:                  return "Dead";

    // Player Only
    case ACTORSTATE::PLAYER_ROLL:           return "Roll";
    case ACTORSTATE::PLAYER_SWORD_COMBO1:   return "Sword_Combo1";
    case ACTORSTATE::PLAYER_SWORD_COMBO2:   return "Sword_Combo2";
    case ACTORSTATE::PLAYER_SWORD_COMBO3:   return "Sword_Combo3";
    case ACTORSTATE::PLAYER_SWORD_HEAVY:    return "Sword_Heavy";

    default:                                return "UnknownState";
    }
}

static const char* ToString(ACTORDIR dir)
{
    switch (dir)
    {
    case ACTORDIR::LEFT:        return "Left";
    case ACTORDIR::RIGHT:       return "Right";
    case ACTORDIR::UP:          return "Up";
    case ACTORDIR::DOWN:        return "Down";
    case ACTORDIR::L_UP:        return "Left Up";
    case ACTORDIR::R_UP:        return "Right Up";
    case ACTORDIR::L_DOWN:      return "Left Down";
    case ACTORDIR::R_DOWN:      return "Right Down";
    default:                    return "UnknownDir";
    }
}

static const char* ToString(COMPONENTTYPE type)
{
    switch (type)
    {
    // Geometry/Mesh Components
    case COMPONENTTYPE::TRI_COLOR:          return "TriColor";
    case COMPONENTTYPE::RC_COLOR:           return "RcColor";
    case COMPONENTTYPE::BOX_COLOR:          return "BoxColor";
    case COMPONENTTYPE::RC_TEX:             return "RcTex";
    case COMPONENTTYPE::BOX_TEX:            return "BoxTex";
    case COMPONENTTYPE::TERRAIN_TEX:        return "TerrainTex";

    // Core Components
    case COMPONENTTYPE::TRANSFORM:          return "Transform";
    case COMPONENTTYPE::STATE:              return "State";
    case COMPONENTTYPE::COMBATSTAT:         return "CombatStat";
    case COMPONENTTYPE::INVENTORY:          return "Inventory";
    case COMPONENTTYPE::ANIMATOR:           return "Animator";
    case COMPONENTTYPE::CAMERA:             return "Camera";
    case COMPONENTTYPE::TERRAIN_RENDER:     return "TerrainRender";
    case COMPONENTTYPE::WALL_RENDERER:      return "WallRenderer";

    // Texture Components (General & Specific)
    case COMPONENTTYPE::TEXTURE:            return "Texture";
    case COMPONENTTYPE::TEX_PLAYER:         return "TexPlayer";
    case COMPONENTTYPE::TEX_MAINBG:         return "TexMainBg";
    case COMPONENTTYPE::TEX_MONSTER:        return "TexMonster";
    case COMPONENTTYPE::TEX_HUMAN_MONSTER:  return "TexHumanMonster";
    case COMPONENTTYPE::TEX_ITEM:           return "TexItem";
    case COMPONENTTYPE::TEX_GRASS:          return "TexGrass";
    case COMPONENTTYPE::TEX_RUBBLE:         return "TexRubble";
    case COMPONENTTYPE::TEX_DECO:           return "TexDeco";

    // UI Texture Components
    case COMPONENTTYPE::TEX_UI_CIRCLE:      return "UI_Circle";
    case COMPONENTTYPE::TEX_UI_COLOR:       return "UI_Color";
    case COMPONENTTYPE::TEX_UI_HEART:       return "UI_Heart";
    case COMPONENTTYPE::TEX_UI_WEAPON:      return "UI_Weapon";
    case COMPONENTTYPE::TEX_MENU_CHAR:      return "UI_ManuChaR";
    case COMPONENTTYPE::TEX_UI_ICON:        return "UI_Icon";

    // Collider Components
    case COMPONENTTYPE::RECT_COLL:          return "RectColl";
    case COMPONENTTYPE::CIRCLE_COLL:        return "CircleColl";
    case COMPONENTTYPE::BOX_COLL:           return "BoxColl";
    case COMPONENTTYPE::SPHERE_COLL:        return "SphereColl";

    // Physics Component
    case COMPONENTTYPE::RIGIDBODY:          return "Rigidbody";

    // Player Components
    case COMPONENTTYPE::WEAPON_EQUIP:       return "WeaponEquip";
    case COMPONENTTYPE::PL_CONTROLLER:      return "PlayerController";
    case COMPONENTTYPE::PL_INPUT:           return "PlayerInput";
    case COMPONENTTYPE::PL_MOVEMENT:        return "PlayerMovement";
    case COMPONENTTYPE::PL_ATTACK:          return "PlayerAttack";

    // End Marker
    case COMPONENTTYPE::COMPONENT_END:      return "ComponentEnd";
    default:                                return "UnknownComponent";
    }
}

static const char* ToString(LAYERTYPE type)
{
    switch (type)
    {
    case LAYERTYPE::ENVIRONMENT:    return "Environment";
    case LAYERTYPE::GAMELOGIC:      return "GameLogic";
    case LAYERTYPE::UI:             return "UI";
    case LAYERTYPE::LAYER_END:      return "LayerEnd";

    default:
        return "UnknownLayerType";
    }
}

static const char* ToString(OBJTYPE type)
{
    switch (type)
    {
    case OBJTYPE::PLAYER:           return "Player";
    case OBJTYPE::MONSTER:          return "Monster";
    case OBJTYPE::CAMERA:           return "Camera";
    case OBJTYPE::ITEM:             return "Item";
    case OBJTYPE::UI:               return "Ui";
    case OBJTYPE::BOSS2:            return "Boss2";
    case OBJTYPE::HUMANMONSTER:     return "HumanMonster";
    case OBJTYPE::TERRAIN:          return "Terrain";
    case OBJTYPE::PLACEMENT:        return "Placement";
    case OBJTYPE::ATTACK_COL:       return "Attack_Collider";

    case OBJTYPE::OBJ_END:          return "Obj_End";
    case OBJTYPE::BOSS1:            return "Boss_Phase1";
    case OBJTYPE::WALL:             return "Wall";

    default:                        return "UnknownObjectType";
    }
}

static const char* ToString(SCENETYPE type)
{
    switch (type)
    {
    case SCENETYPE::SC_LOGO:        return "Logo";
    case SCENETYPE::SC_MAINMENU:    return "MainMenu";
    case SCENETYPE::SC_STAGE:       return "Stage";
    case SCENETYPE::SC_STAGE1:      return "Stage1";
    case SCENETYPE::SC_TARO_TEST:   return "Taro_Test";
    case SCENETYPE::SC_BOSS_STAGE1:  return "Boss_Stage";
    case SCENETYPE::SC_EDIT:        return "Edit";
    case SCENETYPE::SC_END:         return "SCENE_END";
    }
}

static void SetNameFromComponentType(CGameObject* obj, COMPONENTTYPE type)
{
    if (!obj) return;

    const char* base  = ToString(type);
    wstring     wBase = CharToWString(base);

    obj->Set_Name(wBase);
}

END
