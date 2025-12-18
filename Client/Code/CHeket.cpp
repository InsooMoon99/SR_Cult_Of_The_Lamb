#include "pch.h"
#include "CHeket.h"
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

CHeket::CHeket(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_HpBar(nullptr)
{}

CHeket::CHeket(const CHeket& rhs)
    : CMonsterBase(rhs),
      m_HpBar(nullptr)
{}

CHeket::~CHeket()
{}

HRESULT CHeket::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(2.3f, 2.1f, 1.2f));

    m_CombatStatCom->Set_MaxHp(750.f);
    m_CombatStatCom->Set_Speed(65.f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    m_TransformCom->Set_Pos(_vec3(12.8f, 1.2f, 50.5f));

    return S_OK;
}

void CHeket::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();


}

_int CHeket::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);


    return exit;
}

void CHeket::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CHeket::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

void CHeket::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Heket", 60, 0.02f,
        _vec2(0.72f, 1.39f),
        _vec2(0.f, 0.04f));


    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Heket", ANIMSTATE::LOOP);
}

void CHeket::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
}

void CHeket::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

HRESULT CHeket::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_HEKET);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_HEKET, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_HEKET);

    return S_OK;
}

CHeket* CHeket::Create(DEVICE graphicDev)
{
    auto humanmonster = new CHeket(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("Excutioner Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CHeket::Free()
{
    CMonsterBase::Free();
}
