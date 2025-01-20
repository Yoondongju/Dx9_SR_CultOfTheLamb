#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"

/* 우리가 추가한 기능*/
#include "Collider_Manager.h"
#include "Key_Manager.h"
#include "Event_Manager.h"
#include "UI_Manager.h"
#include "Sound_Manager.h"
#include "Picking.h"
#include "Culling_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

bool g_DebugMode = true;

CGameInstance::CGameInstance()
{

}

POINT CGameInstance::Get_MosePos()
{
    POINT      ptMouse = {};
    GetCursorPos(&ptMouse);
    ScreenToClient(m_pGraphic_Device->Get_Handle(), &ptMouse);

    return ptMouse;
}

void CGameInstance::RenderText(const _wstring& strText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
    m_pGraphic_Device->RenderText(strText, rect, color, eFontType);
}

void CGameInstance::RenderText(const wchar_t& szText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
    m_pGraphic_Device->RenderText(szText, rect, color, eFontType);
}

void CGameInstance::RenderTextCenter(const _wstring& strText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
    m_pGraphic_Device->RenderTextCenter(strText, rect, color, eFontType);
}

void CGameInstance::RenderTextCenter(const wchar_t& szText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
    m_pGraphic_Device->RenderTextCenter(szText, rect, color, eFontType);
}

_float2 CGameInstance::Get_FontSize(FONT_TYPE _eFont)
{
    return m_pGraphic_Device->Get_FontSize(_eFont);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, LPDIRECT3DDEVICE9* ppDevice)
{
    /* 그래픽 카드를 초기화하낟. */
    m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    _float4x4      ProjMatrix{};
    (*ppDevice)->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    m_pRenderer = CRenderer::Create(*ppDevice);
    if (nullptr == m_pRenderer)
        return E_FAIL;


    /* 사운드 카드를 초기화하낟. */

    /* 입력장치를 초기화하낟. */


    /* 여러가지 매니져를 초기화하낟. */

    m_pLevel_Manager = CLevel_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    m_pObject_Manager = CObject_Manager::Create(iNumLevels);
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
    if (nullptr == m_pComponent_Manager)
        return E_FAIL;

    m_pCollider_Manager = CCollider_Manager::Create();
    if (nullptr == m_pCollider_Manager)
        return E_FAIL;

    m_pKey_Manager = CKey_Manager::Create(m_pGraphic_Device->Get_Handle());
    if (nullptr == m_pKey_Manager)
        return E_FAIL;

    m_pUI_Manager = CUI_Manager::Create();
    if (nullptr == m_pUI_Manager)
        return E_FAIL;

    m_pSound_Manager = CSound_Manager::Create();
    if (nullptr == m_pSound_Manager)
        return E_FAIL;

    m_pPicking = CPicking::Create(m_pGraphic_Device->Get_Handle(), *ppDevice);
    if (nullptr == m_pPicking)
        return E_FAIL;

    m_pCulling_Manager = CCulling_Manager::Create(*ppDevice);
    if (nullptr == m_pCulling_Manager)
        return E_FAIL;
    


    m_pEvent_Manager = CEvent_Manager::Create(m_pLevel_Manager, m_pObject_Manager, m_pUI_Manager); // << 이벤트매니저는 항상 마지막에 만드세요
    if (nullptr == m_pEvent_Manager)
        return E_FAIL;

    return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
    //키 값은 항상 먼저 update
    m_pKey_Manager->Update();

    m_pObject_Manager->Priority_Update(fTimeDelta);

    m_pObject_Manager->Update(fTimeDelta);

    m_pObject_Manager->Late_Update(fTimeDelta);

    //UIManager는 항상 Render 되기 전에 호출
    m_pUI_Manager->Update();

    m_pPicking->Update();
    m_pCulling_Manager->Update();

    m_pLevel_Manager->Update(fTimeDelta);

    if (m_pKey_Manager->Get_KeyState(KEY::Z) == KEY_STATE::TAP)
        g_DebugMode = !g_DebugMode;
        
}

HRESULT CGameInstance::Draw_Engine()
{
    m_pRenderer->Draw();

    return m_pLevel_Manager->Render();
}

void CGameInstance::FinalUpdate_Engine()
{
    m_pEvent_Manager->update();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
    m_pObject_Manager->Clear(iLevelIndex);

    return S_OK;
}

void CGameInstance::Render_Begin()
{
    m_pGraphic_Device->Render_Begin();
}

void CGameInstance::Render_End(HWND hWnd)
{
    m_pGraphic_Device->Render_End(hWnd);
}

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel* pNextLevel)
{
    CEvent_Manager::EVENT tEvent = {};

    tEvent.eEven = CEvent_Manager::EVENT_TYPE::CHANGE_LEVEL;
    tEvent.lParam = (DWORD_PTR)iLevelIndex;
    tEvent.wParam = (DWORD_PTR)pNextLevel;
    tEvent.CloneDesc = nullptr;

    m_pEvent_Manager->AddEvent(tEvent);

    return S_OK;
}

HRESULT CGameInstance::Start_StageChange(_int iStageIndex)
{
    return m_pLevel_Manager->Start_StageChange(iStageIndex);
}

_uint CGameInstance::Get_LevelIndex()
{
    return m_pLevel_Manager->Get_LevelIndex();
}

void CGameInstance::Set_LevelIndex(_uint iLevelIndex)
{
    m_pLevel_Manager->Set_LevelIndex(iLevelIndex);
}

_uint CGameInstance::Get_StageIndex()
{
    return m_pLevel_Manager->Get_StageIndex();
}

