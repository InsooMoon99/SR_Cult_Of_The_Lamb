#include "pch.h"
#include "CUI_MonsterHp.h"
#include "CRcTex.h"
#include "CCombatStat.h"
#include "CCreateHelper.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRenderer.h"

CUI_MonsterHp::CUI_MonsterHp(DEVICE graphicDev, CGameObject* owner)
    : CUI(graphicDev),
    m_Owner(owner),
    m_OwnerCombatStat(nullptr),
    m_OwnerTransform(nullptr),
    m_HpRatio(1.0f),
    m_YOffset(3.0f),
    m_BarSize(100.f, 10.f, 1.f)
{
    Set_Name(L"Monster Hp");

    if (m_Owner)
        m_Owner->AddRef();
}

CUI_MonsterHp::~CUI_MonsterHp()
{
}

HRESULT CUI_MonsterHp::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_YOffset = 3.0f;
    m_BarSize = _vec3(100.f, 10.f, 1.f);
    m_HpRatio = 1.f;

    return S_OK;
}

void CUI_MonsterHp::Initialize_GameObject()
{
    CUI::Initialize_GameObject();

    if (m_Owner)
    {
        m_OwnerCombatStat = dynamic_cast<CCombatStat*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::COMBATSTAT));

        m_OwnerTransform = dynamic_cast<CTransform*>(
            m_Owner->Get_Component(ID_DYNAMIC, COMPONENTTYPE::TRANSFORM));
    }
}

_int CUI_MonsterHp::Update_GameObject(const _float& timeDelta)
{
    _int exit = CUI::Update_GameObject(timeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);


    if (!m_Owner)
        return exit;

    // HP 비율 계산
    if (m_OwnerCombatStat)
    {
        _float curHp = m_OwnerCombatStat->Get_Hp();
        _float maxHp = m_OwnerCombatStat->Get_MaxHp();

        if (maxHp > 0.f)
            m_HpRatio = curHp / maxHp;
        else
            m_HpRatio = 0.f;

        // 죽으면 삭제
        if (m_HpRatio <= 0.f)
            this->Mark_As_PendingKill();
    }

    if (m_OwnerTransform && m_TransformCom)
    {
        _vec3 ownerPos = m_OwnerTransform->Get_Pos();
        _vec3 uiPos = ownerPos;
        uiPos.y += m_YOffset;   

        //m_TransformCom->Set_Pos(uiPos);
        m_TransformCom->Set_Pos(_vec3(0.f, 300.f, 0.f));
        m_TransformCom->Set_Scale(m_BarSize);
    }

    return exit;
}

void CUI_MonsterHp::LateUpdate_GameObject(const _float& timeDelta)
{
    CUI::LateUpdate_GameObject(timeDelta);

}

void CUI_MonsterHp::Render_GameObject()
{
    OutputDebugStringA("MonsterHP Render called!\n");

    if (!m_Owner || !m_TextureCom || !m_BufferCom)
    {
        OutputDebugStringA("MonsterHP: Component is NULL!\n");
        return;
    }

    // 1. 배경
    {
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
        m_TextureCom->Set_Texture(1);  // 배경은 1번 인덱스
        m_BufferCom->Render_Buffer();
    }

    // 2. HP 바 (왼쪽 정렬, 오른쪽에서 줄어듦)
    if (m_HpRatio > 0.f)
    {
        _matrix hpMatrix = m_TransformCom->Get_World();

        // X 스케일 조정
        hpMatrix._11 *= m_HpRatio;

        // 왼쪽 정렬
        _float offsetX = m_BarSize.x * (1.f - m_HpRatio) * 0.5f;
        hpMatrix._41 -= offsetX;

        m_GraphicDev->SetTransform(D3DTS_WORLD, &hpMatrix);
        m_TextureCom->Set_Texture(0);
        m_BufferCom->Render_Buffer();
    }

    // 3. 테두리
    /*{
        m_GraphicDev->SetTransform(D3DTS_WORLD, &m_TransformCom->Get_World());
        m_TextureCom->Set_Texture(2);
        m_BufferCom->Render_Buffer();
    }*/
}

HRESULT CUI_MonsterHp::Add_Component()
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
    m_TextureCom = CreateProtoComponent<CTexture>(this, COMPONENTTYPE::TEX_UI_MONSTERHP);
    NULL_CHECK_RETURN(m_TextureCom, E_FAIL);
    m_Components[ID_STATIC].insert({ COMPONENTTYPE::TEX_UI_MONSTERHP, m_TextureCom });

    return S_OK;
}

void CUI_MonsterHp::Update_Billboard()
{

}

void CUI_MonsterHp::WorldToScreen(_vec3 worldPos)
{
    _matrix view, proj;
    m_GraphicDev->GetTransform(D3DTS_VIEW, &view);
    m_GraphicDev->GetTransform(D3DTS_PROJECTION, &proj);

    _matrix viewProj = view * proj;

    // 월드 -> NDC
    _vec3 ndc;
    D3DXVec3TransformCoord(&ndc, &worldPos, &viewProj);
}

CUI_MonsterHp* CUI_MonsterHp::Create(DEVICE graphicDev, CGameObject* owner)
{
    CUI_MonsterHp* hp = new CUI_MonsterHp(graphicDev, owner);

    if (FAILED(hp->Ready_GameObject()))
    {
        Safe_Release(hp);
        MSG_BOX("Monster Hp Create Failed");
        return nullptr;
    }

    return hp;
}

void CUI_MonsterHp::Free()
{
    Safe_Release(m_Owner);
    CUI::Free();
}
