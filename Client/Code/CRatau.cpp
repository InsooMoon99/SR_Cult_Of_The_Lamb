#include "pch.h"
#include "CRatau.h"
#include "CAnimator.h"
#include "CAttackBuilder.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CParticleHelper.h"
#include "CPlayer.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUI_MonsterHp.h"

CRatau::CRatau(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_HpBar(nullptr)
{}

CRatau::CRatau(const CRatau& rhs)
    : CMonsterBase(rhs),
      m_HpBar(nullptr)
{}

CRatau::~CRatau()
{}

HRESULT CRatau::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_MaxHp(750.f);
    m_CombatStatCom->Set_Speed(65.f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    //m_TransformCom->Set_Pos(_vec3(8.9f, 1.2f, 50.5f));

    return S_OK;
}

void CRatau::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    if (m_AnimatorCom)
        m_AnimatorCom->Play_Animation(L"Ratau_Enter", ANIMSTATE::ONCE);
}

_int CRatau::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    if (m_AnimatorCom &&
        m_AnimatorCom->Get_CurKey() == L"Ratau_Enter" &&
        m_AnimatorCom->Is_AnimationFinished())
    {
        m_AnimatorCom->Play_Animation(L"Ratau_Idle", ANIMSTATE::LOOP);
    }

    return exit;
}

void CRatau::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CRatau::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
    
}

void CRatau::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Ratau_Idle", 68, 0.035f,
        _vec2(1.13f, 1.16f),
        _vec2(0.f, 0.04f));

    m_AnimatorCom->Create_Animation(L"Ratau_Enter", 46, 0.035f,
        _vec2(1.79f, 2.1f),
        _vec2(-0.04f, 0.87f));

    m_AnimatorCom->Create_Animation(L"Ratau_Exit", 50, 0.035f,
        _vec2(1.91f, 1.67f),
        _vec2(0.47f, 0.52f));

}

void CRatau::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
}

void CRatau::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

HRESULT CRatau::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_RATAU);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_RATAU, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_RATAU);

    return S_OK;
}

CRatau* CRatau::Create(DEVICE graphicDev)
{
    auto humanmonster = new CRatau(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("Excutioner Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CRatau::Free()
{
    CMonsterBase::Free();
}
