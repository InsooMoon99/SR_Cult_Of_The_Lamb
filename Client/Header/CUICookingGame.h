#pragma once
#include "CUI.h"
#include "Client_Struct.h"


class CUICookingGame :
    public CUI
{
private:
    explicit CUICookingGame(DEVICE graphicDev,CGameObject* owner, RecipeInst recipeInst);
    explicit CUICookingGame(const CUICookingGame& rhs) = delete;
    virtual ~CUICookingGame();

public:
    HRESULT Ready_GameObject() override;
    void    Initialize_GameObject() override;
    _int    Update_GameObject(const _float& timeDelta) override;
    void    LateUpdate_GameObject(const _float& timeDelta) override;
    void    Render_GameObject() override;

public:
    void SetStockMax(int maxNum) { m_StockMax = maxNum; }

private:
    HRESULT Add_Component();

    CGameObject* m_Owner;
    RecipeInst  m_recipeInst;
    CTransform* m_OwnerTransform;

    function

    int m_StockMax = 0;
public:
    static CUICookingGame* Create(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst);

protected:
    void Free() override;
};

