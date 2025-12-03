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

private:
    void Render_Priority(DEVICE& graphicDev);
    void Render_Priority_Alpha(DEVICE& graphicDev);
    void Render_NonAlpha(DEVICE& graphicDev);
    void Render_Alpha(DEVICE& graphicDev);
    void Render_UI(DEVICE& graphicDev);

private:
    list<CGameObject*> m_RenderGroup[RENDER_END];

private:
    void Free() override;
};

END
