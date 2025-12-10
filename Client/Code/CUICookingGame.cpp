#include "pch.h"
#include "CUICookingGame.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CRcTex.h"
#include "CUIIcon.h"
#include "CItemDB.h"
#include "CFontMgr.h"



CUICookingGame::CUICookingGame(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst)
    : CUI(graphicDev),m_Owner(owner), m_recipeInst(recipeInst)
{
}

CUICookingGame::~CUICookingGame()
{
}

HRESULT CUICookingGame::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;



    m_TransformCom->Set_RotationAxis(ROT_X, D3DXToRadian(45.f));
    m_TransformCom->Set_Pos(m_Owner->Get_Transform()->Get_Pos().x, m_Owner->Get_Transform()->Get_Pos().y+2, m_Owner->Get_Transform()->Get_Pos().z);

    return S_OK;
}

void CUICookingGame::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    if (m_Owner)
    {
        m_OwnerTransform = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(COMPONENTID::ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    }

    m_TransformCom->Set_Pos({ m_OwnerTransform->Get_Pos().x ,  m_OwnerTransform->Get_Pos().y + 1.f,  m_OwnerTransform->Get_Pos().z });


    //// 결과물 이미지
    //m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM);
    //if (m_ItemIcon)
    //{
    //    SpawnUIOnLayer(m_ItemIcon, OBJTYPE::UI);
    //}

    //// 움직이는 화살표
    //m_ItemIcon = CUIIcon::Create(m_GraphicDev, COMPONENTTYPE::TEX_ITEM);
    //if (m_ItemIcon)
    //{
    //    SpawnUIOnLayer(m_ItemIcon, OBJTYPE::UI);
    //}

//영역 위치 랜덤으로 

}

_int CUICookingGame::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    //커서포즈 계속 왓다 갓다

    if (CDInputMgr::GetInstance()->KeyDown(DIK_P))
    {
        //커서포즈 멈춤
        
        //위치가 에리어 안이라면
        m_DoneEvent();
        m_ResultItemDrop(m_recipeInst.ResultItemID,m_result);
        // 자신 지우기
    }
    

    if (!m_Owner)
        return exit;

    return exit;
}

void CUICookingGame::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);
}

void CUICookingGame::Render_GameObject()
{
    CUI::Render_GameObject();



    //빨간색 
    const _matrix& world = m_TransformCom->Get_World();

    {
        const float BG_WIDTH = 1.0f;
        const float BG_HEIGHT = 1.0f;

        m_BufferCom->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_BufferCom->Set_Offset(0.f, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(4);  // 레드
        m_BufferCom->Render_Buffer();
    }

    //메인 바 데코 

    {
        const float BG_WIDTH = 2.0f;
        const float BG_HEIGHT = 1.0f;

        m_Buffer_Deco->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_Buffer_Deco->Set_Offset(0.f, 0.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(2);  // 데코
        m_Buffer_Deco->Render_Buffer();
    }


    //배경 


    {
        const float BG_WIDTH = 2.0f;
        const float BG_HEIGHT = 1.0f;

        m_Buffer_Info->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_Buffer_Info->Set_Offset(0.f, 1.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(5);  // 위 검은 텍스처
        m_Buffer_Info->Render_Buffer();

        //// 배경 안 음식 아이콘
        //m_ItemIcon->Set_TextureIndex(CItemDB::GetInstance()->GetIndexById(m_recipeInst.ResultItemID));
        //m_ItemIcon->SetCloneTexAspectScale({ .2f, .2f, 1 });
        //m_ItemIcon->Get_Transform()->Set_Pos(-WINCX / 3, WINCY / 4.f, 1);
        //
        //_vec3 center = m_Buffer_Info->Get_Transform()->Get_Pos();
        //_vec2 rectLT{ center.x - WINCX * .5f, WINCY * 0.5f - center.y };

        //wchar_t buffer[32];
        //swprintf_s(buffer, L"%d/%d", m_recipeInst.StockNum, m_StockMax);

        //CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
        //    buffer,
        //    &rectLT,
        //    D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }


    //배경 

    {
        const float BG_WIDTH = 2.0f;
        const float BG_HEIGHT = 1.0f;

        m_Buffer_keyInfo->Set_Scale(BG_WIDTH, BG_HEIGHT);
        m_Buffer_keyInfo->Set_Offset(0.f, -1.f);

        m_GraphicDev->SetTransform(D3DTS_WORLD, &world);
        m_TextureCom->Set_Texture(5);  // 아래 키 텍스처
        m_Buffer_keyInfo->Render_Buffer();

        _vec3 center = m_Buffer_keyInfo->Get_Transform()->Get_Pos();
        _vec2 rectLT{ center.x - WINCX * .5f, WINCY * 0.5f - center.y };

        CFontMgr::GetInstance()->Render_Font(L"NanumSquare_Regular",
            L"\"A\" 요리",
            &rectLT,
            D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }


}

HRESULT CUICookingGame::Add_Component()
{
    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_COOK);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_COOK, m_TextureCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });
    
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    m_Buffer_Deco = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Deco, E_FAIL);
    //m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_Deco });


    m_Buffer_keyInfo = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_keyInfo, E_FAIL);
   // m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_keyInfo });

    m_Buffer_Info = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_Buffer_Info, E_FAIL);
    //m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_Buffer_Info });

    return S_OK;
}

CUICookingGame* CUICookingGame::Create(DEVICE graphicDev, CGameObject* owner, RecipeInst recipeInst)
{
    CUICookingGame* CookingGame = new CUICookingGame(graphicDev, owner, recipeInst);

    if (FAILED(CookingGame->Ready_GameObject()))
    {
        Safe_Release(CookingGame);
        MSG_BOX("CookingGame Create Failed");
        return nullptr;
    }

    return CookingGame;
}

void CUICookingGame::Free()
{

    Safe_Release(m_Buffer_Deco);
    //Safe_Release(m_Buffer_Area);
    Safe_Release(m_Buffer_keyInfo);
    Safe_Release(m_Buffer_Info);

    CUI::Free();
}
