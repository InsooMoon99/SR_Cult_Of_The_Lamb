#include "pch.h"
#include "CLeshy.h"
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

CLeshy::CLeshy(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_HpBar(nullptr)
{}

CLeshy::CLeshy(const CLeshy& rhs)
    : CMonsterBase(rhs),
      m_HpBar(nullptr)
{}

CLeshy::~CLeshy()
{}

HRESULT CLeshy::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_MaxHp(750.f);
    m_CombatStatCom->Set_Speed(65.f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    m_TransformCom->Set_Scale(_vec3(2.3f, 2.1f, 1.2f));
    m_TransformCom->Set_Pos(_vec3(8.9f, 1.2f, 50.5f));

    return S_OK;
}

void CLeshy::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();


}

_int CLeshy::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);


    return exit;
}

void CLeshy::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CLeshy::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

void CLeshy::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Leshy", 84, 0.035f,
        _vec2(1.13f, 1.16f),
        _vec2(0.f, 0.04f));


    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Leshy", ANIMSTATE::LOOP);
}

void CLeshy::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
}

void CLeshy::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

HRESULT CLeshy::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_LESHY);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_LESHY, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_LESHY);

    return S_OK;
}

CLeshy* CLeshy::Create(DEVICE graphicDev)
{
    auto humanmonster = new CLeshy(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("Excutioner Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CLeshy::Free()
{
    CMonsterBase::Free();
}
