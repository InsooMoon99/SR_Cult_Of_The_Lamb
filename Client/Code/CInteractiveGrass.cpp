#include "pch.h"
#include "CInteractiveGrass.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "ItemInstance.h"
#include "CInventory.h"
#include "CCombatStat.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CParticleHelper.h"
#include "CBoxCollider.h"

CInteractiveGrass::CInteractiveGrass(DEVICE graphicDev):CWorldInteractive(graphicDev)
{
}

CInteractiveGrass::CInteractiveGrass(const CInteractiveGrass& rhs): CWorldInteractive(rhs)
{
}

CInteractiveGrass::~CInteractiveGrass()
{
}

HRESULT CInteractiveGrass::Ready_GameObject()
{
    if (FAILED(CWorldInteractive::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_Hp(1.f);
    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
    m_DeadTexIndex = 1;
    m_CurTexIndex = 0;

    if (m_BoxColCom)
    {
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::INTERACTIVEOBJ);
        m_BoxColCom->Set_Size({ 3.f, 3.f, 3.f });
    }
    m_InventoryCom->SetInvenSlotNum(3);

    return S_OK;
}

void CInteractiveGrass::Initialize_GameObject()
{
    CWorldInteractive::Initialize_GameObject();
    ItemInstance Item = { 1003, -1, 0 };

    for (int i = 0; i < 3; i++)
    {
        m_InventoryCom->AddItem(Item);
    }

}

_int CInteractiveGrass::Update_GameObject(const _float& timeDelta)
{
    _int exit = CWorldInteractive::Update_GameObject(timeDelta);

    
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    if (m_CombatStatCom->Is_Dead() == true)
    {
        m_CurTexIndex = m_DeadTexIndex;
    }
    return exit;
}

void CInteractiveGrass::LateUpdate_GameObject(const _float& timeDelta)
{
    CWorldInteractive::LateUpdate_GameObject(timeDelta);
}

void CInteractiveGrass::Render_GameObject()
{
    m_TextureCom->Set_Texture(m_CurTexIndex);
    CWorldInteractive::Render_GameObject();

}

void CInteractiveGrass::OnInteractiveDamaged()
{
    CWorldInteractive::OnInteractiveDamaged();
    CParticleHelper::Spawn_StaticBurstParticle(
        m_TransformCom->Get_Pos(),          // 위치
        COMPONENTTYPE::TEX_GRASS_PARTICLE,  // 텍스처 타입
        5,                                  // 생성할 개수
        0.1f, 0.12f,                 // 크기 범위
        6,                                  // 텍스처 랜덤으로 몇개 출력
        15.f,                               // 땅에 떨어지는 속도
        m_TransformCom->Get_Pos().y - 0.5f  // 바닥 Y (돌 아래쪽)
    );
}

HRESULT CInteractiveGrass::Add_Component()
{
    if (FAILED(CWorldInteractive::Add_Component()))
        return E_FAIL;
    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_INTERACTIVEOBJ);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_INTERACTIVEOBJ, m_TextureCom });

    return S_OK;
}
void CInteractiveGrass::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CWorldInteractive::OnBeginOverlap(self, other);
}

void CInteractiveGrass::OnEndOverlap(CCollider* self, CCollider* other)
{
    CWorldInteractive::OnEndOverlap(self, other);
}

void CInteractiveGrass::Free()
{
    CWorldInteractive::Free();

}
CInteractiveGrass* CInteractiveGrass::Create(DEVICE graphicDev)
{
    auto interactiveGrass = new CInteractiveGrass(graphicDev);

    if (FAILED(interactiveGrass->Ready_GameObject()))
    {
        MSG_BOX("HarvestableHerb Create Failed");
        Safe_Release(interactiveGrass);
        return nullptr;
    }

    return interactiveGrass;
}
