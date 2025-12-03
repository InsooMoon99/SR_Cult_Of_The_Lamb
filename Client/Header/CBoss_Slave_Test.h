#pragma once

#include "CCharacterBase.h"

BEGIN(Engine)
class CTexture;
class CAnimator;
END

class CBoss_SlaveTest : public CCharacterBase
{
private:
    explicit CBoss_SlaveTest(DEVICE graphicDev);
    explicit CBoss_SlaveTest(const CBoss_SlaveTest& rhs);
    ~CBoss_SlaveTest() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

private:
    void    Animation_Setting() override;
    HRESULT Add_Component()  override;

public:
    static CBoss_SlaveTest* Create(DEVICE graphicDev);

private:
    void Free() override;

};

