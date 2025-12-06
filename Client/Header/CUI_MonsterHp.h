#pragma once

#include "CUI.h"

BEGIN(Engine)
class CCombatStat;
END

class CUI_MonsterHp : public CUI
{
private:
    explicit CUI_MonsterHp(DEVICE graphicDev, CGameObject* owner);

    // 특정 몬스터한테 1:1로 매칭될 거기 때문에 복사 생성자 필요 ㄴㄴ
    explicit CUI_MonsterHp(const CUI_MonsterHp& rhs) = delete;
    virtual ~CUI_MonsterHp();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

    void    UpdateHPBarUV(_float hpRatio);

private:
    HRESULT Add_Component();
    void    Update_Billboard();  // 빌보드 계산

public:
    static CUI_MonsterHp* Create(DEVICE graphicDev, CGameObject* owner);

protected:
    void Free() override;

private:
    CGameObject*    m_Owner;

    CCombatStat*    m_OwnerCombatStat;
    CTransform*     m_OwnerTransform;

    _float          m_HpRatio;    
    _float          m_YOffset;    
    _vec3           m_BarSize;    

    _matrix         m_BillboardMatrix;  
};

