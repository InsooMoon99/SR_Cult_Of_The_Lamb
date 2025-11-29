#include "pch.h"
#include "CBossBulletAttack.h"
#include "CMonsterState.h"
#include "CBossPhase1.h"
#include "CBossStateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CRandomMgr.h"
#include "CBossBullet.h"
#include "CCreateHelper.h"
#include "CRandomMgr.h"
#include "CPlayer.h"


CBossBulletAttack::CBossBulletAttack(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CBossBulletAttack::~CBossBulletAttack()
{
}

void CBossBulletAttack::Enter()
{
    CMonsterState::Enter();
    m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);
    // TODO:: 지금 이게 안들어오거든 바로 상태안의 애니로 감 어칼까

    m_ePhase = PHASE::CIRCLE; // 이걸 어케 해야하지??
    //int RandPhase = CRandomMgr::GetInstance()->Get_Int_Random(0, ((int)PHASE::PHASE_END) - 1); // 페이즈 랜덤
    //m_ePhase = (PHASE)RandPhase;


}

void CBossBulletAttack::Exit()
{
    CMonsterState::Exit();
    m_StartFlag = false;

    if (!tmpBullet.empty())
        tmpBullet.clear();

}

HRESULT CBossBulletAttack::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));

    return S_OK;
}

_int CBossBulletAttack::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    Find_Player();
    // 이거 첫프레임 씹힘 방지 ?? 인듯???
    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CBossPhase1::BOSSPHASE1::BULLETATTACK;

    // 처음 애니는 무조건 공격모션 이어야 함
    if (m_pOwnerAnimatorCom->Get_CurFrame() == m_pOwnerAnimatorCom->Get_MaxFrame() - 1 && m_StartFlag == false)
    {
        m_StartFlag = true;
    }

    if (m_StartFlag == false)
        return CBossPhase1::BOSSPHASE1::BULLETATTACK;


    switch (m_ePhase)
    {
    case PHASE::CIRCLE:

        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);


        m_fElapsedTime += timeDelta;

        if (CircleBullet(timeDelta, 9, 10, 0.1) == true)
        {
            if (m_TargetFlag == false)
            {
                _vec3 tmpdir;
                _vec3 tmp = m_pPlayerTransformCom->Get_Pos() - m_pOwnerTransformCom->Get_Pos();
                D3DXVec3Normalize(&tmpdir, &tmp);


                for (auto& it : tmpBullet)
                {
                    it->Start_Shoot();
                    it->Set_TargetDir(tmpdir);
                }
                m_TargetFlag = true;
            }

            m_fCurStateElapsedTime += timeDelta;

            if (m_fCurStateElapsedTime >= 1.0f)
            {
                // 나가기
                for (auto& it : tmpBullet)
                {
                    //일단 테스트
                    it->SetPendingKill();
                }
                tmpBullet.clear();

                m_Count = 0;
                m_TargetFlag = false;
                m_fCurStateElapsedTime = 0;
                m_fElapsedTime = 0;
                return CBossPhase1::BOSSPHASE1::BOSSIDLE;

            }
        }

        break;


    case PHASE::ROTCROSS_SPREAD: //움직임

        m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::LOOP);

        m_fElapsedTime += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;

    case PHASE::SPREAD://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossAttack", ANIMSTATE::ONCE);

        m_fElapsedTime += timeDelta;


        //


        if (m_fCurStateElapsedTime >= 1.0f)
        {
            m_fElapsedTime = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;
    }



    return CBossPhase1::BOSSPHASE1::BULLETATTACK;
}
bool CBossBulletAttack::CircleBullet(const _float& timeDelta, int num, int length, float interval)
{

    if (m_fElapsedTime >= interval && m_Count <= 10)
    {


        CBossBullet* BossBullet = CBossBullet::Create(m_pOwner->Get_GraphicDev(), m_pOwner, m_pPlayer);


        CTransform* BulletTransform =
            dynamic_cast<CTransform*>(BossBullet->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));


        _vec3 ownerPos = m_pOwnerTransformCom->Get_Pos();

        //_vec3 baseDir = { 0,0,1 };

        float rad = D3DXToRadian(360.f / (num));

        float addRad = rad * m_Count;

        _vec3 circle = { cos(addRad) * length, 0.f, sin(addRad) * length };

        BulletTransform->Set_Pos(ownerPos + circle);
        tmpBullet.push_back(BossBullet);

        // 레이어 관리로 넘기기
        SpawnAttackObject(BossBullet);


        m_Count++;
        m_fElapsedTime = 0;

    }

    if (m_Count > 10)
    {
        m_fElapsedTime = 0;
        return true;
    }

    return false;
}
void CBossBulletAttack::LateUpdate_State(const _float& timeDelta)
{
}

void CBossBulletAttack::Render_State()
{
    // m_pOwnerAnimatorCom->Play_Animation(L"BossAwake", ANIMSTATE::LOOP);
}

CBossBulletAttack* CBossBulletAttack::Create(CBossPhase1* pOwner, CBossStateMachine* pStateMachine)
{
    auto BossBA = new CBossBulletAttack(pOwner, pStateMachine);

    if (FAILED(BossBA->Ready_State()))
    {
        MSG_BOX("BossAwake Create Failed");
        Safe_Release(BossBA);
        return nullptr;
    }

    return BossBA;
}

void CBossBulletAttack::Free()
{
    if(!tmpBullet.empty())
     tmpBullet.clear();
}
