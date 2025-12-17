#include "pch.h"
#include "CShamura.h"
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

CShamura::CShamura(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_HpBar(nullptr)
{}

CShamura::CShamura(const CShamura& rhs)
    : CMonsterBase(rhs),
      m_HpBar(nullptr)
{}

CShamura::~CShamura()
{}

HRESULT CShamura::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(2.3f, 2.1f, 1.2f));

    m_CombatStatCom->Set_MaxHp(750.f);
    m_CombatStatCom->Set_Speed(65.f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    m_TransformCom->Set_Pos(_vec3(20.2f, 1.f, 50.5f));

    return S_OK;
}

void CShamura::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();


}

_int CShamura::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);


    return exit;
}

void CShamura::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CShamura::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

void CShamura::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Shamura", 60, 0.02f,
        _vec2(0.72f, 1.14f),
        _vec2(0.f, 0.04f));


    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Shamura", ANIMSTATE::LOOP);
}

void CShamura::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
}

void CShamura::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

HRESULT CShamura::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_SHAMURA);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_SHAMURA, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_SHAMURA);

    return S_OK;
}

CShamura* CShamura::Create(DEVICE graphicDev)
{
    auto humanmonster = new CShamura(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("Excutioner Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CShamura::Free()
{
    CMonsterBase::Free();
}
