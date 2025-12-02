#include "pch.h"
#include "CBossPhase2.h"
#include "CCreateHelper.h"
#include "CAnimator.h"
#include "CTexture.h"

CBossPhase2::CBossPhase2(DEVICE graphicDev)
    : CMonsterBase(graphicDev)
{
}

CBossPhase2::CBossPhase2(const CBossPhase2& rhs)
    : CMonsterBase(rhs)
{
}

CBossPhase2::~CBossPhase2()
{
}

HRESULT CBossPhase2::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CBossPhase2::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();

}

_int CBossPhase2::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);


    return exit;
}

void CBossPhase2::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);

}

void CBossPhase2::Render_GameObject()
{
    CMonsterBase::Render_GameObject();

}

void CBossPhase2::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);

}

void CBossPhase2::OnStayOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnStayOverlap(self, other);

}

void CBossPhase2::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}

void CBossPhase2::Render_Editor()
{
    CMonsterBase::Render_Editor();

}

CBossPhase2* CBossPhase2::Create(DEVICE graphicDev)
{
    CBossPhase2* boss = new CBossPhase2(graphicDev);

    if (FAILED(boss->Ready_GameObject()))
    {
        Safe_Release(boss);
        MSG_BOX("Boss2 Create Failed");
        return nullptr;
    }

    return boss;
}

void CBossPhase2::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Boss_Idle", 58, 0.04f);

    m_AnimatorCom->Play_Animation(L"Boss_Idle", ANIMSTATE::LOOP);
}

HRESULT CBossPhase2::Add_Component()
{
    if(FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    COMPONENTTYPE texType = COMPONENTTYPE::TEX_BOSS2_PHASE2;

    m_TextureCom = CreateProtoComponent<CTexture>(this, texType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ texType, m_TextureCom });

    m_AnimatorCom->Set_TextureType(texType);

    return S_OK;
}

void CBossPhase2::Free()
{
    CMonsterBase::Free();
}
