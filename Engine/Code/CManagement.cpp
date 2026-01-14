#include "CManagement.h"

#include "CComponent.h"
#include "CGameObject.h"
#include "CRenderer.h"
#include "CScene.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
    : m_Scene(nullptr),
      m_NextScene(nullptr),
      m_ShouldChangeScene(false),
      m_IsChangingScene(false)
{ }

CManagement::~CManagement()
{
    CManagement::Free();
}

CComponent* CManagement::Get_Component(COMPONENTID   componentID,
                                       LAYERTYPE     layerType,
                                       OBJTYPE       objType,
                                       COMPONENTTYPE componentType)
{
    if (nullptr == m_Scene)
        return nullptr;

    return m_Scene->Get_Component(componentID, layerType, objType, componentType);
}

HRESULT CManagement::Get_PersistentObject(const wstring& key, CGameObject** outObject)
{
    NULL_CHECK_RETURN(outObject, E_FAIL);

    // 2. 일반 Map 검색
    auto iter = m_PersistentObjects.find(key);
     if (iter == m_PersistentObjects.end())
        return E_FAIL;

    *outObject = iter->second;
    return S_OK;
}

HRESULT CManagement::Cache_PersistentObject(const wstring& key, CGameObject* objectToCache)
{
    NULL_CHECK_RETURN(objectToCache, E_FAIL);

    // 2. 일반 Map 처리
    auto iter = m_PersistentObjects.find(key);
    if (iter != m_PersistentObjects.end())
    {
        // 이미 존재하면 교체
        if (iter->second == objectToCache) return S_OK;

        Safe_Release(iter->second);
        iter->second = objectToCache;
        iter->second->AddRef();
    }
    else
    {
        // 없으면 추가
        m_PersistentObjects.insert({ key, objectToCache });
        objectToCache->AddRef();
    }

    return S_OK;
}

HRESULT CManagement::Set_Scene(CScene* scene)
{
    if (nullptr == scene)
        return E_FAIL;

    Safe_Release(m_Scene);

    m_Scene = scene;

    return S_OK;
}

_int CManagement::Update_Scene(const _float& timeDelta)
{
    if (nullptr == m_Scene)
        return -1;

    return m_Scene->Update_Scene(timeDelta);
}

void CManagement::LateUpdate_Scene(const _float& timeDelta)
{
    m_Scene->LateUpdate_Scene(timeDelta);
}

void CManagement::Render_Scene(DEVICE graphicDev)
{
    // 씬 전환 중에는 렌더링 차단
    if (m_IsChangingScene || !m_Scene)
        return;

    CRenderer::GetInstance()->Render_GameObject(graphicDev);
    m_Scene->Render_Scene();   // 디버깅 출력용
}

HRESULT CManagement::Request_ChangeScene(CScene* nextScene)
{
    if (nullptr == nextScene)
        return E_FAIL;

    if (m_NextScene != nullptr)
        Safe_Release(m_NextScene);

    m_NextScene = nextScene;

    m_ShouldChangeScene = true;

    return S_OK;
}

void CManagement::Commit_ChangeScene()
{
    if (!m_ShouldChangeScene) return;

    // 씬 전환 시작 - 렌더링 차단
    m_IsChangingScene = true;

    // 씬 삭제 전 정리 작업 수행
    if (m_Scene)
        m_Scene->PreDestroy_Scene();

    CRenderer::GetInstance()->Clear_RenderGroup();

    Safe_Release(m_Scene);
    m_Scene             = m_NextScene;
    m_NextScene         = nullptr;
    m_ShouldChangeScene = false;

    // 새 씬 초기화
    if (m_Scene)
        m_Scene->Initialize_Scene();

    // 씬 전환 완료 - 렌더링 재개
    m_IsChangingScene = false;
}

void CManagement::Free()
{
    Safe_Release(m_Scene);
    Safe_Release(m_NextScene);

    for (auto& pair : m_PersistentObjects)
    {
        Safe_Release(pair.second);
    }
    m_PersistentObjects.clear();
}
