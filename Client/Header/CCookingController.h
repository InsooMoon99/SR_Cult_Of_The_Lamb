#pragma once
#include "CComponent.h"
#include <queue>

class CUIButton;
class CInventory;

struct Recipe
{

};



class CCookingController :
    public CComponent
{
private:
    explicit CCookingController(DEVICE graphicDev);

    explicit CCookingController(const CCookingController& rhs);
    ~CCookingController() override;

public:
    virtual HRESULT Ready_Inventory();
    void    Initialize_Component() override;
    _int    Update_Component(const _float& timeDelta) override;
    void    LateUpdate_Component() override;

    //UI
    void SetUIRecipeSlot(/*Slot* slot, */int id, CUIButton* quit, CUIButton* startCook); //데이터 설정(itemId) + 콜백 연결(onClick)만 해주는 초기화 함수


    bool CanCooK(int recipeID, CInventory* playerInven);// 이게 맞나..///
    bool AddRecipe(int id);

    bool RemoveRecipe(int slotNum);// 이건 해야할까 말아야 할까


    //미니 게임
    bool ReloadForMiniGame();
    void DropDish(int dishID, bool quality);

    bool m_EnterFlag = false;
    bool m_bStart = false;
   // bool m_OnGame = false;
    bool m_bDone = false;

    //const vector<InventorySlot>& GetVector() const { return m_vecInven; }


public:
    static CCookingController* Create(DEVICE graphicDev);
    CComponent* Clone() override;
private:
    vector<Recipe*> RecipeDB;
    // 선택후
    vector<int> m_vecTmpRecipeID;
    queue<RecipeInst> m_queRecipeInst; // 주소로 안하는게 낫데 메모리 관리 신경 쓸 필요 없음

    int invenSlotNum = 12;
    int m_StockMax = 0;

public:
    void Free() override;

};

