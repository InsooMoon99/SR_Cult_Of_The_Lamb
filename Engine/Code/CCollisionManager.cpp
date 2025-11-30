#include "CCollisionManager.h"

#include "CCollider.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{
    Initialize_CollisionMatrix();
}

CCollisionManager::~CCollisionManager()
{
    Free();
}

void CCollisionManager::Update()
{
    auto& colliders = m_Colliders;
    const _uint count = static_cast<_uint>(colliders.size());

    for (_uint i = 0; i < count; i++)
    {
        CCollider* src = colliders[i];
        if (src == nullptr)
            continue;

        CGameObject* ownerSrc = src->Get_Owner();
        if (ownerSrc == nullptr || ownerSrc->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
            continue;

        if (src->IsCollisionActive() == false) // 충돌체 Active가 꺼져있다면 무시
            continue;

        for (_uint j = i + 1; j < count; j++)
        {
            CCollider* dst = colliders[j];
            if (dst == nullptr)
                continue;

            CGameObject* ownerDst = dst->Get_Owner();
            if (ownerDst == nullptr || ownerDst->Is_PendingDestroy()) // 소멸 예정인 애들은 충돌 X
                continue;

            if (dst->IsCollisionActive() == false) // 충돌체 Active가 꺼져있다면 무시
                continue;

            // Owner가 같으면 무시
            if (ownerSrc == ownerDst)
                continue;

            // Insigator 체크
            CGameObject* instigatorSrc = src->Get_Instigator();
            if (instigatorSrc != nullptr && instigatorSrc == ownerDst)
                continue;

            CGameObject* instigatorDst = dst->Get_Instigator();
            if (instigatorDst != nullptr && instigatorDst == ownerSrc)
                continue;

            // 레이어 확인
            if (!Can_Collide(src->Get_CollisionLayer(), dst->Get_CollisionLayer()))
                continue;

            bool isColliding = src->CheckCollision(dst);

            if (isColliding)
            {
                // 이번 프레임에 새로 겹치기 시작하면
                if (!src->IsOverlapping(dst))
                {
                    // Owner에게 이벤트 전달, 비교가 아닌 대입임
                    if (CGameObject* ownerSrc = src->Get_Owner())
                        ownerSrc->OnBeginOverlap(src, dst);

                    if (CGameObject* ownerDst = dst->Get_Owner())
                        ownerDst->OnBeginOverlap(dst, src);

                    // 충돌 상태 추가
                    src->AddOverlap(dst);
                    dst->AddOverlap(src);
                }
                // 이미 겹쳐 있을 때
                else
                {
                    if (CGameObject* ownerSrc = src->Get_Owner())
                        ownerSrc->OnStayOverlap(src, dst);

                    if (CGameObject* ownerDst = dst->Get_Owner())
                        ownerDst->OnStayOverlap(dst, src);
                }
            }

            else
            {
                // 이전 프레임까지 겹쳐 있었는데, 이제 떨어졌으면
                if (src->IsOverlapping(dst))
                {
                    if (CGameObject* ownerSrc = src->Get_Owner())
                        ownerSrc->OnEndOverlap(src, dst);

                    if (CGameObject* ownerDst = dst->Get_Owner())
                        ownerDst->OnEndOverlap(dst, src);

                    // 충돌 상태 제거
                    src->RemoveOverlap(dst);
                    dst->RemoveOverlap(src);
                }
            }
        }
    }
}

// 충돌체 추가
void CCollisionManager::AddCollider(CCollider* collider)
{
    if (collider == nullptr)
        return;

    m_Colliders.push_back(collider);
}

void CCollisionManager::RemoveCollider(CCollider* collider)
{
    if (collider == nullptr)
        return;

    for (CCollider* other : m_Colliders)
    {
        if (other && other != collider)
        {
            if (other->IsOverlapping(collider))
            {
                other->RemoveOverlap(collider);

                if (CGameObject* owner = other->Get_Owner())
                    owner->OnEndOverlap(other, collider);
            }
        }
    }

    // 리스트에서 제거
    auto iter = remove(m_Colliders.begin(), m_Colliders.end(), collider);
    m_Colliders.erase(iter, m_Colliders.end());
}

_bool CCollisionManager::Can_Collide(COLLISION_LAYERTYPE layer1, COLLISION_LAYERTYPE layer2)
{
    _int index1 = static_cast<int>(layer1);
    _int index2 = static_cast<int>(layer2);

    return m_CollisionMatrix[index1][index2];
}

void CCollisionManager::Initialize_CollisionMatrix()
{
    // 일단 모든 레이어 충돌 가능하게 설정
    for (_int i = 0; i < static_cast<_int>(COLLISION_LAYERTYPE::COL_LAYER_END); i++)
    {
        for (_int j = 0; j < static_cast<_int>(COLLISION_LAYERTYPE::COL_LAYER_END); j++)
        {
            m_CollisionMatrix[i][j] = true;
        }
    }

    // 벽끼리는 충돌하지 않도록 설정
    _int wall_Index = static_cast<_int>(COLLISION_LAYERTYPE::WALL);
    m_CollisionMatrix[wall_Index][wall_Index] = false;

    // 적끼리도 충돌 X
    _int enemy_Index = static_cast<_int>(COLLISION_LAYERTYPE::ENEMY);
    m_CollisionMatrix[enemy_Index][enemy_Index] = false;

    // 아이템과 벽은 충돌 ㄴㄴ
    _int item_Index = static_cast<_int>(COLLISION_LAYERTYPE::ITEM);
    m_CollisionMatrix[wall_Index][item_Index] = false;
    m_CollisionMatrix[item_Index][wall_Index] = false;

    // 공격이랑 아이템 충돌 ㄴㄴ
    _int attack_Index = static_cast<_int>(COLLISION_LAYERTYPE::ATTACK);
    m_CollisionMatrix[attack_Index][item_Index] = false;
    m_CollisionMatrix[item_Index][attack_Index] = false;

    // 아이템, 적 충돌 ㄴㄴ
    m_CollisionMatrix[item_Index][enemy_Index] = false;
    m_CollisionMatrix[enemy_Index][item_Index] = false;

    // TODO : 다른 충돌 상태 판정 추가하기


}

void CCollisionManager::Free()
{
    m_Colliders.clear();

}
