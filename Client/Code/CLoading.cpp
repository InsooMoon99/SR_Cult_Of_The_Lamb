#include "pch.h"
#include "CLoading.h"

#include "CAnimator.h"
#include "CBoxCol.h"
#include "CBoxCollider.h"
#include "CBoxTex.h"
#include "CCameraCom.h"
#include "CCombatStat.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CPlayerController.h"
#include "CProtoMgr.h"
#include "CRcCol.h"
#include "CRectCollider.h"
#include "CState.h"
#include "CTerrain.h"
#include "CTerrainRenderer.h"
#include "CTerrainTex.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CTriCol.h"
#include "CWeaponEquip.h"
#include "ItemData.h"
#include "ItemInstance.h"
#include "CWeaponEquip.h"
#include "CEnumHelper.h"

CLoading::CLoading(DEVICE pGraphicDev)
    : m_GraphicDev(pGraphicDev),
    m_Thread(nullptr),
    m_Crt{},
    m_LoadingID(LOADING_END),
    m_IsFinish(false)
{
    m_GraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
    // 스레드 동기화를 위한 임계영역(크리티컬 섹션) 초기화
    InitializeCriticalSection(&m_Crt);

    // 로딩 스레드 생성
    m_Thread = (HANDLE)_beginthreadex(
        nullptr,	        // 보안속성(핸들의 상속 여부, NULL인 경우 상속에서 제외)
        0,		            // 스택 크기 (0 = OS 기본값 사용)
        Thread_Main,		// 쓰레드 함수(__stdcall 호출 규약)
        this,	            // 쓰레드 함수를 이용하여 가공할 데이터 주소
        0,		            // 쓰레드 생성 및 실행을 조절하는 flag
        nullptr);	        // 쓰레드 id 반환

    m_LoadingID = eID;

    return S_OK;
}

_uint CLoading::Loading_ForState()
{
    auto pProtoMgr = CProtoMgr::GetInstance();

    m_LoadingText = L"Buffer LOADING..........";

    // Buffer : Triangle Color
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TRI_COLOR, Engine::CTriCol::Create(m_GraphicDev))))
        return E_FAIL;

    // Buffer : Rect Color
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::RC_COLOR, Engine::CRcCol::Create(m_GraphicDev))))
        return E_FAIL;

    // Buffer : Box Color
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_COLOR, Engine::CBoxCol::Create(m_GraphicDev))))
        return E_FAIL;

    // Buffer : Box Tex
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_TEX, Engine::CBoxTex::Create(m_GraphicDev))))
        return E_FAIL;

    // Buffer : Terrain Tex
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TERRAIN_TEX, Engine::CTerrainTex::Create(m_GraphicDev, TILE_CNT_X, TILE_CNT_Z, 1))))
        return E_FAIL;

    // Buffer : Rect UV
    // CLogo 에서 이미 프로토타입 생성중이므로 비활성화
    //if (FAILED(pProtoMgr->Ready_Prototype(
    //    COMPONENTTYPE::RC_TEX, Engine::CRcTex::Create(m_GraphicDev))))
    //    return E_FAIL;

    m_LoadingText = L"Texture LOADING..........";

#pragma region ------   Player Textures     ------

    CTexture* playerTex = CTexture::Create(m_GraphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(playerTex, E_FAIL);

    if (FAILED(playerTex->Add_Texture(L"PlayerIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Player_Idle/Lamb-idle%d.png", 150)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LEFT", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Palyer_RunHorizontal/Lamb-run-horizontal%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_UP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Player_RunUp/Lamb-run-up%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_DOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Player_RunDown/Lamb-run-down%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LUP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Player_RunUpDiagonal/Lamb-run-up-diagonal%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRun_LDOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Player_Run/Lamb-run%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_LEFT", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Palyer_Roll/Lamb-roll%d.png", 19)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_UP", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Palyer_RollUp/Lamb-roll-up%d.png", 16)))
        return E_FAIL;

    if (FAILED(playerTex->Add_Texture(L"PlayerRoll_DOWN", TEX_NORMAL,
        L"../Bin/Resource/Texture/Player/Palyer_RollDown/Lamb-roll-down%d.png", 16)))
        return E_FAIL;

    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_PLAYER, playerTex)))
        return E_FAIL;

