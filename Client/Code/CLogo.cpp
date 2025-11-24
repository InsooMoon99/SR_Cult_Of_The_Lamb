#include "pch.h"
#include "CLogo.h"

#include "CDInputMgr.h"
#include "CEdit.h"
#include "CFontMgr.h"
#include "CLayer.h"
#include "CLoading.h"
#include "CMainBG.h"
#include "CManagement.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CRcTex.h"
#include "CStage.h"
#include "CTexture.h"
#include "CMainMenu.h"

CLogo::CLogo(DEVICE graphicDev)
    : Engine::CScene(graphicDev),
      m_Loading(nullptr)
{ }

CLogo::~CLogo()
{ }

HRESULT CLogo::Ready_Scene()
{
    if (FAILED(Ready_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_Environment_Layer(LAYERTYPE::ENVIRONMENT)))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(LAYERTYPE::UI)))
        return E_FAIL;

    m_Loading = CLoading::Create(m_GraphicDev, CLoading::LOADING_STAGE);

    if (nullptr == m_Loading)
        return E_FAIL;

    return S_OK;
}

_int CLogo::Update_Scene(const _float& timeDelta)
{
    _int exit = Engine::CScene::Update_Scene(timeDelta);

    // 로딩 작업 완료시 다음 스테이지 진행 가능
    //if (true == m_Loading->Get_Finish())
    //{
    //    // Stage
    //    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_1) & 0x80)
    //    {
    //        CScene* pStage = CStage::Create(m_GraphicDev);
    //        CManagement::GetInstance()->Request_ChangeScene(pStage);
    //    }
    //    // Editor
    //    else if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_2) & 0x80)
    //    {
    //        CScene* pEdit = CEdit::Create(m_GraphicDev);
    //        CManagement::GetInstance()->Request_ChangeScene(pEdit);
    //    }
    //}
     if (true == m_Loading->Get_Finish())
    //if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_1) & 0x80)
    {
        CScene* menu = CMainMenu::Create(m_GraphicDev);
        CManagement::GetInstance()->Request_ChangeScene(menu);
    }

    return exit;
}

void CLogo::LateUpdate_Scene(const _float& timeDelta)
{
    Engine::CScene::LateUpdate_Scene(timeDelta);
}

void CLogo::Render_Scene()
{
    // debug 용
    _vec2 pos{ 100.f, 100.f };

    CFontMgr::GetInstance()->Render_Font(L"견명조",
                                         m_Loading->Get_String(),
                                         &pos,
                                         D3DXCOLOR(1.f, 0.f, 1.f, 1.f));
}

HRESULT CLogo::Ready_Environment_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN(layer, E_FAIL)

    Engine::CGameObject* gameObject = nullptr;

    //gameObject = CMainBG::Create(m_GraphicDev);

    //NULL_CHECK_RETURN(m_GraphicDev, E_FAIL)

    //if (FAILED(layer->Add_GameObject(OBJTYPE::PLAYER, gameObject)))
    //    return E_FAIL;

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo::Ready_UI_Layer(LAYERTYPE layerType)
{
    auto layer = Engine::CLayer::Create();

    NULL_CHECK_RETURN(layer, E_FAIL)

    //  CGameObject* gameObject = nullptr;
    //
    //  gameObject = CUIPlayerPanel::Create(m_GraphicDev);
    // 
    //  NULL_CHECK_RETURN(gameObject, E_FAIL)
    // 
    //  if(FAILED(layer->Add_GameObject(OBJTYPE::UI,gameObject)))

    m_Layers.insert({ layerType, layer });

    return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
    // 로고 내 사용하는 프로토타입은 여기서만 추가

    // Buffer : Rect UV
    if (FAILED(CProtoMgr::GetInstance()
        ->Ready_Prototype(COMPONENTTYPE::RC_TEX, Engine::CRcTex::Create(m_GraphicDev))))
        return E_FAIL;

    return S_OK;
}

CLogo* CLogo::Create(DEVICE graphicDev)
{
    auto logo = new CLogo(graphicDev);

    if (FAILED(logo->Ready_Scene()))
    {
        MSG_BOX("Logo Create Failed");
        Safe_Release(logo);
        return nullptr;
    }

    return logo;
}

void CLogo::Free()
{
    Safe_Release(m_Loading);

    Engine::CScene::Free();
}
