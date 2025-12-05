#pragma once
#include "CRenderObject.h"

namespace Engine
{
    class CAnimator;
    class CState;
    class CCombatStat;
    class CBoxCollider;

}
    class CInventory;

class CWorldInteractive :
    public CRenderObject
{
    explicit CWorldInteractive(DEVICE graphicDev);
    explicit CWorldInteractive(const CWorldInteractive& rhs);
    virtual ~CWorldInteractive();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

protected:
    virtual HRESULT Add_Component();

    void    OnBeginOverlap(CCollider* self, CCollider* other) override;
    void    OnEndOverlap(CCollider* self, CCollider* other) override;

    bool Is_Hovering();
protected:
    //m_TransformCom, m_BufferCom, m_TextureCom 부모에 있음, 텍스쳐 만 자식에서 추가 설정해줘야 함
    Engine::CAnimator* m_AnimatorCom;
    Engine::CState* m_StateCom;
    Engine::CCombatStat* m_CombatStatCom;
    Engine::CBoxCollider* m_BoxColCom;

    CInventory* m_InventoryCom;

private:
    // 상태용
    bool m_IsPlayerOverlap = false;
    //bool m_Is_Hovering = false;
protected:
    void Free() override;
};

