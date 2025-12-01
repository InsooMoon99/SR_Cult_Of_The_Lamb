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
#include "CProjectile.h"
#include "Client_Enum.h"


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
    ////
    //m_ePhase = PHASE::ROTCROSS_SPREAD; // 이걸 어케 해야하지??
    int RandPhase = CRandomMgr::GetInstance()->Get_Int_Random(0, ((int)PHASE::PHASE_END) - 1); // 페이즈 랜덤
    m_ePhase = (PHASE)RandPhase;

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

    m_offset = 1;
    m_CircleRadius = 5;
    m_Interval = 0.1;
    m_CrossSpreadInterval = 1;
    m_SpreadInterval = 0.4;
    m_GroundY = 0.002f;
    m_CircleSpeed = 10;
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

        {
            if (m_TargetFlag == false)
            {
                _vec3 tmpdir;
                _vec3 tmp = m_pPlayerTransformCom->Get_Pos() - m_pOwnerTransformCom->Get_Pos();
                D3DXVec3Normalize(&tmpdir, &tmp);


                for (auto& it : tmpBullet)
                {
                    // it->Set_TargetDir(tmpdir);
                    BulletDesc d;
                    d.Direction = {tmpdir.x, 0.f, tmpdir.y};
                    d.Speed = 10;
                    d.moveType = PROJ_TYPE::LINEAR;
                    d.ProjectileOwner = m_pOwner;


                    CProjectile* projectileCom = dynamic_cast<CProjectile*>(it->Get_Component(COMPONENTID::ID_STATIC, COMPONENTTYPE::PROJECTILE));
                    projectileCom->SetMoveDesc(d);

                    it->Start_Shoot();
                }
                m_TargetFlag = true;
            }

            m_fCurStateElapsedTime += timeDelta;

            if (m_fCurStateElapsedTime >= 2.0f)
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

        if (CrossBullet(timeDelta, 9, m_offset,m_Interval) == true)
        {
            for (auto& it : tmpBullet)
            {
                it->Start_Shoot();
            }

            SpreadBullet(timeDelta, 10,m_CrossSpreadInterval, 10);


            m_fCurStateElapsedTime += timeDelta;
            if (m_fCurStateElapsedTime >= 8.0f)
            {
                // 나가기
                for (auto& it : tmpBullet)
                {
                    //일단 테스트
                    it->SetPendingKill();
                }
                tmpBullet.clear();
                m_Count = 0;
                m_fCurStateElapsedTime = 0;
                m_fElapsedTime = 0;
                return CBossPhase1::BOSSPHASE1::BOSSIDLE;
            }
        }
        //


        break;

    case PHASE::SPREAD://움직임 + idle

        m_pOwnerAnimatorCom->Play_Animation(L"BossIdle", ANIMSTATE::ONCE);

        m_fElapsedTime += timeDelta;

        SpreadBullet(timeDelta, 10, m_SpreadInterval, 20, 10.f);

        m_fCurStateElapsedTime += timeDelta;
        //


        if (m_fCurStateElapsedTime >= 5.0f)
        {
            for (auto& it : tmpBullet)
            {
                //일단 테스트
                it->SetPendingKill();
            }
            tmpBullet.clear();

            m_Count = 0;
            m_fCurStateElapsedTime = 0;
            m_fElapsedTime = 0;
            m_fExtraElapsedTime = 0;
            return CBossPhase1::BOSSPHASE1::BOSSIDLE;
        }
        break;
    }



    return CBossPhase1::BOSSPHASE1::BULLETATTACK;
}

bool CBossBulletAttack::CircleBullet(const _float& timeDelta, int num, int radius, float interval ,float speed)
{

    if (m_fElapsedTime >= interval && m_Count <= num)
    {
        CBossBullet* BossBullet = CBossBullet::Create(m_pOwner->Get_GraphicDev(), m_pOwner, m_pPlayer, PROJ_TYPE::LINEAR);


        CTransform* BulletTransform =
            dynamic_cast<CTransform*>(BossBullet->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));

        _vec3 ownerPos = m_pOwnerTransformCom->Get_Pos();

        float rad = D3DXToRadian(360.f / (num));

        float addRad = rad * m_Count;

        _vec3 circle = { cos(addRad) * radius, 0, sin(addRad) * radius };
        _vec3 TR = ownerPos + circle;
        BulletTransform->Set_Pos({ TR.x, m_GroundY,TR.z });



        BulletDesc d;
        d.Direction = circle;
        d.Speed = speed;
        d.moveType = PROJ_TYPE::LINEAR;
        d.ProjectileOwner = m_pOwner;

        CProjectile* BulletProjectile =
            dynamic_cast<CProjectile*>(BossBullet->Get_Component(
                ID_STATIC,
                COMPONENTTYPE::PROJECTILE));

        BulletProjectile->SetMoveDesc(d);

        tmpBullet.push_back(BossBullet);


        m_Count++;
        m_fElapsedTime = 0;

    }

    if (m_Count > num)
    {
        m_fElapsedTime = 0;
        return true;
    }

    return false;
}

