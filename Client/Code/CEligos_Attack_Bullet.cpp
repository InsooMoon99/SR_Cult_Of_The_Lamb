#include "pch.h"
#include "CEligos_Attack_Bullet.h"
#include "CMonsterState.h"
#include "CEligos.h"
#include "CEligos_StateMachine.h"
#include "CTransform.h"
#include "CAnimator.h"
#include "CSceneMgr.h"
#include "CSpriteAnimation.h"
#include "CRigidbody.h"
#include "CRandomMgr.h"
#include "CBossBullet.h"
#include "CProjectile.h"
#include "CCreateHelper.h"

CEligos_Attack_Bullet::CEligos_Attack_Bullet(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    m_pOwner = pOwner;
    m_pStateMachine = pStateMachine;
}

CEligos_Attack_Bullet::~CEligos_Attack_Bullet()
{

}

void CEligos_Attack_Bullet::Enter()
{
    CMonsterState::Enter();
    // 일단 지금은 
    m_pOwnerAnimatorCom->Play_Animation(L"Eligos_fly", ANIMSTATE::LOOP);

    float angle = CRandomMgr::GetInstance()->Get_Int_Random(
        int(-D3DX_PI * 1000), int(D3DX_PI * 1000)) / 1000.f;
    m_MoveDir = { cosf(angle), 0.f, sinf(angle) };
    D3DXVec3Normalize(&m_MoveDir, &m_MoveDir);
    // 속도 리셋
    m_pOwnerRigidbodyCom->Set_Velocity({ 0,0,0 });


    m_ePhase = PHASE::MOVE;
}

void CEligos_Attack_Bullet::Exit()
{
    CMonsterState::Exit();
}

