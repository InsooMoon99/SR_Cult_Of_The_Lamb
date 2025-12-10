#include "pch.h"
#include "CWorkBench.h"
#include "CDInputMgr.h"
#include "CBoxCollider.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CSpriteEff.h"
#include "CPlayer.h"
#include "CRcTex.h"
#include "CDInputMgr.h"
#include "CUIBuildMenu.h"

CWorkBench::CWorkBench(DEVICE graphicDev)
    : CBuildingBase(graphicDev),
    m_SpriteEffCom(nullptr),
    m_BuildMenu(nullptr)
{}

CWorkBench::CWorkBench(const CWorkBench& rhs)
    : CBuildingBase(rhs),
    m_SpriteEffCom(nullptr)
{
}

CWorkBench::~CWorkBench()
{
}

HRESULT CWorkBench::Ready_GameObject()
{
    if (FAILED(CBuildingBase::Ready_GameObject()))
        return E_FAIL;
    m_SpriteEffCom->InitOutLine(D3DCOLOR_ARGB(255, 255, 255, 255), 0.025f);
    m_Active = false;

    return S_OK;
}

void CWorkBench::Initialize_GameObject()
{
    CBuildingBase::Initialize_GameObject();

    m_TransformCom->Set_Pos(12.7f, 1.1f, 24.7f);
    m_TransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(30.f));

    m_BoxCollCom->Set_RawSize(_vec3(2.0f, 2.0f, 2.0f));
    m_BoxCollCom->Set_CollisionLayer(COLLISION_LAYERTYPE::WORKBENCH);

    m_BuildMenu->SetOnDeactivate([this]() {m_Active = false; });
}

_int CWorkBench::Update_GameObject(const _float& timeDelta)
{
    _int exit = CBuildingBase::Update_GameObject(timeDelta);

    if (m_bPlayerOverlap == true)
    {
        //플레이어 상태 변환

        if (Is_Hovering() == true)
        {
            // 호버링이 되어 있을떄 
            m_bHovering = true; // 렌더 아웃라인 용
            // cout << "hovering" << endl;

            if (CDInputMgr::GetInstance()->MouseDown(DIM_LB)&&m_Active==false)
            {
                m_BuildMenu->Set_IsActive(true);
                m_BuildMenu->PlayOpenAnimation();

                m_Active = true;
            }
        }
        else
            m_bHovering = false;

    }
     else
         m_bHovering = false;
        return exit;
}
    void CWorkBench::LateUpdate_GameObject(const _float & timeDelta)
    {
        CBuildingBase::LateUpdate_GameObject(timeDelta);
    }


    void CWorkBench::Render_GameObject()
    {

        if (m_bHovering == true)
        {
            m_SpriteEffCom->OutLineEff(m_BufferCom);

            m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

            if (m_BufferCom)
                m_BufferCom->Render_Buffer();
        }

        NULL_CHECK(m_TextureCom);
        m_TextureCom->Set_Texture(0);

        CBuildingBase::Render_GameObject();
    }

    void CWorkBench::Render_Editor()
    {
        CBuildingBase::Render_Editor();

        ImGui::Text("=== WorkBench ===");
    }

    bool CWorkBench::Is_Hovering()
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(CDInputMgr::GetInstance()->Get_HWND(), &pt);

        D3DVIEWPORT9 ViewPort;
        ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
        m_GraphicDev->GetViewport(&ViewPort);

        _vec3 vMousePos;
        vMousePos.x = ((2.0f * pt.x) / ViewPort.Width) - 1.0f;
        vMousePos.y = ((-2.0f * pt.y) / ViewPort.Height) + 1.0f;
        vMousePos.z = 1.0f;

        D3DXMATRIX matProj;
        m_GraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
        D3DXMatrixInverse(&matProj, nullptr, &matProj);
        D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

        D3DXMATRIX matView;
        m_GraphicDev->GetTransform(D3DTS_VIEW, &matView);
        D3DXMatrixInverse(&matView, nullptr, &matView);

        _vec3 vRayPos = { matView._41, matView._42, matView._43 };

        _vec3 vRayDir;
        D3DXVec3TransformNormal(&vRayDir, &vMousePos, &matView);
        D3DXVec3Normalize(&vRayDir, &vRayDir);

        _vec3 Min = m_BoxCollCom->Get_Transform()->Get_Pos() - m_BoxCollCom->Get_Size() * .5f;
        _vec3 Max = m_BoxCollCom->Get_Transform()->Get_Pos() + m_BoxCollCom->Get_Size() * .5f;

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

        if (timeMin > 0.f)
            return true;

        return false;
    }

    void CWorkBench::OnBeginOverlap(CCollider * self, CCollider * other)
    {
        CBuildingBase::OnBeginOverlap(self, other);

        CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
        if (ply)
            m_bPlayerOverlap = true;
    }


    void CWorkBench::OnStayOverlap(CCollider * self, CCollider * other)
    {
        CBuildingBase::OnStayOverlap(self, other);
    }

    void CWorkBench::OnEndOverlap(CCollider * self, CCollider * other)
    {
        CBuildingBase::OnEndOverlap(self, other);

        CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());

        if (m_bPlayerOverlap == true && ply != nullptr)
            m_bPlayerOverlap = false;
    }

    HRESULT CWorkBench::Add_Component()
    {
        // Texture
        m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_WORKBENCH);
        NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
        m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_WORKBENCH, m_TextureCom });


        // effect
        m_SpriteEffCom = CreateProtoComponent<CSpriteEff>(this, COMPONENTTYPE::SPRITEEFFECT);
        NULL_CHECK_RETURN(m_SpriteEffCom, E_FAIL);
        m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::SPRITEEFFECT, m_SpriteEffCom });

        return CBuildingBase::Add_Component();
    }

    CWorkBench* CWorkBench::Create(DEVICE graphicDev)
    {
        auto workBench = new CWorkBench(graphicDev);

        if (FAILED(workBench->Ready_GameObject()))
        {
            MSG_BOX("DottedCircle Create Failed");
            Safe_Release(workBench);
            return nullptr;
        }

        return workBench;
    }

    void CWorkBench::Free()
    {
        Safe_Release(m_SpriteEffCom);
        Safe_Release(m_BuildMenu);
        CBuildingBase::Free();
    }

