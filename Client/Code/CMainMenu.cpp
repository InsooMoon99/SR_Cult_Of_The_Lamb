#include "pch.h"
#include "CMainMenu.h"
#include "CDInputMgr.h"
#include "CEdit.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CMainCamera.h"
#include "CMenuChar.h"
#include "CPersistentObjectHelper.h"
#include "CSceneMgr.h"
#include "CStage.h"
#include "CTransform.h"
#include "CUIIcon.h"
#include "CUIButton.h"
#include "CMainBG.h"
#include "CFontMgr.h"

CMainMenu::CMainMenu(DEVICE graphicDev)
    : Engine::CScene(graphicDev)
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
void CMainMenu::RenderFont(const wstring& font, const  wstring &text, CUIButton* btt)
{
    _vec2 pos{ btt->Get_Transform()->Get_Pos().x , btt->Get_Transform()->Get_Pos().y };

    _vec2 screen = { pos.x + WINCX * 0.5f ,WINCY * 0.5f - pos.y };


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
    playerTransform->Set_Pos({ 0,0,2 });
    //playerTransform->Set_Scale({ 2.2f,1.2f,0.f });
    pGameObject->SetAspectScale(playerTransform, { 1.1f,1.2f,1.f });
        if (FAILED(layer->Add_GameObject(OBJTYPE::PLAYER, pGameObject)))
            return E_FAIL;


    m_Layers.insert({ layerType, layer });          // scene -> map

    return S_OK;
}

HRESULT CMainMenu::Ready_GameLogic_Layer(LAYERTYPE layerType)
{
    Engine::CLayer* layer = Engine::CLayer::Create();

    Engine::CGameObject* pGameObject = nullptr;

    pGameObject = CMenuChar::Create(m_GraphicDev);
    NULL_CHECK_RETURN_MSG(pGameObject, E_FAIL, L"CMainMenu::Ready_GameLogic_Layer() failed: CMenuChar::Create() returned null")

    // TODO ↓ 임시로 구성한, 메인 화면 애니메이션 재생용 더미 플레이어. 나중에 수정할 것
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, pGameObject),
        L"CMainMenu::Ready_GameLogic_Layer() failed: CLayer::Add_GameObject(CMenuChar) failed");
    //const auto playerTransform =
    //    dynamic_cast<CTransform*>(pGameObject->Get_Component(
    //        ID_DYNAMIC,
    //        COMPONENTTYPE::TRANSFORM));
    //playerTransform->Set_Pos({ 0,0,20 });


    // 카메라
    FAILED_CHECK_MSG(
        Engine::AcquirePersistentObject<CMainCamera>(
            OBJTYPE::CAMERA,
            m_GraphicDev,
            layer,
            &pGameObject),
        L"Persistent object setup failed");

    // 카메라 타겟을 null로 설정
    dynamic_cast<CMainCamera*>(pGameObject)->Set_CamTarget(nullptr);

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
    pGameObject->SetCloneTexAspectScale({.2f,.2f, 1 });
    pGameObject->Get_Transform()->Set_Pos(-WINCX/3, WINCY / 4.f, 1);
   // pGameObject = CUIIcon::Create(m_GraphicDev);

    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, pGameObject),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    float scaleX = 0.20f;
    float scaleY = 0.25f;
    // 버튼.
    bt = CUIButton::Create(m_GraphicDev,1,0);
    //bt->Set_TextureIndex(1,0);
    bt->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt->Get_Transform()->Set_Pos(-WINCX / 3, -0, .3);
    bt->SetOnClick([]() {CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_STAGE);});
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
     // 버튼
    bt1 = CUIButton::Create(m_GraphicDev,1,0);
    //bt1->Set_TextureIndex(1, 0);
    bt1->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt1->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 18, .3);
    bt1->SetOnClick([]() {CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_EDIT);});
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt1),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
    // 버튼
    bt2 = CUIButton::Create(m_GraphicDev,1,0);
    //bt2->Set_TextureIndex(1, 0);
    bt2->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt2->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 9, .3);
    bt2->SetOnClick([]() {CSceneMgr::GetInstance()->Change_SceneMgr(SCENETYPE::SC_BOSS_STAGE);});
    FAILED_CHECK_MSG(
        layer->Add_GameObject(OBJTYPE::UI, bt2),
        L"CMainMenu::Ready_UI_Layer() failed: CLayer::Add_GameObject(CUIIcon) failed");
    ///////////////////////////////////////////////////////////////////////////////////
        // 버튼
    bt3 = CUIButton::Create(m_GraphicDev,1,0);
    bt3->Set_TextureIndex(1, 0);
    bt3->SetCloneTexAspectScale({ scaleX, scaleY, 1 });
    bt3->Get_Transform()->Set_Pos(-WINCX / 3, -WINCX / 6, .3);
    bt3->SetOnClick([]() {DestroyWindow(g_hWnd);});
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
    Engine::CScene::Free();
}