HRESULT CEligos_Attack_Bullet::Ready_State()
{
    m_fMinStateTime = 0.f;

    m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    m_pOwnerAnimatorCom = static_cast<CAnimator*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
    m_pOwnerRigidbodyCom = static_cast<CRigidbody*>(m_pOwner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    Find_Player();

    return S_OK;
}

_int CEligos_Attack_Bullet::Update_State(const _float& timeDelta)
{
    CMonsterState::Update_State(timeDelta);

    if (m_pOwnerAnimatorCom->Get_CurAnimation() == nullptr)
        return CEligos::ELIGOSSTATE::ATTACK_BULLET;

    switch (m_ePhase)
    {
    case PHASE::MOVE:
    {
        if (m_MoveDir.x > 0)// 플레이어 오른쪽
        {
            if (m_pOwnerTransformCom->Get_Scale().x > 0)
                m_pOwnerTransformCom->Set_Scale(m_pOwnerTransformCom->Get_Scale().x * -1, m_pOwnerTransformCom->Get_Scale().y, m_pOwnerTransformCom->Get_Scale().z);
        }
        else
        {
            if (m_pOwnerTransformCom->Get_Scale().x < 0)
                m_pOwnerTransformCom->Set_Scale(m_pOwnerTransformCom->Get_Scale().x * -1, m_pOwnerTransformCom->Get_Scale().y, m_pOwnerTransformCom->Get_Scale().z);
        }

        m_pOwnerRigidbodyCom->Set_Velocity(m_MoveDir * 8.0f);

        if (m_pOwner->Get_OnWall() == true)
        {

            m_pOwnerRigidbodyCom->Set_Velocity({ 0,0,0 });
            // 이거 말고 제 위치에 도달 하면 레디로 가자
            m_ePhase = PHASE::READY;

            //if (m_OnTalkStart)
            //    m_OnTalkStart();
        }

        break;
    }
    case PHASE::READY:

        //플레이어 위치 캐싱
        if (m_flag == false)
        {
            tmpOwnerPos = { m_pOwnerTransformCom->Get_Pos().x,m_pPlayerTransformCom->Get_Pos().y,m_pOwnerTransformCom->Get_Pos().z };
            m_TargetDir = m_pPlayerTransformCom->Get_Pos() - tmpOwnerPos;
            D3DXVec3Normalize(&m_TargetDir, &m_TargetDir);

            if (m_pOwnerTransformCom->Get_Pos().x < m_pPlayerTransformCom->Get_Pos().x)// 플레이어 오른쪽
            {
                if (m_pOwnerTransformCom->Get_Scale().x > 0)
                    m_pOwnerTransformCom->Set_Scale(m_pOwnerTransformCom->Get_Scale().x * -1, m_pOwnerTransformCom->Get_Scale().y, m_pOwnerTransformCom->Get_Scale().z);
            }
            else
            {
                if (m_pOwnerTransformCom->Get_Scale().x < 0)
                    m_pOwnerTransformCom->Set_Scale(m_pOwnerTransformCom->Get_Scale().x * -1, m_pOwnerTransformCom->Get_Scale().y, m_pOwnerTransformCom->Get_Scale().z);
            }

            m_flag = true;
        }

        m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attackcharge_bullet", ANIMSTATE::ONCE);

        if (m_pOwnerAnimatorCom->Is_AnimationFinished())
        {
            m_flag = false;
            m_ePhase = PHASE::SHOOT;

        }
        break;

    case PHASE::SHOOT: //움직이면서 쏘는거
        m_pOwnerAnimatorCom->Play_Animation(L"Eligos_attack_bullet", ANIMSTATE::ONCE);


        m_pOwnerRigidbodyCom->Set_Velocity(m_TargetDir * 10.0f);

        Shoot_Bullet();
        if (m_pOwner->Get_OnWall() == true)
        {
            m_pOwnerRigidbodyCom->Set_Velocity({ 0,0,0 });
        }

        if (m_pOwnerAnimatorCom->Is_AnimationFinished())
        {
            m_flag = false;
            m_pOwnerRigidbodyCom->Set_Velocity({ 0,0,0 });
            m_ShootDistAcc = 0;
            return CEligos::ELIGOSSTATE::BOSSIDLE;
        }
        break;
    }

    return CEligos::ELIGOSSTATE::ATTACK_BULLET;
}

void CEligos_Attack_Bullet::LateUpdate_State(const _float& timeDelta)
{
}

void CEligos_Attack_Bullet::Render_State()
{
}

void CEligos_Attack_Bullet::Shoot_Bullet()
{

    float m_GroundY = 1.f;

    if (m_flag == false)
    {
        m_PrevPos = m_pOwnerTransformCom->Get_Pos();
        m_flag = true;
    }
    _vec3 curPos = m_pOwnerTransformCom->Get_Pos();
    _vec3 vec = curPos - m_PrevPos;
    float length = D3DXVec3Length(&vec);

    m_ShootDistAcc += length;

    if (m_ShootDistAcc > 2.f)
    {
        // 불랫 만들기
        for (int i = 0; i < 2;i++)
        {
        CBossBullet* BossBullet = CBossBullet::Create(m_pOwner->Get_GraphicDev(), m_pOwner, (CGameObject*)m_pPlayer, PROJ_TYPE::LINEAR);

        CTransform* BulletTransform =
            dynamic_cast<CTransform*>(BossBullet->Get_Component(
                ID_DYNAMIC,
                COMPONENTTYPE::TRANSFORM));

        BulletTransform->Set_Scale({ 1.0f,1.0f,1.0f });


       

        _vec3 ownerPos = m_pOwnerTransformCom->Get_Pos();
        _vec3 Up = { 0, 1, 0 };
        _vec3 AddPos;
            switch (i)
            {
            case 0: //오
                D3DXVec3Cross(&AddPos, &Up, &m_TargetDir);
                AddPos *= 1.1f;
                break;
            case 1://왼
                D3DXVec3Cross(&AddPos, &Up, &m_TargetDir);
                AddPos *= -1.1f;

                break;

            }

            _vec3 TR = ownerPos + AddPos;
            BulletTransform->Set_Pos({ TR.x, m_GroundY,TR.z });

            _vec3 Forward = m_TargetDir;
            D3DXVec3Normalize(&Forward, &Forward);
           
           
           
            _vec3 vDir45;
            D3DXMATRIX rotMat;

            switch (i)
            {
            case 0: //오
               
               
                D3DXMatrixRotationAxis(&rotMat, &Up, D3DXToRadian(45.f)); // 45도 회전

                D3DXVec3TransformNormal(&vDir45, &Forward, &rotMat);
                D3DXVec3Normalize(&vDir45, &vDir45);


                break;
            case 1://왼
                
               
                D3DXMatrixRotationAxis(&rotMat, &Up, D3DXToRadian(-45.f)); // 45도 회전

                D3DXVec3TransformNormal(&vDir45, &Forward, &rotMat);
                D3DXVec3Normalize(&vDir45, &vDir45);

                break;

            }
            {
                //불렛 회전 빌보드 안하고 이걸로  방향 셋팅
                _vec3 forward = vDir45;
                D3DXVec3Normalize(&forward, &forward);
                _vec3 tmpUp(0.f, 1.f, 0.f);

                // 1. 진행 방향 LookAt
                _matrix rotMat = BulletTransform->BuildLookRotation(forward, _vec3(0.f, 1.f, 0.f));

                _quat lookRot;
                D3DXQuaternionRotationMatrix(&lookRot, &rotMat);

                _vec3 tmp (1.f, 0.f, 0.f);
                // 2. 기본 모델 보정 (X축 90도)
                _quat baseOffset;
                D3DXQuaternionRotationAxis(
                    &baseOffset,
                    &tmp,
                    D3DXToRadian(90.f)
                );

                // 3. 추가 기울기 (X축 +25도)
                _quat tilt25;

                D3DXQuaternionRotationAxis(
                    &tilt25,
                    &tmp,
                    D3DXToRadian(25.f)
                );

                // ⭐ 최종 회전 = LookAt → 기본보정 → 추가기울기
                _quat finalRot;
                D3DXQuaternionMultiply(&finalRot, &baseOffset, &lookRot);
                D3DXQuaternionMultiply(&finalRot, &tilt25, &finalRot);

                BulletTransform->Set_Quat(finalRot);
                BulletTransform->Set_Dirty(true);
            }



            BulletDesc d;
            d.Direction = vDir45;
            d.Speed =5;
            d.moveType = PROJ_TYPE::LINEAR;
            d.ProjectileOwner = m_pOwner;

            CProjectile* BulletProjectile =
                dynamic_cast<CProjectile*>(BossBullet->Get_Component(
                    ID_STATIC,
                    COMPONENTTYPE::PROJECTILE));

            BulletProjectile->SetMoveDesc(d);
            SpawnObjectOnLayer(BossBullet);

            tmpBullet.push_back(BossBullet);
            BossBullet->Start_Shoot();
        }
            m_ShootDistAcc = 0;
    }
            m_PrevPos = curPos;
    // 초기 위치 저장
   //현제 위치와 비교 lenght 가 얼마 기준이 되면 총알 발사

}

CEligos_Attack_Bullet* CEligos_Attack_Bullet::Create(CEligos* pOwner, CEligos_StateMachine* pStateMachine)
{
    auto BossDead = new CEligos_Attack_Bullet(pOwner, pStateMachine);

    if (FAILED(BossDead->Ready_State()))
    {
        MSG_BOX("BossDead Create Failed");
        Safe_Release(BossDead);
        return nullptr;
    }

    return BossDead;
}

void CEligos_Attack_Bullet::Free()
{

}
