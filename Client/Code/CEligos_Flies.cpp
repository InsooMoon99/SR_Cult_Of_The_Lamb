#include "pch.h"
#include "CEligos_Flies.h"


#include "CAnimator.h"
#include "CAttackBuilder.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CParticleHelper.h"
#include "CPlayer.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUI_MonsterHp.h"
#include "CProjectile.h"
#include "CRandomMgr.h"
#include "CSoundMgr.h"

CEligos_Flies::CEligos_Flies(DEVICE graphicDev, CGameObject* owner, CGameObject* Target)
    : CMonsterBase(graphicDev), m_pOwner(owner), m_pTarget(Target)
{
}

CEligos_Flies::CEligos_Flies(const CEligos_Flies& rhs) : CMonsterBase(rhs)
{
}

CEligos_Flies::~CEligos_Flies()
{
}

HRESULT CEligos_Flies::Ready_GameObject()
{
    if (FAILED(CMonsterBase::Ready_GameObject()))
        return E_FAIL;

    m_TransformCom->Set_Pos(_vec3(5.f, 1.f, 7.f));
    m_TransformCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));


    m_CombatStatCom->Set_MaxHp(1.f);
    m_CombatStatCom->Set_Attack(1.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);
    m_CombatStatCom->Set_HitState(false);


    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::FLY);


    return S_OK;
}

void CEligos_Flies::Initialize_GameObject()
{
    CMonsterBase::Initialize_GameObject();


    m_TargetTransform = dynamic_cast<CTransform*>(
        CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
            LAYERTYPE::GAMELOGIC,
            OBJTYPE::PLAYER,
            COMPONENTTYPE::TRANSFORM));

    if (m_TargetTransform)
        m_Target = m_TargetTransform->Get_Owner();

    BulletDesc d;
    d.Radius = 0;
    d.OrbitCenter = { 0,1,0 };
    d.StartAngle = 0;
    d.Speed = 1.5f;
    d.moveType = PROJ_TYPE::HOMING;
    d.ProjectileOwner = m_pOwner;
    d.Target = m_pTarget;
    d.TurnSpeed = d.Speed * 2.f;;

    m_ProjectileCom->SetMoveDesc(d);


    RandHeight = (CRandomMgr::GetInstance()->Get_Int_Random(30, 40)) / 100;
    RandTerm = (CRandomMgr::GetInstance()->Get_Int_Random(10,20)) / 100;
}

_int CEligos_Flies::Update_GameObject(const _float& timeDelta)
{
    _int exit = CMonsterBase::Update_GameObject(timeDelta);

    m_FlapTime += timeDelta;
    float y = sinf(m_FlapTime * RandTerm) * RandHeight;
    _vec3 curPos = m_TransformCom->Get_Pos();
    m_TransformCom->Set_Pos(curPos.x, curPos.y + y, curPos.z);

    if (m_spawnSound == false)
    {
        CSoundMgr::GetInstance()->Play_Once(L"Boss/Eligos/Fly_Spawn.wav", SOUNDTYPE::SOUND_BOSS4, 1.0f);
       
        m_spawnSound = true;
    }

    m_ProjectileCom->Update_Component(timeDelta);
    
    if (m_CombatStatCom->Is_Dead() == true||m_flyDie==true)
    {
        if (m_CombatStatCom->Is_Dead() == true)
        {
        //죽었을떄 애니메이션 추가
        CParticleHelper::Spawn_StaticBurstParticle(
            m_TransformCom->Get_Pos(),					 // 위치
            COMPONENTTYPE::TEX_MONSTER_HIT_PARTICLE,     // 텍스처 타입
            9,											 // 생성할 개수
            0.12f, 0.15f,								 // 크기 범위
            4,											 // 텍스처 랜덤으로 몇개 출력
            19.f,										 // 땅에 떨어지는 속도
            0.5f										 // 바닥 Y
        );

        }

        //끝나면
        CSoundMgr::GetInstance()->Stop_Sound(L"Boss/Eligos/Fly_Fly_Loop.wav");
        CSoundMgr::GetInstance()->Play_Once(L"Boss/Eligos/Flying_Hopper_Miniboss_Bite.wav", SOUNDTYPE::SOUND_BOSS3, 0.7f);

        Mark_As_PendingKill();
    }

    if (m_flag == false)
    {
        m_AnimatorCom->Play_Animation(L"Mortar_fly", ANIMSTATE::LOOP);
        m_flag = true;
    }

    m_Timer += timeDelta;
    if (m_Timer > 10)
    {
        Mark_As_PendingKill();
    }
    return exit;
}

void CEligos_Flies::LateUpdate_GameObject(const _float& timeDelta)
{
    CMonsterBase::LateUpdate_GameObject(timeDelta);
}

void CEligos_Flies::Render_GameObject()
{
    CMonsterBase::Render_GameObject();
}


void CEligos_Flies::Update_Direction()
{
}

