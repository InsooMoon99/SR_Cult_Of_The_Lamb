#include "pch.h"
#include "CUI_Dialogue.h"
#include "CFontMgr.h"
#include "CDialogueDB.h"
#include "CRenderer.h"
#include "CRcTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CDInputMgr.h"
#include "CCreateHelper.h"

CUI_Dialogue::CUI_Dialogue(DEVICE graphicDev)
    : CUI(graphicDev),
    m_BufferCom(nullptr),
    m_TransformCom(nullptr),
    m_TextureCom(nullptr),
    m_CurrentDialogue(nullptr),
    m_CurrentLineIndex(0),
    m_IsActive(false),
    m_IsShowingChoices(false),
    m_SelectedLeft(true),
    m_ChoiceSpacing(0)
{
}

CUI_Dialogue::CUI_Dialogue(const CUI_Dialogue& rhs)
    : CUI(rhs), m_BufferCom(nullptr),
    m_TransformCom(nullptr),
    m_TextureCom(nullptr),
    m_CurrentDialogue(nullptr),
    m_CurrentLineIndex(0),
    m_IsActive(false),
    m_IsShowingChoices(false),
    m_SelectedLeft(rhs.m_SelectedLeft),
    m_ChoiceSpacing(0)
{
}

CUI_Dialogue::~CUI_Dialogue()
{
}

HRESULT CUI_Dialogue::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_BoxPos = { 0.f, -200.f };   
    m_BoxSize = { 500.f, 120.f }; 

    m_TextOffset = { 0.f, 20.f };    
    m_SpeakerOffset = { -200.f, -30.f };

    m_ChoiceStartOffset = { 0.f, 0.f };     
    m_ChoiceSpacing = 30.f;             

    m_IsActive = false;

    return S_OK;
}

void CUI_Dialogue::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

}

_int CUI_Dialogue::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    if (!m_IsActive)
        return exit;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    Handle_Input();

    return exit;
}

void CUI_Dialogue::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}

void CUI_Dialogue::Render_GameObject()
{
    if (!m_IsActive || !m_CurrentDialogue)
        return;

    CUI::Render_GameObject();

    // 텍스트 박스
    Render_DialogueBox();
    
    // 말하는놈 이름
    Render_SpeakerName();

    // 대사 or 선택지
    if (m_IsShowingChoices)
    {
        Render_Choices();
    }
    else
    {
        Render_DialogueText();
    }
}

void CUI_Dialogue::Start_Dialogue(const string& dialogueId)
{
    m_CurrentDialogue = CDialogueDB::GetInstance()->GetDialogueById(dialogueId);

    NULL_CHECK(m_CurrentDialogue);

    m_CurrentLineIndex = 0;
    m_IsActive = true;
    m_IsShowingChoices = false; // 선택지 보일지
    m_SelectedLeft = true;

    // 첫 라인이 선택지인지 확인
    if (!m_CurrentDialogue->lines.empty() &&
        m_CurrentDialogue->lines[0].hasChoices)
    {
        m_IsShowingChoices = true;
    }
}

void CUI_Dialogue::End_Dialogue()
{
    string lastId = m_CurrentDialogue ? m_CurrentDialogue->dialogueId : "";

    m_CurrentDialogue = nullptr;
    m_CurrentLineIndex = 0;
    m_IsActive = false;
    m_IsShowingChoices = false;

    if (m_OnDialogueEnd)
    {
        m_OnDialogueEnd(lastId);
    }
}

void CUI_Dialogue::Next_Line()
{
    NULL_CHECK(m_CurrentDialogue);

    m_CurrentLineIndex++;

    // 대화 끝 체크
    if (m_CurrentLineIndex >= (_int)m_CurrentDialogue->lines.size())
    {
        End_Dialogue();
        return;
    }

    // 다음 대화가 선택지인지?
    const DialogueLine& nextLine = m_CurrentDialogue->lines[m_CurrentLineIndex];

    if (nextLine.hasChoices)
    {
        m_IsShowingChoices = true;
        m_SelectedLeft = true;
    }
    else
    {
        m_IsShowingChoices = false;
    }
}

void CUI_Dialogue::Select_Choice(_bool isLeft)
{
    if (!m_CurrentDialogue || !m_IsShowingChoices)
        return;

    const DialogueLine& line = m_CurrentDialogue->lines[m_CurrentLineIndex];
    const DialogueChoice& choice = line.choice;

    // 선택에 따른 다음 대화 ID
    string nextId = isLeft ? choice.leftNextId : choice.rightNextId;
    if (!nextId.empty())
    {
        // 다음 대화로 이동
        Start_Dialogue(nextId);
    }
    else
    {
        End_Dialogue();
    }
}

HRESULT CUI_Dialogue::Add_Component()
{
    // Buffer
    m_BufferCom = CreateProtoComponent<CRcTex>(this, COMPONENTTYPE::RC_TEX);
    NULL_CHECK_RETURN(m_BufferCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::RC_TEX, m_BufferCom });

    // Transform
    m_TransformCom = CreateProtoComponent<CTransform>(this, COMPONENTTYPE::TRANSFORM);
    NULL_CHECK_RETURN(m_TransformCom, E_FAIL);
    m_Components[ID_DYNAMIC].insert({ COMPONENTTYPE::TRANSFORM, m_TransformCom });

    // Texture
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_DIALOGUE);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_DIALOGUE, m_TextureCom });

    return S_OK;
}

