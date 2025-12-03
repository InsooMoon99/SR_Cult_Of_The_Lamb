#pragma once
#include "CGameObject.h"
#include "ItemInstance.h"

BEGIN(Engine)
class CTransform;
class CBoxCollider;
class CRcTex;
class CTexture;
END

class CDroppedItem : public CGameObject
{
    explicit CDroppedItem(DEVICE graphicDev, ItemInstance itemInst);
    explicit CDroppedItem(const CDroppedItem& rhs);
    ~CDroppedItem() override;

public:
    HRESULT Ready_GameObject() override;
    HRESULT Ready_GameObject(_vec3 pos);
    void    Initialize_GameObject() override;;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component();

    void PopDrop(float range, float height); // 바닥완성 후 추가 작업 예정
private:
    Engine::CRcTex*       m_BufferCom;
    Engine::CTransform*   m_TransformCom;
    Engine::CTexture*     m_TextureCom;
    Engine::CBoxCollider* m_BoxColCom;
    // 지형 타는 컴포넌트 추가예정

public:
    static CDroppedItem* Create(DEVICE graphicDev, ItemInstance itemInst, _vec3 pos);

private:
    ItemInstance m_itemInst;
    int          m_index = 0;

private:
    void Free() override;
};
