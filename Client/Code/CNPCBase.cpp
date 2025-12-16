#include "pch.h"
#include "CNPCBase.h"

#include <CDInputMgr.h>
#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CInteractionChecker.h"
#include "CNPCController.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CRigidbody.h"
#include "CStage.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"

CNPCBase::CNPCBase(DEVICE graphicDev)
    : CCharacterBase(graphicDev)
{ }

CNPCBase::CNPCBase(const CNPCBase& rhs)
    : CCharacterBase(rhs)
{ }

CNPCBase::~CNPCBase()
{ }

HRESULT CNPCBase::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CNPCBase::Initialize_GameObject()
{
    CCharacterBase::Initialize_GameObject();

    // NPC는 방향에 따라 자동 반전을 사용
    if (m_StateCom)
        m_StateCom->Set_StateIndependentDir(true);

    if (m_CombatStatCom)
        m_CombatStatCom->Set_Speed(3.f);

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    if (m_Rigidbody)
    {
        // 벽 충돌판정을 위해 존재는 하나 연산은 하나도 안하겠다
        m_Rigidbody->Set_UseAccel(false);
        m_Rigidbody->Set_UseFriction(false);
        m_Rigidbody->Set_UseGravity(false);
    }
}

_int CNPCBase::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);

    m_bHovering = Is_Hovering();


    return exit;
}

void CNPCBase::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);
}

void CNPCBase::Render_GameObject()
{

    if (m_TransformCom)
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    // 텍스처 바인딩
    if (m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        int            frame = m_AnimatorCom->Get_CurFrame();

        if (m_TextureCom)
            m_TextureCom->Set_Texture(key, frame);
    }

    // 기본 초기화
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    // 버텍스 버퍼 렌더
    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    Render_Reset();

    // 충돌체 렌더
    if (m_BoxColCom)
        m_BoxColCom->Render();
}

void CNPCBase::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnBeginOverlap(self, other);
    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (ply)
        m_bPlayerOverlap = true;
}

void CNPCBase::OnStayOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnStayOverlap(self, other);
}

void CNPCBase::OnEndOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnEndOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());

    if (m_bPlayerOverlap == true && ply != nullptr)
        m_bPlayerOverlap = false;

}

bool CNPCBase::Is_Hovering()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);

    D3DVIEWPORT9 ViewPort;
    ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

    m_GraphicDev->GetViewport(&ViewPort);

    _vec3 vMousePos;

    //vMousePos.x = pt.x / (ViewPort.Width * 0.5) - 1.f;
    //vMousePos.y = pt.y / (ViewPort.Height * 0.5) + 1.f;
    //vMousePos.z = 1.f;
    vMousePos.x = ((2.0f * pt.x) / ViewPort.Width) - 1.0f;
    vMousePos.y = ((-2.0f * pt.y) / ViewPort.Height) + 1.0f;
    vMousePos.z = 1.0f; // z=1로 해야 정상적인 pick ray가 됨

    D3DXMATRIX matProj;
    m_GraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixInverse(&matProj, nullptr, &matProj);
    //coord
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

    D3DXMATRIX matView;
    m_GraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, nullptr, &matView);
    // 뷰 행렬에서 레이랑 해야함
    _vec3 vRayPos = { matView._41, matView._42, matView._43 };// RayPos 는 카메라 위치
    //_vec3 vRayDir = vMousePos - vRayPos;

    _vec3 vRayDir; //마우스 방향을 월드 방향으로 변환
    D3DXVec3TransformNormal(&vRayDir, &vMousePos, &matView);
    D3DXVec3Normalize(&vRayDir, &vRayDir);

    /////////

    // 이건 할 필요 없음 월드 공간 레이 vs 월드 AABB 테스트 할거라서
    /*D3DXMATRIX matWorld = (this->Get_Transform()->Get_World());
    D3DXMatrixInverse(&matWorld, 0, &matWorld);

    D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
    D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);*/

    //_vec3 half = m_BoxColCom->Get_RawSize() * .5f;

    _vec3 Min = m_BoxColCom->Get_Transform()->Get_Pos() - m_BoxColCom->Get_Size() * .5f;
    _vec3 Max = m_BoxColCom->Get_Transform()->Get_Pos() + m_BoxColCom->Get_Size() * .5f;


    float timeMin = (Min.x - vRayPos.x) / vRayDir.x;
    float timeMax = (Max.x - vRayPos.x) / vRayDir.x;
    if (timeMin > timeMax) swap(timeMin, timeMax);

    float timeMinY = (Min.y - vRayPos.y) / vRayDir.y;
    float timeMaxY = (Max.y - vRayPos.y) / vRayDir.y;
    if (timeMinY > timeMaxY) swap(timeMinY, timeMaxY);

    if (timeMinY > timeMax || timeMin > timeMaxY) return false;

    if (timeMinY > timeMin) timeMin = timeMinY;
    if (timeMaxY < timeMax) timeMax = timeMaxY;

    float timeMinZ = (Min.z - vRayPos.z) / vRayDir.z;
    float timeMaxZ = (Max.z - vRayPos.z) / vRayDir.z;
    if (timeMinZ > timeMaxZ) swap(timeMinZ, timeMaxZ);

    if (timeMinZ > timeMax || timeMin > timeMaxZ) return false;

    if (timeMinZ > timeMin) timeMin = timeMinZ;
    if (timeMaxZ < timeMax) timeMax = timeMaxZ;

    //

    if (timeMin > 0.f)
        return true;

    return false;
}

HRESULT CNPCBase::Add_Component()
{
    if (FAILED(CCharacterBase::Add_Component()))
    {
        MSG_BOX("CNPCBase: CCharacterBase::Add_Component Failed");
        return E_FAIL;
    }

    // --- Interaction Checker ---
    m_Checker = CreateProtoComponent<CInteractionChecker>(
        this,
        COMPONENTTYPE::NPC_INTERACTION_CHECKER);

    NULL_CHECK_RETURN(m_Checker, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::NPC_INTERACTION_CHECKER, m_Checker });

    // --- NPC Controller ---
    m_NPCController = CreateProtoComponent<CNPCController>(
        this,
        COMPONENTTYPE::NPC_CONTROLLER);

    NULL_CHECK_RETURN(m_NPCController, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::NPC_CONTROLLER, m_NPCController });

    // --- Rigidbody ---
    m_Rigidbody = CreateProtoComponent<CRigidbody>(
        this,
        COMPONENTTYPE::RIGIDBODY);

    NULL_CHECK_RETURN(m_Rigidbody, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_Rigidbody });

    return S_OK;
}

void CNPCBase::Free()
{
    CCharacterBase::Free();
}
