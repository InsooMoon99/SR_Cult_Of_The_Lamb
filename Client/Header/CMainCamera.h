#pragma once
#include "CGameObject.h"

BEGIN(Engine)
class CCameraCalc;
class CTransform;
END

class CCameraController;
class CCameraMovement;

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

public:  // --- Getter ---

public:  // --- 주요 기능 ---

private: // --- 내부 로직 ---
    HRESULT Add_Component();

private: // --- 소유 컴포넌트 ---
    Engine::CTransform*  m_TransformCom;     // 카메라 자체 Transform
    Engine::CCameraCalc* m_CamCalc;         // 카메라 행렬 연산 컴포넌트

    CCameraController* m_CamController;     // 카메라 컨트롤러 컴포넌트
    CCameraMovement*   m_CamMovement;       // 카메라 이동 컴포넌트

public:
    static CMainCamera* Create(DEVICE graphicDev);

private:
    void Free() override;
};
