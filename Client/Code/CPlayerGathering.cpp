#include "pch.h"
#include "CPlayerGathering.h"
#include "CCollider.h"
#include "CWorldInteractive.h"
#include "CTransform.h"
#include "CState.h"
#include "CDInputMgr.h"
#include "CCombatStat.h"


CPlayerGathering::CPlayerGathering(DEVICE graphicDev) : CComponent(graphicDev)
{
    ZeroMemory(&h, sizeof(h));
}

CPlayerGathering::CPlayerGathering(const CPlayerGathering& rhs) : CComponent(rhs)
{
}

CPlayerGathering::~CPlayerGathering()
{
}

HRESULT CPlayerGathering::Ready_PlayerGathering()
{

    return S_OK;
}

void CPlayerGathering::Initialize_Component()
{
    CComponent::Initialize_Component();
    m_OwnerStateCom = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));

    h.attacker = m_Owner;
    h.damage = 1;
    h.dir = { 0,0,0 };
    h.hitter = nullptr;
    h.knockback = 0;;
    h.knockbackTimer = 0;
}

_int CPlayerGathering::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

    if (CDInputMgr::GetInstance()->MouseUp(DIM_LB)/*&& m_Target */ || m_Target->Get_Hovering() == false)
    {
           m_OwnerStateCom->Change_State(ACTORSTATE::IDLE);
           return exit;

    }

       //애니메이션 방향 결정
       _vec3  vTargetDir = m_Target->Get_Transform()->Get_Pos();

       _vec3 dir = vTargetDir - m_Owner->Get_Transform()->Get_Pos();

       D3DXVec3Normalize(&dir, &dir);

       if (dir.x <= 0.f)
           m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
       else
           m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);

       /*dir.z = 0;*/


       m_AttackTimer += timeDelta;

       if (m_AttackTimer >= 0.5)
       {
           // 공격 1 씩 주기
           CCombatStat* combat = dynamic_cast<CCombatStat*>(m_Target->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
           h.dir = dir;
           h.hitter = m_Target;

           combat->Apply_Damage(h);

           m_AttackTimer = 0;
       }


   //switch(m_Target->GetInteractiveType())
   // {
   // case CWorldInteractive::TYPE::BERRY:
   // {
   //
   // }
   //     break;
   // case CWorldInteractive::TYPE::WOOD:
   //     break;
   // case CWorldInteractive::TYPE::ROCK:
   //     break;
   // }

    return exit;
}

void CPlayerGathering::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();

}

void CPlayerGathering::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CWorldInteractive* interact = dynamic_cast<CWorldInteractive*>(other->Get_Owner());
    if (interact)
    {
        // 레퍼런스 올리고 내려야 할듯
        m_Target = interact;
    }
}

void CPlayerGathering::OnEndOverlap(CCollider* self, CCollider* other)
{
    if (other->Get_Owner() == m_Target)
    {
        
        m_Target = nullptr;
}
}

CPlayerGathering* CPlayerGathering::Create(DEVICE graphicDev)
{
    CPlayerGathering* PlayerGathering = new CPlayerGathering(graphicDev);

    if (FAILED(PlayerGathering->Ready_PlayerGathering()))
    {
        Safe_Release(PlayerGathering);
        MSG_BOX("PlayerGathering Create Failed");
        return nullptr;
    }

    return PlayerGathering;
}

CComponent* CPlayerGathering::Clone()
{
    return new CPlayerGathering(*this);
}

void CPlayerGathering::Free()
{
    CComponent::Free();

}
