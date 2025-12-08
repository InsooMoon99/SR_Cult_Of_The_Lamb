#include "pch.h"
#include "CUI_BossHp.h"
#include "CRcTex.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"

CUI_BossHp::CUI_BossHp(DEVICE graphicDev, CGameObject* owner)
     : CUI(graphicDev),
     m_Owner(owner),
     m_OwnerCombatStat(nullptr),
     m_HpRatio(1.0f),
     m_HpDelayRatio(1.0f),
     m_HpDelaySpeed(1.5f),
     m_BarSize({ 0.6f, 0.7f, 1.f }),
     m_Buffer_Hp(nullptr), m_Buffer_Delay(nullptr)
{
    Set_Name(L"Boss Hp");
    if (m_Owner)
        m_Owner->AddRef();
}

CUI_BossHp::~CUI_BossHp()
{
}

HRESULT CUI_BossHp::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_BarSize = _vec3(80.6f, 10.7f, 1.f);
    m_HpRatio = 1.f;
    m_HpDelaySpeed = 1.5f;

    if (m_TransformCom)
    {
        m_TransformCom->Set_Pos(_vec3(0.f, -250.f, 0.f));  // 화면 하단
        m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
    }

    return S_OK;
}

void CUI_BossHp::Initialize_GameObject()
{
    CUI::Initialize_GameObject();
    if (m_Owner)
    {
        m_OwnerCombatStat = dynamic_cast<CCombatStat*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
    }

}

_int CUI_BossHp::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    if (!m_Owner)
        return exit;

    if (m_OwnerCombatStat)
    {
        _float curHp = m_OwnerCombatStat->Get_Hp();
        _float maxHp = m_OwnerCombatStat->Get_MaxHp();

        if (maxHp > 0.f)
            m_HpRatio = curHp / maxHp;
        else
            m_HpRatio = 0.f;

        if (m_HpRatio <= 0.f)
            this->Mark_As_PendingKill();
    }

    if (m_HpDelayRatio > m_HpRatio)
    {
        m_HpDelayRatio -= m_HpDelaySpeed * timeDelta;

        if (m_HpDelayRatio < m_HpRatio)
            m_HpDelayRatio = m_HpRatio;
    }
    return exit;
}

void CUI_BossHp::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}
void CUI_BossHp::Render_GameObject()
{
    if (!m_Owner || !m_TextureCom)
        return;

    float ratio = clamp(m_HpRatio, 0.f, 1.f);
    float delayRatio = clamp(m_HpDelayRatio, 0.f, 1.f);
    const _matrix& world = m_TransformCom->Get_World();

    const float HALF_WIDTH = 150.f;
    const float HALF_HEIGHT = 75.f;
    
    // 1) 배경

    m_BufferCom->Set_Scale(HALF_WIDTH, HALF_HEIGHT);
    m_BufferCom->Set_Offset(0.f, 0.f);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
    m_TextureCom->Set_Texture(3);
    m_BufferCom->Render_Buffer();
    // 2) 딜레이 바
    if (delayRatio > 0.f)
    {
        float hw = HALF_WIDTH * delayRatio;
        float offsetX = HALF_WIDTH * (1.f - delayRatio);
        m_Buffer_Delay->Set_Scale(hw, HALF_HEIGHT);
        m_Buffer_Delay->Set_Offset(offsetX, 0.f);
        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(5);
        m_Buffer_Delay->Render_Buffer();
    }
    // 3) HP 바
    if (ratio > 0.f)
    {
        float hw = HALF_WIDTH * ratio * 0.97f;  
        float offsetX = HALF_WIDTH * (1.f - ratio);

        m_Buffer_Hp->Set_Scale(hw, HALF_HEIGHT);
        m_Buffer_Hp->Set_Offset(offsetX, 0.f);
        const float UV_LEFT = 30.f / 300.f;
        const float UV_RIGHT = 270.f / 300.f;
        const float UV_RANGE = UV_RIGHT - UV_LEFT;
        float u_end = UV_LEFT + UV_RANGE * ratio;
        m_Buffer_Hp->Set_UV(UV_LEFT, 0.f, u_end, 1.f);
        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(4);
        m_Buffer_Hp->Render_Buffer();

    }
}

HRESULT CUI_BossHp::Add_Component()
{
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_MONSTERHP);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_MONSTERHP, m_TextureCom });

    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);

    m_Buffer_Delay = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Delay, E_FAIL);

    m_Buffer_Hp = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Hp, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    return S_OK;
}

CUI_BossHp* CUI_BossHp::Create(DEVICE graphicDev, CGameObject* owner)
{
    CUI_BossHp* hp = new CUI_BossHp(graphicDev, owner);

    if (FAILED(hp->Ready_GameObject()))
    {
        Safe_Release(hp);
        MSG_BOX("Boss Hp Create Failed");
        return nullptr;
    }

    hp->Initialize_GameObject();
    return hp;
}

void CUI_BossHp::Free()
{
    Safe_Release(m_Buffer_Delay);
    Safe_Release(m_Buffer_Hp);
    Safe_Release(m_Owner);

    CUI::Free();
}
