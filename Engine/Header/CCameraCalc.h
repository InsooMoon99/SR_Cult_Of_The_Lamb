#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTransform;

/// @brief 카메라를 위한 View/Proj 행렬 변환용 컴포넌트
class ENGINE_DLL CCameraCalc : public CComponent
{
private:
    explicit CCameraCalc();
    explicit CCameraCalc(DEVICE graphicDev);
    explicit CCameraCalc(const CCameraCalc& rhs);
    ~CCameraCalc() override;

public:
    HRESULT Ready_Camera();
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public: // Setter
    void Set_FOV(const _float fov)
    {
        m_FOV     = fov;
        m_isDirty = true;
    }

    void Set_Aspect(const _float _aspect)
    {
        m_Aspect  = _aspect;
        m_isDirty = true;
    }

    void Set_Near(const _float _near)
    {
        m_Near    = _near;
        m_isDirty = true;
    }

    void Set_Far(const _float _far)
    {
        m_Far     = _far;
        m_isDirty = true;
    }

public: // Getter
    const _matrix& Get_View() const { return m_matView; }    // 카메라의 뷰 행렬 Get
    const _matrix& Get_Proj() const { return m_matProj; }    // 카메라의 투영 행렬 Get

    _float Get_FOV() const { return m_FOV; }
    _float Get_Aspect() const { return m_Aspect; }
    _float Get_Near() const { return m_Near; }
    _float Get_Far() const { return m_Far; }

private:
    void Compute_View_FPS();
    void Compute_View_TPS();
    void Compute_View_QUARTER();

    void Compute_Proj_Perspective();
    void Compute_Proj_Orthographic();

private:
    _matrix m_matView, m_matProj;

    _float m_FOV, m_Aspect, m_Near, m_Far;

    _bool m_isDirty;        // 투영 변환 재연산 트리거

public:
    static CCameraCalc* Create(DEVICE graphicDev);
    CComponent*         Clone() override;

private:
    void Free() override;
};

END
