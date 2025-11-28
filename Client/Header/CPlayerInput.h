#pragma once
#include "CComponent.h"

/// @brief 키보드/마우스 입력을 받아 그 방향을 구해 다른 컴포넌트로 전달하는 다이나믹 컴포넌트
class CPlayerInput : public CComponent
{
private:
    explicit CPlayerInput(DEVICE graphicDev);
    explicit CPlayerInput(const CPlayerInput& rhs);
    ~CPlayerInput() override;

public:
    HRESULT Ready_PlayerInput();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:     // --- Getter ---
    const _vec3& Get_MoveDir() const { return m_MoveDir; }
    const _vec3& Get_AttackDir() const { return m_MouseDir; }

private:    // --- 멤버 변수 ---
    _vec3 m_MoveDir;    // 이동 방향
    _vec3 m_MouseDir;   // 마우스 방향. 공격 등에 사용

public:
    static CPlayerInput* Create(DEVICE graphicDev);
    CComponent*          Clone() override;

private:
    void Free() override;
};
