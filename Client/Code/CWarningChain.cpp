#include "pch.h"
#include "CWarningChain.h"
#include "CAnimator.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CAttackBox.h"
#include "CBossChain.h"
#include "CBossPhase1.h"

CWarningChain::CWarningChain(DEVICE graphicDev, CGameObject* owner)
    : CGameObject(graphicDev), m_pOwner(owner)
{
    m_pOwner->AddRef();
}

CWarningChain::CWarningChain(const CWarningChain& rhs) : CGameObject(rhs)
{
}

CWarningChain::~CWarningChain()
{
}

HRESULT CWarningChain::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    SetTexAspectInit(m_TextureCom, m_TransformCom, 0);

    SetAspectScale(m_TransformCom, { .5f,.5f,0.f });

    const auto ownerTransform =
        dynamic_cast<CTransform*>(m_pOwner->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    //TODO:: 나중에 땅의 위치를 알게 되면 수정 
    m_TransformCom->Set_Pos({ ownerTransform->Get_Pos().x, -ownerTransform->Get_Scale().y * .5f,ownerTransform->Get_Pos().z });



    return S_OK;

}

_int CWarningChain::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    m_fElapsedTime += timeDelta;

    CBossChain* chain = nullptr;
    if (m_fElapsedTime > 0.5f)
    {

        CBossPhase1* owner = dynamic_cast<CBossPhase1*>(m_pOwner);
        chain = CBossChain::Create(owner->Get_GraphicDev(), owner);
        const auto ChainTransform =
            dynamic_cast<CTransform*>(chain->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));
        ChainTransform->Set_Pos(m_TransformCom->Get_Pos());

        SpawnAttackObject(chain);
    }
    if (chain != nullptr)
    {

    const auto ChainAni =
        dynamic_cast<CAnimator*>(chain->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    if (ChainAni->Get_CurFrame() == ChainAni->Get_MaxFrame() - 2) // 혹시 사슬객체가 먼저 사라지는지 확인 필요
    {
        m_bFade = true;
    }

    if (m_bFade)
    {
        m_fAlpha -= timeDelta * 0.3;
        if (m_fAlpha == 0)
        {
            m_fAlpha = 0;
            Mark_As_PendingKill();
        }
    }
    }
    // 해당 프레임이 끝나면 자기삭제

    return exit;
}

void CWarningChain::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CWarningChain::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    //


    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
    //

    m_TextureCom->Set_Texture(0);

    m_GraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((int)(m_fAlpha * 255), 255, 255, 255));

    m_BufferCom->Render_Buffer();
    //
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


}

void CWarningChain::OnBeginOverlap(CCollider* self, CCollider* other)
{
}

void CWarningChain::OnEndOverlap(CCollider* self, CCollider* other)
{
}

void CWarningChain::Animation_Setting()
{
}

HRESULT CWarningChain::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_OUT);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_OUT, m_TextureCom });



    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    return S_OK;
}

CWarningChain* CWarningChain::Create(DEVICE graphicDev, CGameObject* owner)
{
    auto WarningChain = new CWarningChain(graphicDev, owner);

    if (FAILED(WarningChain->Ready_GameObject()))
    {
        MSG_BOX("WarningChain Create Failed");
        Safe_Release(WarningChain);
        return nullptr;
    }

    return WarningChain;
}

void CWarningChain::Free()
{
    CGameObject::Free();
}
