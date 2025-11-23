#pragma once
#include "CComponent.h"
#include "Engine_Define.h"
// @formatter:off

BEGIN(Engine)
/// @brief 상호작용이 있는 오브젝트에 대한 물리 컴포넌트
/// @details
/// - 질량, 마찰력, 힘, 가속, 속도, 속도 상한 변수를 통해 오브젝트를 제어
/// - 중력 작용은 비활성화 상태
class ENGINE_DLL CRigidbody : public CComponent
{
private:
    explicit CRigidbody(DEVICE graphicDev);
    explicit CRigidbody(const CRigidbody& rhs);
    ~CRigidbody() override;

public:
    HRESULT Ready_Rigidbody();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;
    void    Render_Editor() override;

public: // Getter
    _bool  Get_IsGround()      const { return m_IsGround; }
    _float Get_Mass()          const { return m_Mass; }
    _float Get_Friction()      const { return m_Friction; }
    _vec3  Get_Force()         const { return m_Force; }
    _vec3  Get_Acceleration()  const { return m_Accel; }
    _vec3  Get_Velocity()      const { return m_Vel; }
    _vec3  Get_VelocityLimit() const { return m_VelLimit; }
    //_bool  Get_GravityOn()   const { return m_GravityOn; }
    //_vec3  Get_Gravity()     const { return m_Gravity; }

public: // Setter
    void Set_IsGround(_bool isGround)      { m_IsGround = isGround; }
    void Set_Mass(_float mass)             { m_Mass = mass; }
    void Set_Friction(_float friction)     { m_Friction = friction; }
    void Add_Force(const _vec3& vForce)    { m_Force += vForce; } // 힘을 누적하는 함수 (이동, 점프, 넉백 등에 사용)
    void Set_Acceleration(_vec3 accel)     { m_Accel = accel; }
    void Set_Velocity(_vec3 velocity)      { m_Vel = velocity; }
    void Set_VelocityLimit(_vec3 velLimit) { m_VelLimit = velLimit; }
    //void Set_GravityOn(_bool gravityOn)  { m_GravityOn = gravityOn; }
    //void Set_Gravity(_vec3 gravity)      { m_Gravity = gravity; }

private:
    _bool m_GravityOn;  // 중력 온오프 스위치
    _bool m_IsGround;   // 지상일 때 true. 중력, 마찰 계산에 영향을 미침

    _float m_Mass;
    _float m_Friction;

    _vec3 m_Force;
    _vec3 m_Accel;
    _vec3 m_Vel;
    _vec3 m_VelLimit;   // 속력 상한
    _vec3 m_Gravity;

public:
    static CRigidbody* Create(DEVICE graphicDev);

    CComponent* Clone() override;

private:
    void Free() override;
};

END
// @formatter:on
