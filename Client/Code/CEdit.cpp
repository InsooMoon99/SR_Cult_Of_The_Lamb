#include "pch.h"
#include "CEdit.h"

#include "CLayer.h"
#include "CMainCamera.h"
#include "CMonster.h"
#include "CPersistentObjectHelper.h"
#include "CPlayer.h"
#include "CState.h"
#include "CTerrain.h"
#include "CTransform.h"
#include "CDInputMgr.h"
#include "CMainEditorMgr.h"
#include "CMainMenu.h"
#include "CSceneMgr.h"
#include "CWall.h"
#include "CSphereCollider.h"
#include "CWallRenderer.h"
#include "CBoxCollider.h"

CEdit::CEdit(DEVICE graphicDev)
    : Engine::CScene(graphicDev)
{ }

CEdit::~CEdit()
{ }

HRESULT CEdit::Ready_Scene()
{
    // 로딩 스레드로 역할 이동
    //if (FAILED(Ready_Prototype()))
    //    return E_FAIL;

    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    return S_OK;
}

_int CEdit::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

   
    return exit;
}

void CEdit::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);


}

void CEdit::Render_Scene()
{
    // debug 용
}

HRESULT CEdit::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    CGameObject* gameObject = nullptr;
    // -----------------------------
    // Tile
    // -----------------------------
    CTerrain* terrain = CTerrain::Create(m_GraphicDev);

    NULL_CHECK_RETURN_MSG(terrain, E_FAIL, L"Terrain create failed");
    terrain->LoadMapOnStart("../Bin//Resource/Data/Terrain/Terrain_WallTest1.map");

    layer->Add_GameObject(OBJTYPE::TERRAIN, terrain);

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CEdit::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN_MSG(
        layer,
        E_FAIL,
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Create() returned null");

    Engine::CGameObject* gameObject = nullptr;

    // -----------------------------
    // Player
    // -----------------------------

    // 플레이어 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CPlayer>(
            OBJTYPE::PLAYER,
            L"Player",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed");
    
    // 플레이어의 Transform 가져오기
    const auto playerTransform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));
    
    // -----------------------------
    // Camera
    // -----------------------------
    
    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA,
            L"MainCamera",
            m_GraphicDev,
            layer,
            &gameObject),
        L"Persistent object setup failed");
    
    // 카메라 타겟을 플레이어로 설정
    auto mainCam = dynamic_cast<CMainCamera*>(gameObject);
    if (mainCam)
    {
        mainCam->Set_CamTarget(playerTransform);

        auto camCom = dynamic_cast<CCameraCom*>(
            mainCam->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAMERA));

        if (camCom)
            CMainEditorMgr::GetInstance()->Set_MainCamera(camCom); // 에디터에 카메라 설정
    }

    // -----------------------------
    // Monster
    // -----------------------------

    // monster
    //gameObject = CMonster::Create(m_GraphicDev);
    //
    //if (nullptr == gameObject)
    //    return E_FAIL;
    //
    //if (FAILED(layer->Add_GameObject(OBJTYPE::MONSTER, gameObject)))
    //    return E_FAIL;

    //  //testMonster
    /*gameObject = CTestMonster::Create(m_GraphicDev);

    NULL_CHECK_RETURN_MSG(
        gameObject,
        E_FAIL,
        L"CStage::Ready_GameLogic_Layer() failed: CMonster::Create() returned null");

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::BOSS2, gameObject),
        L"CStage::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(BOSS2) failed");*/

    m_Layers.insert({ layerType, layer });

    return S_OK;
 
}

HRESULT CEdit::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CEdit::Ready_Prototype()
{
    // Move Loading Scene

    return S_OK;
}

CEdit* CEdit::Create(DEVICE graphicDev)
{
    auto stage = new CEdit(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CEdit::Free()
{
    Engine::CScene::Free();
}