#pragma endregion

    //Texture : Player Heart
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(COMPONENTTYPE::TEX_UI_HEART,
            Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL,
                L"../Bin/Resource/Texture/UI/PlayerState/Hp/Hp%d.png", 3))))
        return E_FAIL;

    // Boss2 Test
    //CTexture* bossTex = CTexture::Create(m_GraphicDev, TEX_NORMAL, L"", 0);
    //NULL_CHECK_RETURN(bossTex, E_FAIL);
    //
    //if (FAILED(bossTex->Add_Texture(L"BossIdle", TEX_NORMAL,
    //    L"../Bin/Resource/Texture/Test/Boss2_Idle/Boss2_Idle%d.png", 40)))
    //    return E_FAIL;
    //
    //if (FAILED(pProtoMgr->Ready_Prototype(
    //    COMPONENTTYPE::TEX_MONSTER, bossTex)))
    //    return E_FAIL;

    //HumanMonster
    CTexture* humanmonsterTex = CTexture::Create(m_GraphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(humanmonsterTex, E_FAIL);

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterIdle", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Idle/Idle%d.png", 34)))
        return E_FAIL;

    if (FAILED(humanmonsterTex->Add_Texture(L"HumanMonsterRun", TEX_NORMAL,
        L"../Bin/Resource/Texture/Monster/Human/Run/Run%d.png", 18)))
        return E_FAIL;

    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_HUMANMONSTER, humanmonsterTex)))
        return E_FAIL;

    //아이템
    vector<Item>    item = CItemDB::GetInstance()->GetVector();
    vector<wstring> pathVec;
    for (int i = 0; i < item.size(); i++)
    {
        wstring path = CItemDB::GetInstance()->Utf8ToWstring(item[i].UIPath);
        pathVec.push_back(path);
    }

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(
        COMPONENTTYPE::TEX_ITEM, Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL, pathVec))))
        return E_FAIL;

    // Terrain
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_TILE_284, CTexture::Create(m_GraphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/Terrain/TileAtlas_284.png", 1))))
        return E_FAIL;

    // Grass
    /*if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_GRASS, CTexture::Create(m_GraphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/"))))
        return E_FAIL;*/

        //circleColorTex
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(COMPONENTTYPE::TEX_UI_COLOR,
            Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL,
                L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Circle1.png", 1))))
        return E_FAIL;
    //circleTex
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(COMPONENTTYPE::TEX_UI_CIRCLE,
            Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL,
                L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Circle_Red.png", 1))))
        return E_FAIL;

    //circleTex
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(COMPONENTTYPE::TEX_UI_WEAPON,
            Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL,
                L"../Bin/Resource/Texture/UI/PlayerState/Icon/Weapon%d.png", 2))))
        return E_FAIL;

    // 메인 메뉴 이미지

    CTexture* menuChar = CTexture::Create(m_GraphicDev, TEX_NORMAL, L"", 0);
    NULL_CHECK_RETURN(menuChar, E_FAIL);

    if (FAILED(menuChar->Add_Texture(L"MenuChar", TEX_NORMAL,
        L"../Bin/Resource/Texture/MainMenu/MenuChar/Main menu-animation%d.png", 100)))
        return E_FAIL;

    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_MANUCHAR, menuChar)))
        return E_FAIL;

    //UIICON 텍스쳐

    vector<wstring> pathVec1;
    pathVec1.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Title0.png");
	pathVec1.push_back(L"../Bin/Resource/Texture/UI/PlayerState/Sermon/Sermon_Icon.png");

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(
        COMPONENTTYPE::TEX_UIICON, Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL, pathVec1))))
        return E_FAIL;

    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_SKILLCOUNT, CTexture::Create(m_GraphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/PlayerState/Skill/Skillcount%d.png",2))))
        return E_FAIL;


    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_CELL, CTexture::Create(m_GraphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/InventoryIcon_BG.png", 1))))
        return E_FAIL;

    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEX_UI_HIGHLIGHT, CTexture::Create(m_GraphicDev, TEX_NORMAL,
            L"../Bin/Resource/Texture/UI/Inventory/Outline%d.png", 2))))
    //UIBUTTON 텍스쳐
    vector<wstring> pathVec2;
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Null.png");
    pathVec2.push_back(L"../Bin/Resource/Texture/UI/MainMenu/Button Black.png");


    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(
        COMPONENTTYPE::TEX_UIBUTTON, Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL, pathVec2))))
        return E_FAIL;

    //// Texture : MainMenu BackGround
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(COMPONENTTYPE::TEX_MAINBG,
            Engine::CTexture::Create(
                m_GraphicDev,
                TEX_NORMAL,
                L"../Bin/Resource/Texture/BackGround/MainMenu/MenuMask1.png",
                1))))
        return E_FAIL;

