#include "pch.h"
#include "CDottedCircle.h"

#include <CTimerMgr.h>
#include "CCollider.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CScene.h"
#include "CTemple.h"
#include "CTexture.h"
#include "CTransform.h"

CDottedCircle::CDottedCircle(DEVICE graphicDev)
    : CBuildingBase(graphicDev)
{ }

CDottedCircle::CDottedCircle(const CDottedCircle& rhs)
    : CBuildingBase(rhs)
{ }

CDottedCircle::~CDottedCircle()
{ }

HRESULT CDottedCircle::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

void CDottedCircle::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(90.f));
}

_int CDottedCircle::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    return exit;
}

void CDottedCircle::LateUpdate_GameObject(const _float& timeDelta)
{
    CBuildingBase::LateUpdate_GameObject(timeDelta);
}

void CDottedCircle::Render_GameObject()
{
    NULL_CHECK(m_TextureCom);

    if (!m_IsPreview)
    {
        m_TextureCom->Set_Texture(0);
        CBuildingBase::Render_GameObject();
    }
    else
    {
        if (m_TransformCom)
            m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

        m_TextureCom->Set_Texture(0);

        m_BufferCom->Render_Buffer();
    }
}

void CDottedCircle::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnBeginOverlap(self, other);
}

void CDottedCircle::OnStayOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnStayOverlap(self, other);

    if (m_IsPreview)
        return;

    if (Engine::CDInputMgr::GetInstance()->KeyPress(DIK_E))
    {
        CGameObject* owner = other->Get_Owner();
        NULL_CHECK(owner);

        // Player Tag check or dynamic_cast
        if (dynamic_cast<CPlayer*>(owner))
        {
            m_fProgress += 30.f * CTimerMgr::GetInstance()->Get_TimeDelta();

            if (m_fProgress >= 100.f)
            {
                // Create Building
                CGameObject* newObj = nullptr;
                switch (m_TargetBuildingType)
                {
                case BUILDING_TYPE::TEMPLE:
                    newObj = CTemple::Create(m_GraphicDev);
                    break;
                case BUILDING_TYPE::KITCHEN:
                    // pNewObj = CKitchen::Create(m_GraphicDev);
                    break;
                default: break;
                }

                if (newObj)
                {
                    newObj->Initialize_GameObject();

                    auto* transform = dynamic_cast<CTransform*>(
                        newObj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

                    if (transform)
                    {
                        // Use current circle position (with offset adjustments if needed based on building type)
                        _vec3 myPos = m_TransformCom->Get_Pos();
                        // Adjust Y if necessary. Temple was preview offset 3. Circle was offset + some.
                        // We should probably trust the Circle's position to be the ground/base?
                        // In Placer, Circle was at pickPos + 0.2f. Temple was pickPos + 3.0f.
                        // So Temple is Circle + 2.8f?
                        // Simplest: match logical position.

                        // For now, let's just place it at the same X,Z and adjust Y based on building type hardcoded or relative.
                        if (m_TargetBuildingType == BUILDING_TYPE::TEMPLE)
                            myPos.y += 2.f;
                        else if (m_TargetBuildingType == BUILDING_TYPE::KITCHEN)
                            myPos.y += 1.f; // Example

                        transform->Set_Pos(myPos);
                    }

                    CScene* pScene = CManagement::GetInstance()->Get_Scene();
                    if (pScene)
                        pScene->AddObjectOnLayer(LAYERTYPE::ENVIRONMENT, newObj, OBJTYPE::TEMPLE);

                    // Kill Self
                    this->Mark_As_PendingKill();
                }
            }
        }
    }
}

void CDottedCircle::OnEndOverlap(CCollider* self, CCollider* other)
{
    CBuildingBase::OnEndOverlap(self, other);
}

void CDottedCircle::Setup_Construction(BUILDING_TYPE type)
{
    m_IsPreview          = false;
    m_TargetBuildingType = type;
}

HRESULT CDottedCircle::Add_Component()
{
    if (FAILED(CBuildingBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_DOTTEDCIRCLE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_DOTTEDCIRCLE, m_TextureCom });

    return S_OK;
}

CDottedCircle* CDottedCircle::Create(DEVICE graphicDev, _vec3 scale)
{
    auto dottedCircle = new CDottedCircle(graphicDev);

    if (FAILED(dottedCircle->Ready_GameObject()))
    {
        MSG_BOX("DottedCircle Create Failed");
        Safe_Release(dottedCircle);
        return nullptr;
    }

    dottedCircle->m_TransformCom->Set_Scale(scale);

    return dottedCircle;
}

void CDottedCircle::Free()
{
    CBuildingBase::Free();
}