void CEligos_Flies::Animation_Setting()
{
    // 애니메이션 생성
    m_AnimatorCom->Create_Animation(L"Mortar_fly", 27, 0.02f);
}

void CEligos_Flies::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnBeginOverlap(self, other);



    CPlayer* player = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (player)
    {
        CCombatStat* Player_combat = dynamic_cast<CCombatStat*>(player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
        CTransform* Player_tans = dynamic_cast<CTransform*>(player->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

        _vec3 tmp = Player_tans->Get_Pos() - m_TransformCom->Get_Pos();

        _vec3 dir;
            D3DXVec3Normalize(&dir, &tmp);

        HITDATA hit;
        hit.  dir = dir;              // 넉백 방향
        hit. damage = m_CombatStatCom->Get_Attack();           // 데미지
        hit. knockback=8.f;        // 넉백량
        hit. knockbackTimer=0.5f;         // 넉백 유지 시간

        hit. attacker = this;   // 때린 놈
        hit. hitter = player;     // 맞은 놈


        Player_combat->Apply_Damage(hit);

		//CParticleHelper::Spawn_StaticBurstParticle(
		//	m_TransformCom->Get_Pos(),					 // 위치
		//	COMPONENTTYPE::TEX_MONSTER_HIT_PARTICLE,     // 텍스처 타입
		//	9,											 // 생성할 개수
		//	0.12f, 0.15f,								 // 크기 범위
		//	4,											 // 텍스처 랜덤으로 몇개 출력
		//	19.f,										 // 땅에 떨어지는 속도
		//	0.5f										 // 바닥 Y
		//);

        // 나 죽을 수 있게 
        m_flyDie =true;
    }
}


void CEligos_Flies::OnStayOverlap(CCollider* self, CCollider* other)
{
    CRenderObject::OnStayOverlap(self, other);



    CEligos_Flies* fly = dynamic_cast<CEligos_Flies*>(other->Get_Owner());
    if (!fly) return;

    CTransform* otherTransform = dynamic_cast<CTransform*>(
        fly->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    CBoxCollider* playerCollider = dynamic_cast<CBoxCollider*>(other);



    //// 속도 제거
    //CRigidbody* rigid = dynamic_cast<CRigidbody*>(
    //    hitObject->Get_Component(ID_DYNAMIC, COMPONENTTYPE::RIGIDBODY));

    //if (rigid)
    //{
    //    _vec3 vel = rigid->Get_Velocity();
    //    vel.x = 0.f;
    //    vel.z = 0.f;
    //    rigid->Set_Velocity(vel);
    //}
    //
    //else
    //    return;



    if (!otherTransform || !playerCollider) return;

    _vec3 wallPos = m_TransformCom->Get_Pos();
    _vec3 objPos = otherTransform->Get_Pos();

    // 벽/플레이어 사이즈 합의 절반
    float halfX = ((m_BoxColCom->Get_Size().x + playerCollider->Get_Size().x) * 0.5f);
    float halfZ = ((m_BoxColCom->Get_Size().z + playerCollider->Get_Size().z) * 0.5f);

    // 중심 거리
    float dx = objPos.x - wallPos.x;
    float dz = objPos.z - wallPos.z;

    float penX = halfX - abs(dx);
    float penZ = halfZ - abs(dz);

    // 덜 침투한 축으로 밀어냄
    if (penX < penZ)
        objPos.x = wallPos.x + (dx > 0 ? halfX : -halfX) + (dx > 0 ? 0.01f : -0.01f);
    else
        objPos.z = wallPos.z + (dz > 0 ? halfZ : -halfZ) + (dz > 0 ? 0.01f : -0.01f);

    otherTransform->Set_Pos(objPos);
}

void CEligos_Flies::OnEndOverlap(CCollider* self, CCollider* other)
{
    CMonsterBase::OnEndOverlap(self, other);

}

HRESULT CEligos_Flies::Add_Component()
{
    if (FAILED(CMonsterBase::Add_Component()))
        return E_FAIL;

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_FLY);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_FLY, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_FLY);

    // projectile
    m_ProjectileCom = CreateProtoComponent<CProjectile>(this, COMPONENTTYPE::PROJECTILE);
    NULL_CHECK_RETURN(m_ProjectileCom, E_FAIL);
    
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PROJECTILE, m_ProjectileCom });
    return S_OK;
}

CEligos_Flies* CEligos_Flies::Create(DEVICE graphicDev, CGameObject* owner, CGameObject* Target)
{
    auto Eligos_Flies = new CEligos_Flies(graphicDev, owner, Target);

    if (FAILED(Eligos_Flies->Ready_GameObject()))
    {
        MSG_BOX("Eligos_Flies Create Failed");
        Safe_Release(Eligos_Flies);
        return nullptr;
    }

    return Eligos_Flies;
}

void CEligos_Flies::Free()
{
    CMonsterBase::Free();
}
