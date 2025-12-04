#include "pch.h"
#include "CPlayerSkill.h"
#include "CAttackBuilder.h"
#include "CGameObject.h"
#include "CState.h"
#include "CCombatStat.h"
#include "CPlayerInput.h"

CPlayerSkill::CPlayerSkill(DEVICE graphicDev)
    : m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerInputCom(nullptr),
      m_CurrentAttackObj(nullptr),
      m_IsSkillUnlocked(true),
      m_SkillInfo{},
      m_SkillTimer(0),
      m_ChargeTimer(0),
      m_MaxChargeTime(8.f),
      m_ProjectileFired(false),
      m_SkillIndex(5.f),
      m_IsCharged(false),
      m_IsCharging(false),
      m_SingleSkillEnable(false),
      m_TripleSkillEnable(false)
{}

CPlayerSkill::CPlayerSkill(const CPlayerSkill& rhs)
    : m_OwnerStateCom(nullptr),
      m_OwnerCombatStatCom(nullptr),
      m_OwnerInputCom(nullptr),
      m_CurrentAttackObj(nullptr),
      m_IsSkillUnlocked(true),
      m_SkillTimer(rhs.m_SkillTimer),
      m_ChargeTimer(rhs.m_ChargeTimer),
      m_MaxChargeTime(8.f),
      m_ProjectileFired(false),
      m_SkillIndex(rhs.m_SkillIndex),
      m_IsCharged(false),
      m_IsCharging(false),
      m_SingleSkillEnable(false),
      m_TripleSkillEnable(false)
{
    copy_n(&rhs.m_SkillInfo[0], static_cast<_int>(SKILL_MAX), &m_SkillInfo[0]);
}

CPlayerSkill::~CPlayerSkill() {}

HRESULT CPlayerSkill::Ready_PlayerCombat() 
{
    //충전
    m_SkillInfo[SKILL_CHARGING].state = ACTORSTATE::PLAYER_SKILL_CHARGING;
    m_SkillInfo[SKILL_CHARGING].duration = 1.5f;
    m_SkillInfo[SKILL_CHARGING].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_CHARGING].knockbackPower = 50.f;
    m_SkillInfo[SKILL_CHARGING].spawnOffset = 1.3f;
    m_SkillInfo[SKILL_CHARGING].hitboxScale = _vec3(1.f, 1.f, 1.f);
    // 1발
    m_SkillInfo[SKILL_SINGLE].state = ACTORSTATE::PLAYER_SKILL_SINGLE;
    m_SkillInfo[SKILL_SINGLE].duration = 1.5f;
    m_SkillInfo[SKILL_SINGLE].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_SINGLE].knockbackPower = 50.f;
    m_SkillInfo[SKILL_SINGLE].spawnOffset = 1.3f;
    m_SkillInfo[SKILL_SINGLE].hitboxScale = _vec3(1.f, 1.f, 1.f);
    m_SkillInfo[SKILL_SINGLE].chargeTime = 3.f;
    // 3발
    m_SkillInfo[SKILL_TRIPLE].state = ACTORSTATE::PLAYER_SKILL_TRIPLE;
    m_SkillInfo[SKILL_TRIPLE].duration = 1.5f;
    m_SkillInfo[SKILL_TRIPLE].damageMultiplier = 2.f;
    m_SkillInfo[SKILL_TRIPLE].knockbackPower = 50.f;
    m_SkillInfo[SKILL_TRIPLE].spawnOffset = 1.3f;
    m_SkillInfo[SKILL_TRIPLE].hitboxScale = _vec3(1.f, 1.f, 1.f);
    m_SkillInfo[SKILL_TRIPLE].chargeTime =7.f;
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
    cout << "Update_Charging" << endl;

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
        cout << "3발 공격 사용 가능" << endl;
        return ACTORSTATE::PLAYER_SKILL_TRIPLE;
    }

	if(m_ChargeTimer >= m_SkillInfo[SKILL_SINGLE].chargeTime)
	{
        cout << "1발 공격 사용 가능" << endl;
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
        return ACTORSTATE::IDLE;

    if(!m_IsSkillUnlocked)
    return ACTORSTATE::STATE_END;

    //TODO 유림 : 시간 초 확인해서 해당 스킬을 사용할 수 있는 범위 내인지 판단하는 로직 추가
    if (m_ChargeTimer >= m_SkillInfo[SKILL_SINGLE].chargeTime)
    {
        cout << "첫번째 스킬 차지 완료" << endl;
        m_SingleSkillEnable = true;
        m_IsCharging = false;
        m_SkillIndex = SKILL_SINGLE;
    }
   // else
   // {
   //     return m_SkillInfo[SKILL_CHARGING].state;
   // }
    //return m_SkillInfo[SKILL_SINGLE].state;
    return m_SkillInfo[SKILL_CHARGING].state;
}

