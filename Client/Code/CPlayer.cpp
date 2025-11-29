#include "pch.h"
#include "CPlayer.h"
#include "CAnimator.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CInventory.h"
#include "CPlayerController.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CUIInventory.h"
#include "CWeaponEquip.h"
#include "CAttackBuilder.h"

CPlayer::CPlayer(DEVICE graphicDev)
	: CCharacterBase(graphicDev),
	m_RigidbodyCom(nullptr),
	m_WeaponEquipCom(nullptr),
	m_InventoryCom(nullptr),
	m_PlayerControllerCom(nullptr),
	m_UIInventory(nullptr)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacterBase(rhs),
	m_RigidbodyCom(nullptr),
	m_WeaponEquipCom(nullptr),
	m_InventoryCom(nullptr),
	m_PlayerControllerCom(nullptr),
	m_UIInventory(nullptr)
{
}

CPlayer::~CPlayer()
{ }

HRESULT CPlayer::Ready_GameObject()
{
	// AddCompnoent가 아닌 부모의 Ready를 호출해야한다. 부모가 알아서 AddComponent 실행함
	if (FAILED(CCharacterBase::Ready_GameObject()))
		return E_FAIL;

    m_CombatStatCom->Set_Hp(6.f);
    m_CombatStatCom->Set_MaxHp(100.f);
    m_CombatStatCom->Set_Attack(10.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 1.f));
    m_TransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

    //!! Rigidbody에서 중력만 사용 안함
    m_RigidbodyCom->Set_UseAccel(true);
    m_RigidbodyCom->Set_UseFriction(true);
    m_RigidbodyCom->Set_UseGravity(false);

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& timeDelta)
{
    _int exit = CCharacterBase::Update_GameObject(timeDelta);


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

void CPlayer::Test_Attack()
{
    // 사용 예시
    CAttackBuilder(m_GraphicDev, this)
        .Sphere()
        .Damage(12.f)
        .Knockback(50.f)
        .Scale(5.f)
        .InstigatorPos()
        .Offset(_vec3(2.f, 0.f, 0.f))
        .DestroyOnHit(true)
        .Spawn();
}

void CPlayer::OnBeginOverlap(CCollider* self, CCollider* other)
{
	CCharacterBase::OnBeginOverlap(self, other);

    //cout << "Player Hit" << endl;
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

    // 이제 ImGui는 GameObject의 OnEditor를 상속받아서 Render하면 된다. 따로 호출해줄 필요도 없음.
    // 추가로 띄우고 싶은거 있으면 추가하기
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
    m_PlayerControllerCom = CreateProtoComponent<CPlayerController>(this, COMPONENTTYPE::CONTROLLER_PLAYER);
    NULL_CHECK_RETURN(m_PlayerControllerCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::CONTROLLER_PLAYER, m_PlayerControllerCom });

    m_PlayerControllerCom->Init_OwnerComponent();

    return S_OK;
}

void CPlayer::Animation_Setting()
{
#pragma region ------   IDLE    ------
    m_AnimatorCom->Create_Animation(L"PlayerIdle", 150, 0.02f);

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
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO1,
                             L"PlayerSwordCombo1",
                             ANIMSTATE::ONCE);


    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo2",
                                    38,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO2,
                             L"PlayerSwordCombo2",
                             ANIMSTATE::ONCE);


    m_AnimatorCom->Create_Animation(L"PlayerSwordCombo3",
                                    38,
                                    0.02f,
                                    _vec2(2.2f, 2.2f),
                                    _vec2(0.f, 0.f));

    m_StateCom->Set_AnimInfo(ACTORSTATE::PLAYER_SWORD_COMBO3,
                             L"PlayerSwordCombo3",
                             ANIMSTATE::ONCE);
#pragma endregion
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
