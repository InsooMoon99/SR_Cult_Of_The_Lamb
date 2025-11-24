#include "pch.h"
#include "CPlayer.h"

#include "CAnimator.h"
#include "CBoxCollider.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CDInputMgr.h"
#include "CEnumHelper.h"
#include "CInventory.h"
#include "CPlayerController.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CRigidbody.h"
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CWeaponEquip.h"
#include "CAttackBox.h"
#include "CManagement.h"
#include "CScene.h"

CPlayer::CPlayer(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr),
      m_BoxColCom(nullptr),
      m_StateCom(nullptr),
      m_CombatStatCom(nullptr),
      m_RigidbodyCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_InventoryCom(nullptr),
      m_PlayerControllerCom(nullptr)
{}

CPlayer::CPlayer(const CPlayer& rhs)
    : CGameObject(rhs),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr),
      m_BoxColCom(nullptr),
      m_StateCom(nullptr),
      m_CombatStatCom(nullptr),
      m_RigidbodyCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_InventoryCom(nullptr),
      m_PlayerControllerCom(nullptr)
{}

CPlayer::~CPlayer()
{ }

HRESULT CPlayer::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // 플레이어 상태 초기값
    m_StateCom->Change_State(ACTORSTATE::IDLE);
    m_StateCom->Change_Dir(ACTORDIR::LEFT);

    m_CombatStatCom->Set_Hp(6.f);
    m_CombatStatCom->Set_MaxHp(100.f);
    m_CombatStatCom->Set_Attack(10.f);
    m_CombatStatCom->Set_Mp(5.f);
    m_CombatStatCom->Set_Speed(5.f);

    m_TransformCom->Set_Pos(_vec3(0.f, 0.f, 1.f));

    Animation_Setting();

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& timeDelta)
{
#pragma region 안은수 테스트

    auto inputMgr = CDInputMgr::GetInstance();

    if (inputMgr->Get_DIKeyState(DIK_1) & 0x80)
    {
        m_InventoryCom->DropItemfromSlot(
            1,
            {
                m_TransformCom->Get_Pos().x,
                m_TransformCom->Get_Pos().y,
                m_TransformCom->Get_Pos().z + 5.f
            });
    }

#pragma endregion

    _int exit = CGameObject::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);



    if (CDInputMgr::GetInstance()->KeyDown(DIK_5))
    {
        Test_Attack();
    }

    return exit;
}

void CPlayer::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CPlayer::Render_GameObject()
{
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    Render_Setting();

    if (m_TextureCom && m_AnimatorCom)
    {
        const wstring& key   = m_AnimatorCom->Get_CurKey();
        _int           frame = m_AnimatorCom->Get_CurFrame();

        m_TextureCom->Set_Texture(key, frame);
    }

    m_BufferCom->Render_Buffer();

    Render_Reset();

    m_BoxColCom->Render(); // Render Reset 이후 호출해야함
}

void CPlayer::Render_Setting()
{
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
}

void CPlayer::Render_Reset()
{
    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CPlayer::Test_Attack()
{
    // 1. 공격 박스 생성
    auto obj = CAttackBox::Create(m_GraphicDev);
    obj->Set_Instigator(this);

    // 2. 구조체 데이터 세팅
    HITDATA hit{};
    hit.damage = 12;
    hit.knockback = 50.f;
    obj->Set_HitDesc(hit);

    // 3. 공격 박스 위치 설정, 방향값 추가 가능
    _vec3 pos = m_TransformCom->Get_Pos();

    auto transform = dynamic_cast<CTransform*>(
        obj->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));

    transform->Set_Pos(pos);

    // 4. 레이어에 추가, 헬퍼 함수 SpawnAttackObject 사용
    //CManagement::GetInstance()->Get_Scene()->AddObjectOnLayer(LAYERTYPE::GAMELOGIC, obj, OBJTYPE::ATTACK_COL);
    SpawnAttackObject(obj);
}

void CPlayer::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);

    cout << "Player Hit" << endl;
}

void CPlayer::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);

    cout << "Player HitOut" << endl;
}

void CPlayer::Render_Editor()
{
    CGameObject::Render_Editor();

    ImGui::Text("=== Player ===");

    // 이제 ImGui는 GameObject의 OnEditor를 상속받아서 Render하면 된다. 따로 호출해줄 필요도 없음.
    // 추가로 띄우고 싶은거 있으면 추가하기
}

HRESULT CPlayer::Add_Component()
{
    // buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEXTURE, L"Player");
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEXTURE, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    m_AnimatorCom->Ready_Animator();

    // RectCol Componet
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    // Stat
    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    // inventory
    m_InventoryCom = CreateProtoComponent<CInventory>(this, COMPONENTTYPE::INVENTORY);
    NULL_CHECK_RETURN(m_InventoryCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::INVENTORY, m_InventoryCom });

    m_InventoryCom->SetInvenSlotNum(12);

    // weaponequip
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
    m_AnimatorCom->Create_Animation(L"PlayerRoll_LEFT", 19, 0.02f,
        _vec2(1.3f, 1.3f),       // Scale
        _vec2(0.f, 0.f));  // Offset

    m_AnimatorCom->Create_Animation(L"PlayerRoll_UP", 16, 0.02f * 19 / 16);
    m_AnimatorCom->Create_Animation(L"PlayerRoll_DOWN", 16, 0.02f * 19 / 16);

    m_StateCom->Set_AnimInfo(ACTORSTATE::ROLL, L"PlayerRoll", ANIMSTATE::ONCE);
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
    CGameObject::Free();
}
