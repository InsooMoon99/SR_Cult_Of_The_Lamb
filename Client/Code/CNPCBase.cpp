#include "pch.h"
#include "CNPCBase.h"

#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CInteractionChecker.h"
#include "CNPCController.h"
#include "CRcTex.h"
#include "CRigidbody.h"
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
}

void CNPCBase::OnStayOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnStayOverlap(self, other);
}

void CNPCBase::OnEndOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnEndOverlap(self, other);
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
