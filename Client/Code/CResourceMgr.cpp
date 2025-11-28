#include "pch.h"
#include "CResourceMgr.h"

#include "CAnimator.h"
#include "CBoxCol.h"
#include "CBoxCollider.h"
#include "CBoxTex.h"
#include "CCameraCom.h"
#include "CCombatStat.h"
#include "CEnumHelper.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CPlayerController.h"
#include "CProtoMgr.h"
#include "CRcCol.h"
#include "CRectCollider.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTerrain.h"
#include "CTerrainRenderer.h"
#include "CTerrainTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CTriCol.h"
#include "CWallRenderer.h"
#include "CWeaponEquip.h"
#include "CWeaponEquip.h"
#include "ItemData.h"
#include "ItemInstance.h"

IMPLEMENT_SINGLETON(CResourceMgr)

CResourceMgr::CResourceMgr()
{ }

CResourceMgr::~CResourceMgr()
{
    CResourceMgr::Free();
}

HRESULT CResourceMgr::Ready_Resource(DEVICE graphicDev)
{
    if (FAILED(Ready_Buffer(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Player(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Monster(graphicDev))) return E_FAIL;
   // if (FAILED(Ready_Boss(graphicDev))) return E_FAIL;
    if (FAILED(Ready_UI(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Item(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Common(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Terrain(graphicDev))) return E_FAIL;
    if (FAILED(Ready_BossP1(graphicDev))) return E_FAIL;
    if (FAILED(Ready_Decoration(graphicDev))) return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_BossP1(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    CTexture* BossP1Tex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(BossP1Tex, E_FAIL);

    if (FAILED(BossP1Tex->Add_Texture(L"BossIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-animation/TOWW-Spirit-animation%d.png", 100)))
        return E_FAIL;
    if (FAILED(BossP1Tex->Add_Texture(L"BossAwake", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-transform2/TOWW-Spirit-transform%d.png", 105)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossAttack", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-shoot/TOWW-Spirit-shoot%d.png", 97)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossDead", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-hurt-talk/TOWW-Spirit-hurt-talk%d.png", 167)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossMove_up", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-appear/TOWW-Spirit-appear%d.png", 50)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossMove_Down", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-disappear/TOWW-Spirit-disappear%d.png", 28)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossGroggy_start", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-hurt/TOWW-Spirit-hurt%d.png", 115)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossGroggy_loop", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-loop/TOWW-Spirit-loop%d.png", 84)))
        return E_FAIL;

    if (FAILED(BossP1Tex->Add_Texture(L"BossGroggy_end", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/TOWW-Spirit-hurt-stop/TOWW-Spirit-hurt-stop%d.png", 67)))
        return E_FAIL;

    // 사슬
    if (FAILED(BossP1Tex->Add_Texture(L"BossChainEff", TEX_NORMAL,
        L"../Bin/Resource/Texture/BossPhase1/ChainAttack/Summon_Chain%d.png", 34)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_BOSSPHASE1, BossP1Tex)))
        return E_FAIL;

	///////////////////////////////////////////////////////////////////////
	
	//FadeOut Tex
	vector<wstring> pathVec1;
	pathVec1.push_back(L"../Bin/Resource/Texture/BossPhase1/ChainAttackGround.png");
	pathVec1.push_back(L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Sermon_Icon.png");


	if (FAILED(protoMgr->Ready_Prototype(
		COMPONENTTYPE::TEX_OUT, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec1))))
		return E_FAIL;

    ///////////////////////////////////////////////////////////////////////

    vector<wstring> pathVec2;
    pathVec2.push_back(L"../Bin/Resource/Texture/BossPhase1/Bullet/Bullet.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/BossPhase1/Bullet/Bullet0.png");


    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_BULLET, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec2))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Common(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Transform
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TRANSFORM, Engine::CTransform::Create(graphicDev))))
        return E_FAIL;

    // Animator
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::ANIMATOR, CAnimator::Create(graphicDev))))
        return E_FAIL;

    // Camera
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::CAMERA, Engine::CCameraCom::Create(graphicDev))))
        return E_FAIL;

    // Rect Collider
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::RECT_COLL, CRectCollider::Create(graphicDev))))
        return E_FAIL;

    // Box Collider
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_COLL, CBoxCollider::Create(graphicDev))))
        return E_FAIL;

    // State
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::STATE, CState::Create(graphicDev))))
        return E_FAIL;

    // CombatStat
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::COMBATSTAT, CCombatStat::Create(graphicDev))))
        return E_FAIL;

    // Inventory
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::INVENTORY, CInventory::Create(graphicDev))))
        return E_FAIL;

    // Player Controller
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::CONTROLLER_PLAYER, CPlayerController::Create(graphicDev))))
        return E_FAIL;

    // weapon equip
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::WEAPON_EQUIP, CWeaponEquip::Create(graphicDev))))
        return E_FAIL;

    // Rigidbody
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::RIGIDBODY, CRigidbody::Create(graphicDev))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Buffer(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Buffer : Triangle Color
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TRI_COLOR, Engine::CTriCol::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Rect Color
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::RC_COLOR, Engine::CRcCol::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Box Color
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_COLOR, Engine::CBoxCol::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Box Tex
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_TEX, Engine::CBoxTex::Create(graphicDev))))
        return E_FAIL;

    // Buffer : Terrain Tex
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TERRAIN_TEX, Engine::CTerrainTex::Create(graphicDev, TILE_CNT_X, TILE_CNT_Z, 1))))
        return E_FAIL;

    // Buffer : Rect UV
    // CLogo 에서 이미 프로토타입 생성중이므로 비활성화
    //if (FAILED(protoMgr->Ready_Prototype(
    //    COMPONENTTYPE::RC_TEX, Engine::CRcTex::Create(graphicDev))))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Player(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    CTexture* playerTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(playerTex, E_FAIL);

    // Idle

    if (FAILED(playerTex->Add_Texture(L"PlayerIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Idle/Lamb-idle%d.png", 150)))
        return E_FAIL;

    // Run

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LEFT", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunHorizontal/Lamb-run-horizontal%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_UP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunUp/Lamb-run-up%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_DOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunDown/Lamb-run-down%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LUP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RunUpDiagonal/Lamb-run-up-diagonal%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LDOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Run/Lamb-run%d.png", 19)))
        return E_FAIL;

    // Roll

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_LEFT", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Roll/Lamb-roll%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_UP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RollUp/Lamb-roll-up%d.png", 16)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_DOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/RollDown/Lamb-roll-down%d.png", 16)))
        return E_FAIL;

    // Sword

    if (FAILED(playerTex->Add_Texture(L"PlayerSwordCombo1", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Sword_Combo1/Lamb-attack-combo1-%d.png", 31)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerSwordCombo2", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Sword_Combo2/Lamb-attack-combo2-%d.png", 38)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerSwordCombo3", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Sword_Combo3/Lamb-attack-combo3-%d.png", 38)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_PLAYER, playerTex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Monster(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    //HumanMonster
    CTexture* humanmonsterTex = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(humanmonsterTex, E_FAIL);

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Idle/Idle%d.png", 34)))
        return E_FAIL;

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterRun", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Run/Run%d.png", 18)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_HUMAN_MONSTER, humanmonsterTex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Boss(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    return S_OK;
}

HRESULT CResourceMgr::Ready_UI(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    //Texture : Player Heart
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_HEART,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Hp/Hp%d.png", 3))))
        return E_FAIL;

    //circleColorTex
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_COLOR,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Circle1.png", 1))))
        return E_FAIL;

    //circleTex
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_CIRCLE,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Circle_Red.png", 1))))
        return E_FAIL;

    //circleTex
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_WEAPON,
        Engine::CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Icon/Weapon%d.png", 2))))
        return E_FAIL;

    // 메인 메뉴 이미지

    CTexture* menuChar = CTexture::Create(graphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(menuChar, E_FAIL);

    if (FAILED(menuChar->Add_Texture(L"MenuChar", TEX_NORMAL,
        L"../Bin/Resource/Texture/MainMenu/MenuChar/Main menu-animation%d.png", 100)))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_MENU_CHAR, menuChar)))
        return E_FAIL;

    //UIICON 텍스쳐
    vector<wstring> pathVec1;
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Title0.png");
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Sermon_Icon.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_ICON, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec1))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_SKILLCOUNT, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Skill/Skillcount%d.png", 2))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_CELL, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/InventoryIcon_BG.png", 1))))
        return E_FAIL;

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_HIGHLIGHT, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/Outline%d.png", 2))))
        return E_FAIL;

    //UIButton 텍스쳐
    vector<wstring> pathVec2;
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Null.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Button Black.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/Inventory/ButtonTab0.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/Inventory/ButtonTab1.png");

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_BUTTON, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec2))))
        return E_FAIL;

    //// Texture : MainMenu BackGround
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_MAINBG,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/BackGround/MainMenu/MenuMask1.png",
            1))))
        return E_FAIL;

    // Texture : Inventory BackGround
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_INVEN,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/Background.png",
            1))))
        return E_FAIL;


    // Texture : Inventory BackGround
    if (FAILED(protoMgr->Ready_Prototype(COMPONENTTYPE::TEX_UI_RITUALCARD,
        Engine::CTexture::Create(
            graphicDev,
            TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/RitualCard.png",
            1))))
        return E_FAIL;



    return S_OK;
}

