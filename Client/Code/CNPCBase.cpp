#include "pch.h"
#include "CNPCBase.h"

#include "CCreateHelper.h"
#include "CInteractionChecker.h"
#include "CNPCController.h"

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
    CCharacterBase::Render_GameObject();
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
        return E_FAIL;

    // BoxCollider
    m_Checker = CreateProtoComponent<CInteractionChecker>(
        this,
        COMPONENTTYPE::NPC_INTERACTION_CHECKER);
    NULL_CHECK_RETURN(m_Checker, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::NPC_INTERACTION_CHECKER, m_Checker });

    // NPC Controller
    // TODO: COMPONENTTYPE::NPC_CONTROLLER 로 변경 필요 (유저가 직접 등록 예정)
    // 현재는 임시로 주석 처리 또는 유저가 정의한 Enum 사용 가정
    /*
    m_NPCController = CreateProtoComponent<CNPCController>(
        this, 
        COMPONENTTYPE::NPC_CONTROLLER);
    NULL_CHECK_RETURN(m_NPCController, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::NPC_CONTROLLER, m_NPCController });
    */

    return S_OK;
}

void CNPCBase::Free()
{
    CCharacterBase::Free();
}
