#pragma once
#include "CComponent.h"
#include "CCombatStat.h"
class CWorldInteractive;

namespace Engine
{
    class CCollider;
    class CState;
    class CombatStat;
}
class CWorldInteractive;

class CPlayerGathering :
    public CComponent
{
private:
    explicit CPlayerGathering(DEVICE graphicDev);
    explicit CPlayerGathering(const CPlayerGathering& rhs);
    ~CPlayerGathering() override;

public:
    HRESULT Ready_PlayerGathering();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    void    OnBeginOverlap(CCollider* self, CCollider* other);
    void    OnEndOverlap(CCollider* self, CCollider* other);

    CWorldInteractive* const GetInteractiveObj() { return m_Target; }
   /* CWorldInteractive::TYPE  GetInteractiveType() { return m_Target->GetInteractiveType(); }*/
private:
    CState* m_OwnerStateCom = nullptr;
    CAnimator* m_OwnerAnimationCom;
    CWorldInteractive* m_Target = nullptr;

    float m_AttackTimer = 0;
    HITDATA h;
    bool m_AttackFlag = false;
    
public:
    static CPlayerGathering* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    void Free() override;

};