HRESULT CResourceMgr::Ready_Item(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    //아이템
    vector<Item>    item = CItemDB::GetInstance()->GetVector();
    vector<wstring> pathVec;
    for (int i = 0; i < item.size(); i++)
    {
        wstring path = CItemDB::GetInstance()->Utf8ToWstring(item[i].UIPath);
        pathVec.push_back(path);
    }

    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_ITEM, Engine::CTexture::Create(graphicDev, TEX_NORMAL, pathVec))))
        return E_FAIL;

    return S_OK;
}

HRESULT CResourceMgr::Ready_Terrain(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // TerrainRenderer
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TERRAIN_RENDER, CTerrainRenderer::Create(graphicDev))))
        return E_FAIL;

    // WallRenderer
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::WALL_RENDERER, CWallRenderer::Create(graphicDev))))
        return E_FAIL;

    // Terrain_284
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_284, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileAtlas_284_Blank.png", 1))))
        return E_FAIL;

    // Terrain_1024
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_1024, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileAtlas_1024.png", 1))))
        return E_FAIL;

    // Terrain_Outline
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_OUTLINE, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileOutline%d.png", 3))))
        return E_FAIL;


    return S_OK;
}

HRESULT CResourceMgr::Ready_Decoration(DEVICE graphicDev)
{
    auto protoMgr = CProtoMgr::GetInstance();

    // Grass
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_GRASS, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/WeedShrub/WeedShrub%d.png", 20))))
        return E_FAIL;

    // Rubble
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_RUBBLE, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/Rubble/Rubble%d.png", 6))))
        return E_FAIL;

    // Deco
    if (FAILED(protoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_DECO, CTexture::Create(graphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/MapObject/Deco/Village_Deco%d.png", 77))))
        return E_FAIL;

    return S_OK;
}

void CResourceMgr::Free()
{ }
