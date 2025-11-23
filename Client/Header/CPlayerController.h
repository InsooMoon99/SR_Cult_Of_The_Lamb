#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CCombatStat;
class CState;
END

/// @brief 플레이어의 조작을 담당하는 컴포넌트
class CPlayerController : public CComponent
{
private:
    struct KeyInputData
    {
        _vec3 moveDir = { 0.f, 0.f, 0.f };
        int   axisX{};
        int   axisZ{};
        bool  isMove{};
    };

private:
    explicit CPlayerController(DEVICE graphicDev);
    explicit CPlayerController(const CPlayerController& rhs);
    ~CPlayerController() override;

public:
    HRESULT Ready_PlayerController();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:
    // m_OwnerStateCom 등 Owner의 컴포넌트를 가져와 사용하기 위한 초기 세팅 함수
    HRESULT Init_OwnerComponent();

private:
    void Key_Input(const _float& timeDelta);
    void Calc_InputVector(KeyInputData& inputData); // 입력 벡터 및 축 값 계산
    void Determine_State(KeyInputData& inputData); // 입력을 바탕으로 State에 상태 변경 요청
    void Apply_Direction(KeyInputData& inputData); // 입력을 바탕으로 애니메이션 방향 설정
    void Apply_Movement(KeyInputData& inputData, const _float& timeDelta); // 입력을 바탕으로 이동

private:
#pragma region Caching owner components 가나다라

    Engine::CState*      m_OwnerStateCom;
    Engine::CCombatStat* m_OwnerCombatStat;

#pragma endregion

public:
    static CPlayerController* Create(DEVICE graphicDev);
    CComponent*               Clone() override;

protected:
    void Free() override;
};
