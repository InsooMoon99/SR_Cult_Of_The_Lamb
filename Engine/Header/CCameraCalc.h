#pragma once
#include "CComponent.h"

BEGIN(Engine)
/// @brief 카메라를 위한 View/Proj 행렬 변환용 다이나믹 컴포넌트
class ENGINE_DLL CCameraCalc : public CComponent
{
private:
    explicit CCameraCalc(DEVICE graphicDev);
    explicit CCameraCalc(const CCameraCalc& rhs);
    ~CCameraCalc() override;

public:
    HRESULT Ready_CameraCalc();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

public:  // --- Getter ---
    const _matrix& Get_View() const { return m_matView; }    // 카메라의 뷰 행렬 Get
    const _matrix& Get_Proj() const { return m_matProj; }    // 카메라의 투영 행렬 Get

    PROJ_TYPE Get_ProjType() const { return m_ProjType; }

    _float Get_FOV() const { return m_FOV; }
    _float Get_Aspect() const { return m_Aspect; }
    _float Get_Near() const { return m_Near; }
    _float Get_Far() const { return m_Far; }

public:  // --- Setter ---
    void Set_ProjType(PROJ_TYPE projType);

    void Set_FOV(const _float _fov)
    {
        m_FOV     = _fov;
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

    void Set_Dirty() { m_isDirty = true; }

private: // --- 내부 로직 ---
    void Update_View_Matrix();
    void Update_Proj_Matrix();

private: // --- 멤버 변수 ---
    _matrix m_matView, m_matProj;

    PROJ_TYPE m_ProjType;   // 투영 타입

    _float m_FOV, m_Aspect, m_Near, m_Far;

    _bool m_isDirty;        // 투영 변환 재연산 플래그

public:
    static CCameraCalc* Create(DEVICE graphicDev);
    CComponent*         Clone() override;

private:
    void Free() override;
};

END
