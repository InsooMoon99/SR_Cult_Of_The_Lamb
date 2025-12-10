#pragma once
#include "CBuildingBase.h"

/// @brief 요리용 화덕 오브젝트
/// @details 플레이어와 충돌 중 E키 상호작용으로 요리 UI 띄울 예정
///


class CCookingController;

class CKitchen : public CBuildingBase
{
private:
    explicit CKitchen(DEVICE graphicDev);
    explicit CKitchen(const CKitchen& rhs);
    ~CKitchen() override;

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void Render_Editor() override;

public:
    void OnBeginOverlap(CCollider* self, CCollider* other) override;
    void OnStayOverlap(CCollider* self, CCollider* other) override;
    void OnEndOverlap(CCollider* self, CCollider* other) override;

private:
    HRESULT Add_Component() override;
    CCookingController* m_CookingCtrlCom;

public:
    static CKitchen* Create(DEVICE graphicDev);

private:
    void Free() override;
};
