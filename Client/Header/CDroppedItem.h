#pragma once
#include "CGameObject.h"
#include "ItemInstance.h"
#include "Client_Struct.h"

class CUIWeaponExplain;
BEGIN(Engine)
class CTransform;
class CBoxCollider;
class CRcTex;
class CTexture;
END

class CSpriteEff;
struct SpriteBounceDesc;

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
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    static void Reset_WeaponExplainUI() { m_WeaponExplainUI = nullptr; }

private:
    HRESULT Add_Component();

private:
    Engine::CRcTex*             m_BufferCom;
    Engine::CTransform*         m_TransformCom;
    Engine::CTexture*           m_TextureCom;
    Engine::CBoxCollider*       m_BoxColCom;
    static CUIWeaponExplain*    m_WeaponExplainUI;

    CSpriteEff* m_SpriteEff;

public:
    static CDroppedItem* Create(DEVICE graphicDev, ItemInstance itemInst, _vec3 pos);

    void SetBounceDesc(SpriteBounceDesc& d) { m_BounceDesc = d; is_SetBounceDesc = true; } // 바닥완성 후 추가 작업 예정
private:
    SpriteBounceDesc m_BounceDesc;
    bool is_SetBounceDesc = false;

    ItemInstance m_itemInst;
    int          m_index = 0;

private:
    void Free() override;
};
