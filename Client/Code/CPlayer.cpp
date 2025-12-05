#include "pch.h"
#include "CPlayer.h"

#include "CAnimator.h"
#include "CAttackBuilder.h"
#include "CAttackSphere.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CDropSystem.h"
#include "CInventory.h"
#include "CPlayerAttack.h"
#include "CPlayerController.h"
#include "CPlayerInput.h"
#include "CPlayerMovement.h"
#include "CPlayerSkill.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CWeaponEquip.h"

CPlayer::CPlayer(DEVICE graphicDev)
    : CCharacterBase(graphicDev),
      m_RigidbodyCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_InventoryCom(nullptr),
      m_PlayerControllerCom(nullptr),
      m_PlayerInputCom(nullptr),
      m_PlayerMovementCom(nullptr),
      m_PlayerAttackCom(nullptr)

{}

CPlayer::CPlayer(const CPlayer& rhs)
    : CCharacterBase(rhs),
      m_RigidbodyCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_InventoryCom(nullptr),
      m_PlayerControllerCom(nullptr),
      m_PlayerInputCom(nullptr),
      m_PlayerMovementCom(nullptr),
      m_PlayerAttackCom(nullptr)
{}

CPlayer::~CPlayer()
{ }

HRESULT CPlayer::Ready_GameObject()
{
    if (FAILED(CCharacterBase::Ready_GameObject()))
        return E_FAIL;

    m_CombatStatCom->Set_Hp(100.f);
    m_CombatStatCom->Set_MaxHp(100.f);
    m_CombatStatCom->Set_Attack(50.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);

    m_TransformCom->Set_Pos(_vec3(20.f, 1.f, 15.f));
    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::PLAYER);

    return S_OK;
}

void CPlayer::Initialize_GameObject()
{
    CCharacterBase::Initialize_GameObject();
}

_int CPlayer::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);

    // 안은수 테스트
    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_BACKSPACE))
        CDropSystem::GetInstance()->SpawnAllFromInven(m_GraphicDev, m_InventoryCom);

    return exit;
}

void CPlayer::LateUpdate_GameObject(const _float& timeDelta)
{
    CCharacterBase::LateUpdate_GameObject(timeDelta);
}

void CPlayer::Render_GameObject()
{
    CCharacterBase::Render_GameObject();
}

void CPlayer::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnBeginOverlap(self, other);
}

void CPlayer::OnEndOverlap(CCollider* self, CCollider* other)
{
    CCharacterBase::OnEndOverlap(self, other);

    //cout << "Player HitOut" << endl;
}

void CPlayer::Render_Editor()
{
    CCharacterBase::Render_Editor();

    ImGui::Text("=== Player ===");
}

HRESULT CPlayer::Add_Component()
{
    // 부모쪽에 Transform, Buffer, Animator, State, CombatStat, BoxCollider 있음
    if (FAILED(CCharacterBase::Add_Component()))
        return E_FAIL;

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_PLAYER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_PLAYER, m_TextureCom });

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_PLAYER);

    // Inventory
    m_InventoryCom = CreateProtoComponent<CInventory>(this, COMPONENTTYPE::INVENTORY);
    NULL_CHECK_RETURN(m_InventoryCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::INVENTORY, m_InventoryCom });

    m_InventoryCom->SetInvenSlotNum(12);

    // Weapon Equip
    m_WeaponEquipCom = CreateProtoComponent<CWeaponEquip>(this, COMPONENTTYPE::WEAPON_EQUIP);
    NULL_CHECK_RETURN(m_WeaponEquipCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::WEAPON_EQUIP, m_WeaponEquipCom });

    // Rigidbody
    m_RigidbodyCom = CreateProtoComponent<CRigidbody>(this, COMPONENTTYPE::RIGIDBODY);
    NULL_CHECK_RETURN(m_RigidbodyCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::RIGIDBODY, m_RigidbodyCom });

    // Player Controller
    m_PlayerControllerCom = CreateProtoComponent<CPlayerController>(this, COMPONENTTYPE::PL_CONTROLLER);
    NULL_CHECK_RETURN(m_PlayerControllerCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::PL_CONTROLLER, m_PlayerControllerCom });

    // Player Input
    m_PlayerInputCom = CreateProtoComponent<CPlayerInput>(this, COMPONENTTYPE::PL_INPUT);
    NULL_CHECK_RETURN(m_PlayerInputCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::PL_INPUT, m_PlayerInputCom });

    // Player Movement
    m_PlayerMovementCom = CreateProtoComponent<CPlayerMovement>(this, COMPONENTTYPE::PL_MOVEMENT);
    NULL_CHECK_RETURN(m_PlayerMovementCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PL_MOVEMENT, m_PlayerMovementCom });

    // Player Attack
    m_PlayerAttackCom = CreateProtoComponent<CPlayerAttack>(this, COMPONENTTYPE::PL_ATTACK);
    NULL_CHECK_RETURN(m_PlayerAttackCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PL_ATTACK, m_PlayerAttackCom });

    // Player Skill
    m_PlayerSkillCom = CreateProtoComponent<CPlayerSkill>(this, COMPONENTTYPE::PL_SKILL);
    NULL_CHECK_RETURN(m_PlayerSkillCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::PL_SKILL, m_PlayerSkillCom });

    m_PlayerControllerCom->Init_OwnerComponent();
    m_PlayerMovementCom->Init_OwnerComponent();
    m_PlayerAttackCom->Init_OwnerComponent();
    m_PlayerSkillCom->Init_OwnerComponent();

    return S_OK;
}

