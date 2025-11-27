#include "pch.h"
#include "CBossChainAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSpriteAnimation.h"
#include "CBossChain.h"
#include "CCreateHelper.h"
#include "CWarningChain.h"

CBossChainAttack::CBossChainAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
    ZeroMemory(m_fElapsedTime, sizeof(m_fElapsedTime));
    ZeroMemory(m_fExtraElapsedTime, sizeof(m_fExtraElapsedTime));

}

CBossChainAttack::~CBossChainAttack()
{
}

void CBossChainAttack::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);
    m_ePhase = PHASE::PLAYERLINE; // 이걸 어케 해야하지??


}

void CBossChainAttack::Exit()
{
    CMonsterState::Exit();
}

HRESULT CBossChainAttack::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));


    m_fExtraElapsedTime[(int)PHASE::PLAYERLINE] = 0;

    return S_OK;
}

_int CBossChainAttack::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    Find_Player();

    m_fCurStateElapsedTime += timeDelta;

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::CHAINATTACK;

    switch (m_ePhase)
    {
    case PHASE::PLAYERLINE:
        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);


        if (PlayerLineUpdate(timeDelta) == true)
        {
            LineCount++;
        }
             //경고이팩트다 나가면 -> 다음 플레이어 위치 캐싱 -> 



        if (/*m_RandCount*/3 <= LineCount)
        {

            m_fCurStateElapsedTime = 0;
            m_fExtraElapsedTime[(int)PHASE::PLAYERLINE] = 0;
            m_Count = 0;
            m_LineFlag = false;
            m_fElapsedTime[(int)PHASE::PLAYERLINE] = 0.f;
            LineCount = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;

        }

        break;


    case PHASE::CROSS: //움직임

        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE); 

        m_fElapsedTime[(int)PHASE::CROSS] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::CROSS] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;

    case PHASE::RANDCHAIN://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);

        m_fElapsedTime[(int)PHASE::RANDCHAIN] += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime[(int)PHASE::RANDCHAIN] = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;
    }



    return CBossPhase1::BOSSPHASE1::CHAINATTACK;
}

void CBossChainAttack::PlayerLine(_vec3 PlayerDir, _int  MaxCount, _float m_Offset, float Speed)
{

    // 지금 안되는거 TODO:: 플레이어 위치 캐싱 , 충돌박스 없음 이슈

        auto chain = CBossChain::Create(m_pOwner->Get_GraphicDev(), m_pOwner);
        const auto ChainTransform =
            dynamic_cast<CTransform*>(chain->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));

        //_vec3 dir;
        //_vec3 tmp = m_pPlayerTransformCom->Get_Pos() - m_pOwnerTransformCom->Get_Pos();
        //D3DXVec3Normalize(&dir, &tmp);


        _vec3 CurPos = ChainTransform->Get_Pos();
        _vec3 AddPos = { PlayerDir.x + (m_Count * m_Offset), 0.f, PlayerDir.z + (m_Count * m_Offset) };
        ChainTransform->Set_Pos(CurPos + AddPos);

        SpawnAttackObject(chain);

        m_fElapsedTime[(int)PHASE::PLAYERLINE] = 0;

        m_Count++;
 
}
bool CBossChainAttack::PlayerLineUpdate(const _float& timeDelta)
{
    m_fElapsedTime[(int)PHASE::PLAYERLINE] += timeDelta;

    if (m_LineFlag == false)
    {
        _vec3 tmp = m_pPlayerTransformCom->Get_Pos() - m_pOwnerTransformCom->Get_Pos();
        D3DXVec3Normalize(&m_PlayerDir, &tmp);

        m_LineFlag = true;
    }

    if (m_fElapsedTime[(int)PHASE::PLAYERLINE] > .5 && m_Count < 10)
    {
        _vec3 PlayerDir = m_PlayerDir;
        int offset = 5;

        CWarningChain* WarningChain = CWarningChain::Create(m_pOwner->Get_GraphicDev(), m_pOwner);
        const auto ChainTransform =
            dynamic_cast<CTransform*>(WarningChain->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));
        _vec3 CurPos = ChainTransform->Get_Pos();
        _vec3 AddPos = { PlayerDir.x * (m_Count * offset), 0.f, PlayerDir.z * (m_Count * offset) };
        ChainTransform->Set_Pos(CurPos + AddPos);

        SpawnAttackObject(WarningChain);

        m_fElapsedTime[(int)PHASE::PLAYERLINE] = 0;

        m_Count++;

    }

    if (m_Count >= 10)
    {
        // 다음 패턴을 위해 초기화
        m_Count = 0;
        m_LineFlag = false;
        m_fElapsedTime[(int)PHASE::PLAYERLINE] = 0.f;

        return true;  // ← 이 패턴 1회 완료!
    }
    return false;

}
void CBossChainAttack::LateUpdate_State(const _float& timeDelta)
{
}

void CBossChainAttack::Render_State()
{
}

CBossChainAttack* CBossChainAttack::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossCA = new CBossChainAttack(pOwner, pStateMachine);

    if (FAILED(BossCA->Ready_State()))
    {
        MSG_BOX("BossCA Create Failed");
        Safe_Release(BossCA);
        return nullptr;
    }

    return BossCA;
}

void CBossChainAttack::Free()
{
}
