#include "CCollisionManager.h"

#include "CCollider.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{ }

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

            // TODO : Owner 세팅 후 비교

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
// 충돌체 제거
void CCollisionManager::RemoveCollider(CCollider* collider)
{
    if (collider == nullptr)
        return;

    auto iter = remove(m_Colliders.begin(), m_Colliders.end(), collider);
    m_Colliders.erase(iter, m_Colliders.end());
}

void CCollisionManager::Free()
{
    m_Colliders.clear();

}
