#pragma once

#include "CUI.h"
#include "Client_Struct.h"

BEGIN(Engine)
class CRcTex;
class CTransform;
class CTexture;
END

class CUI_Dialogue : public CUI
{
private:
    explicit CUI_Dialogue(DEVICE graphicDev);
    explicit CUI_Dialogue(const CUI_Dialogue& rhs);
    virtual ~CUI_Dialogue();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual void    Initialize_GameObject() override;
    virtual _int    Update_GameObject(const _float& timeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& timeDelta) override;
    virtual void    Render_GameObject() override;

public:
    // 대화
    void    Start_Dialogue(const string& dialogueId);
    void    End_Dialogue();

    void    Next_Line();
    void    Select_Choice(_bool isLeft); // 선택지 선택 (노예, 살해)
    
    _bool   IsActive() const { return m_IsActive; }
    _bool   IsShowingChoices() const { return m_IsShowingChoices; }

public:
    // 대화 종료
    using   OnDialougeEnd = function<void(const string&)>;
    void    Set_OnDialogueEnd(OnDialougeEnd callback) { m_OnDialogueEnd = callback; }

private:
    HRESULT Add_Component();

    void    Render_DialogueBox();
    void    Render_SpeakerName();
    void    Render_DialogueText();
    void    Render_Choices();

    void    Handle_Input();

public:
    static CUI_Dialogue* Create(DEVICE graphicDev);

private:
    virtual void Free() override;

private:
    CRcTex*         m_BufferCom;
    CTransform*     m_TransformCom;  
    CTexture*       m_TextureCom;

    // 대화 상태
    Dialogue*       m_CurrentDialogue;
    _int            m_CurrentLineIndex;
    _bool           m_IsActive;

    // 선택지
    _bool           m_IsShowingChoices;
    bool            m_SelectedLeft;

    _vec2           m_BoxPos;   
    _vec2           m_BoxSize;
    _vec2           m_TextOffset;
    _vec2           m_SpeakerOffset;        // 말하는놈 위치
    _vec2           m_ChoiceStartOffset;
    _float          m_ChoiceSpacing;        // 선택지 간격

    // 대화 종료
    OnDialougeEnd   m_OnDialogueEnd;
    
};
