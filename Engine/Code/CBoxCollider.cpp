#include "CBoxCollider.h"

#include "CBoxCol.h"
#include "CBoxTex.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CTransform.h"

CBoxCollider::CBoxCollider(DEVICE graphicDev)
    : CCollider(graphicDev, COLLIDERTYPE::BOX)
      , m_BufferCom(nullptr), m_Size(1.f, 1.f, 1.f)
{
    if (m_BufferCom)
        m_BufferCom->AddRef();

}

CBoxCollider::CBoxCollider(const CBoxCollider& rhs)
    : CCollider(rhs), m_BufferCom(rhs.m_BufferCom), m_Size(rhs.m_Size)
{
    if (m_BufferCom)
        m_BufferCom->AddRef();

}

CBoxCollider::~CBoxCollider()
{ }

HRESULT CBoxCollider::Ready_BoxCollider(DEVICE graphicDev)
{
    if (FAILED(CCollider::Ready_Collider()))
        return E_FAIL;

    // 렌더용 RcCol Create, 일단 테스트 후 프로토타입으로 변경
    m_BufferCom = CBoxCol::Create(graphicDev);

    if (m_BufferCom == nullptr)
        return E_FAIL;

    return S_OK;
}

_int CBoxCollider::Update_Component(const _float& timeDelta)
{
    _int exit = CCollider::Update_Component(timeDelta);

    return exit;
}

void CBoxCollider::LateUpdate_Component()
{
    CCollider::LateUpdate_Component();
}

void CBoxCollider::Render()
{
    if (m_ShowDebug == false || m_BufferCom == nullptr)
        return;

    CTransform* transform = Get_Transform();

    if (transform == nullptr)
        return;

    _vec3 pos = transform->Get_Pos();

    _vec3 worldSize = Get_Size();

    _matrix matScale, matTrans, matWorld;

    D3DXMatrixScaling(
        &matScale,
        worldSize.x,
        worldSize.y,
        worldSize.z);

    D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);

    matWorld = matScale * matTrans;

    m_GraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    // Collider는 와이어프레임으로 보이기
    DWORD prevFillMode = 0; // 현재 FillMode 저장
    m_GraphicDev->GetRenderState(D3DRS_FILLMODE, &prevFillMode);
    m_GraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    // Rect 그리기
    m_BufferCom->Render_Buffer();

    m_GraphicDev->SetRenderState(D3DRS_FILLMODE, prevFillMode); // 복원
}

bool CBoxCollider::CheckCollision(CCollider* other)
{
    if (other == nullptr)
        return false;

    switch (other->GetColliderType())
    {
    case COLLIDERTYPE::BOX:
        return CheckCollisionBox2Box(this, static_cast<CBoxCollider*>(other));

    case COLLIDERTYPE::SPHERE:
        return CheckCollisionSphere2Box(static_cast<CSphereCollider*>(other), this);
    }

    return false;
}

const _vec3 CBoxCollider::Get_Size() const
{
    const CTransform* transform = Get_Transform();

    _vec3 size = m_Size;

    if (transform)
    {
        _vec3 scale = transform->Get_Scale();

        size.x *= abs(scale.x);
        size.y *= abs(scale.y);
        size.z *= abs(scale.z);
    }

    return size;
}

CBoxCollider* CBoxCollider::Create(DEVICE GraphicDev)
{
    auto box = new CBoxCollider(GraphicDev);

    if (FAILED(box->Ready_BoxCollider(GraphicDev)))
    {
        Safe_Release(box);
        MSG_BOX("BoxCollider Create Failed");
        return nullptr;
    }

    return box;
}

CComponent* CBoxCollider::Clone()
{
    auto col = new CBoxCollider(*this);

    col->Ready_Collider();

    return col;
}

void CBoxCollider::Free()
{
    CCollider::Free();

    Safe_Release(m_BufferCom);
}
