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
        m_TransformCom->Set_Pos(_vec3(0.f, -300.f, 0.f));
        m_TransformCom->Set_Scale(_vec3(1.5f, 2.7f, 1.f));
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

    const float FULL_HALF_WIDTH = 150.0f;
    const float FULL_HALF_HEIGHT = 35.f;

    const _matrix& world = m_TransformCom->Get_World();

    {
        const float BG_WIDTH = FULL_HALF_WIDTH + 14.f;
        const float BG_HEIGHT = FULL_HALF_HEIGHT + 13.f;

        m_BufferCom->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_BufferCom->Set_Offset(0.f, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(3);  // 배경 텍스처
        m_BufferCom->Render_Buffer();
    }

    if (delayRatio > 0.f)
    {
        float hw = FULL_HALF_WIDTH * delayRatio;
        float offsetX = FULL_HALF_WIDTH * (1.f - delayRatio);

        m_Buffer_Delay->Set_Scale(hw, FULL_HALF_HEIGHT * 0.3f);
        m_Buffer_Delay->Set_Offset(offsetX, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(2);  // 흰색 텍스처
        m_Buffer_Delay->Render_Buffer();
    }

    if (ratio > 0.f)
    {
        ImGui::Begin("HP Bar Test");

        // 슬라이더로 크기 조정
        ImGui::SliderFloat("Width Scale", &m_HpbarWidthScale, 0.5f, 10.0f);
        ImGui::SliderFloat("Height Scale", &m_HpbarHeightScale, 0.5f, 10.0f);

        // 실제 적용될 크기 표시
        float appliedWidth = FULL_HALF_WIDTH * m_HpbarWidthScale;
        float appliedHeight = FULL_HALF_HEIGHT * m_HpbarHeightScale;
        ImGui::Text("Applied Width: %.1f", appliedWidth);
        ImGui::Text("Applied Height: %.1f", appliedHeight);

        // 리셋 버튼
        if (ImGui::Button("Reset to Default"))
        {
            m_HpbarWidthScale = 1.0f;
            m_HpbarHeightScale = 0.8f;
        }

        ImGui::End();
        // 스케일 적용
        const float HP_BAR_HALF_WIDTH = FULL_HALF_WIDTH * m_HpbarWidthScale;
        const float HP_BAR_HEIGHT = FULL_HALF_HEIGHT * m_HpbarHeightScale;

        float hw = HP_BAR_HALF_WIDTH * ratio;
        float offsetX = HP_BAR_HALF_WIDTH * (1.f - ratio);

        m_Buffer_Hp->Set_Scale(hw, HP_BAR_HEIGHT);
        m_Buffer_Hp->Set_Offset(offsetX, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);

        m_TextureCom->Set_Texture(4);  // HP 바 텍스처
        m_Buffer_Hp->Render_Buffer();

    }
}

HRESULT CUI_BossHp::Add_Component()
{
    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_MONSTERHP);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_MONSTERHP, m_TextureCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Buffer
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
