#include "pch.h"
#include "CPlayerGathering.h"
#include "CCollider.h"
#include "CWorldInteractive.h"

CPlayerGathering::CPlayerGathering(DEVICE graphicDev) : CComponent(graphicDev)
{
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

}

_int CPlayerGathering::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);

   switch(m_Target->GetInteractiveType())
    {
    case CWorldInteractive::TYPE::BERRY:
    {
        //if (!m_OwnerCombatStatCom || !m_OwnerInputCom) return false;

        ////애니메이션 방향 결정
        //_vec3  vMouseDir = m_OwnerInputCom->Get_MouseDir();

        //if (vMouseDir.x <= 0.f)
        //    m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
        //else
        //    m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);
        //m_SkillTimer += timeDelta;
        //m_ChargeTimer += timeDelta;
        //const SkillInfo& skillInfo = m_SkillInfo[SKILL_TRIPLE];

        //vMouseDir.z = 0;

       // D3DXVec3Normalize(&vMouseDir, &vMouseDir);


        return false;

        return true;
    }
        break;
    case CWorldInteractive::TYPE::WOOD:
        break;
    case CWorldInteractive::TYPE::ROCK:
        break;
    }

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
