#pragma once
#include "CEffectBaseCom.h"

struct SpriteDesc
{
    // 일단 바운스 기준으로 필요 한거
    _vec3 pos;
    _vec3 Velocity;
    float Ground;
};


class CSpriteEff :
    public CEffectBaseCom
{
protected:
    explicit CSpriteEff();
    explicit CSpriteEff(DEVICE graphicDev);
    explicit CSpriteEff(const CSpriteEff& rhs);
    ~CSpriteEff() override;

public:
    HRESULT     Ready_Effect() override;
    void      Initialize_Component()override;
    _int      Update_Component(const _float& timeDelta)override;
    void      LateUpdate_Component()override;
    void       Render_Effect() override;

    void SetSpriteDesc(SpriteDesc s)
    {

    }

    bool is_SetSpriteDesc;

public:
    CSpriteEff* Create(DEVICE graphicDev);
    CComponent* Clone();

private:
    // 간단 물리
    float m_Gravity;
    


protected:
    void Free() override;
};

