#include "pch.h"
#include "CUIPlayerPanel.h"
#include "CEnumHelper.h"
#include "CCombatStat.h"
#include "CPlayer.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CUIHeartBar.h"
#include "CUIPanel.h"

CUIPlayerPanel::CUIPlayerPanel(DEVICE graphicDev)
    : CUIPanel(graphicDev),
    m_CombatStatCom(nullptr)
{
    this->Set_Name(L"PlayerPanel");
}

CUIPlayerPanel::CUIPlayerPanel(const CUIPlayerPanel& rhs)
    : CUIPanel(rhs),
    m_CombatStatCom(nullptr)
{
    this->Set_Name(L"PlayerPanel");
}

CUIPlayerPanel::~CUIPlayerPanel()
{}

HRESULT CUIPlayerPanel::Ready_GameObject()
{
    CUIPanel::Ready_GameObject();

    auto heart1 = CUIHeartBar::Create(m_GraphicDev);
    NULL_CHECK_RETURN(heart1, E_FAIL)
    AddChild(heart1);

    auto heart2 = CUIHeartBar::Create(m_GraphicDev);
    NULL_CHECK_RETURN(heart2, E_FAIL)
    AddChild(heart2);

    auto heart3 = CUIHeartBar::Create(m_GraphicDev);
    NULL_CHECK_RETURN(heart3, E_FAIL)
    AddChild(heart3);

    return S_OK;
}

_int CUIPlayerPanel::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUIPanel::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    if (!m_CombatStatCom)
        return 0;


    if (m_CombatStatCom == nullptr)
        return 0;

    float hp = m_CombatStatCom->Get_Hp();

    int i = 0;

    for (auto child : m_Children)
    {
        auto  heartBar = dynamic_cast<CUIHeartBar*>(child);
        float start    = i * 2.0f;     //0,2,4
        float end      = start + 2.0f;   //2,4,6
        //hp 2당 하트 하나
        //0번 하트 : start = 0, end = 2 → HP 0~2
        //1번 하트 : start = 2, end = 4 → HP 2~4
        //2번 하트 : start = 4, end = 6 → HP 4~6

        HEARTSTATE state;

        if (hp >= end)
            state = HEARTSTATE::FULL;

        else if (hp > start)
            state = HEARTSTATE::HALF;

        else
            state = HEARTSTATE::EMPTY;

        heartBar->Set_State(state);

        i++;
    }

    return exit;
}

void CUIPlayerPanel::LateUpdate_GameObject(const _float& timeDelta)
{
    CUIPanel::LateUpdate_GameObject(timeDelta);
}

void CUIPlayerPanel::Render_GameObject()
{
    CUIPanel::Render_GameObject();

    
}

void CUIPlayerPanel::Render_Editor()
{
    CUIPanel::Render_Editor();

    if (ImGui::Begin("UI_Heart Inspector"))
    {
        for (auto child : m_Children)
        {
            CUIHeartBar* heartBar = dynamic_cast<CUIHeartBar*>(child);

            if (!heartBar)
                continue;

            ImGui::PushID(heartBar->Get_ID());

            CTransform* m_HeartTransformCom = dynamic_cast<CTransform*>(heartBar->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

            // TransformComponent
            if (m_HeartTransformCom && ImGui::CollapsingHeader(("Transform Component" + to_string(heartBar->Get_ID())).c_str(),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                const _vec3& pos = m_HeartTransformCom->Get_Pos();

                ImGui::Text("Position");

                ImGui::Text("X :");
                ImGui::SameLine();
                ImGui::InputFloat("##HeartX", (float*)&pos.x);

                ImGui::Text("Y :");
                ImGui::SameLine();
                ImGui::InputFloat("##HeartY", (float*)&pos.y);

                ImGui::Text("Z :");
                ImGui::SameLine();
                ImGui::InputFloat("##HeartZ", (float*)&pos.z);

                m_HeartTransformCom->Set_Pos(pos);

                const _vec3& scale = m_HeartTransformCom->Get_Scale();

                ImGui::InputFloat("##ScaleX", (float*)&scale.x);
                ImGui::InputFloat("##ScaleY", (float*)&scale.y);

                m_HeartTransformCom->Set_Scale(scale);
            }

            ImGui::PopID();
        }
    }
    ImGui::End();
}

CUIPlayerPanel* CUIPlayerPanel::Create(DEVICE graphicDev)
{
    auto mainmenuPanel = new CUIPlayerPanel(graphicDev);

    if (FAILED(mainmenuPanel->Ready_GameObject()))
    {
        MSG_BOX("MainMenu Create Failed");
        Safe_Release(mainmenuPanel);
        return nullptr;
    }

    return mainmenuPanel;
}

void CUIPlayerPanel::Free()
{
    Engine::CUIPanel::Free();
}
