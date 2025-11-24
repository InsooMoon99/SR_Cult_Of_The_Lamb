#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CCollider;

// 프레임마다 모든 콜라이더를 탐색해서
// BeginOverlap, EndOverlap을 GameObject에 전달
class ENGINE_DLL CCollisionManager : public CBase
{
    DECLARE_SINGLETON(CCollisionManager)

private:
    explicit CCollisionManager();
    ~CCollisionManager() override;

public:
    void Update();

    void AddCollider(CCollider* collider);
    void RemoveCollider(CCollider* collider);

    static CCollisionManager* GetInstance_NoCreate() { return m_Instance; }

private:
    void Free() override;

private:
    vector<CCollider*> m_Colliders;
};

END
