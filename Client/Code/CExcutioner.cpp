#include "pch.h"
#include "CExcutioner.h"

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

CExcutioner::CExcutioner(DEVICE graphicDev)
    : CMonsterBase(graphicDev),
      m_HpBar(nullptr)
{}

CExcutioner::CExcutioner(const CExcutioner& rhs)
    : CMonsterBase(rhs),
      m_HpBar(nullptr)
{}

CExcutioner::~CExcutioner()
{}

HRESULT CExcutioner::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(2.3f, 2.1f, 1.2f));

    m_CombatStatCom->Set_MaxHp(750.f);
    m_CombatStatCom->Set_Speed(65.f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    m_TransformCom->Set_Pos(_vec3(20.6f, 1.f, 45.2f));

    return S_OK;
}

void CExcutioner::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();


}

_int CExcutioner::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    ACTORSTATE curState = m_StateCom->Get_State();

    switch (curState)
    {
    case ACTORSTATE::IDLE: Update_State_Idle(timeDelta);
        break;
    case ACTORSTATE::ATTACK: Update_State_Attack(timeDelta);
        break;
    }

    return exit;
}

void CExcutioner::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CExcutioner::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}

void CExcutioner::Update_State_Idle(const _float timeDelta)
{
    m_AttackTimer += timeDelta;

}

void CExcutioner::Update_State_Attack(const _float timeDelta)
{
    if (m_AnimatorCom->Get_CurFrame() == 30)
    {
        
    }
    
}

void CExcutioner::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"Excutioner_Idle", 45, 0.02f,
        _vec2(0.76f, 0.78f),
        _vec2(0.f, 0.32f));

    m_AnimatorCom->Create_Animation(L"Excutioner_Excute", 55, 0.02f,
        _vec2(1.f, 1.f),
        _vec2(0.f, 0.32f));

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"Excutioner_Idle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::ATTACK, L"Excutioner_Excute", ANIMSTATE::LOOP);
}

void CExcutioner::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);
}

void CExcutioner::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);
}

HRESULT CExcutioner::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_EXCUTIONER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_EXCUTIONER, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_EXCUTIONER);

    return S_OK;
}

CExcutioner* CExcutioner::Create(DEVICE graphicDev)
{
    auto humanmonster = new CExcutioner(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("Excutioner Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CExcutioner::Free()
{
    CMonsterBase::Free();
}
