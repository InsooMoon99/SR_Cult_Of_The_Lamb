#include "pch.h"
#include "CHumanMonster.h"
#include "CCombatStat.h"
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CPlayer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRigidbody.h"

CHumanMonster::CHumanMonster(DEVICE graphicDev)
    : CMonsterBase(graphicDev)
{
}

CHumanMonster::CHumanMonster(const CHumanMonster& rhs)
    : CMonsterBase(rhs)
{
}

CHumanMonster::~CHumanMonster()
{}

HRESULT CHumanMonster::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 0.f));
    m_TransformCom->Set_Scale(_vec3(1.2f, 1.2f, 1.2f));

    m_CombatStatCom->Set_MaxHp(50.f);
    m_CombatStatCom->Set_Hp(50);

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    m_RigidbodyCom->Set_Friction(1.0f);

    return S_OK;
}

void CHumanMonster::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

    m_TargetTransform = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
            LAYERTYPE::GAMELOGIC,
            OBJTYPE::PLAYER,
            COMPONENTTYPE::TRANSFORM));

    if (m_TargetTransform)
        m_Target = m_TargetTransform->Get_Owner();
}

_int CHumanMonster::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    _float distance = Get_DistanceToTarget();

    if (1.f < distance && distance < 10.f)
        Chase_Target(timeDelta, m_CombatStatCom->Get_Speed());

    return exit;
}

void CHumanMonster::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

    
}

void CHumanMonster::Render_GameObject()
{
    CMonsterBase::Render_GameObject();


}

void CHumanMonster::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"HumanMonsterIdle", 34, 0.02f);
    m_AnimatorCom->Create_Animation(L"HumanMonsterRun", 18, 0.02f);
    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"HumanMonsterIdle", ANIMSTATE::LOOP);
    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"HumanMonsterRun", ANIMSTATE::LOOP);
}

void CHumanMonster::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

}

void CHumanMonster::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}

void CHumanMonster::Chase_Target(const float& timeDelta, _float speed)
{
    if (!m_TargetTransform || !m_TransformCom)
        return;

    _vec3 pos = m_TransformCom->Get_Pos();
    _vec3 targetPos = m_TargetTransform->Get_Pos();
    //m_TransformCom->Chase_Target(&targetPos, timeDelta, speed);

    _vec3 dir = targetPos - pos;
    dir.y = 0.f;

    D3DXVec3Normalize(&dir, &dir);

    _vec3 moveForce = dir * m_CombatStatCom->Get_Speed();
    m_RigidbodyCom->Add_Force(moveForce);
}

HRESULT CHumanMonster::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_HUMAN_MONSTER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_HUMAN_MONSTER, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_HUMAN_MONSTER);

    // Rigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });

    return S_OK;
}

CHumanMonster* CHumanMonster::Create(DEVICE graphicDev)
{
    auto humanmonster = new CHumanMonster(graphicDev);

    if (FAILED(humanmonster->Ready_GameObject()))
    {
        MSG_BOX("HumanMonster Create Failed");
        Safe_Release(humanmonster);
        return nullptr;
    }

    return humanmonster;
}

void CHumanMonster::Free()
{
    CMonsterBase::Free();
}
