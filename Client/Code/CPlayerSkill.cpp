#include "pch.h"
#include "CPlayerSkill.h"

#include "CLayer.h"
#include "CAttackBuilder.h"
#include "CGameObject.h"
#include "CState.h"
#include "CCombatStat.h"
#include "CFireball.h"
#include "CPlayerInput.h"

CPlayerSkill::CPlayerSkill(DEVICE graphicDev) : CComponent(graphicDev),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_Fireball(nullptr),
      m_OwnerInputCom(nullptr),
      m_CurrentAttackObj(nullptr),
      m_IsSkillUnlocked(true),
      m_SkillInfo{},
      m_SkillTimer(0),
      m_ChargeTimer(0),
      m_MaxChargeTime(5.f),
      m_ProjectileFired(false),
      m_SkillIndex(5.f),
      m_IsCharged(false),
      m_IsCharging(false), 
      m_SingleSkillEnable(false),
      m_TripleSkillEnable(false),
      m_SingleFired(false),
      m_TripleFired(false)

{}

CPlayerSkill::CPlayerSkill(const CPlayerSkill& rhs) : CComponent(rhs),
      m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerInputCom(nullptr),
      m_CurrentAttackObj(nullptr),
      m_IsSkillUnlocked(true),
      m_SkillTimer(rhs.m_SkillTimer),
      m_ChargeTimer(rhs.m_ChargeTimer),
      m_MaxChargeTime(5.f),
      m_ProjectileFired(false),
      m_SkillIndex(rhs.m_SkillIndex),
      m_IsCharged(false),
      m_IsCharging(false),
      m_SingleSkillEnable(false),
      m_TripleSkillEnable(false),
      m_SingleFired(false)
{
    copy_n(&rhs.m_SkillInfo[0], static_cast<_int>(SKILL_MAX), &m_SkillInfo[0]);
}

CPlayerSkill::~CPlayerSkill() {}

HRESULT CPlayerSkill::Ready_PlayerCombat() 
{
    //충전
    m_SkillInfo[SKILL_CHARGING].state = ACTORSTATE::PLAYER_SKILL_CHARGING;
    m_SkillInfo[SKILL_CHARGING].duration = 0.5f;
    m_SkillInfo[SKILL_CHARGING].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_CHARGING].knockbackPower = 50.f;
    m_SkillInfo[SKILL_CHARGING].spawnOffset = 1.3f;
    // m_SkillInfo[SKILL_CHARGING].hitboxScale = _vec3(1.f, 1.f, 1.f);
    // 1발
    m_SkillInfo[SKILL_SINGLE].state = ACTORSTATE::PLAYER_SKILL_SINGLE;
    m_SkillInfo[SKILL_SINGLE].duration = 0.5f;
    m_SkillInfo[SKILL_SINGLE].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_SINGLE].knockbackPower = 50.f;
    m_SkillInfo[SKILL_SINGLE].spawnOffset = 1.3f;
    m_SkillInfo[SKILL_SINGLE].hitboxScale = _vec3(0.5f, 0.5f, 0.5f);
    m_SkillInfo[SKILL_SINGLE].chargeTime = 0.5f;
    // 3발
    m_SkillInfo[SKILL_TRIPLE].state = ACTORSTATE::PLAYER_SKILL_TRIPLE;
    m_SkillInfo[SKILL_TRIPLE].duration = 0.5f;
    m_SkillInfo[SKILL_TRIPLE].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_TRIPLE].knockbackPower = 50.f;
    m_SkillInfo[SKILL_TRIPLE].spawnOffset = 1.3f;
    m_SkillInfo[SKILL_TRIPLE].hitboxScale  = _vec3(0.5f, 0.5f, 0.5f);
    m_SkillInfo[SKILL_TRIPLE].chargeTime =1.5f;
    return S_OK;
}

_int CPlayerSkill::Update_Component(const _float& timeDelta)
{
    return 0;
}

void CPlayerSkill::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();
}

HRESULT     CPlayerSkill::Init_OwnerComponent()
{
    m_OwnerStateCom = dynamic_cast<CState*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
    m_OwnerCombatStatCom = dynamic_cast<CCombatStat*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));
    m_OwnerInputCom = dynamic_cast<CPlayerInput*>(m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::PL_INPUT));

    HRESULT result = S_OK;

    if (nullptr == m_OwnerStateCom)
    {
        MessageBoxW(nullptr,
            L"CPlayerSkill:Owner State Not Found",
            L"System Message",
            MB_ICONERROR);

        result = E_FAIL;
    }

    if (nullptr == m_OwnerCombatStatCom)
    {
        MessageBoxW(nullptr,
            L"CPlayerSkill:Owner CombatStat Not Found",
            L"System Message",
            MB_ICONERROR);

        result = E_FAIL;
    }


    if (nullptr == m_OwnerInputCom)
    {
        MessageBoxW(nullptr,
            L"CPlayerSkill:Owner PlayerInput Not Found",
            L"System Message",
            MB_ICONERROR);

        result = E_FAIL;
    }

    return result;
}

