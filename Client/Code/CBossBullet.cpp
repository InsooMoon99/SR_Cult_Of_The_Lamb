#include "pch.h"
#include "CBossBullet.h"
#include "CAnimator.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CAttackBox.h"
#include "CBossPhase1.h"

CBossBullet::CBossBullet(DEVICE graphicDev, CGameObject* owner)
    : CGameObject(graphicDev), m_pOwner(owner)
{
        m_pOwner->AddRef();
}

CBossBullet::CBossBullet(const CBossBullet& rhs) : CGameObject(rhs)
{
}

CBossBullet::~CBossBullet()
{
}

HRESULT CBossBullet::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    SetTexAspectInit(m_TextureCom, m_TransformCom, 0);

    SetAspectScale(m_TransformCom, { 1.f,1.f,0.f });

    const auto ownerTransform =
        dynamic_cast<CTransform*>(m_pOwner->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    //TODO:: 나중에 땅의 위치를 알게 되면 수정 
    m_TransformCom->Set_Pos({ ownerTransform->Get_Pos().x, /*-ownerTransform->Get_Scale().y * .5f*/0.f, ownerTransform->Get_Pos().z });

    m_TransformCom->Set_Rotation({ D3DXToRadian(90),0,0 });
}

_int CBossBullet::Update_GameObject(const _float& timeDelta)
{
   
    return _int();
}

void CBossBullet::LateUpdate_GameObject(const _float& timeDelta)
{
}

void CBossBullet::Render_GameObject()
{
    Render_Core();
    Render_Trail();
}

void CBossBullet::Render_Core()
{
}

void CBossBullet::Render_Trail()
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
    //
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR); // ← texturefactor.alpha 사용

    m_BufferCom->Render_Buffer();
    //
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    //
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //
    m_GraphicDev->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        D3DCOLOR_ARGB(255, 255, 255, 255)); // 알파 1.0, 색 그대로
}

void CBossBullet::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CBossBullet::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);
}

void CBossBullet::Animation_Setting()
{
}

HRESULT CBossBullet::Add_Component()
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


CBossBullet* CBossBullet::Create(DEVICE graphicDev, CGameObject* owner)
{
    auto BossBullet = new CBossBullet(graphicDev, owner);

    if (FAILED(BossBullet->Ready_GameObject()))
    {
        MSG_BOX("BossBullet Create Failed");
        Safe_Release(BossBullet);
        return nullptr;
    }

    return BossBullet;
}
