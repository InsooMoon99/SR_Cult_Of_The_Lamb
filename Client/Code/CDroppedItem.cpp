#include"pch.h"
#include"CDroppedItem.h"
#include "CState.h"
#include "CLayer.h"
#include "CBoxCollider.h"
#include "CComponent.h"
#include "CCreateHelper.h"
#include "CDropSystem.h"
#include "CInventory.h"
#include "CItemDB.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CRcTex.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CWeaponEquip.h"
#include "ItemData.h"
#include "CSpriteEff.h"
#include "CUIWeaponExplain.h"
#include "CDInputMgr.h"
#include "CAnimator.h"

CUIWeaponExplain* CDroppedItem::m_WeaponExplainUI = nullptr;

CDroppedItem::CDroppedItem(DEVICE graphicDev, ItemInstance itemInst)
    : CGameObject(graphicDev),
      m_BufferCom(nullptr),
      m_TransformCom(nullptr),
      m_TextureCom(nullptr),
      m_BoxColCom(nullptr),
      m_itemInst(itemInst)

{ }

CDroppedItem::CDroppedItem(const CDroppedItem& rhs)
    : CGameObject(rhs),
      m_BufferCom(rhs.m_BufferCom),
      m_TransformCom(rhs.m_TransformCom),
      m_TextureCom(rhs.m_TextureCom),
      m_BoxColCom(rhs.m_BoxColCom),
      m_itemInst(rhs.m_itemInst)

{ }

CDroppedItem::~CDroppedItem()
{ }

HRESULT CDroppedItem::Ready_GameObject()
{
    // 이건 오버라이드 용으로 만들어 논거임 의미 없을듯
    return S_OK;
}

HRESULT CDroppedItem::Ready_GameObject(_vec3 pos)
{
    m_index = CItemDB::GetInstance()->GetIndexById(m_itemInst.itemId);

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_TransformCom->Set_Pos(pos);
    m_TransformCom->Set_Scale({ 0.4f, 0.4f, 0.4f });
    m_BoxColCom->Set_Size(_vec3(2.f, 2.f, 2.f));
    m_Active = false;

    return S_OK;
}

void CDroppedItem::Initialize_GameObject()
{
    CGameObject::Initialize_GameObject();

    if (m_BoxColCom)
        m_BoxColCom->Set_CollisionLayer(COLLISION_LAYERTYPE::ITEM);

   // if (!m_WeaponExplainUI)
   // {
        m_WeaponExplainUI = CUIWeaponExplain::Create(m_GraphicDev);
        m_WeaponExplainUI->Initialize_GameObject();
        CManagement::GetInstance()->Get_Scene()->Get_Layer(LAYERTYPE::UI)->Add_GameObject(OBJTYPE::UI, m_WeaponExplainUI);
   // }
}

_int CDroppedItem::Update_GameObject(const _float& timeDelta)
{
    _int exit = CGameObject::Update_GameObject(timeDelta);

    if (is_SetBounceDesc == true)
    {
        m_Active = true;
        m_SpriteEff->InitBounce(m_BounceDesc); // 끝나면 이 안에서 active fasle
        is_SetBounceDesc = false;
    }

    if (m_Active == true)
        m_SpriteEff->Update_Component(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return exit;
}

void CDroppedItem::LateUpdate_GameObject(const _float& timeDelta)
{
    CGameObject::LateUpdate_GameObject(timeDelta);
}

void CDroppedItem::Render_GameObject()
{
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_GraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);

    //
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    m_TextureCom->Set_Texture(m_index);

    m_BufferCom->Render_Buffer();

    //

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    m_GraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    //

    m_BoxColCom->Render(); // Render Reset 이후 호출해야함
}

void CDroppedItem::OnBeginOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnBeginOverlap(self, other);
}

void CDroppedItem::OnStayOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnStayOverlap(self, other);

    CPlayer* ply = dynamic_cast<CPlayer*>(other->Get_Owner());
    if (ply != nullptr)
    {
        //CDropSystem::GetInstance()->

        if (CItemDB::GetInstance()->GetItemById(m_itemInst.itemId)->type == ItemType::Weapon)
        {
            //*** 아이템 지우기 ***

            //1. 무기 정보 UI 띄우기
            switch (m_itemInst.itemId)
            {
            case 3001:
            {
                m_WeaponExplainUI->Set_ItemID(3001);
                break;
            }
            case 3002:
            {
                m_WeaponExplainUI->Set_ItemID(3002);
                break;
            }
            }
            m_WeaponExplainUI->Set_Active(true);

            // ImGui 클릭 시 무시
            //ImGuiIO& io = ImGui::GetIO();
            //if (io.WantCaptureKeyboard)
            //    return;

            if (CDInputMgr::GetInstance()->KeyDown(DIK_E))
            {
                dynamic_cast<CWeaponEquip*>(ply->Get_Component(COMPONENTID::ID_STATIC, COMPONENTTYPE::WEAPON_EQUIP))->Equip_Weapon(m_itemInst);

                auto state = dynamic_cast<CState*>(ply->Get_Component(ID_DYNAMIC, COMPONENTTYPE::STATE));
                //auto animator = dynamic_cast<CAnimator*>(ply->Get_Component(ID_DYNAMIC, COMPONENTTYPE::ANIMATOR));
                //
                if (state)
                    state->Change_State(ACTORSTATE::PLAYER_WEAPONGET);

                //if (animator->Is_AnimationFinished())
                //    state->Change_State(ACTORSTATE::IDLE);

                Mark_As_PendingKill();
                m_WeaponExplainUI->Set_Active(false);
            }
        }
        else
        {
            CComponent* com = ply->Get_Component(COMPONENTID::ID_STATIC, COMPONENTTYPE::INVENTORY);
            if (dynamic_cast<CInventory*>(com)->AddItem(m_itemInst) == true)
            {
                //*** 아이템 지우기 ***
                Mark_As_PendingKill();
            }
        }
    }

    //cout << "item Hit" << endl;
}

void CDroppedItem::OnEndOverlap(CCollider* self, CCollider* other)
{
    CGameObject::OnEndOverlap(self, other);

    if (m_WeaponExplainUI)
        m_WeaponExplainUI->Set_Active(false);
}

HRESULT CDroppedItem::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_ITEM);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);

    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_ITEM, m_TextureCom });

    // Box Col
    m_BoxColCom = CreateProtoComponent<CBoxCollider>(this, COMPONENTTYPE::BOX_COLL);
    NULL_CHECK_RETURN(m_BoxColCom, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::BOX_COLL, m_BoxColCom });

    // effect
    m_SpriteEff = CreateProtoComponent<CSpriteEff>(this, COMPONENTTYPE::SPRITEEFFECT);
    NULL_CHECK_RETURN(m_SpriteEff, E_FAIL);

    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::SPRITEEFFECT, m_SpriteEff });

    return S_OK;
}

CDroppedItem* CDroppedItem::Create(DEVICE graphicDev, ItemInstance itemInst, _vec3 pos)
{
    auto DroppedItem = new CDroppedItem(graphicDev, itemInst);

    if (FAILED(DroppedItem->Ready_GameObject(pos)))
    {
        MSG_BOX("DroppedItem Create Failed");
        Safe_Release(DroppedItem);
        return nullptr;
    }

    return DroppedItem;
}

void CDroppedItem::Free()
{
    CDropSystem::GetInstance()->Remove_Item(this);

    CGameObject::Free();
}
