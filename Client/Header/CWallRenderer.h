#pragma once

#include "CComponent.h"

class CWallRenderer : public CComponent
{
private:
    explicit CWallRenderer(DEVICE graphicDev);
    explicit CWallRenderer(const CWallRenderer& rhs);
    virtual  ~CWallRenderer() override;

public:
    HRESULT Ready_WallRenderer(DEVICE graphicDev);
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    void    Render_Editor() override;

    static CWallRenderer* Create(DEVICE graphicDev);
    CComponent* Clone() override;

protected:
    void Free() override;
};

