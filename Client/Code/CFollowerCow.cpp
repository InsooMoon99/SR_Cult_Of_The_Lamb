#include "pch.h"
#include "CFollowerCow.h"

#include "CAnimator.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CNPCController.h"
#include "CStage.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUI_FollowerCommand.h"
#include "CVillageStateMgr.h"

CFollowerCow::CFollowerCow(DEVICE graphicDev)
    : CNPCBase(graphicDev)
{ }

CFollowerCow::CFollowerCow(const CFollowerCow& rhs)
    : CNPCBase(rhs)

{ }

CFollowerCow::~CFollowerCow()
{ }

HRESULT CFollowerCow::Ready_GameObject()
{
    Set_Name(L"FollowerCow");

    if (FAILED(CNPCBase::Ready_GameObject()))
        return E_FAIL;

    // 초기 생성 위치
    m_TransformCom->Set_Pos(23.5f, 1.f, 24.5f);

    m_NPCController->Set_Work(CNPCController::NPC_WORK::CHOPWOOD);
    m_NPCController->Set_PrayTargetPos(_vec3(18.f, 1.f, 21.5f));
    m_NPCController->Set_AnimKeyPrefix(L"Cow");

    return S_OK;
}

void CFollowerCow::Initialize_GameObject()
{
    CNPCBase::Initialize_GameObject();

    // 초기 상태 설정 (저장된 데이터 기반)
    SavedNpc* savedState = CVillageStateMgr::GetInstance()->Get_NPCState(Get_Name());

    // 현재 씬이 CStage(마을)인지 확인
    auto curScene = CManagement::GetInstance()->Get_Scene();

    if (dynamic_cast<CStage*>(curScene))
    {
        if (savedState && savedState->firstSpawnAnimPlayed == false)
        {
            // 데이터 업데이트: 첫 등장 애니메이션 봤음 처리
            CVillageStateMgr::GetInstance()->Save_NPC(Get_Name(), true, true);
        }
    }
    else
    {
        // 마을이 아니라면 (전투 씬 등) -> 기본적으로 IDLE 상태로 시작 (테스트 용도)
        if (m_NPCController)
        {
            if (CStateMachine* stateMachine = m_NPCController->Get_StateMachine())
                stateMachine->Change_State(static_cast<_uint>(CNPCStateMachine::NPC_STATE::IDLE));
        }
    }
}

_int CFollowerCow::Update_GameObject(const _float& timeDelta)
{
    _int exit = CNPCBase::Update_GameObject(timeDelta);

    if (m_bHovering == true)
    {
        if (CDInputMgr::GetInstance()->MouseDown(DIM_LB))
        {
            // 동적 바인딩: 현재 씬(CStage)에서 UI를 가져온다.
            auto stageScene = dynamic_cast<CStage*>(CManagement::GetInstance()->Get_Scene());

            if (stageScene)
            {
                CUI_FollowerCommand* followerCommand = stageScene->Get_FollowerCommand();

                if (followerCommand)
                {
                    followerCommand->Set_TargetNPC(this);
                    followerCommand->Set_IsActive(true);
                }
            }
        }
    }

    // UI 닫기 로직은 UI 자체에서 처리하거나, 여기서 처리하더라도 UI 포인터가 필요함.
    // 하지만 일단 열기 로직에 집중. 닫기는 UI가 Active 상태일 때 우클릭으로 닫는 로직이 있었는데,
    // 이는 UI 쪽에서 처리하는 것이 더 깔끔할 수 있으나, 기존 로직 유지를 위해 유지한다면:
    // UI 포인터를 멤버로 들고 있지 않으므로, 닫기 로직도 Scene을 통해 접근해야 함.
    // 혹은 CStage에서 일괄 처리. 
    // 여기서는 간단히 활성화 로직만 변경하고, 비활성화는 UI가 스스로 하거나 외부에서 처리하도록 둠.

    return exit;
}

void CFollowerCow::LateUpdate_GameObject(const _float& timeDelta)
{
    CNPCBase::LateUpdate_GameObject(timeDelta);
}

void CFollowerCow::Render_GameObject()
{
    CNPCBase::Render_GameObject();
}

HRESULT CFollowerCow::Add_Component()
{
    if (FAILED(CNPCBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_COW);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_COW, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_COW);

    // 디버그: 텍스처 컴포넌트 추가 성공 여부 확인
    if (!m_TextureCom)
        MSG_BOX("FollowerCow: Texture Component Missing in Add_Component");
    // else { MSG_BOX("FollowerCow: Texture Component Added Successfully"); }

    return S_OK;
}

void CFollowerCow::Animation_Setting()
{
    m_AnimatorCom->Create_Animation(L"CowIdle",
                                    49,
                                    0.02f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowRun",
                                    29,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowWalkPast",
                                    86,
                                    0.02f,
                                    _vec2(0.7f, 0.7f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowBuild",
                                    53,
                                    0.02f,
                                    _vec2(0.8f, 0.8f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowChopWood",
                                    81,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowMining",
                                    48,
                                    0.02f,
                                    _vec2(1.1f, 1.1f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowPray",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowPrayPeek",
                                    62,
                                    0.04f,
                                    _vec2(0.75f, 0.67f),
                                    _vec2(0.f, -0.13f));

    m_AnimatorCom->Create_Animation(L"CowEat",
                                    50,
                                    0.05f,
                                    _vec2(0.72f, 0.65f),
                                    _vec2(0.f, -0.12f));

    m_AnimatorCom->Create_Animation(L"CowSpawn",
                                    172,
                                    0.04f,
                                    _vec2(1.5f, 1.5f),
                                    _vec2(0.f, 0.2f));

    m_AnimatorCom->Create_Animation(L"CowTransform1",
                                    94,
                                    0.04f,
                                    _vec2(1.f, 1.f),
                                    _vec2(0.f, 0.f));

    m_AnimatorCom->Create_Animation(L"CowTransform2",
                                    26,
                                    0.04f,
                                    _vec2(1.f, 1.f),
                                    _vec2(0.f, 0.f));

    m_AnimatorCom->Create_Animation(L"CowTransform3",
                                    89,
                                    0.04f,
                                    _vec2(1.f, 1.f),
                                    _vec2(0.f, 0.f));
}

CFollowerCow* CFollowerCow::Create(DEVICE graphicDev)
{
    auto instance = new CFollowerCow(graphicDev);

    if (FAILED(instance->Ready_GameObject()))
    {
        MSG_BOX("Follower Cow Create Failed");
        Safe_Release(instance);
        return nullptr;
    }

    return instance;
}

void CFollowerCow::Free()
{
    CNPCBase::Free();
}
