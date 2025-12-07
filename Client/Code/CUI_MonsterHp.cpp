#include "pch.h"
#include "CUI_MonsterHp.h"
#include "CRcTex.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"

CUI_MonsterHp::CUI_MonsterHp(DEVICE graphicDev, CGameObject* owner)
    : CUI(graphicDev),
    m_Owner(owner),
    m_OwnerCombatStat(nullptr),
    m_OwnerTransform(nullptr),
    m_HpRatio(1.0f),
    m_HpDelayRatio(1.0f),
    m_HpDelaySpeed(1.5f),   
    m_YOffset(3.0f),
    m_BarSize(0.6f, 0.7f, 1.f)
{
    Set_Name(L"Monster Hp");

    if (m_Owner)
        m_Owner->AddRef();
}

CUI_MonsterHp::~CUI_MonsterHp()
{
}

HRESULT CUI_MonsterHp::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_YOffset = 1.2f;
    m_BarSize = _vec3(0.6f, 0.7f, 1.f);
    m_HpRatio = 1.f;
    m_HpDelaySpeed = 1.5f;

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));

    return S_OK;
}

void CUI_MonsterHp::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    if (m_Owner)
    {
        m_OwnerCombatStat = dynamic_cast<CCombatStat*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

        m_OwnerTransform = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    }

}

_int CUI_MonsterHp::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (!m_Owner)
        return exit;

    // HP 비율 계산
    if (m_OwnerCombatStat)
    {
        _float curHp = m_OwnerCombatStat->Get_Hp();
        _float maxHp = m_OwnerCombatStat->Get_MaxHp();

        if (maxHp > 0.f)
            m_HpRatio = curHp / maxHp;
        else
            m_HpRatio = 0.f;

        // 죽으면 삭제
        if (m_HpRatio <= 0.f)
            this->Mark_As_PendingKill();
    }

    if (m_OwnerTransform && m_TransformCom)
    {
        _vec3 ownerPos = m_OwnerTransform->Get_Pos();
        ownerPos.y += m_YOffset; 
        
        m_TransformCom->Set_Pos(ownerPos);
        m_TransformCom->Set_Scale(m_BarSize);
    }

    if (m_HpDelayRatio > m_HpRatio)
    {
        m_HpDelayRatio -= m_HpDelaySpeed * timeDelta;
        if (m_HpDelayRatio < m_HpRatio)
            m_HpDelayRatio = m_HpRatio;
    }

    return exit;
}

void CUI_MonsterHp::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}

void CUI_MonsterHp::Render_GameObject()
{
    if (!m_Owner || !m_TextureCom || !m_BufferCom)
        return;

    float ratio = clamp(m_HpRatio, 0.f, 1.f);
    float delayRatio = clamp(m_HpDelayRatio, 0.f, 1.f);

    const float FULL_HALF_WIDTH = 1.0f;
    const float FULL_HALF_HEIGHT = 0.15f;

    const _matrix& world = m_TransformCom->Get_World();

    auto ApplyBarScaleOffset = [&](float barRatio)
        {
            float hw_full = FULL_HALF_WIDTH;
            float hh_full = FULL_HALF_HEIGHT;

            float hw = hw_full * barRatio;

            float offsetX = hw_full * (1.f - barRatio);

            m_BufferCom->Set_Scale(hw, hh_full);
            m_BufferCom->Set_Offset(offsetX, 0.f);
        };

    {
        const float BG_WIDTH_SCALE = 1.08f;
        const float BG_HEIGHT_SCALE = 0.18f;

        m_BufferCom->Set_Scale(BG_WIDTH_SCALE, BG_HEIGHT_SCALE);
        m_BufferCom->Set_Offset(0.f, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(1); // 배경
        m_BufferCom->Render_Buffer();
    }

    if (delayRatio > 0.f)
    {
        ApplyBarScaleOffset(delayRatio);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(2); // 흰색 딜레이 바
        m_BufferCom->Render_Buffer();
    }

    if (ratio > 0.f)
    {
        ApplyBarScaleOffset(ratio);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(0); 
        m_BufferCom->Render_Buffer();
    }
}

HRESULT CUI_MonsterHp::Add_Component()
{
    // Buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_MONSTERHP);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_MONSTERHP, m_TextureCom });

    return S_OK;
}

void CUI_MonsterHp::Update_Billboard()
{

}

CUI_MonsterHp* CUI_MonsterHp::Create(DEVICE graphicDev, CGameObject* owner)
{
    CUI_MonsterHp* hp = new CUI_MonsterHp(graphicDev, owner);

    if (FAILED(hp->Ready_GameObject()))
    {
        Safe_Release(hp);
        MSG_BOX("Monster Hp Create Failed");
        return nullptr;
    }

    return hp;
}

void CUI_MonsterHp::Free()
{
    Safe_Release(m_Owner);
    CUI::Free();
}
