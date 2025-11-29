#pragma once
#include <utility>

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
    ~CAttackBox() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

public:
    void Set_HitDesc(const HITDATA& hit) { m_HitDesc = hit; }
    void Set_Instigator(CGameObject* inst) { m_Instigator = inst; }

    /// @brief CAttackBox가 소멸할 때 자동으로 호출할 람다 함수를 등록해두는 Setter
    /// @param callback 소멸시 자동 호출할 람다함수 등록
    void Set_OnDestroyCallback(function<void()> callback) { m_OnDestroyCallback = move(callback); }

    static CAttackBox* Create(DEVICE graphicDev);

private:
    HRESULT Add_Component();
    void    Free() override;

private:
    CBoxCollider* m_BoxColCom;
    CTransform*   m_TransformCom;

    HITDATA      m_HitDesc;          // 데미지 구조체
    CGameObject* m_Instigator;       // 공격자 (플레이어)
    float        m_LifeTime;         // 살아있는 시간 ex) 1초뒤 충돌체 삭제

    function<void()> m_OnDestroyCallback;
};

END
