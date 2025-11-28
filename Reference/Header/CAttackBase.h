#pragma once

#include "CCombatStat.h"
#include "CGameObject.h"

BEGIN(Engine)

class CBoxCollider;
class CTrnasform;

class ENGINE_DLL CAttackBase : public CGameObject
{
protected:
    explicit CAttackBase(DEVICE graphicDev);
    explicit CAttackBase(const CAttackBase& rhs);
    virtual ~CAttackBase();

public:
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    void Set_AttackData(const ATTACKDATA& hit) { m_AttackData = hit; }
    void Set_Instigator(CGameObject* inst) { m_Instigator = inst; }
    void Set_LifeTime(float LifeTime) {m_LifeTime = LifeTime;}

protected:
    void Free() override;

    virtual CCollider* Get_Collider() = 0;

protected:
    CTransform*   m_TransformCom;

    ATTACKDATA    m_AttackData;       // 데미지 구조체
    CGameObject*  m_Instigator;       // 공격자 (플레이어)
    float         m_LifeTime;         // 살아있는 시간 ex) 1초뒤 충돌체 삭제

};

END