bool CBossBulletAttack::CrossBullet(const _float& timeDelta, int num, int offset, float interval)
{
    if (m_fElapsedTime >= interval && m_Count <= num)
    {

        for (int i = 0; i < 4;i++)
        {
            CBossBullet* BossBullet = CBossBullet::Create(m_pOwner->Get_GraphicDev(), m_pOwner, m_pPlayer, PROJ_TYPE::ORBIT);

            CTransform* BulletTransform =
                dynamic_cast<CTransform*>(BossBullet->Get_Component(
                    ID_DYNAMIC,
                    COMPONENTTYPE::TRANSFORM));

            _vec3 OwnerPos = m_pOwnerTransformCom->Get_Pos();

            float distance = ((float)m_Count * offset);

            _vec3 AddPos;
            switch (i)
            {
            case 0: //오
                AddPos = { distance, 0.f,0.f };
                break;
            case 1://왼
                AddPos = { -distance, 0.f,  0.f };
                break;
            case 2://앞
                AddPos = { 0.f, 0.f,distance };
                break;
            case 3://뒤
                AddPos = { 0.f, 0.f, -distance };
                break;
            }
            _vec3 TR = OwnerPos + AddPos;
                BulletTransform->Set_Pos({TR.x, m_GroundY,TR.z});

            BulletDesc d;
            d.Radius = distance;
            d.OrbitCenter = { OwnerPos.x ,m_GroundY, OwnerPos.x };
            d.StartAngle = 0 + (i * 90);
            d.Speed = 100;
            d.moveType = PROJ_TYPE::ORBIT;
            d.ProjectileOwner = m_pOwner;

            CProjectile* BulletProjectile =
                dynamic_cast<CProjectile*>(BossBullet->Get_Component(
                    ID_STATIC,
                    COMPONENTTYPE::PROJECTILE));

            BulletProjectile->SetMoveDesc(d);
            SpawnAttackObject(BossBullet);

            tmpBullet.push_back(BossBullet);
        }


        m_Count++;
        m_fElapsedTime = 0;
    }

    if (m_Count > num)
    {
        m_fElapsedTime = 0;
        return true;
    }

    return false;

}
bool CBossBulletAttack::SpreadBullet(const _float& timeDelta, int num, float interval, float Speed, int angleOffset)
{
    m_fExtraElapsedTime += timeDelta;

    if (m_fExtraElapsedTime >= interval)
    {


        for (int i = 0; i < num;i++)
        {
            CBossBullet* BossBullet = CBossBullet::Create(m_pOwner->Get_GraphicDev(), m_pOwner, m_pPlayer, PROJ_TYPE::LINEAR);

            CTransform* BulletTransform =
                dynamic_cast<CTransform*>(BossBullet->Get_Component(
                    ID_DYNAMIC,
                    COMPONENTTYPE::TRANSFORM));

            _vec3 ownerPos = m_pOwnerTransformCom->Get_Pos();

            BulletTransform->Set_Pos(ownerPos.x, m_GroundY, ownerPos.z);
            //_vec3 baseDir = { 0,0,1 };

            float rad = 360.f / (num);

            float addRad = rad * i;

            if (angleOffset != 0)
            {
                addRad += m_Count * angleOffset;
            }

            _vec3 circle = { cos(D3DXToRadian(addRad)) , 0.f, sin(D3DXToRadian(addRad)) };


            BulletDesc d;
            d.Direction = circle;
            d.Speed = Speed;
            d.moveType = PROJ_TYPE::LINEAR;
            d.ProjectileOwner = m_pOwner;


            CProjectile* projectileCom = dynamic_cast<CProjectile*>(BossBullet->Get_Component(COMPONENTID::ID_STATIC, COMPONENTTYPE::PROJECTILE));
            projectileCom->SetMoveDesc(d);

            // 레이어 관리로 넘기기
            SpawnAttackObject(BossBullet);

            tmpBullet.push_back(BossBullet);

            BossBullet->Start_Shoot();
        }
        m_Count++;
        m_fExtraElapsedTime = 0;
    }

    return true;

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
    if (!tmpBullet.empty())
        tmpBullet.clear();
}