_bool       CPlayerSkill::Update_SingleSkill(const float& timeDelta)
{
    if (!m_OwnerCombatStatCom || !m_OwnerInputCom) return false;

    //애니메이션 방향 결정
    _vec3  vMouseDir = m_OwnerInputCom->Get_MouseDir();

    if (vMouseDir.x <= 0.f)
        m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
    else
        m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);

    //if (m_SkillIndex < SKILL_SINGLE || m_SkillIndex>SKILL_MAX)
	m_SkillTimer += timeDelta;
	m_ChargeTimer += timeDelta;

    const SkillInfo& skillInfo = m_SkillInfo[SKILL_SINGLE];

    vMouseDir.z = 0;

    D3DXVec3Normalize(&vMouseDir, &vMouseDir);

    //if(!m_ProjectileFired&& m_SkillTimer >=skillInfo.projectileInterval) // 나중에 필요하면 시간 제어 조건도 추가
    //{
    //    Create_Projectile(skillInfo,vMouseDir);
    //    m_SkillTimer = 0.f;
    //    m_ProjectileFired = true;
    //}
	//
    // (m_ChargeTimer >= skillInfo.tripleChargeTime)
    //  m_IsCharged = true;
	//
    //if(m_IsCharged)
    //{
    //    m_OwnerStateCom->Change_State(m_SkillInfo[SKILL_SINGLE].state);
    //}
    //else
    //{
    //    m_OwnerStateCom->Change_State(ACTORSTATE::PLAYER_SKILL_SINGLE);
    //}


    //발사했으면 false 반환
    return false;
    return true;
}
ACTORSTATE  CPlayerSkill::Start_TripleSkill()
{

    if (m_IsCharging)
        return ACTORSTATE::IDLE;

    if (m_ChargeTimer >= m_SkillInfo[SKILL_TRIPLE].chargeTime)
    {
        cout << "두번째 스킬 차지 완료" << endl;
        m_SkillIndex = SKILL_TRIPLE;
    }
    else
    {
        return m_SkillInfo[SKILL_CHARGING].state;
    }
	//if (m_IsCharged)
	//{
	//	Reset_AttackData();
    //
	//	m_SkillIndex = SKILL_TRIPLE;
	//	return m_SkillInfo[SKILL_MAX].state;
	//}
	//else
	//	return ACTORSTATE::STATE_END;
    return m_SkillInfo[SKILL_TRIPLE].state;
}
_bool       CPlayerSkill::Update_TripleSkill(const _float& timeDelta)
{
    if (!m_OwnerCombatStatCom || !m_OwnerInputCom) return false;

    //애니메이션 방향 결정
    _vec3  vMouseDir = m_OwnerInputCom->Get_MouseDir();

    if (vMouseDir.x <= 0.f)
        m_OwnerStateCom->Change_Dir(ACTORDIR::LEFT);
    else
        m_OwnerStateCom->Change_Dir(ACTORDIR::RIGHT);
    m_SkillTimer += timeDelta;
    m_ChargeTimer += timeDelta;
    const SkillInfo& skillInfo = m_SkillInfo[SKILL_TRIPLE];

    vMouseDir.z = 0;

    D3DXVec3Normalize(&vMouseDir, &vMouseDir);

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
    return false;

    return true;
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
void        CPlayerSkill::Create_Projectile(const SkillInfo& skillInfo, const _vec3& vSkillDir)
{
    if (!m_OwnerCombatStatCom) return;

    CAttackBuilder(m_GraphicDev, m_Owner)
        .Box()
        .Track(m_CurrentAttackObj)
        .Damage(m_OwnerCombatStatCom->Get_Attack() * skillInfo.damageMultiplier)
        .Knockback(skillInfo.knockbackPower)
        .Scale(_vec3())
        .InstigatorPos()
        .Offset(vSkillDir * skillInfo.spawnOffset)
        .DestroyOnHit(true)
        .Spawn();

    if (m_CurrentAttackObj)
    {
        CAttackBase* pBox = m_CurrentAttackObj;
        m_CurrentAttackObj->Set_OnDestroyCallback([this, pBox]()
            {
                if (m_CurrentAttackObj == pBox)
                    m_CurrentAttackObj = nullptr;
            });
    }
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

    CComponent::Free();
}
