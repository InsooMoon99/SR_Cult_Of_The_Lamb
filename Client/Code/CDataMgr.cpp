#include "pch.h"
#include "CDataMgr.h"

#include <CBoxCol.h>
#include <CBoxTex.h>
#include <CRcCol.h>

#include "CItemDB.h"
#include "CTriCol.h"

#include "CResourceManager.h"
#include "CProtoMgr.h"

#include "CTransform.h"
#include "CRcTex.h"
#include "CTerrainTex.h"
#include "CTexture.h"
#include "ItemData.h"


USING(Engine)

IMPLEMENT_SINGLETON(CDataMgr)

CDataMgr::CDataMgr()
{
}

CDataMgr::~CDataMgr()
{
    Free();
}

HRESULT CDataMgr::Ready_Data(DEVICE graphicDev)
{
    if (FAILED(Ready_Sound_Resource()))
        return E_FAIL;

    // 공통 리소스
    if (FAILED(Ready_Common_Resource(graphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Player_Resource(graphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Monster_Resource(graphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Boss_Resource(graphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Item_Resource(graphicDev)))
        return E_FAIL;

    if (FAILED(Ready_UI_Resource(graphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Map_Resource(graphicDev)))
        return E_FAIL;

    // 컴포넌트 프로토타입 등록
    if (FAILED(Ready_Prototype(graphicDev)))
        return E_FAIL;

    return S_OK;
}

// 엔진에서 공용으로 쓸 컴포넌트 프로토타입 등록
HRESULT CDataMgr::Ready_Prototype(DEVICE graphicDev)
{
    auto* pProtoMgr = CProtoMgr::GetInstance();
    NULL_CHECK_RETURN(pProtoMgr, E_FAIL);

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

    // TerrainTex
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TERRAIN_TEX, CTerrainTex::Create(graphicDev, 37, 37, 1))))
        return E_FAIL;

    // Buffer : Rect UV
    // CLogo 에서 이미 프로토타입 생성중이므로 비활성화
    //if (FAILED(pProtoMgr->Ready_Prototype(
    //    COMPONENTTYPE::RC_TEX, Engine::CRcTex::Create(m_GraphicDev))))
    //    return E_FAIL;

    // Transform
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TRANSFORM,
        CTransform::Create(graphicDev))))
    {
        MSG_BOX("Transform Prototype Create Failed");
        return E_FAIL;
    }

    // 실제 텍스처 리소스는 ResourceManager에 들어있고
    // CTexture는 그 리소스를 키/인덱스로 참조하는 역할
    if (FAILED(pProtoMgr->Ready_Prototype(
        COMPONENTTYPE::TEXTURE,
        CTexture::Create(graphicDev, TEX_NORMAL, L"", 0))))
    {
        MSG_BOX("Texture Prototype Create Failed");
        return E_FAIL;
    }

    // TODO: 나중에 Collider, Animator, State, CombatStat 등도 여기서 등록
    // if (FAILED(pProtoMgr->Ready_Prototype(COMPONENTTYPE::RECT_COLL, CRectCollider::Create(graphicDev)))) ...

    return S_OK;
}

HRESULT CDataMgr::Ready_Common_Resource(DEVICE graphicDev)
{
    CResourceManager& Res = *CResourceManager::GetInstance();

    // if (FAILED(Res.Add_Sprite(graphicDev, L"Main_Title",
    //     L"./Bin/Resource/Texture/UI/Main/MainTitle_%d.png", 1)))
    //     return E_FAIL;

    return S_OK;
}

