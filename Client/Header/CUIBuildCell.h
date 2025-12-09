#pragma once
#include "CUI.h"

class CUIIcon;

enum BUILDITEM
{
    COOKINGFIRE, //성지
    TEMPLE, //사원
    BUILD_MAX
};

class CUIBuildCell : public CUI
{
private:
    explicit CUIBuildCell(DEVICE pGraphicDev);
    explicit CUIBuildCell(const CUIBuildCell& rhs);
    ~CUIBuildCell() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

    HRESULT Add_Component();

    static CUIBuildCell* Create(DEVICE graphicDev);

    void    Set_IsHovered(bool tf) { m_IsHovered = tf; }
    bool    Get_IsHovered() { return m_IsHovered; }
 
    void    Set_BuildIndex(BUILDITEM buildItem) { m_BuildIndex = buildItem; }
    void    Set_TextureIndex(int textureIndex) { m_TextureIndex = textureIndex; }

    CUIIcon* Get_Icon() { return m_ItemIcon; }

protected:
    void Free() override;

private:
    int             m_BuildIndex;
    CUIIcon*        m_ItemIcon;
    CUIIcon*        m_CardIcon;
    CUIIcon*        m_CardItemIcon;
    CTexture*       m_HighlightTexCom;
    D3DXCOLOR       m_D3dxColor;
    bool            m_IsHovered; //외곽선 그리기 여부 판단용
    int             m_TextureIndex;

};
