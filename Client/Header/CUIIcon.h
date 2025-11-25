#pragma once
#include "CUI.h"

class CUIIcon : public CUI
{
private:
    explicit CUIIcon(DEVICE pGraphicDev);
    explicit CUIIcon(const CUIIcon& rhs);
    ~CUIIcon() override;

public:
    HRESULT Ready_GameObject(const wstring& texProtoTag);
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component(const wstring& texProtoTag);

public:
    static CUIIcon* Create(DEVICE graphicDev, const wstring& texProtoTag);

    void        Set_TextureIndex(int textureIndex) { m_textureIndex = textureIndex; }
    CTransform* Get_Transform() const { return m_TransformCom; }
    bool SetCloneTexAspectScale(_vec3 vec);
    void SetRegister(bool enable) { m_Register = enable; }

private:
    static int m_nextID;
    int        m_textureIndex = 0;
    bool       m_Register;
private:
    void Free() override;
};
