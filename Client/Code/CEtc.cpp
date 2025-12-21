#include "pch.h"
#include "CEtc.h"

#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CProtoMgr.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"

CEtc::CEtc(DEVICE graphicDev, COMPONENTTYPE _TexComType, wstring _AnimationKey, int _SpriteNum)
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr),
      m_StateCom(nullptr)
{
    m_TexComType   = _TexComType;
    m_AnimationKey = _AnimationKey;
    m_SpriteNum    = _SpriteNum;
}

CEtc::CEtc(const CEtc& rhs)
    : CGameObject(rhs),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr),
      m_StateCom(nullptr)
{}

CEtc::~CEtc()
{}

HRESULT CEtc::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    Animation_Setting();

    m_TransformCom->Set_Pos({ 0.f, 1.1f, 1.f });
    m_TransformCom->Set_Scale(2.1f, 2.5f, 2.5f);
    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(25.f));

    wstring key = m_AnimationKey;
    if (false == SetTexAspectInit(m_TextureCom, m_TransformCom, 1, &key))
        return E_FAIL;

    return S_OK;
}

_int CEtc::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    return exit;
}

void CEtc::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
}

void CEtc::Render_GameObject()
{
    //_GraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();
    //

    Render_Reset();
    //m_GraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CEtc::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(m_AnimationKey, m_SpriteNum, 0.02f);

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, m_AnimationKey, ANIMSTATE::LOOP);
}

HRESULT CEtc::Add_Component()
{
    //buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    //transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, m_TexComType);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ m_TexComType, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);

    m_AnimatorCom->Set_TextureType(m_TexComType);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    return S_OK;
}

void CEtc::Render_Setting()
{
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
}

void CEtc::Render_Reset()
{
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CEtc::OnBeginOverlap(CCollider* self, CCollider* other)
{}

void CEtc::OnEndOverlap(CCollider* self, CCollider* other)
{}

CEtc* CEtc::Create(DEVICE graphicDev, COMPONENTTYPE _TexComType, wstring _AnimationKey, int _SpriteNum)
{
    auto MenuChar = new CEtc(graphicDev, _TexComType, _AnimationKey, _SpriteNum);

    if (FAILED(MenuChar->Ready_GameObject()))
    {
        MSG_BOX("MenuChar Create Failed");
        Safe_Release(MenuChar);
        return nullptr;
    }

    return MenuChar;
    //return nullptr;
}

void CEtc::Free()
{
    CGameObject::Free();
}
