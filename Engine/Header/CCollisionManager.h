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

    _bool Can_Collide(COLLISION_LAYERTYPE layer1, COLLISION_LAYERTYPE layer2);

    void Render_Debug(bool& flag);

private:
    void Free() override;

    void Initialize_CollisionMatrix();

private:
    vector<CCollider*> m_Colliders;

    bool m_CollisionMatrix
        [static_cast<int>(COLLISION_LAYERTYPE::COL_LAYER_END)]
        [static_cast<int>(COLLISION_LAYERTYPE::COL_LAYER_END)];
};

END
