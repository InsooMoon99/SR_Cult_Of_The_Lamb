#pragma once
#include "CUI.h"
#include "Client_Struct.h"
#include <functional>

class CUIIcon;

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


    void SetDoneEvent(function<void()> doneEvent) { m_DoneEvent = doneEvent; }
    void SetResultItemDrop(function<void(int ,bool)> DropDish) { m_ResultItemDrop = DropDish; }

private:
    HRESULT Add_Component();

    CGameObject* m_Owner = nullptr;
    RecipeInst  m_recipeInst;

    CTransform* m_OwnerTransform = nullptr;

    CRcTex* m_Buffer_Deco = nullptr;
    //CRcTex* m_Buffer_Area = nullptr;
    CRcTex* m_Buffer_Info = nullptr;
    CRcTex* m_Buffer_keyInfo = nullptr;

    function<void()> m_DoneEvent;
    function<void(int,bool)> m_ResultItemDrop;

    CUIIcon* m_ItemIcon = nullptr;

    bool m_result = false;
    int m_StockMax = 0;
    _vec3 m_CurserPos;
public:
    static CUICookingGame* Create(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst);

protected:
    void Free() override;
};

