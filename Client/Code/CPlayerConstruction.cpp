#include "pch.h"
#include "CPlayerConstruction.h"

#include "CCollider.h"
#include "CConstructCircle.h"

CPlayerConstruction::CPlayerConstruction(DEVICE graphicDev)
    : CComponent(graphicDev)
{ }

CPlayerConstruction::CPlayerConstruction(const CPlayerConstruction& rhs)
    : CComponent(rhs)
{ }

CPlayerConstruction::~CPlayerConstruction()
{ }

HRESULT CPlayerConstruction::Ready_PlayerConstruction()
{
    return S_OK;
}

void CPlayerConstruction::Initialize_Component()
{ }

_int CPlayerConstruction::Update_Component(const _float& timeDelta)
{
    // Normally does nothing unless called by Action_Construction or managing state locally
    return 0;
}

void CPlayerConstruction::LateUpdate_Component()
{ }

void CPlayerConstruction::Action_Construction(const _float& timeDelta)
{
    if (m_pConstructZone)
    {
        // Add custom logic here if needed (e.g. effects, stamina drain)
    }
}

void CPlayerConstruction::OnOverlapEnter(CCollider* self, CCollider* other)
{
    if (other->Get_CollisionLayer() == COLLISION_LAYERTYPE::INTERACTIVEOBJ)
    {
        m_pConstructZone = dynamic_cast<CConstructCircle*>(other->Get_Owner());
    }
}

void CPlayerConstruction::OnOverlapExit(CCollider* self, CCollider* other)
{
    if (other->Get_CollisionLayer() == COLLISION_LAYERTYPE::INTERACTIVEOBJ)
    {
        if (m_pConstructZone == dynamic_cast<CConstructCircle*>(other->Get_Owner()))
        {
            m_pConstructZone = nullptr;
        }
    }
}

CPlayerConstruction* CPlayerConstruction::Create(DEVICE graphicDev)
{
    auto playerConstruction = new CPlayerConstruction(graphicDev);

    if (FAILED(playerConstruction->Ready_PlayerConstruction()))
    {
        Safe_Release(playerConstruction);
        MSG_BOX("PlayerConstruction Create Failed");
        return nullptr;
    }

    return playerConstruction;
}

CComponent* CPlayerConstruction::Clone()
{
    return new CPlayerConstruction(*this);
}

void CPlayerConstruction::Free()
{
    m_pConstructZone = nullptr;

    CComponent::Free();
}
