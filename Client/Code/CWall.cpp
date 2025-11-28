#include "pch.h"
#include "CWall.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CTransform.h"
#include "CCreateHelper.h"
#include "CRigidbody.h"

CWall::CWall(DEVICE graphicDev)
    : CGameObject(graphicDev), m_TransformCom(nullptr), m_BoxCollider(nullptr)
{
}

CWall::CWall(const CWall& rhs)
    : CGameObject(rhs), m_TransformCom(nullptr), m_BoxCollider(nullptr)
{
}

CWall::~CWall()
{ }

HRESULT CWall::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
    m_TransformCom->Set_Pos(_vec3(10.f, 0.f, 10.f));

    m_BoxCollider->Set_Size(_vec3(3.f, 3.f, 3.f));

    return S_OK;
}

_int CWall::Update_GameObject(const _float& timeDelta)
{                                                                                                                                                                                                                                                                                                                                                                                        
    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDERID::RENDER_PRIORITY, this);

    return exit;
}

void CWall::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);

}

void CWall::Render_GameObject()
{
    if (m_BoxCollider == nullptr)
        return;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
    m_BoxCollider->Render();

}

void CWall::Render_Editor()
{
    CGameObject::Render_Editor();
}

void CWall::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);

    
}

void CWall::OnStayOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnStayOverlap(self, other);

    CGameObject* hitObject = other->Get_Owner();
    if (!hitObject) return;

    // 속도 제거
    CRigidbody* rigid = dynamic_cast<CRigidbody*>(
        hitObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    if (rigid)
    {
        _vec3 vel = rigid->Get_Velocity();
        vel.x = 0.f;
        vel.z = 0.f;
        rigid->Set_Velocity(vel);
    }

    CTransform* otherTransform = dynamic_cast<CTransform*>(
        hitObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    if (!otherTransform) return;

    _vec3 wallPos = m_TransformCom->Get_Pos();
    _vec3 objPos = otherTransform->Get_Pos();
    _vec3 wallSize = m_BoxCollider->Get_Size();

    CBoxCollider* playerCollider = dynamic_cast<CBoxCollider*>(other);
    if (playerCollider == nullptr) return;

    _vec3 playerSize = playerCollider->Get_Size();

    float dx = objPos.x - wallPos.x;
    float dz = objPos.z - wallPos.z;

    float halfSumX = (wallSize.x + playerSize.x) * 0.5f;
    float halfSumZ = (wallSize.z + playerSize.z) * 0.5f;

    float penetrationX = halfSumX - abs(dx);
    float penetrationZ = halfSumZ - abs(dz);

    _vec3 originalPos = objPos;  // 원래 위치 저장

    if (penetrationX < penetrationZ)
    {
        if (dx > 0) // 오른쪽
            objPos.x = wallPos.x + halfSumX + 0.01f;
        else        // 왼쪽
            objPos.x = wallPos.x - halfSumX - 0.01f;
    }
    else
    {
        if (dz > 0) 
            objPos.z = wallPos.z + halfSumZ + 0.01f;
        else        
            objPos.z = wallPos.z - halfSumZ - 0.01f;
    }

    otherTransform->Set_Pos(objPos);
}

HRESULT CWall::Add_Component()
{
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_BoxCollider = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxCollider, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxCollider });

    return S_OK;
}

CWall* CWall::Create(DEVICE graphicDev)
{
    CWall* wall = new CWall(graphicDev);

    if (FAILED(wall->Ready_GameObject()))
    {
        Safe_Release(wall);
        MSG_BOX("Create Failed Wall !");
        return nullptr;
    }

    return wall;
}

void CWall::Free()
{
    CGameObject::Free();

}