ACTORSTATE CPlayerSkill::Start_Charging()
{
    Reset_AttackData();

	if (!m_IsSkillUnlocked)
		return ACTORSTATE::STATE_END;

	CCombatStat* playerCombatStat = dynamic_cast<CCombatStat*>(m_Owner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

	if (playerCombatStat->Get_CurrentCharges() <= 0)
		return ACTORSTATE::STATE_END;

    if (!m_IsCharging)
    {
        m_ChargeTimer = 0.f;
    }

    m_IsCharging = true;

	return m_SkillInfo[SKILL_CHARGING].state;
}
ACTORSTATE      CPlayerSkill::Update_Charging(const float& timeDelta)
{
    //cout << "Update_Charging" << endl;

    _vec3 vMouseDir = m_OwnerInputCom->Get_MouseDir();

    if(vMouseDir.x <= 0.f)
    {
        m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
    }
    else
    {
        m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);
    }

    if(!m_IsCharging)
       return ACTORSTATE::IDLE;

    m_ChargeTimer+= timeDelta;

    cout << m_ChargeTimer << endl;

    if (m_ChargeTimer >= m_SkillInfo[SKILL_TRIPLE].chargeTime)
    {
        //cout << "3발 공격 사용 가능" << endl;
        return ACTORSTATE::PLAYER_SKILL_TRIPLE;
    }

	if(m_ChargeTimer >= m_SkillInfo[SKILL_SINGLE].chargeTime)
	{
        //cout << "1발 공격 사용 가능" << endl;
        return ACTORSTATE::PLAYER_SKILL_SINGLE;
	}


    if (m_ChargeTimer >= m_MaxChargeTime)
    {
        m_ChargeTimer = m_MaxChargeTime;
        m_IsCharging = false;

        return ACTORSTATE::IDLE;
    }

    return ACTORSTATE::PLAYER_SKILL_CHARGING;
}



ACTORSTATE  CPlayerSkill::Start_SingleSkill()
{
    //Reset_AttackData();

    if (!m_IsCharging)
        return ACTORSTATE::STATE_END;

    if(!m_IsSkillUnlocked)
    return ACTORSTATE::STATE_END;

    if (m_ChargeTimer >= m_SkillInfo[SKILL_SINGLE].chargeTime)
    {
        m_SkillTimer = 0;
        m_IsCharging = true;
        m_SingleFired = false;
        m_SkillIndex = SKILL_SINGLE;
    }
  
    return m_SkillInfo[SKILL_SINGLE].state;
}

_bool       CPlayerSkill::Update_SingleSkill(const float& timeDelta)
{
   // if (m_SingleFired)
   //     return false;
   //
    for (auto iter = m_Fireballs.begin(); iter != m_Fireballs.end();)
    {
        if ((*iter)->Is_PendingDestroy())
            iter = m_Fireballs.erase(iter);
        else
        {
            iter++;
        }
    }

    //if (m_SkillIndex < SKILL_SINGLE || m_SkillIndex>SKILL_MAX)
	m_SkillTimer += timeDelta;
	m_ChargeTimer += timeDelta;

    const SkillInfo& skillInfo = m_SkillInfo[SKILL_SINGLE];
    _vec3 MouseDir = m_OwnerInputCom->Get_MouseDir();

    // MouseDir.z = 0;
    D3DXVec3Normalize(&MouseDir, &MouseDir);

    if(!m_SingleFired)
    {
        CFireball* fireball = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev,m_Owner, MouseDir);
		m_Fireballs.push_back(fireball);
        cout << "single 투사체 발사 완료" << endl;
        
        m_SingleFired = true;
    }

    if(m_SkillTimer >= m_SkillInfo[SKILL_SINGLE].duration)
    {
        return false;
    }

    return true;
}
ACTORSTATE  CPlayerSkill::Start_TripleSkill()
{

    if (!m_IsCharging)
        return ACTORSTATE::STATE_END;

    if (!m_IsSkillUnlocked)
        return ACTORSTATE::STATE_END;

    if (m_ChargeTimer >= m_SkillInfo[SKILL_TRIPLE].chargeTime)
    {
        m_SkillTimer = 0;
        m_IsCharging = true;
        m_TripleFired = false;
        m_SkillIndex = SKILL_TRIPLE;
    }
	
    return m_SkillInfo[SKILL_TRIPLE].state;
}
_bool       CPlayerSkill::Update_TripleSkill(const _float& timeDelta)
{
    if (!m_OwnerCombatStatCom || !m_OwnerInputCom) return false;

   // if (m_TripleFired)
   //     return false;

    for (auto iter = m_Fireballs.begin(); iter != m_Fireballs.end();)
    {
        if ((*iter)->Is_PendingDestroy())
            iter = m_Fireballs.erase(iter);
        else
        {
            iter++;
        }
    }
  
  _vec3  MouseDir = m_OwnerInputCom->Get_MouseDir();

    m_SkillTimer += timeDelta;
    m_ChargeTimer += timeDelta;
    const SkillInfo& skillInfo = m_SkillInfo[SKILL_TRIPLE];

   // MouseDir.z = 0;

    D3DXVec3Normalize(&MouseDir, &MouseDir);

    if (!m_TripleFired)
    {
        cout << "triple 투사체 발사 완료" << endl;
        CFireball* center = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev, m_Owner, MouseDir);

        float baseAngle = atan2f(MouseDir.z, MouseDir.x);

        float offset = D3DXToRadian(15.f);
		float leftAngle = baseAngle - offset;
		float RightAngle = baseAngle + offset;

		_vec3 vLeft = { cosf(leftAngle), 0.f, sinf(leftAngle) };
		_vec3 vRight = { cosf(RightAngle), 0.f, sinf(RightAngle) };

		CFireball* left = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev, m_Owner, vLeft);

		CFireball* right = Create_Fireball(m_SkillInfo[m_SkillIndex], m_GraphicDev, m_Owner, vRight);

		m_Fireballs.push_back(center);
		m_Fireballs.push_back(left);
		m_Fireballs.push_back(right);
        
        m_TripleFired = true;
    }

	//for(auto fireball : m_Fireballs)
	//{
	//	fireball->Update_GameObject(timeDelta);
	//}
	//
    if (m_SkillTimer >= m_SkillInfo[SKILL_TRIPLE].duration)
    {
        return false;
    }

  //if (!m_ProjectileFired && m_SkillTimer >= skillInfo.projectileInterval) // 나중에 필요하면 시간 제어 조건도 추가
  //{
  //    Create_Projectile(skillInfo, vMouseDir);
  //    m_SkillTimer = 0.f;
  //    m_ProjectileFired = true;
  //}
  //
  //if (m_ChargeTimer >= skillInfo.tripleChargeTime)
  //    m_IsCharged = true;
  //                                                                                                                                                                                                                                                                                                                                                                                                                                                           
