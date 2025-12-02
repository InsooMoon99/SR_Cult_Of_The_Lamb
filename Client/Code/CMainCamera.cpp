#include "pch.h"
#include "CMainCamera.h"

#include "CCameraCom.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CRenderer.h"
#include "CTransform.h"

CMainCamera::CMainCamera(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_CamCalc(nullptr),
      m_TransformCom(nullptr),
      m_TargetTransformCom(nullptr)
{ }

CMainCamera::CMainCamera(const CMainCamera& rhs)
    : CGameObject(rhs),
      m_CamCalc(nullptr),
      m_TransformCom(nullptr),
      m_TargetTransformCom(nullptr)
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

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

    switch (m_CamCalc->Get_CamMode())
    {
    case CCameraCalc::CAM_FREE:
        Key_Input(timeDelta);
        break;
    case CCameraCalc::CAM_TARGET:
        Chase_CamTarget(timeDelta);
        break;
    default:
        break;
    }

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
    // 타겟이 변경되었는지 확인하고, 변경되었다면 새로운 타겟 설정
    if (m_TargetTransformCom != targetTransform)
        m_TargetTransformCom = targetTransform;

    // 새로운 타겟 상태에 따라 카메라 모드 설정
    if (nullptr == m_TargetTransformCom)
    {
        //MSG_BOX("Camera target set to null. Switching to Free Mode.");
        m_CamCalc->Set_CamMode(CCameraCalc::CAM_FREE);
    }
    else
        m_CamCalc->Set_CamMode(CCameraCalc::CAM_TARGET);

    return S_OK;
}

HRESULT CMainCamera::Add_Component()
{
    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Camera Component
    m_CamCalc = CreateProtoComponent<CCameraCalc>(this, COMPONENTTYPE::CAMERA);
    NULL_CHECK_RETURN(m_CamCalc, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::CAMERA, m_CamCalc });

    return S_OK;
}

void CMainCamera::Key_Input(const _float& timeDelta)
{
    auto inputMgr = CDInputMgr::GetInstance();

    const _float speed = 10.f;

    _vec3 dir    = { 0.f, 0.f, 0.f };
    bool  moving = false;

    if (inputMgr->Get_DIKeyState(DIK_LSHIFT) & 0x80)
    {
        // Y축 회전
        if (inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80)
        {
            m_TransformCom->Rotation(ROT_Y, D3DXToRadian(-180), timeDelta);
            moving = true;
        }

        if (inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80)
        {
            m_TransformCom->Rotation(ROT_Y, D3DXToRadian(180), timeDelta);
            moving = true;
        }

        // X축 회전
        if (inputMgr->Get_DIKeyState(DIK_UP) & 0x80)
        {
            m_TransformCom->Rotation_World(ROT_X, D3DXToRadian(-180), timeDelta);
            moving = true;
        }

        if (inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80)
        {
            m_TransformCom->Rotation_World(ROT_X, D3DXToRadian(180), timeDelta);
            moving = true;
        }
    }
    else
    {
        // 앞뒤
        dir = m_TransformCom->Get_Look();
        if (inputMgr->Get_DIKeyState(DIK_UP) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_TransformCom->Move_Pos(dir, timeDelta, speed);
            moving = true;
        }

        if (inputMgr->Get_DIKeyState(DIK_DOWN) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_TransformCom->Move_Pos(dir, timeDelta, -speed);
            moving = true;
        }

        // 좌우
        dir = m_TransformCom->Get_Right();
        if (inputMgr->Get_DIKeyState(DIK_RIGHT) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_TransformCom->Move_Pos(dir, timeDelta, speed);
            moving = true;
        }

        if (inputMgr->Get_DIKeyState(DIK_LEFT) & 0x80)
        {
            D3DXVec3Normalize(&dir, &dir);
            m_TransformCom->Move_Pos(dir, timeDelta, -speed);
            moving = true;
        }
    }

    // 상하
    dir = m_TransformCom->Get_Up();
    if (inputMgr->Get_DIKeyState(DIK_P) & 0x80)
    {
        D3DXVec3Normalize(&dir, &dir);
        m_TransformCom->Move_Pos(dir, timeDelta, speed);
        moving = true;
    }

    if (inputMgr->Get_DIKeyState(DIK_SEMICOLON) & 0x80)
    {
        D3DXVec3Normalize(&dir, &dir);
        m_TransformCom->Move_Pos(dir, timeDelta, -speed);
        moving = true;
    }
}

void CMainCamera::Chase_CamTarget(const _float& timeDelta)
{
    m_TransformCom->Set_Pos(m_TargetTransformCom->Get_Pos());
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
