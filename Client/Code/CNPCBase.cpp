#include "pch.h"
#include "CNPCBase.h"

#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CInteractionChecker.h"
#include "CNPCController.h"
#include "CRcTex.h"
#include "CRigidbody.h"
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

    if (m_Rigidbody)
    {
        m_Rigidbody->Set_UseAccel(true);
        m_Rigidbody->Set_UseFriction(true);
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

    // 4) 텍스처 바인딩
    if (m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        int            frame = m_AnimatorCom->Get_CurFrame();
        // TextureCom은 각 NPC 자식이 가지고 있으므로 map에서 찾아서 바인딩하거나
        // m_TextureCom을 CNPCBase로 올리는 것도 방법이지만,
        // 현재 구조상 자식이 Add_Component에서 m_TextureCom을 멤버로 갖고 있지 않고
        // m_Components map에만 넣었을 수 있음.
        // 하지만 CFollowerCow를 보면 m_TextureCom 멤버변수를 사용함.
        // CNPCBase는 CCharacterBase를 상속받고, CCharacterBase는 CRenderObject를 상속받음.
        // CRenderObject에 m_TextureCom이 있음.
        // 따라서 m_TextureCom 사용 가능.
        if (m_TextureCom)
            m_TextureCom->Set_Texture(key, frame);
    }

    // 5) 기본 초기화
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    // 7) 버텍스 버퍼 렌더
    if (m_BufferCom)
        m_BufferCom->Render_Buffer();

    Render_Reset();

    // 충돌체 렌더 (디버그용)
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

    // --- Checker ---
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