void CGameInstance::Set_StageIndex(_uint iStageIndex)
{
    m_pLevel_Manager->Set_StageIndex(iStageIndex);
}

 CLevel* CGameInstance::Get_CurLevel()
{
    return m_pLevel_Manager->Get_CurLevel();
}

HRESULT CGameInstance::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
    return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
    return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}
HRESULT CGameInstance::Add_CloneObject_ToLayer_UI(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg, void** pOut)
{
    return m_pObject_Manager->Add_CloneObject_ToLayer_UI(iLevelIndex, strLayerTag, strPrototypeTag, pArg, pOut);
}


CGameObject* CGameInstance::Find_Player()
{
    return m_pObject_Manager->Find_Player();
}

CGameObject* CGameInstance::Find_Camera()
{
    return m_pObject_Manager->Find_Camera();
}

CGameObject* CGameInstance::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
    return m_pObject_Manager->Find_Object(iLevelIndex, strLayerTag, iIndex);
}

CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
    return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
    return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Find_Prototype(const _wstring& strPrototypeTag)
{
    return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
    return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
    return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
    return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
    return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Update_Timer(const _wstring& strTimerTag)
{
    return m_pTimer_Manager->Update(strTimerTag);
}

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
    return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}

list<class CGameObject*>& CGameInstance::Get_RenderList(CRenderer::RENDERGROUP eGroup)
{
    return m_pRenderer->Get_RenderList(eGroup);
}

void CGameInstance::Collision_Cube(_wstring strLayerTag1, _wstring strLayerTag2, _uint iLevelIndex1, _uint iLevelIndex2)
{
    m_pCollider_Manager->Collision_Cube(strLayerTag1, strLayerTag2, iLevelIndex1, iLevelIndex2);
}

KEY_STATE CGameInstance::Get_KeyState(KEY _eKey)
{
    return m_pKey_Manager->Get_KeyState(_eKey);
}

HRESULT CGameInstance::Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize, void* pArg)
{
    CEvent_Manager::EVENT tEvent = {};
    tEvent.eEven = CEvent_Manager::EVENT_TYPE::CREATE_OBJECT;


    CEvent_Manager::CLONE_DESC* pCloneDesc = new CEvent_Manager::CLONE_DESC;
    pCloneDesc->iLevelIndex = iLevelIndex;
    pCloneDesc->strLayerTag = strLayerTag;
    pCloneDesc->strPrototypeTag = strPrototypeTag;

    if (nullptr != pArg)
    {
        pCloneDesc->pArg = new char[iSize];
        memcpy(pCloneDesc->pArg, pArg, iSize);
    }

    tEvent.CloneDesc = pCloneDesc;

    m_pEvent_Manager->AddEvent(tEvent);

    return S_OK;
}

HRESULT CGameInstance::Event_DestoryObject(CGameObject* pGameObject)
{
    CEvent_Manager::EVENT tEvent = {};
    tEvent.eEven = CEvent_Manager::EVENT_TYPE::DELETE_OBJECT;


    CEvent_Manager::CLONE_DESC* pCloneDesc = new CEvent_Manager::CLONE_DESC;
    tEvent.lParam = (DWORD_PTR)pGameObject;

    tEvent.CloneDesc = pCloneDesc;

    m_pEvent_Manager->AddEvent(tEvent);

    return S_OK;
}

void CGameInstance::Play_Sound(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
    m_pSound_Manager->PlaySound(pSoundKey, eID, fVolume);
}

void CGameInstance::Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
    m_pSound_Manager->Play_SoundRepeat(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(const TCHAR* pSoundKey, _float fVolume)
{
    m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(_uint eID)
{
    m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
    m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(_uint eID, _float fVolume)
{
    m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

void CGameInstance::SetPlayeSpeed(_uint eID, _float fSpeedRatio)
{
    m_pSound_Manager->SetPlayeSpeed(eID, fSpeedRatio);
}

void CGameInstance::Pause(_uint eID)
{
    m_pSound_Manager->Pause(eID);
}

_bool CGameInstance::Check_IsPlaying(_uint eID)
{
    return m_pSound_Manager->Check_IsPlaying(eID);
}

void CGameInstance::Change_To_LocalSpace(_float4x4 WorldMatrixInverse)
{
    m_pPicking->Change_To_LocalSpace(WorldMatrixInverse);
}

_bool CGameInstance::isPicked_InWorldSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut)
{
    return m_pPicking->isPicked_InWorldSpace(vPointA, vPointB, vPointC, pOut);
}

_bool CGameInstance::isPicked_InLocalSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut)
{
    return m_pPicking->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}

_bool CGameInstance::Is_In_Sight(_float3 vPosition, _float fRadius)
{
    return m_pCulling_Manager->Is_In_Sight(vPosition, fRadius);
}

void CGameInstance::Release_Engine()
{
    Safe_Release(m_pCollider_Manager);
    Safe_Release(m_pCulling_Manager);
    Safe_Release(m_pPicking);
    Safe_Release(m_pSound_Manager);
    Safe_Release(m_pKey_Manager);
    Safe_Release(m_pRenderer);
    Safe_Release(m_pTimer_Manager);
    Safe_Release(m_pComponent_Manager);

    Safe_Release(m_pEvent_Manager);
    Safe_Release(m_pUI_Manager);
    Safe_Release(m_pObject_Manager);
    Safe_Release(m_pLevel_Manager);
    Safe_Release(m_pGraphic_Device);

    CGameInstance::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
    __super::Free();

}
