#pragma once
#include "CUI.h"
#include "ItemInstance.h"

class CUIInventoryCell : public CUI
{
private:
    explicit CUIInventoryCell(DEVICE pGraphicDev);
    explicit CUIInventoryCell(const CUIInventoryCell& rhs);
    ~CUIInventoryCell() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    HRESULT Add_Component();

    static CUIInventoryCell* Create(DEVICE graphicDev);

    void SetInventoryCell(const ItemInstance& itemInstance, int count)
    {
        m_ItemInst = itemInstance;
        m_Count    = count;
    }

protected:
    void Free() override;

private:
    CTexture*    m_ItemTexCom;
    CTexture*    m_HighlightTexCom;
    ItemInstance m_ItemInst;
    int          m_index;
    int          m_Count;
    bool         m_IsHovered; //외곽선 그리기 여부 판단용
};
