#pragma once
#include "CCameraCom.h"
#include "CGameObject.h"

BEGIN(Engine)
class CTransform;
END

class CMainCamera : public CGameObject
{
private:
    explicit CMainCamera(DEVICE graphicDev);
    explicit CMainCamera(const CMainCamera& rhs);
    ~CMainCamera() override;

public:
    HRESULT Ready_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;
    void    Render_Editor() override;

public:  // --- Setter ---
    HRESULT Set_CamTarget(CTransform* targetTransform); // 카메라가 추적할 타겟 세팅

public:  // --- Getter ---

public:  // --- 주요 기능 ---
    // TODO : 카메라 이동, 카메라 쉐이킹 등 기능 함수

private: // --- 내부 로직 ---
    HRESULT Add_Component();

    void Key_Input(const _float& timeDelta);
    void Chase_CamTarget(const _float& timeDelta);

private: // --- 소유 컴포넌트 ---
    Engine::CCameraCalc* m_CameraCom;
    Engine::CTransform* m_TransformCom;

private: // --- 외부 컴포넌트 캐싱 ---
    Engine::CTransform* m_TargetTransformCom;   // 카메라 타겟의 Transform

    _vec3 m_Offset;        // 3인칭 카메라의 타겟 기준 오프셋 (거리)

public:
    static CMainCamera* Create(DEVICE graphicDev);

private:
    void Free() override;
};
