#include "pch.h"
#include "CMainMenu.h"

#include <CSoundMgr.h>
#include "CCameraController.h"
#include "CCamSetupBuilder.h"
#include "CDInputMgr.h"
#include "CEdit.h"
#include "CFontMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CMainBG.h"
#include "CMainCamera.h"
#include "CMenuChar.h"
#include "CPersistentObjectHelper.h"
#include "CSceneMgr.h"
#include "CStage.h"
#include "CTransform.h"
#include "CUIButton.h"
#include "CUIIcon.h"

CMainMenu::CMainMenu(DEVICE graphicDev)
    : Engine::CScene(graphicDev),

      bt(nullptr),
      bt1(nullptr),
      bt2(nullptr),
      bt3(nullptr)
{ }

CMainMenu::~CMainMenu()
{ }

HRESULT CMainMenu::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(LAYERTYPE::GAMELOGIC)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    // 메인 화면 BGM
    CSoundMgr::GetInstance()->Play_BGM(L"BGM/BGM_Intro.wav", 0.3f);

    return S_OK;
}

_int CMainMenu::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    return exit;
}

void CMainMenu::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CMainMenu::Render_Scene()
{
    RenderFont(L"NanumSquare_Regular", L"플레이", bt);
    RenderFont(L"NanumSquare_Regular", L"맵툴", bt1);
    // RenderFont(L"견명조", L"설정", bt);
    RenderFont(L"NanumSquare_Regular", L"제작진", bt2);
    RenderFont(L"NanumSquare_Regular", L"종료", bt3);
}

void CMainMenu::RenderFont(const wstring& font, const wstring& text, CUIButton* btt)
{
    _vec2 pos{ btt->Get_Transform()->Get_Pos().x, btt->Get_Transform()->Get_Pos().y };

    _vec2 screen = { pos.x + WINCX * 0.5f,WINCY * 0.5f - pos.y };

    if (btt->Get_Hovered() == true)
    {
        CFontMgr::GetInstance()->Render_Font(font,
                                             text,
                                             &screen,
                                             D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }
    else
    {
        CFontMgr::GetInstance()->Render_Font(font,
                                             text,
                                             &screen,
                                             D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
    }
}

HRESULT CMainMenu::Ready_Environment_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    pGameObject = CMainBG::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_GraphicDev, E_FAIL)

    const auto playerTransform =
        dynamic_cast<CTransform*>(pGameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));
    playerTransform->Set_Pos({ 0, 0, 2 });
    //playerTransform->Set_Scale({ 2.2f,1.2f,0.f });
    pGameObject->SetAspectScale(playerTransform, { 1.1f, 1.2f, 1.f });
    if (FAILED(layer->Add_GameObject(OBJTYPE::PLAYER, pGameObject)))
        return E_FAIL;

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CMainMenu::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    Engine::CGameObject* gameObject = nullptr;

    gameObject = CMenuChar::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CMainMenu::Ready_GameLogic_Layer() failed: CMenuChar::Create() returned null")

    // TODO ↓ 임시로 구성한, 메인 화면 애니메이션 재생용 더미 플레이어. 나중에 수정할 것
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, gameObject),
        L"CMainMenu::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CMenuChar) failed");
    //const auto playerTransform =
    //    dynamic_cast<CTransform*>(pGameObject->Get_Component(
    //        ID_DYNAMIC,
    //        COMPONENTTYPE::TRANSFORM));
    //playerTransform->Set_Pos({ 0,0,20 });

    // 카메라 생성
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA, L"MainCamera", m_GraphicDev, layer, &gameObject),
        L"Persistent object setup failed"
    );

    auto camTransform = dynamic_cast<CTransform*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    camTransform->Set_Pos(0.f, 0.f, 0.f);
    camTransform->Set_Rotation({ 0.f, 0.f, 0.f });

    auto camController = dynamic_cast<CCameraController*>(
        gameObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::CAM_CONTROLLER));

    // 카메라 설정값 초기화
    if (camController != nullptr)
    {
        // 씬에서 카메라 생성 시 값 초기화
        // : CCameraSetup을 카메라 설정을 중도에 바꿀 때 사용 가능하고,
        //   그럴 때는 필요한 것만 사용해 .Apply() 하면 된다.
        CCamSetupBuilder(camController)
            .SetTarget(nullptr)
            //.PivotOffset({ 0.f, 0.f, 0.f })
            //.Distance(12.f)
            //.RotX(45.f)
            //.RotY(0.f)
            //.LagSpeed(10.f)
            //.ZoomSpeed(5.f)
            .Snap()
            .Apply();
    }

    if (nullptr == layer)
        return E_FAIL;

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CMainMenu::Ready_UI_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    if (nullptr == layer)
        return E_FAIL;
    //////////////////////////////////////////////////////////////////////////////////
    //아이콘
    CUIIcon* pGameObject = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_UI_ICON);
    pGameObject->Set_TextureIndex(0);
    //pGameObject->Get_Transform()->Set_Scale(262.f / 1.3f,143.f / 1.3f,1.f);
    pGameObject->SetCloneTexAspectScale({ .2f, .2f, 1 });
    pGameObject->Get_Transform()->Set_Pos(-WINCX / 3, WINCY / 4.f, 1);
    // pGameObject = CUIIcon::Create(m_GraphicDev);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, pGameObject),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    float scaleX = 0.20f;
    float scaleY = 0.25f;
    // 버튼.
    bt = CUIButton::Create(m_GraphicDev, 1, 0);
    //bt->Set_TextureIndex(1,0);
    bt->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt->Get_Transform()->Set_Pos(-WINCX / 3, -0, .3);
    bt->SetOnClick([]() { CSceneMgr::GetInstance()->Change_SceneMgr_WithFade(SCENETYPE::SC_STAGE); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    // 버튼
    bt1 = CUIButton::Create(m_GraphicDev, 1, 0);
    //bt1->Set_TextureIndex(1, 0);
    bt1->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt1->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 18, .3);
    bt1->SetOnClick([]() { CSceneMgr::GetInstance()->Change_SceneMgr_WithFade(SCENETYPE::SC_EDIT); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt1),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    // 버튼
    bt2 = CUIButton::Create(m_GraphicDev, 1, 0);
    //bt2->Set_TextureIndex(1, 0);
    bt2->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt2->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 9, .3);
    bt2->SetOnClick([]() { CSceneMgr::GetInstance()->Change_SceneMgr_WithFade(SCENETYPE::SC_BOSS_STAGE1); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt2),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    // 버튼
    bt3 = CUIButton::Create(m_GraphicDev, 1, 0);
    bt3->Set_TextureIndex(1, 0);
    bt3->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt3->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 6, .3);
    bt3->SetOnClick([]() { DestroyWindow(g_hWnd); });
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt3),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");

    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CMainMenu::Ready_Prototype()
{
    return S_OK;
}

CMainMenu* CMainMenu::Create(DEVICE graphicDev)
{
    auto stage = new CMainMenu(graphicDev);

    if (FAILED(stage->Ready_Scene()))
    {
        MSG_BOX("Stage Create Failed");
        Safe_Release(stage);
        return nullptr;
    }

    return stage;
}

void CMainMenu::Free()
{
    CSoundMgr::GetInstance()->Stop_SoundChannel(SOUND_BGM);

    Engine::CScene::Free();
}
