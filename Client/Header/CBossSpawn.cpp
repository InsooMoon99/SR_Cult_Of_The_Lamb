#include "pch.h"
#include "CBossSpawn.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"
#include "CRandomMgr.h"
#include "CCollider.h"
#include "CHumanMonster.h"
#include "CCreateHelper.h"
#include "CSoundMgr.h"

CBossSpawn::CBossSpawn(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossSpawn::~CBossSpawn()
{
}

void CBossSpawn::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);
    
}

void CBossSpawn::Exit()
{
    CMonsterState::Exit();

}

HRESULT CBossSpawn::Ready_State()
{
    CMonsterState::Ready_State();
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossSpawn::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::SPAWNATTACK;
    if (m_Sound == false)
    {
        CSoundMgr::GetInstance()->Play_Once(L"Boss/DeathCat/DeathCat_Grunt.wav", SOUNDTYPE::SOUND_BOSS2, 1.f);
        m_Sound = true;
    }
 

    if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 20)
    {
        _vec3 BaseCenter = m_pOwner->Get_Center();
        int BaseLength = m_pOwner->Get_Length();
        float interval = 1.f;
    
        RandomMove(timeDelta, BaseCenter, BaseLength, interval);
    }


        if (m_pOwnerAnimatorCom->Is_AnimationFinished())
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;// 이거 아님 공격상태전환 아이들로

  

    return CBossPhase1::BOSSPHASE1::SPAWNATTACK;
}

void CBossSpawn::RandomMove(const _float& timeDelta, _vec3 BaseCenter, int BaseLenght, float interval)
{

    for (int i = 0; i < 5;i++)
    {
    _vec3 LT = { BaseCenter.x - BaseLenght * 0.5f, 0.f, BaseCenter.z + BaseLenght * 0.5f };
    _vec3 RB = { BaseCenter.x + BaseLenght * 0.5f,0.f,BaseCenter.z - BaseLenght * 0.5f };

    int m_CellSize = 5;// TOOD:: 나중에 맴버 변수로 오프셋 조정 (사이의 거리)

    int xCell = BaseLenght / m_CellSize;
    int zCell = BaseLenght / m_CellSize;


    int tmpx = CRandomMgr::GetInstance()->Get_Int_Random(0, xCell); // x 몇번쨰 칸인지는 랜덤
    int tmpz = CRandomMgr::GetInstance()->Get_Int_Random(0, zCell); // x 몇번쨰 칸인지는 랜덤

    float xPos = tmpx * (m_CellSize);
    float zPos = tmpz * (m_CellSize);

   // 스폰 잡몹

    CGameObject* gameObject = CHumanMonster::Create(m_pOwner->Get_GraphicDev());
    //NULL_CHECK_RETURN_MSG(gameObject, E_FAIL, L"CStage::Ready_GameLogic_Layer() failed: CHumanMonster::Create() returned null");
    SpawnObjectOnLayer(gameObject,OBJTYPE::MONSTER);

    gameObject->Initialize_GameObject();

    CTransform* Transform =
        dynamic_cast<CTransform*>(gameObject->Get_Component(
            ID_DYNAMIC,
            COMPONENTTYPE::TRANSFORM));

    gameObject->Initialize_GameObject();

    Transform->Set_Pos(LT.x + xPos, 1, LT.z - zPos);
    }
}

void CBossSpawn::LateUpdate_State(const _float& timeDelta)
{
}

CBossSpawn* CBossSpawn::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossMove = new CBossSpawn(pOwner, pStateMachine);

    if (FAILED(BossMove->Ready_State()))
    {
        MSG_BOX("BossMove Create Failed");
        Safe_Release(BossMove);
        return nullptr;
    }

    return BossMove;
}

void CBossSpawn::Free()
{
}
