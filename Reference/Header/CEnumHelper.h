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

    std::wstring result;
    result.resize(required);

    // 실제 변환
    int converted = MultiByteToWideChar(CP_UTF8, 0, cstr, srcLen,
        &result[0], required);
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
        CP_UTF8, 0,
        wstr.c_str(), (int)wstr.size(),
        nullptr, 0, nullptr, nullptr);

    if (sizeNeeded <= 0)
        return {};

    string result(sizeNeeded, 0);
    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), (int)wstr.size(),
        result.data(), sizeNeeded,
        nullptr, nullptr);

    return result;
}

// ImGui와의 호환성때문에 기본적으로 const char*로 반환
static const char* ToString(ACTORSTATE state)
{
    switch (state)
    {
    case ACTORSTATE::IDLE:
        return "Idle";

    case ACTORSTATE::RUN:
        return "Run";

    case ACTORSTATE::ATTACK:
        return "Attack";

    case ACTORSTATE::HIT:
        return "Hit";

    case ACTORSTATE::DEAD:
        return "Dead";

    case ACTORSTATE::STATE_END:
    default:
        return "UnknownState";
    }
}

static const char* ToString(ACTORDIR dir)
{
    switch (dir)
    {
    case ACTORDIR::LEFT:
        return "Left";

    case ACTORDIR::RIGHT:
        return "Right";

    case ACTORDIR::UP:
        return "Up";

    case ACTORDIR::DOWN:
        return "Down";

    case ACTORDIR::L_UP:
        return "Left Up";

    case ACTORDIR::R_UP:
        return "Right Up";

    case ACTORDIR::L_DOWN:
        return "Left Down";

    case ACTORDIR::R_DOWN:
        return "Right Down";

    default:
        return "UnknownDir";
    }
}

static const char* ToString(COMPONENTTYPE type)
{
    switch (type)
    {
    // Geometry/Mesh Components
    case COMPONENTTYPE::TRI_COLOR: return "TriColor";
    case COMPONENTTYPE::RC_COLOR: return "RcColor";
    case COMPONENTTYPE::BOX_COLOR: return "BoxColor";
    case COMPONENTTYPE::RC_TEX: return "RcTex";
    case COMPONENTTYPE::BOX_TEX: return "BoxTex";
    case COMPONENTTYPE::TERRAIN_TEX: return "TerrainTex";

    // Core Components
    case COMPONENTTYPE::TRANSFORM: return "Transform";
    case COMPONENTTYPE::STATE: return "State";
    case COMPONENTTYPE::COMBATSTAT: return "CombatStat";
    case COMPONENTTYPE::INVENTORY: return "Inventory";
    case COMPONENTTYPE::ANIMATOR: return "Animator";
    case COMPONENTTYPE::CAMERA: return "Camera";
    case COMPONENTTYPE::TERRAIN_RENDER: return "TerrainRender";

    // Texture Components (General & Specific)
    case COMPONENTTYPE::TEXTURE: return "Texture";
    case COMPONENTTYPE::TEX_PLAYER: return "TexPlayer";
    case COMPONENTTYPE::TEX_MAINBG: return "TexMainBg";
    case COMPONENTTYPE::TEX_MONSTER: return "TexMonster";
    case COMPONENTTYPE::TEX_HUMANMONSTER: return "TexHumanMonster";
    case COMPONENTTYPE::TEX_ITEM: return "TexItem";
    case COMPONENTTYPE::TEX_GRASS: return "TexGrass";
    case COMPONENTTYPE::TEX_ROCK: return "TexRock";

    // UI Texture Components
    case COMPONENTTYPE::TEX_UI_CIRCLE: return "UI_Circle";
    case COMPONENTTYPE::TEX_UI_COLOR: return "UI_Color";
    case COMPONENTTYPE::TEX_UI_HEART: return "UI_Heart";
    case COMPONENTTYPE::TEX_UI_WEAPON: return "UI_Weapon";
    case COMPONENTTYPE::TEX_MANUCHAR: return "UI_ManuChaR";
    case COMPONENTTYPE::TEX_UIICON: return "UI_Icon";

    // Tile Texture Components
    case COMPONENTTYPE::TEX_TILE_284: return "TexTile284";
    case COMPONENTTYPE::TEX_TILE_1024: return "TexTile1024";

    // Collider Components
    case COMPONENTTYPE::RECT_COLL: return "RectColl";
    case COMPONENTTYPE::CIRCLE_COLL: return "CircleColl";
    case COMPONENTTYPE::BOX_COLL: return "BoxColl";
    case COMPONENTTYPE::SPHERE_COLL: return "SphereColl";

    // End Marker
    case COMPONENTTYPE::COMPONENT_END: return "ComponentEnd";

    case COMPONENTTYPE::CONTROLLER_PLAYER: return "PlayerController";
    case COMPONENTTYPE::WEAPON_EQUIP: return "WeaponEquip";

    default:
        return "UnknownComponent";
    }
}

static const char* ToString(LAYERTYPE type)
{
    switch (type)
    {
    case LAYERTYPE::ENVIRONMENT: return "Environment";
    case LAYERTYPE::GAMELOGIC: return "GameLogic";
    case LAYERTYPE::CAMERA: return "Camera";
    case LAYERTYPE::UI: return "UI";
    case LAYERTYPE::LAYER_END: return "LayerEnd";

    default:
        return "UnknownLayerType";
    }
}

static const char* ToString(OBJTYPE type)
{
    switch (type)
    {
    case OBJTYPE::PLAYER:       return "Player";
    case OBJTYPE::MONSTER:      return "Monster";
    case OBJTYPE::CAMERA:       return "Camera";
    case OBJTYPE::ITEM:         return "Item";
    case OBJTYPE::UI:           return "Ui";
    case OBJTYPE::BOSS2:        return "Boss2";
    case OBJTYPE::HUMANMONSTER: return "HumanMonster";
    case OBJTYPE::TERRAIN:      return "Terrain";

    default:
        return "UnknownObjectType";
    }
}

static void SetNameFromComponentType(CGameObject* obj, COMPONENTTYPE type)
{
    if (!obj) return;

    const char* base = ToString(type);   
    wstring wBase = CharToWString(base);

    obj->Set_Name(wBase);  
}


END
