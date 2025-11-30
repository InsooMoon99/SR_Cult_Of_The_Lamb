#pragma once

#include "CCharacterBase.h"

class CWeaponEquip;
class CInventory;
class CUIInventory;
class CPlayerController;
class CPlayerInput;
class CPlayerMovement;
class CPlayerAttack;

BEGIN(Engine)
class CRigidbody;
class CTexture;
END

class CPlayer : public CCharacterBase
{
private:
    explicit CPlayer(DEVICE graphicDev);
    explicit CPlayer(const CPlayer& rhs);
    ~CPlayer() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    void Render_Editor() override;

public:
    // 테스트 공격용
    // 나중에 Compoennt를 새로 만들던, 클래스로 나누던 자유
    void Test_Attack();

private:
    HRESULT Add_Component()     override;
    void    Animation_Setting() override;

private:
#pragma region Engine Components
    Engine::CRigidbody*  m_RigidbodyCom;

#pragma endregion
#pragma region Client Components
    CWeaponEquip*      m_WeaponEquipCom;
    CInventory*        m_InventoryCom;
    CUIInventory*      m_UIInventory;
    CPlayerController* m_PlayerControllerCom;
    CPlayerInput*      m_PlayerInputCom;
    CPlayerMovement*   m_PlayerMovementCom;
    CPlayerAttack*     m_PlayerAttackCom;

#pragma endregion

    map<ACTORSTATE, wstring> m_StateAnim;

public:
    static CPlayer* Create(DEVICE graphicDev);

private:
    void Free() override;
};
