#pragma once

#include "CCombatStat.h"
#include "CAttackBase.h"
#include "CSphereCollider.h"

BEGIN(Engine)

class ENGINE_DLL CAttackSphere : public CAttackBase
{
private:
    explicit CAttackSphere(DEVICE graphicDev);
    explicit CAttackSphere(const CAttackSphere& rhs);
    virtual ~CAttackSphere();

public:
    HRESULT Ready_GameObject() override;
    _int Update_GameObject(const _float& timeDelta) override;
    void LateUpdate_GameObject(const _float& timeDelta) override;
    void Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    static CAttackSphere* Create(DEVICE graphicDev);

protected:
    virtual CCollider* Get_Collider() override { return m_SphereColCom; }

private:
    HRESULT Add_Component();
    void Free() override;

private:
    CSphereCollider* m_SphereColCom;
  

};

END
