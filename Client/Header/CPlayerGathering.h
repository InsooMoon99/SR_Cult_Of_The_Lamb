#pragma once
#include "CComponent.h"

class CWorldInteractive;

namespace Engine
{
    class CCollider;
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
    CWorldInteractive* m_Target = nullptr;
public:
    static CPlayerGathering* Create(DEVICE graphicDev);
    CComponent* Clone() override;

private:
    void Free() override;

};

