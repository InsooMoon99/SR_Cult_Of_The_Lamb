#pragma once
#include "CUI.h"

BEGIN(Engine)
class CTransform;
END

class  CUIButton : public CUI
{
protected:
    explicit CUIButton(DEVICE graphicDev, int texOn, int texOff);
    explicit CUIButton(const CUIButton& rhs, int texOn, int texOff);
    ~CUIButton() override;

public:
    HRESULT Ready_GameObject();
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    HRESULT Add_Component();


    void OnHoverStart();
    void OnHoverEnd();
protected:
    bool m_bHovered = false;
    //bool m_bClicked = false;
    RECT m_tRect;
public:
    static CUIButton* Create(DEVICE graphicDev, int texOn, int texOff);

    void        Set_TextureIndex(int textureOn, int textureOff) { m_textureOn = textureOn; m_textureOff = textureOff;}
    CTransform* Get_Transform() const { return m_TransformCom; }
    bool Get_Hovered() { return m_bHovered; }
    void Set_Hovered(bool tf) { m_bHovered = tf; }
    void SetOnClick(std::function<void()> callback) { m_OnClick = callback; }

    bool SetCloneTexAspectScale(_vec3 vec);
private:
    std::function<void()> m_OnClick;

    static int m_nextID;
    int        m_textureIndex = 0;

    int m_textureOn;
    int m_textureOff;

private:
    void Free() override;


};


