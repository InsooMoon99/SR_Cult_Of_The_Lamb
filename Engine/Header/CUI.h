#pragma once
#include "CGameObject.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CRcTex;
class CTexture;
class CTransform;

class ENGINE_DLL CUI : public CGameObject
{
public:
    explicit CUI(DEVICE pGraphicDev);
    explicit CUI(const CUI& rhs);
    ~CUI() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    virtual int Get_ID() { return m_ID; }
    //! 석호 : Get_ID를 CUI의 자식에서도 사용 가능하도록 가상함수(virtual) 로 바꿈
    // ↑ 확인했으면 지우세요
protected:
    bool m_bHovered = false;
    bool m_bClicked = false;
    RECT m_tRect;
protected:
    void Free() override;

protected:
    CTransform* m_TransformCom;
    CTexture*   m_TextureCom;
    CRcTex*     m_BufferCom;
    int         m_ID;
    bool        m_Invisible;
};

END
