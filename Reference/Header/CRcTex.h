#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CRcTex : public CVIBuffer
{
private:
    explicit CRcTex();
    explicit CRcTex(DEVICE graphicDev);
    explicit CRcTex(const CRcTex& rhs);
    ~CRcTex() override;

public:
    HRESULT Ready_Buffer() override;
    void    Render_Buffer() override;

    void    Set_UV(_float u0, _float v0, _float u1, _float v1);
    void    Set_Scale(_float width, _float height);

private:
    void    Apply_VertexSize();

public:
    static CRcTex* Create(DEVICE graphicDev);
    CComponent*    Clone() override;

private:
    void Free() override;

private:
    _float  m_HalfWidth;   
    _float  m_HalfHeight;

};

END