#pragma endregion

    m_LoadingText = L"Etc LOADING..........";

    // Transform
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TRANSFORM, Engine::CTransform::Create(m_GraphicDev))))
        return E_FAIL;

    // Animator
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::ANIMATOR, CAnimator::Create(m_GraphicDev))))
        return E_FAIL;

    // Camera
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::CAMERA, Engine::CCameraCom::Create(m_GraphicDev))))
        return E_FAIL;

    // Rect Collider
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::RECT_COLL, CRectCollider::Create(m_GraphicDev))))
        return E_FAIL;

    // Box Collider
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::BOX_COLL, CBoxCollider::Create(m_GraphicDev))))
        return E_FAIL;

    // State
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::STATE, CState::Create(m_GraphicDev))))
        return E_FAIL;

    // CombatStat
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::COMBATSTAT, CCombatStat::Create(m_GraphicDev))))
        return E_FAIL;

    // Inventory
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::INVENTORY, CInventory::Create(m_GraphicDev))))
        return E_FAIL;

    // TerrainRenderer
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TERRAIN_RENDER, CTerrainRenderer::Create(m_GraphicDev))))
        return E_FAIL;

    // Player Controller
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::CONTROLLER_PLAYER, CPlayerController::Create(m_GraphicDev))))
        return E_FAIL;

    // weapon equip
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::WEAPON_EQUIP, CWeaponEquip::Create(m_GraphicDev))))
        return E_FAIL;

    m_LoadingText = L"COMPLETE: PRESS 1 -> Stage|PRESS 2 -> Editor";

    m_IsFinish = true;

    return S_OK;
}

unsigned CLoading::Thread_Main(void* pArg)
{
    auto pLoading = static_cast<CLoading*>(pArg);

    _uint iFlag(0);

    // 임계영역 진입 (다른 스레드의 접근 차단)
    EnterCriticalSection(pLoading->Get_Crt());

    switch (pLoading->Get_LoadingID())
    {
    case LOADING_STAGE:
        iFlag = pLoading->Loading_ForState();
        break;
    case LOADING_BOSS:
        break;
    default:
        break;
    }

    // 임계영역 해제 (다른 스레드 접근 허용)
    LeaveCriticalSection(pLoading->Get_Crt());

    //_endthreadex(0);
    // -> _beginthreadex()로 생성된 스레드는 함수가 return 될 때
    // 내부적으로 _endthreadex() 가 자동 호출되어 정리됨.
    // 따라서 명시적으로 호출할 필요가 없음

    return iFlag;
}

CLoading* CLoading::Create(DEVICE pGraphicDev, LOADINGID eID)
{
    auto pLoading = new CLoading(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID)))
    {
        Safe_Release(pLoading);

        MSG_BOX("Loading Create Failed");
        return nullptr;
    }

    return pLoading;
}

void CLoading::Free()
{
    // 생성한 스레드가 완전히 끝날 때까지 대기
    WaitForSingleObject(m_Thread, INFINITE);

    // 스레드 핸들 닫기 (핸들 리소스 해제)
    CloseHandle(m_Thread);

    // 크리티컬 섹션 삭제 (운영체제 리소스 해제)
    DeleteCriticalSection(&m_Crt);

    Safe_Release(m_GraphicDev);
}
