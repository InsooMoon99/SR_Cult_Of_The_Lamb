#include "pch.h"
#include "CWallRenderer.h"

CWallRenderer::CWallRenderer(DEVICE graphicDev)
    : CComponent(graphicDev)
{
}

CWallRenderer::CWallRenderer(const CWallRenderer& rhs)
    : CComponent(rhs)
{
}

CWallRenderer::~CWallRenderer()
{
}

HRESULT CWallRenderer::Ready_WallRenderer(DEVICE graphicDev)
{


    return S_OK;
}

_int CWallRenderer::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);



    return exit;
}

void CWallRenderer::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();

}

void CWallRenderer::Render_Editor()
{
    CComponent::Render_Editor();

}

CWallRenderer* CWallRenderer::Create(DEVICE graphicDev)
{
    CWallRenderer* wall = new CWallRenderer(graphicDev);

    if (FAILED(wall->Ready_WallRenderer(graphicDev)))
    {
        Safe_Release(wall);
        MSG_BOX("Create WallRenderer Failed");
        return nullptr;
    }

    return wall;
}

CComponent* CWallRenderer::Clone()
{
    return new CWallRenderer(*this);
}

void CWallRenderer::Free()
{
    CComponent::Free();

}
