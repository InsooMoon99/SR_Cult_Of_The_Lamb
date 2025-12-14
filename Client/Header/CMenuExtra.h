#pragma once
#include "CRenderObject.h"
class CMenuExtra :
    public CRenderObject
{
private:
    explicit CMenuExtra(DEVICE graphicDev);
    explicit CMenuExtra(const CMenuExtra& rhs);
    ~CMenuExtra() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:

    HRESULT Add_Component();


public:
   

private:
    CRcTex* m_BufferComOffset1;
    CRcTex* m_BufferComOffset2;

    

public:
    static CMenuExtra* Create(DEVICE graphicDev);

private:
    void Free() override;
};

