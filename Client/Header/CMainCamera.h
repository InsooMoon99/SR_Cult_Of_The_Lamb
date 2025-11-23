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

public: // Setter
    HRESULT Set_CamTarget(CTransform* targetTransform); // 카메라가 추적할 타겟 세팅

public: // Getter

public: // TODO : 카메라 이동, 카메라 쉐이킹 등 기능 함수

private:
    HRESULT Add_Component();

    void Key_Input(const _float& timeDelta);
    void Chase_CamTarget(const _float& timeDelta);

private:
    Engine::CCameraCom* m_CameraCom;
    Engine::CTransform* m_TransformCom;

    Engine::CTransform* m_TargetTransformCom;   // 따라가는 타겟의 Transform에 접근

public:
    static CMainCamera* Create(DEVICE graphicDev);

private:
    void Free() override;
};
