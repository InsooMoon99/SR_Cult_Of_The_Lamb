#include "pch.h"
#include "CMainCamera.h"

#include "CCameraCom.h"
#include "CCameraController.h"
#include "CCameraMovement.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CRenderer.h"
#include "CTransform.h"

CMainCamera::CMainCamera(DEVICE graphicDev)
    : CGameObject(graphicDev),

      m_TransformCom(nullptr),
      m_CamCalc(nullptr),
      m_CamController(nullptr),
      m_CamMovement(nullptr)
{ }

CMainCamera::CMainCamera(const CMainCamera& rhs)
    : CGameObject(rhs),

      m_TransformCom(nullptr),
      m_CamCalc(nullptr),
      m_CamController(nullptr),
      m_CamMovement(nullptr)
{ }

CMainCamera::~CMainCamera()
{ }

HRESULT CMainCamera::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CMainCamera::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    return exit;
}

void CMainCamera::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CMainCamera::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
}

void CMainCamera::Render_Editor()
{
    CGameObject::Render_Editor();

    ImGui::Text("=== Main Camera === ");
}

HRESULT CMainCamera::Set_CamTarget(CTransform* targetTransform)
{
    if (m_CamController)
        m_CamController->Set_CamTarget(targetTransform);

    return S_OK;
}

HRESULT CMainCamera::Add_Component()
{
    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Camera Component
    m_CamCalc = CreateProtoComponent<CCameraCalc>(this, COMPONENTTYPE::CAM_CALC);
    NULL_CHECK_RETURN(m_CamCalc, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::CAM_CALC, m_CamCalc });

    // Camera Movement
    m_CamMovement = CCameraMovement::Create(m_GraphicDev);
    NULL_CHECK_RETURN(m_CamMovement, E_FAIL);

    return S_OK;
}

void CMainCamera::Key_Input(const _float& timeDelta)
{
    // Moved to CCameraController
}

void CMainCamera::Chase_CamTarget(const _float& timeDelta)
{
    // Moved to CCameraMovement
}

CMainCamera* CMainCamera::Create(DEVICE graphicDev)
{
    auto mainCamera = new CMainCamera(graphicDev);

    if (FAILED(mainCamera->Ready_GameObject()))
    {
        MSG_BOX("MainCamera Create Failed");
        Safe_Release(mainCamera);
        return nullptr;
    }

    return mainCamera;
}

void CMainCamera::Free()
{
    CGameObject::Free();
}
