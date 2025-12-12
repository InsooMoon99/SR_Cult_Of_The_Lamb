#include "pch.h"
#include "CCookingController.h"
#include "CUIButton.h"
#include "CDropSystem.h"
#include "ItemInstance.h"
#include "CTransform.h"
#include "CItemDB.h"
#include "CRandomMgr.h"
#include "CDroppedItem.h"
#include "CUICookingGame.h"
#include "CCreateHelper.h"
#include "CInventory.h"
#include "CRecipeDB.h"
#include "CDInputMgr.h"

CCookingController::CCookingController(DEVICE graphicDev) : CComponent(graphicDev)
{
    m_vecTmpRecipeID.reserve(invenSlotNum); // 메모리 공간 확보용 
}

CCookingController::CCookingController(const CCookingController& rhs) : CComponent(rhs)
{
}

CCookingController::~CCookingController()
{
}

HRESULT CCookingController::Ready_Inventory()
{

    return S_OK;
}

void CCookingController::Initialize_Component()
{
    CComponent::Initialize_Component();
}

_int CCookingController::Update_Component(const _float& timeDelta)
{
    _int exit = CComponent::Update_Component(timeDelta);


    //// 테스트
    //if (CDInputMgr::GetInstance()->KeyDown(DIK_U))
    //{
    //    m_bStart = true;
    //}
    //else
    //{
    //    m_bStart = false;
    //}

    if (m_bStart)
    {
        if (!m_EnterFlag)
        {
            // 초기 셋팅
            ReloadForMiniGame();
            m_StockMax = m_queRecipeInst.front().StockNum; // 첫 슬롯의 최대 수량 저장
            m_EnterFlag = true;
        }


        if (m_queRecipeInst.empty())
            return -1;// 이건 문제 있음


        CUICookingGame* miniGame = CUICookingGame::Create(m_GraphicDev, m_Owner, m_queRecipeInst.front());//얇은 참조(Pointer)만 가지고 있는 것
        if (miniGame)
        {
            SpawnUIOnLayer(miniGame, OBJTYPE::UI);

            miniGame->Initialize_GameObject();
            miniGame->SetStockMax(m_StockMax);

            CInventory* inven = dynamic_cast<CInventory*>(m_Owner->Get_Component(ID_STATIC, COMPONENTTYPE::INVENTORY));

            miniGame->SetDoneEvent([this]() {m_bDone = true; });
            miniGame->SetResultItemDrop([this](int _dishID, bool _quality) {DropDish(_dishID, _quality); });
        }
        m_queRecipeInst.front().StockNum--;

        if (m_queRecipeInst.front().StockNum == 0)
        {
            m_queRecipeInst.pop();

            if (!m_queRecipeInst.empty())
                m_StockMax = m_queRecipeInst.front().StockNum; // 다음 슬롯의 최대 수량 저장
        }

        m_bStart = false;
    }

    if (m_bDone)
    {
        if (!m_queRecipeInst.empty())
        {

            m_bStart = true;
            m_bDone = false;
        }
        else
        {
            // 이건 진짜 끝난거 초기화
            m_EnterFlag = false;
            m_bStart = false;
            m_bDone = false;

           

        }

    }

    return exit;
}

void CCookingController::LateUpdate_Component()
{
    CComponent::LateUpdate_Component();

}

void CCookingController::SetUIRecipeSlot(/*Slot* slot,*/ int id, CUIButton* quit, CUIButton* startCook)
{
    // 콜백 셋팅 이건 ui랑 컨트롤러 둘다 알고 있는 씬이나 ui매니져 등에서 넣기 
    //slot->SetCallBack([]() {});// AddRecipe () 이거 안에 넣기
    // 버튼은
    //quit->SetQuitCallBack // free
    //startCook->SetCookCallBack
}

bool CCookingController::CanCook(int recipeID, CInventory* playerInven)
{

    Recipe* recipe = CRecipeDB::GetInstance()->GetRecipeById(recipeID);
    for (auto& [ID, Num] : recipe->ingredients)
    {
        auto Playeriter = playerInven->GetCountMap().find(ID);
        if (Playeriter == playerInven->GetCountMap().end())
            return false;

        if (playerInven->GetCountMap().at(ID) < Num)
            return false;
    }
    //playerInven->Get_Cnt
    return true;
}

