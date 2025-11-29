#pragma once

#include "CCombatStat.h"
#include "CGameObject.h"

BEGIN(Engine)

class CBoxCollider;
class CTransform;

class ENGINE_DLL CAttackBox : public CGameObject
{
private:
    explicit CAttackBox(DEVICE graphicDev);
    explicit CAttackBox(const CAttackBox& rhs);
    virtual ~CAttackBox();

public:

    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

    virtual void OnBeginOverlap(CCollider* self, CCollider* other) override;
    virtual void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    void Set_HitDesc(const HITDATA& hit) { m_HitDesc = hit; }
    void Set_Instigator(CGameObject* inst) { m_Instigator = inst; }

    static CAttackBox* Create(DEVICE graphicDev);

private:
    HRESULT Add_Component();
    void Free() override;

private:
    CBoxCollider* m_BoxColCom;
    CTransform*   m_TransformCom;

    HITDATA       m_HitDesc;          // 데미지 구조체
    CGameObject*  m_Instigator;       // 공격자 (플레이어)
    float         m_LifeTime;         // 살아있는 시간 ex) 1초뒤 충돌체 삭제

};

END