//발사했으면 반환

    return true;
}

ACTORSTATE CPlayerSkill::Evaluate_ChargeLevel()
{

    return ACTORSTATE::STATE_END;
}

void        CPlayerSkill::Cancel_Attack()
{
    Reset_AttackData();
}
void        CPlayerSkill::Reset_AttackData()
{
    m_SkillTimer = 0.f;
    m_ChargeTimer = 0.f;
}

CFireball* CPlayerSkill::Create_Fireball(const SkillInfo& skillInfo, DEVICE graphicDev, CGameObject* instigator,_vec3 skillDir)
{
    CFireball* fireball = nullptr;

    CAttackBuilder(graphicDev, instigator)
        .SetType<CFireball>() // .Box 박스 충돌체 소환하던걸 이제 Fireball, 이게 Sphere를 상속받아서 자체적으로 충돌체가 있어
        .Track(fireball)  
        .Damage(dynamic_cast<CCombatStat*>(instigator->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT))->Get_Attack() * skillInfo.damageMultiplier)
        .Knockback(skillInfo.knockbackPower)
        .Scale(skillInfo.hitboxScale)
        .InstigatorPos()
        .Offset(skillDir * skillInfo.spawnOffset)
        .LifeTime(3.f)
        .DestroyOnHit(true)
        .CollisionLayer(COLLISION_LAYERTYPE::PLAYERPROJECTILE)
        .Spawn();

    if (fireball)
    {
        auto projectile = dynamic_cast<CProjectile*>(fireball->Get_Component(ID_STATIC, COMPONENTTYPE::PROJECTILE));
        if (projectile)
        {
            BulletDesc d;
            d.ProjectileOwner = instigator;
            d.moveType = PROJ_TYPE::LINEAR;
            d.Speed = 10.f;
            d.Direction = skillDir;
            projectile->SetMoveDesc(d);
        }

        auto transform = fireball->Get_Transform();
        if (transform)
        {
            //transform->Set_Scale(0.7f, 0.7f, 0.f);
            transform->Set_Rotation(_vec3{ D3DXToRadian(30.f), 0.f, 0.f });
        }
    }

    return fireball;
}

CPlayerSkill* CPlayerSkill::Create(DEVICE graphicDev)
{
    auto playerskill = new CPlayerSkill(graphicDev);

    if (FAILED(playerskill->Ready_PlayerCombat()))
    {
        Safe_Release(playerskill);
        MSG_BOX("PlayerCombat Create Failed");
        return nullptr;
    }

    return playerskill;
}
CComponent* CPlayerSkill::Clone()
{
    return new CPlayerSkill(*this);
}

void CPlayerSkill::Free()
{
    m_OwnerStateCom = nullptr;
    m_OwnerCombatStatCom = nullptr;
    m_OwnerInputCom = nullptr;

    //총알은 레이어에 추가해서 CLayer LateUpdate에서 삭제

    CComponent::Free();
}
