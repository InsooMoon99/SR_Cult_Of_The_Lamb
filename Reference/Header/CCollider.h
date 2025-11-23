#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CRectCollider;
class CSphereCollider;
class CBoxCollider;

class ENGINE_DLL CCollider : public CComponent
{
protected:
    explicit CCollider(DEVICE graphicDev, COLLIDERTYPE colliderType);
    explicit CCollider(const CCollider& rhs);
    ~CCollider() override;

public:
    HRESULT      Ready_Collider();
    _int         Update_Component(const _float& timeDelta) override;
    void         LateUpdate_Component() override;
    virtual void Render();

    virtual bool CheckCollision(CCollider* other);

    COLLIDERTYPE GetColliderType() const { return m_ColliderType; }

    void Set_ShowDebug(bool show) { m_ShowDebug = show; }
    bool Get_ShowDebug() const { return m_ShowDebug; }

    // [충돌 상태 관리] - CollisionManager에서 사용
    bool IsOverlapping(CCollider* other);
    void AddOverlap(CCollider* other);      // 충돌시작
    void RemoveOverlap(CCollider* other);   // 충돌 끝

public:
    // Rect vs Rect
    static bool CheckCollisionRect2Rect(CRectCollider* b1, CRectCollider* b2);
    // Box vs Box
    static bool CheckCollisionBox2Box(CBoxCollider* b1, CBoxCollider* b2);
    // Sphere vs Box
    static bool CheckCollisionSphere2Box(CSphereCollider* s1, CBoxCollider* b2);
    // Sphere vs Sphere
    static bool CheckCollisionSphere2Sphere(CSphereCollider* s1, CSphereCollider* s2);

protected:
    void Free() override;

protected:
    
    // TODO : 충돌처리 조건 더 필요하면 추가하기

    COLLIDERTYPE  m_ColliderType;
    _bool         m_ShowDebug = true;      // 디버그 렌더링 여부

private:
    // 현재 프레임 기준으로 겹쳐있는(충돌중인) 다른 Collider들
    unordered_set<CCollider*> m_ColliderSet;
};

END
