#include "pch.h"
#include "CBoss_Slave_Test.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CAnimator.h"
#include "CBoxCollider.h"

CBoss_SlaveTest::CBoss_SlaveTest(DEVICE graphicDev)
    : CCharacterBase(graphicDev)
{
}

CBoss_SlaveTest::CBoss_SlaveTest(const CBoss_SlaveTest& rhs)
    : CCharacterBase(rhs)
{
}

CBoss_SlaveTest::~CBoss_SlaveTest()
{
}

HRESULT CBoss_SlaveTest::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(25.f, 3.1f, 13.f));
    m_TransformCom->Set_Scale(_vec3(1.8f, 3.8f, 1.f));

    m_BoxColCom->Set_CollisionActive(false);

    return S_OK;
}

_int CBoss_SlaveTest::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_ALPHA, this);

    return exit;
}

void CBoss_SlaveTest::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);

}

void CBoss_SlaveTest::Render_GameObject()
{
    CCharacterBase::Render_GameObject();

}

void CBoss_SlaveTest::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"Boss_Slave_Appear", 42, 0.02f);

    m_AnimatorCom->Play_Animation(L"Boss_Slave_Appear", ANIMSTATE::LOOP);
}

HRESULT CBoss_SlaveTest::Add_Component()
{
    if (FAILED(CCharacterBase::Add_Component()))
        return E_FAIL;

    COMPONENTTYPE texType = COMPONENTTYPE::TEX_SLAVE_TEST;

    m_TextureCom = CreateProtoComponent<CTexture>(this, texType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ texType, m_TextureCom });

    m_AnimatorCom->Set_TextureType(texType);


    return S_OK;
}

CBoss_SlaveTest* CBoss_SlaveTest::Create(DEVICE graphicDev)
{
    auto slave = new CBoss_SlaveTest(graphicDev);

    if (FAILED(slave->Ready_GameObject()))
    {
        MSG_BOX("Boss_Slave Create Failed");
        Safe_Release(slave);
        return nullptr;
    }

    return slave;
}

void CBoss_SlaveTest::Free()
{
    CCharacterBase::Free();
}