bool CCookingController::AddRecipe(int id, CInventory* playerInven)
{
    if (m_vecTmpRecipeID.size() < invenSlotNum)
    {
        // 대기열 등록
        m_vecTmpRecipeID.push_back(id);

        //픟레이어 인벤에서 삭제
        Recipe* recipe = CRecipeDB::GetInstance()->GetRecipeById(id);
        for (auto& [ItemID, Num] : recipe->ingredients)
        {
            playerInven->RemoveItem(ItemID, Num);
        }

        return true;
    }

    return false;
}

bool CCookingController::RemoveRecipe(int slotNum)
{
    for (auto i = m_vecTmpRecipeID.begin(); i < m_vecTmpRecipeID.end();)
    {
        if ((*i) == slotNum)
        {
            (*i) = 999;

            i = m_vecTmpRecipeID.erase(i);
            return true;// 슬롯의 위치를 받아 지우는 거라 바로 나가도 ㄱㅊ(겹치는 거 없음)
        }
        else
        {
            i++;
        }
    }
    return false;
}

bool CCookingController::ReloadForMiniGame()
{
    unordered_map<int, RecipeInst> tmpMap;// 포인터로 하면 new 해 줘야해서 그냥 객체로 ㄱㄱ
    for (auto& id : m_vecTmpRecipeID)
    {

        int i = id;
        Recipe* r = CRecipeDB::GetInstance()->GetRecipeById(i);
        if (tmpMap.count(i) == 0)
        {
            RecipeInst inst;
            inst.RecipeID = id;
            inst.StockNum = 1;
            inst.ResultItemID = r->result; //여기에 결과물

            tmpMap[i] = inst;
        }
        else
        {
            tmpMap[i].StockNum++;
        }

    }

    for (auto& it : tmpMap)
    {
        m_queRecipeInst.push(it.second);
    }

    m_vecTmpRecipeID.clear(); //대기열 삭제
    return true;
}

void CCookingController::DropDish(int ResultItemID, bool quality)
{

    if (CItemDB::GetInstance()->GetIndexById(ResultItemID)); // 유효성 검사 함수 내부에 assert

    ItemInstance inst{ ResultItemID,-1,0 };


    int RandOffSet = CRandomMgr::GetInstance()->Get_Int_Random(0,360); // y 축 회전
    _vec3 tmpvec = { cos(D3DXToRadian(RandOffSet)),1,sin(D3DXToRadian(RandOffSet)) };
    D3DXVec3Normalize(&tmpvec, &tmpvec);

    SpriteBounceDesc b;
    b.pos = m_Owner->Get_Transform()->Get_Pos();

    b.velocity = tmpvec * 30;
    b.groundY = .5f;
    // 중력
    b.gravity = -300.f;
    b.restitution = 0.8f;// 튕김 계수
    b.damping = 0.7f;// 충돌 감쇠(마찰)
    b.m_DampingPerSecond = 0.9f;// 공기 저항


    // 인자 - DEVICE pGraphicDev, const ItemInstance& item, const _vec3& worldPos,std::optional<_vec3> scale, std::optional<SpriteBounceDesc> bounceDesc
   CDroppedItem* tmpItem =  CDropSystem::GetInstance()->SpawnDrop(m_GraphicDev, inst, m_Owner->Get_Transform()->Get_Pos(), _vec3(0.5f, 0.5f, 0.5f), b);

   if (quality == true)
       tmpItem->SetFoodQuality(CDroppedItem::FOODQUALITY::GOOD);
   else if(quality == false)
       tmpItem->SetFoodQuality(CDroppedItem::FOODQUALITY::BAD);


}
void CCookingController::Quit()
{
    m_vecTmpRecipeID.clear(); // int 라서 그냥 클리어 해도 되는듯

    if (!m_queRecipeInst.empty())
    {
        m_queRecipeInst.pop();// 이건 안에 new 한거 아니라서 따로지우지는 않고  clear 없으니까 pop을 하나하나 지워야힘
    }
}

CCookingController* CCookingController::Create(DEVICE graphicDev)
{
    auto CookingController = new CCookingController(graphicDev);

    if (FAILED(CookingController->Ready_Inventory()))
    {
        MSG_BOX("CookingController Create Failed");
        Safe_Release(CookingController);
        return nullptr;
    }

    return CookingController;
}

CComponent* CCookingController::Clone()
{
    return new CCookingController(*this);
}

void CCookingController::Free()
{
    CComponent::Free();

    Quit();

}
