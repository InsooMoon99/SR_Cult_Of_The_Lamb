#pragma once
#include "CEffectBaseCom.h"
#include "Client_Struct.h"
struct SpriteDesc
{
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

    bool BounceEff(const _float& timeDelta);


    void SetSpriteDesc(SpriteDesc s)
    {

    }

public:
    void InitBounce(SpriteBounceDesc& b)
    {
        m_Pos = b.pos;
        m_Velocity = b.velocity;
        m_GroundY = b.groundY;
        m_Gravity = b.gravity;
        m_Restitution = b.restitution;
        m_Damping = b.damping;
        m_DampingPerSecond = b.m_DampingPerSecond;

        m_IsSetBouncingInit = true;
    }
public:
    static CSpriteEff* Create(DEVICE graphicDev);
    CComponent* Clone();

private:
    bool is_SetSpriteDesc;

    // 간단 물리//////
    _vec3 m_Pos;        // 위치
    _vec3 m_Velocity;   // 초기 힘
    float m_GroundY;    // 땅 위치
    float m_Gravity;
    float m_Restitution; // 튕김계수 (1: 비탄성 ~ 0: 탄성)
    float m_Damping;    // 수평 속도 감쇠 (튕길떄)
    float m_DampingPerSecond;    // 수평 속도 감쇠 (공기저항)


    bool m_IsSetBouncingInit = false;
    //    

    float m_STOP_VEL = 5.f;
protected:
    void Free() override;
};

