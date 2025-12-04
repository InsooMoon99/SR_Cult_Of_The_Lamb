#pragma once
#include "CEffectBaseCom.h"

struct TrailNode
{
    _vec3 pos;  // 월드 위치
    float life; // 현제 남은 수명
    float maxLife;//초기 최대 수명
};


class CTrailEff :
    public CEffectBaseCom
{
protected:
    explicit CTrailEff();
    explicit CTrailEff(DEVICE graphicDev);
    explicit CTrailEff(const CTrailEff& rhs);
    ~CTrailEff() override;

public:
    HRESULT Ready_Effect() override;
    void      Initialize_Component()override;
    _int      Update_Component(const _float& timeDelta)override;
    void      LateUpdate_Component()override;
    void       Render_Effect() override;

public:
    CTrailEff* Create(DEVICE graphicDev);
    CComponent* Clone();

protected:
    void Free() override;
};

