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
#include "CState.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CWeaponEquip.h"

CPlayer::CPlayer(DEVICE graphicDev)
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_AnimatorCom(nullptr),
      m_BoxColCom(nullptr),
      m_StateCom(nullptr),
      m_CombatStatCom(nullptr),
      m_PlayerControllerCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_Inventory(nullptr)
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
      m_PlayerControllerCom(nullptr),
      m_WeaponEquipCom(nullptr),
      m_Inventory(nullptr)
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

    // Transform 테스트
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
        m_Inventory->DropItemfromSlot(
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_PLAYER);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_PLAYER, m_TextureCom });

    // Animator
    m_AnimatorCom = CreateProtoComponent<CAnimator>(this, COMPONENTTYPE::ANIMATOR);
    NULL_CHECK_RETURN(m_AnimatorCom, E_FAIL);

    m_AnimatorCom->Set_TextureType(COMPONENTTYPE::TEX_PLAYER);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::ANIMATOR, m_AnimatorCom });

    // RectCol Componet
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);
    //m_BoxColCom->Set_Size(_vec3(2.f, 2.f, 2.f));

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    // Stat
    m_StateCom = CreateProtoComponent<CState>(this, COMPONENTTYPE::STATE);
    NULL_CHECK_RETURN(m_StateCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::STATE, m_StateCom });

    // CombatStat
    m_CombatStatCom = CreateProtoComponent<CCombatStat>(this, COMPONENTTYPE::COMBATSTAT);
    NULL_CHECK_RETURN(m_CombatStatCom, E_FAIL);

    // TODO 인수) CombatStat 컴포넌트에서 Update쓸거면 Dynamic으로 변경하기
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::COMBATSTAT, m_CombatStatCom });

    // inventory
    m_Inventory = CreateProtoComponent<CInventory>(this, COMPONENTTYPE::INVENTORY);
    NULL_CHECK_RETURN(m_Inventory, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::INVENTORY, m_Inventory });
    m_Inventory->SetInvenSlotNum(12);

    // Player Controller
    m_PlayerControllerCom =
        CreateProtoComponent<CPlayerController>(this, COMPONENTTYPE::CONTROLLER_PLAYER);
    NULL_CHECK_RETURN(m_PlayerControllerCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert(
        { COMPONENTTYPE::CONTROLLER_PLAYER, m_PlayerControllerCom });

    // weaponequip
    m_WeaponEquipCom = CreateProtoComponent<CWeaponEquip>(this, COMPONENTTYPE::WEAPON_EQUIP);
    NULL_CHECK_RETURN(m_WeaponEquipCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::WEAPON_EQUIP, m_WeaponEquipCom });

    return S_OK;
}

void CPlayer::Animation_Setting()
{
    // 애니메이션 생성
    // 여기서는 CLoading에서 만들어둔 텍스처 이름대로 가져온다
    m_AnimatorCom->Create_Animation(L"PlayerIdle", 150, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_LEFT", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_UP", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_DOWN", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_LUP", 19, 0.02f);
    m_AnimatorCom->Create_Animation(L"PlayerRun_LDOWN", 19, 0.02f);

    // State -> Animation 연동
    m_StateCom->Set_AnimInfo(ACTORSTATE::IDLE, L"PlayerIdle", ANIMSTATE::LOOP);
    // CState쪽에는 PlayerRun이라는 앞쪽 이름(상태)만 적어주면,
    // 자동으로 _LEFT 등 뒤쪽에 적어놓은 이름(방향)을 찾아준다
    m_StateCom->Set_AnimInfo(ACTORSTATE::RUN, L"PlayerRun", ANIMSTATE::LOOP);
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