void CPlayer::Animation_Setting()
{
    _float dirX = m_PlayerInputCom->Get_MouseDir().x;
    m_AnimatorCom->Set_FlipX(dirX < 0);

#pragma region ------   IDLE    ------
    m_AnimatorCom->Create_Animation(L"PlayerIdle", 150, 0.02f, _vec2(0.75f, 1.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"PlayerIdle", ANIMSTATE::LOOP);
#pragma endregion

#pragma region ------   RUN     ------
    m_AnimatorCom->Create_Animation(L"PlayerRun_LEFT", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_UP", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_DOWN", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_LUP", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_LDOWN", 19, 0.02f);

    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"PlayerRun", ANIMSTATE::LOOP);
#pragma endregion

#pragma region ------   ROLL    ------
    m_AnimatorCom->Create_Animation(L"PlayerRoll_LEFT",
                                    19,
                                    0.02f,
                                    _vec2(1.3f, 1.3f), // Scale
                                    _vec2(0.f, 0.f));  // Offset

    m_AnimatorCom->Create_Animation(L"PlayerRoll_UP",
                                    16,
                                    0.02f * 19 / 16,
                                    _vec2(0.85f, 0.85f),
                                    _vec2(0.f, 0.f));

    m_AnimatorCom->Create_Animation(L"PlayerRoll_DOWN",
                                    16,
                                    0.02f * 19 / 16,
                                    _vec2(0.85f, 0.85f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_ROLL, L"PlayerRoll", ANIMSTATE::ONCE);
#pragma endregion

#pragma region ------   SWORD    ------
    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo1",
                                    31,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.18f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO1,
                             L"PlayerSwordCombo1",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo2",
                                    38,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.24f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO2,
                             L"PlayerSwordCombo2",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo3",
                                    38,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.49f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO3,
                             L"PlayerSwordCombo3",
                             ANIMSTATE::ONCE);
#pragma endregion

#pragma region ------   Gauntlet   ------
    m_AnimatorCom->Create_Animation(L"PlayerGauntletCombo1",
                                        41,
                                        0.02f,
                                        _vec2(1.8f, 1.8f),
                                        _vec2(0.f, 0.7f));
    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_COMBO1,
                             L"PlayerGauntletCombo1",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerGauntletCombo2",
                                    43,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.6f));
    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_COMBO2,
                             L"PlayerGauntletCombo2",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerGauntletCombo3",
                                    44,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.3f));
    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_COMBO3,
                             L"PlayerGauntletCombo3",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerGauntletCombo4",
                                    40,
                                    0.02f,
                                    _vec2(2.4f, 2.4f),
                                    _vec2(0.f, 0.6f));
    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_COMBO4,
                             L"PlayerGauntletCombo4",
                             ANIMSTATE::ONCE);

    m_AnimatorCom->Create_Animation(L"PlayerGauntletHeavy",
                                    70,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.f));
    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_GAUNTLET_HEAVY,
                                                 L"PlayerGauntletHeavy",
                                                ANIMSTATE::ONCE);

#pragma region -----------Skill-----------


    m_AnimatorCom->Create_Animation(L"PlayerSkillCharging",
                                              17,
                                              0.02f,
                                              _vec2(0.6f, 1.f),
                                              _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SKILL_CHARGING,
                                            L"PlayerSkillCharging",
                                            ANIMSTATE::LOOP);

    m_AnimatorCom->Create_Animation(L"PlayerSkillAttack",
                                             32,
                                             0.02f,
                                             _vec2(1.f, 1.5f),
                                             _vec2(0.f, 0.5f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SKILL_SINGLE,
                                               L"PlayerSkillAttack",
                                               ANIMSTATE::ONCE);

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SKILL_TRIPLE,
                                                L"PlayerSkillAttack",
                                                ANIMSTATE::ONCE);

#pragma endregion
#pragma end
}

CPlayer* CPlayer::Create(DEVICE graphicDev)
{
    auto player = new CPlayer(graphicDev);

    if (FAILED(player->Ready_GameObject()))
    {
        MSG_BOX("Player Create Failed");
        Safe_Release(player);
        return nullptr;
    }

    return player;
}

void CPlayer::Free()
{
    CCharacterBase::Free();
}