void CUI_Dialogue::Render_DialogueBox()
{
    m_TransformCom->Set_Pos(m_BoxPos.x, m_BoxPos.y, 0.f);
    m_TransformCom->Set_Scale(m_BoxSize.x, m_BoxSize.y, 1.f);
    m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());

    // 반투명 블렌딩 -> 안쓸듯? 테스트해보고 지우기
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_GraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_GraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // 텍스처가 없으면 검은색 반투명으로 그리기
    if (m_TextureCom)
    {
        m_TextureCom->Set_Texture(0);
    }

    m_BufferCom->Render_Buffer();

    // 렌더 상태 복원
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_GraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_GraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CUI_Dialogue::Render_SpeakerName()
{
    if (!m_CurrentDialogue || m_CurrentLineIndex >= (_int)m_CurrentDialogue->lines.size())
        return;

    const DialogueLine& line = m_CurrentDialogue->lines[m_CurrentLineIndex];
    if (line.speaker.empty())
        return;

    _vec2 screenPos =
    {
        m_BoxPos.x + WINCX * 0.5f,
        WINCY * 0.5f - m_BoxPos.y + m_SpeakerOffset.y
    };

    wstring speakerName = CDialogueDB::GetInstance()->Utf8ToWstring(line.speaker);

    CFontMgr::GetInstance()->Render_Font(
        L"NanumSquare_Regular",
        speakerName,
        &screenPos,
        D3DXCOLOR(1.f, 0.9f, 0.3f, 1.f)  // 일단 노란색
    );
}

void CUI_Dialogue::Render_DialogueText()
{
    if (!m_CurrentDialogue || m_CurrentLineIndex >= (_int)m_CurrentDialogue->lines.size())
        return;

    const DialogueLine& line = m_CurrentDialogue->lines[m_CurrentLineIndex];

    _vec2 screenPos =
    {
        m_BoxPos.x + WINCX * 0.5f,
        WINCY * 0.5f - m_BoxPos.y + m_TextOffset.y
    };

    wstring dialogueText = CDialogueDB::GetInstance()->Utf8ToWstring(line.text);

    CFontMgr::GetInstance()->Render_Font(
        L"NanumSquare_Regular",
        dialogueText,
        &screenPos,
        D3DXCOLOR(1.f, 1.f, 1.f, 1.f)
    );
}

void CUI_Dialogue::Render_Choices()
{
    if (!m_CurrentDialogue || m_CurrentLineIndex >= (_int)m_CurrentDialogue->lines.size())
        return;

    const DialogueLine& line = m_CurrentDialogue->lines[m_CurrentLineIndex];
    const DialogueChoice& choice = line.choice;

    // 기준 스크린 좌표
    _float centerX = m_BoxPos.x + WINCX * 0.5f;
    _float centerY = WINCY * 0.5f - m_BoxPos.y + m_TextOffset.y;

    // 왼쪽
    _vec2 leftPos = { centerX - m_ChoiceSpacing, centerY };
    wstring leftText = CDialogueDB::GetInstance()->Utf8ToWstring(choice.leftText);

    D3DXCOLOR leftColor = m_SelectedLeft
        ? D3DXCOLOR(1.f, 1.f, 0.f, 1.f)    
        : D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.f);

    // 선택된 쪽에 화살표 표시 -> 텍스쳐로 변경 예정
    if (m_SelectedLeft)
        leftText = L"<- " + leftText;

    CFontMgr::GetInstance()->Render_Font(
        L"NanumSquare_Regular",
        leftText,
        &leftPos,
        leftColor
    );

    // 오른쪽
    _vec2 rightPos = { centerX + m_ChoiceSpacing, centerY };

    wstring rightText = CDialogueDB::GetInstance()->Utf8ToWstring(choice.rightText);

    D3DXCOLOR rightColor = !m_SelectedLeft
        ? D3DXCOLOR(1.f, 1.f, 0.f, 1.f)    
        : D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.f);

    if (!m_SelectedLeft)

        rightText = L"-> " + rightText;
    CFontMgr::GetInstance()->Render_Font(
        L"NanumSquare_Regular",
        rightText,
        &rightPos,
        rightColor
    );

}

void CUI_Dialogue::Handle_Input()
{
    // 일단 키보드로 입력처리 테스트 해보기
    if (m_IsShowingChoices)
    {
        // Left
        if (CDInputMgr::GetInstance()->KeyDown(DIK_A) ||
            CDInputMgr::GetInstance()->KeyDown(DIK_LEFT))
        {
            m_SelectedLeft = true;
        }

        if (CDInputMgr::GetInstance()->KeyDown(DIK_D) ||
            CDInputMgr::GetInstance()->KeyDown(DIK_RIGHT))
        {
            m_SelectedLeft = false;
        }

        // 선택 확정
        if (CDInputMgr::GetInstance()->KeyDown(DIK_E))
        {
            Select_Choice(m_SelectedLeft);
        }
    }

    else
    {
        // 다음 대사로
        if (CDInputMgr::GetInstance()->KeyDown(DIK_E))
        {
            Next_Line();
        }
    }
}

CUI_Dialogue* CUI_Dialogue::Create(DEVICE graphicDev)
{
    auto dialogue = new CUI_Dialogue(graphicDev);

    if (FAILED(dialogue->Ready_GameObject()))
    {
        Safe_Release(dialogue);
        MSG_BOX("CUI_Dialogue Create Failed");
        return nullptr;
    }

    return dialogue;
}

void CUI_Dialogue::Free()
{
    CUI::Free();
}
