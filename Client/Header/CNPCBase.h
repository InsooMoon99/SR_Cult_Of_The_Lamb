#pragma once
#include "CCharacterBase.h"

class CInteractionChecker;
class CNPCController;

// 모든 추종자 NPC들의 기본 베이스
class CNPCBase : public CCharacterBase
{
protected:
    explicit CNPCBase(DEVICE graphicDev);
    explicit CNPCBase(const CNPCBase& rhs);
    ~CNPCBase() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

protected:
    HRESULT Add_Component() override;
    void    Free() override;

protected:
    CInteractionChecker* m_Checker = nullptr;
    CNPCController*      m_NPCController = nullptr;
};