HRESULT CDataMgr::Ready_Player_Resource(DEVICE graphicDev)
{
    auto resource = CResourceManager::GetInstance();

    // Player Idle
    if (FAILED(resource->Add_Sprite(graphicDev, L"Player_Idle",
        L"./Bin/Resource/Texture/Player/Player_Idle/Lamb-idle%d.png", 150)))
    {
        MSG_BOX("Player Idle Texture Load Failed");
        return E_FAIL;
    }

    // Player Run
    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRun_LEFT",
        L"../Bin/Resource/Texture/Player/Palyer_RunHorizontal/Lamb-run-horizontal%d.png", 19)))
    {
        MSG_BOX("Player Run Left Texture Load Failed");
        return E_FAIL;
    }

    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRun_UP",
        L"../Bin/Resource/Texture/Player/Player_RunUp/Lamb-run-up%d.png", 19)))
    {
        MSG_BOX("Player Run Up Texture Load Failed");
        return E_FAIL;
    }

    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRun_DOWN",
        L"../Bin/Resource/Texture/Player/Player_RunDown/Lamb-run-down%d.png", 19)))
    {
        MSG_BOX("Player Run Down Texture Load Failed");
        return E_FAIL;
    }
        
    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRun_LUP",
        L"../Bin/Resource/Texture/Player/Player_RunUpDiagonal/Lamb-run-up-diagonal%d.png", 19)))
    {
        MSG_BOX("Player Run LUP Texture Load Failed");
        return E_FAIL;
    }

    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRun_LDOWN",
        L"../Bin/Resource/Texture/Player/Player_Run/Lamb-run%d.png", 19)))
    {
        MSG_BOX("Player Run LDOWN Texture Load Failed");
        return E_FAIL;
    }

    // Player Roll
    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRoll_LEFT",
        L"../Bin/Resource/Texture/Player/Palyer_Roll/Lamb-roll%d.png", 19)))
    {
        MSG_BOX("Player Roll Left Texture Load Failed");
        return E_FAIL;
    }

    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRoll_UP",
        L"../Bin/Resource/Texture/Player/Palyer_RollUp/Lamb-roll-up%d.png", 16)))
    {
        MSG_BOX("Player Roll Up Texture Load Failed");
        return E_FAIL;
    }


    if (FAILED(resource->Add_Sprite(graphicDev, L"PlayerRoll_DOWN",
        L"../Bin/Resource/Texture/Player/Palyer_RollDown/Lamb-roll-down%d.png", 16)))
    {
        MSG_BOX("Player Roll Down Texture Load Failed");
        return E_FAIL;
    }
    

    return S_OK;
}

HRESULT CDataMgr::Ready_Monster_Resource(DEVICE graphicDev)
{
    auto resource = CResourceManager::GetInstance();

    if (FAILED(resource->Add_Sprite(graphicDev, L"HumanMonsterIdle",
        L"../Bin/Resource/Texture/Monster/Human/Idle/Idle%d.png", 34)))
    {
        MSG_BOX("HumanMonster Idle Load Failed");
        return E_FAIL;
    }

    if (FAILED(resource->Add_Sprite(graphicDev, L"HumanMonsterRun",
        L"../Bin/Resource/Texture/Monster/Human/Run/Run%d.png", 18)))
    {
        MSG_BOX("HumanMonster Run Load Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CDataMgr::Ready_Boss_Resource(DEVICE graphicDev)
{
    return S_OK;
}

HRESULT CDataMgr::Ready_Item_Resource(DEVICE graphicDev)
{
    auto resource = CResourceManager::GetInstance();

    //아이템
    /*vector<Item>    item = CItemDB::GetInstance()->GetVector();
    vector<wstring> pathVec;
    for (int i = 0; i < item.size(); i++)
    {
        wstring path = CItemDB::GetInstance()->Utf8ToWstring(item[i].UIPath);
        pathVec.push_back(path);
    }

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(
        COMPONENTTYPE::TEX_ITEM, Engine::CTexture::Create(m_GraphicDev, TEX_NORMAL, pathVec))))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CDataMgr::Ready_UI_Resource(DEVICE graphicDev)
{
    auto resource  = CResourceManager::GetInstance();

    //Texture : Player Heart
    if (FAILED(resource->Add_Sprite(graphicDev, L"UI_HP",
        L"../Bin/Resource/Texture/UI/PlayerState/Hp/Hp%d.png", 3)))
    {
        MSG_BOX("UI HP Texture Load Failed");
        return E_FAIL;
    }

    // 인벤토리 배경
    //if (FAILED(Res.Add_Sprite(graphicDev,
    //    L"UI_Inventory_BG",
    //    L"./Bin/Resource/Texture/UI/Inventory/Inventory_BG%d.png", 1)))
    //{
    //    MSG_BOX("UI Inventory BG Load Failed");
    //    return E_FAIL;
    //}

    // TODO: 아이콘, 버튼, 스킬 슬롯 등 필요한 UI 전부 여기서 추가
    // Res.Add_Sprite(graphicDev, L"UI_Button_Start", L"...%d.png", 1);
    // Res.Add_Sprite(graphicDev, L"UI_SkillSlot",    L"...%d.png", 1);

    return S_OK;
}

HRESULT CDataMgr::Ready_Map_Resource(DEVICE graphicDev)
{
    CResourceManager& Res = *CResourceManager::GetInstance();

    if (FAILED(Res.Add_Sprite(graphicDev,
        L"Tile_Atlas_284",
        L"./Bin/Resource/Texture/Terrain/TileAtlas_284.png",
        1)))
    {
        MSG_BOX("Tile_Atlas_284 Load Failed");
        return E_FAIL;
    }

    // TODO : 마을 배경 / 오브젝트 추가


    return S_OK;
}

HRESULT CDataMgr::Ready_Sound_Resource()
{
    return S_OK;
}

void CDataMgr::Free()
{
    CResourceManager::DestroyInstance();
    // CSoundManager::DestroyInstance();
}
