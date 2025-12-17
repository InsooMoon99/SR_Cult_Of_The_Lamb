#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CRenderer : public CBase
{
    DECLARE_SINGLETON(CRenderer)

private:
    explicit CRenderer();
    ~CRenderer() override;

public:
    void Add_RenderGroup(RENDERID type, CGameObject* gameObject);
    void Render_GameObject(DEVICE& graphicDev);
    void Clear_RenderGroup();

    void SetFog(DWORD _color, float _fogStart, float _fogEnd) { color = _color; fogStart = _fogStart; fogEnd = _fogEnd; m_UseFog = true; }
    void OffFog() { m_UseFog = false; }
private:
    void Render_Priority(DEVICE& graphicDev);
    void Render_Priority_Alpha(DEVICE& graphicDev);
    void Render_NonAlpha(DEVICE& graphicDev);
    void Render_Alpha(DEVICE& graphicDev);
    void Render_Particle(DEVICE& graphicDev);
    void Render_UI(DEVICE& graphicDev);
    // 추가 (백 ui , 트레일용)
    void Render_Priority_UI(DEVICE& graphicDev);
    void Render_Trail(DEVICE& graphicDev);


    void SetFogOn(LPDIRECT3DDEVICE9 graphicDev);
    void SetFogOff(LPDIRECT3DDEVICE9 graphicDev);
    ///
    DWORD color = D3DCOLOR_XRGB(255, 240, 211);
    float fogStart = 5.f;
    float fogEnd = 25.f;

private:
    list<CGameObject*> m_RenderGroup[RENDER_END];

    bool m_UseFog = false;
private:
    void Free() override;
};

END
