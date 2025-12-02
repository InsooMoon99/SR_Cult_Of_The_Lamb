#include "CCameraCalc.h"

#include "CDInputMgr.h"
#include "CTransform.h"

class CMainCamera;

CCameraCalc::CCameraCalc()
    : m_FOV(D3DXToRadian(60.f)),
      m_Aspect(static_cast<_float>(WINCX) / WINCY),
      m_Near(0.1f),
      m_Far(1000.f),

      m_isDirty(true)
{ }

CCameraCalc::CCameraCalc(DEVICE graphicDev)
    : CComponent(graphicDev),

      m_FOV(D3DXToRadian(60.f)),
      m_Aspect(static_cast<_float>(WINCX) / WINCY),
      m_Near(0.1f),
      m_Far(1000.f),

      m_isDirty(true)
{ }

CCameraCalc::CCameraCalc(const CCameraCalc& rhs)
    : CComponent(rhs),

      m_FOV(rhs.m_FOV),
      m_Aspect(rhs.m_Aspect),
      m_Near(rhs.m_Near),
      m_Far(rhs.m_Far),

      m_isDirty(true)
{ }

CCameraCalc::~CCameraCalc()
{ }

HRESULT CCameraCalc::Ready_Camera()
{
    return S_OK;
}

_int CCameraCalc::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CCameraCalc::LateUpdate_Component()
{
    NULL_CHECK(m_TransformCom);

    _vec3 pos  = m_TransformCom->Get_Pos();
    _vec3 Up   = m_TransformCom->Get_Up();
    _vec3 look = m_TransformCom->Get_Look();

    switch (m_ViewType)
    {
    case VIEW_FPS:
        Compute_View_FPS();
        break;
    case VIEW_TPS:
        Compute_View_TPS();
        break;
    case VIEW_QUARTER:
        Compute_View_QUARTER();
        break;
    default:
        break;
    }

    m_GraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

    // 투영 타입 변경 시에만 한 번 실행
    if (m_isDirty)
    {
        switch (m_ProjType)
        {
        case PROJ_PERSPECTIVE:
            Compute_Proj_Perspective();
            break;
        case PROJ_ORTHOGRAPHIC:
            Compute_Proj_Orthographic();
            break;
        default:
            break;
        }

        m_GraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
        m_isDirty = false;
    }
}

void CCameraCalc::Set_CamMode(CAM_MODE camMode)
{
    if (m_CamMode != camMode && camMode < CAM_END)
    {
        if (camMode == CAM_FREE)
            m_ViewType = VIEW_FPS;
        else if (camMode == CAM_TARGET)
            m_ViewType = VIEW_QUARTER;

        m_CamMode = camMode;
    }
}

void CCameraCalc::Set_ViewType(VIEW_TYPE viewType)
{
    if (m_ViewType != viewType && viewType < VIEW_END)
        m_ViewType = viewType;
}

void CCameraCalc::Set_ProjType(PROJ_TYPE projType)
{
    if (m_ProjType != projType && projType < PROJ_END)
    {
        m_ProjType = projType;
        m_isDirty  = true;
    }
}

void CCameraCalc::Set_Offset_Spherical(_float distance, _float angleX, _float angleY)
{
    _vec3 baseOffset = _vec3(0.f, 0.f, -distance);

    _matrix matRot;
    D3DXMatrixRotationYawPitchRoll(
        &matRot,
        D3DXToRadian(angleY),
        D3DXToRadian(angleX),
        0.f);

    D3DXVec3TransformCoord(&m_Offset, &baseOffset, &matRot);
}

void CCameraCalc::Compute_View_FPS()
{
    m_Eye = m_Pos;
    m_At  = m_Pos + m_Look;

    D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_At, &m_Up);
}

void CCameraCalc::Compute_View_TPS()
{
    _matrix matRotX, matRotY;
    D3DXMatrixRotationX(&matRotX, m_TransformCom->Get_Rotation(ROT_X));
    D3DXMatrixRotationY(&matRotY, m_TransformCom->Get_Rotation(ROT_Y));

    _matrix matRot = matRotY * matRotX;

    _vec3 vLocalOffset;
    D3DXVec3TransformCoord(&vLocalOffset, &m_Offset, &matRot);

    m_Eye = m_Pos + vLocalOffset;
    m_At  = m_Pos;

    const _vec3 vWorldUp = { 0, 1, 0 };

    D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_At, &vWorldUp);
}

void CCameraCalc::Compute_View_QUARTER()
{
    m_Eye = m_Pos + m_Offset;
    m_At  = m_Pos;

    const _vec3 vWorldUp = { 0, 1, 0 };

    D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_At, &vWorldUp);
}

void CCameraCalc::Compute_Proj_Perspective()
{
    D3DXMatrixPerspectiveFovLH(&m_matProj, m_FOV, m_Aspect, m_Near, m_Far);
}

void CCameraCalc::Compute_Proj_Orthographic()
{
    D3DVIEWPORT9 Viewport;
    m_GraphicDev->GetViewport(&Viewport);

    D3DXMatrixOrthoLH(
        &m_matProj,
        static_cast<_float>(Viewport.Width),
        static_cast<_float>(Viewport.Height),
        0.f,
        1.f);
}

CCameraCalc* CCameraCalc::Create(DEVICE graphicDev)
{
    auto CameraCom = new CCameraCalc(graphicDev);

    if (FAILED(CameraCom->Ready_Camera()))
    {
        Safe_Release(CameraCom);
        MSG_BOX("CameraCalc Create Failed");
        return nullptr;
    }

    return CameraCom;
}

CComponent* CCameraCalc::Clone()
{
    return new CCameraCalc(*this);
}

void CCameraCalc::Free()
{
    CComponent::Free();
}
