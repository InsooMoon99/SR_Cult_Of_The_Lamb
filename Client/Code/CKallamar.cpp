#include "pch.h"
#include "CKallamar.h"
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

CKallamar::CKallamar(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_HpBar(nullptr)
{}

CKallamar::CKallamar(const CKallamar& rhs)
    : CMonsterBase(rhs),
      m_HpBar(nullptr)
{}

CKallamar::~CKallamar()
{}

HRESULT CKallamar::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(2.3f, 2.1f, 1.2f));

    m_CombatStatCom->Set_MaxHp(750.f);
    m_CombatStatCom->Set_Speed(65.f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    m_TransformCom->Set_Pos(_vec3(16.4f, 1.2f, 50.5f));

    return S_OK;
}

void CKallamar::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();


}

_int CKallamar::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);


    return exit;
}

void CKallamar::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CKallamar::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

void CKallamar::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Kallamar", 60, 0.02f,
        _vec2(0.64f, 1.21f),
        _vec2(0.f, 0.04f));


    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Kallamar", ANIMSTATE::LOOP);
}

void CKallamar::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
}

void CKallamar::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

HRESULT CKallamar::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_KALLAMAR);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_KALLAMAR, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_KALLAMAR);

    return S_OK;
}

CKallamar* CKallamar::Create(DEVICE graphicDev)
{
    auto humanmonster = new CKallamar(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("Excutioner Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CKallamar::Free()
{
    CMonsterBase::Free();
}
