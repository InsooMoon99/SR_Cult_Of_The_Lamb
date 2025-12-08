#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CCollider;
END

class CConstructCircle;

class CPlayerConstruction : public CComponent
{
private:
    explicit CPlayerConstruction(DEVICE graphicDev);
    explicit CPlayerConstruction(const CPlayerConstruction& rhs);
    ~CPlayerConstruction() override;

public:
    HRESULT Ready_PlayerConstruction();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    // Interaction Methods
    void Action_Construction(const _float& timeDelta);

    // Collision Callbacks
    void OnOverlapEnter(CCollider* self, CCollider* other);
    void OnOverlapExit(CCollider* self, CCollider* other);

    // Getters
    CConstructCircle* Get_ConstructZone() const { return m_pConstructZone; }

private:
    CConstructCircle* m_pConstructZone = nullptr;

public:
    static CPlayerConstruction* Create(DEVICE graphicDev);
    CComponent*                 Clone() override;

    void Free() override;
};
