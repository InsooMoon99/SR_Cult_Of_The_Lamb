#pragma once
#include "CComponent.h"

class CCameraMovement : public CComponent
{
private:
    explicit CCameraMovement(DEVICE graphicDev);
    explicit CCameraMovement(const CCameraMovement& rhs);
    ~CCameraMovement() override;

public:
    HRESULT Ready_CameraMovement();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- Getter ---

public:  // --- Getters for Inspector --- : 필요할 경우에 사용

public:  // --- Setter ---

public:  // --- 기능 메서드 ---
    // TODO : 카메라 이동, 카메라 쉐이킹 등 기능 함수

private: // --- 내부 로직 ---

private: // --- Owner Components 캐싱 ---

private: // --- 일반 멤버 변수 ---

public:
    static CCameraMovement* Create(DEVICE graphicDev);
    CComponent*             Clone() override;

private:
    void Free() override;
};
