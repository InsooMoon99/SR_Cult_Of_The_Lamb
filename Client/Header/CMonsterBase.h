#pragma once

#include "CCharacterBase.h"

BEGIN(Engine)
class CTransform;
END

// 모든 몬스터들의 기본 베이스 (HumanMonster, Boss ..)
class CMonsterBase : public CCharacterBase
{
protected:
    explicit CMonsterBase(DEVICE graphicDev);
    explicit CMonsterBase(const CMonsterBase& rhs);
    virtual ~CMonsterBase();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;

public:
    _float Get_DistanceToTarget();

    // 상속받아서 몬스터마다 다르게 사용 ㄱㄱ
    virtual void Chase_Target(const float& timeDelta, _float speed) {}

protected:
    void Free() override;

protected:
    CGameObject* m_Target;
    CTransform*  m_TargetTransform;

};

