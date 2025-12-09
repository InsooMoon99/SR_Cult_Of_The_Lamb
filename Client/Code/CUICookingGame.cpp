#include "pch.h"
#include "CUICookingGame.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"


CUICookingGame::CUICookingGame(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst)
    : CUI(graphicDev),m_Owner(owner), m_recipeInst(recipeInst)
{
}

CUICookingGame::~CUICookingGame()
{
}

HRESULT CUICookingGame::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;



    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    return S_OK;
}

void CUICookingGame::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    if (m_Owner)
    {
      
        m_OwnerTransform = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    }
}

_int CUICookingGame::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (!m_Owner)
        return exit;


   
    return exit;
}

void CUICookingGame::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUICookingGame::Render_GameObject()
{
    CUI::Render_GameObject();
}

HRESULT CUICookingGame::Add_Component()
{
    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_COOK);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_COOK, m_TextureCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });


    return S_OK;
}

CUICookingGame* CUICookingGame::Create(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst)
{
    CUICookingGame* CookingGame = new CUICookingGame(graphicDev, owner, recipeInst);

    if (FAILED(CookingGame->Ready_GameObject()))
    {
        Safe_Release(CookingGame);
        MSG_BOX("CookingGame Create Failed");
        return nullptr;
    }

    return CookingGame;
}

void CUICookingGame::Free()
{

    Safe_Release(m_Owner);

    CUI::Free();
}
