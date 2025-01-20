#pragma once

#include "Renderer.h"
#include "Component_Manager.h"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	POINT Get_MosePos();
	void RenderText(const _wstring& strText, RECT * rect, D3DCOLOR color, FONT_TYPE eFontType);
	void RenderText(const wchar_t& szText, RECT * rect, D3DCOLOR color, FONT_TYPE eFontType);
	void RenderTextCenter(const _wstring& strText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType);
	void RenderTextCenter(const wchar_t& szText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType);

	_float2 Get_FontSize(FONT_TYPE _eFont);

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	void FinalUpdate_Engine();

	HRESULT Clear(_uint iLevelIndex);

#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin();
	void Render_End(HWND hWnd = nullptr);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	HRESULT Start_StageChange(_int iNextStageIndex);
	_uint   Get_LevelIndex();
	void	Set_LevelIndex(_uint iLevelIndex);
	_uint	Get_StageIndex();
	void	Set_StageIndex(_uint iStageIndex);
	class CLevel* Get_CurLevel();


#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_CloneObject_ToLayer_UI(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void* pArg, void** pOut);


	class CGameObject* Find_Player();
	class CGameObject* Find_Camera();
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);

	class CComponent*  Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CLayer*	   Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject* Find_Prototype(const _wstring & strPrototypeTag);
#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);
	void Update_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	list<class CGameObject*>& Get_RenderList(CRenderer::RENDERGROUP eGroup);
#pragma endregion

#pragma region COLLIDER_MANAGER
	void Collision_Cube(_wstring strLayerTag1, _wstring strLayerTag2, _uint iLevelIndex1, _uint iLevelIndex2);
#pragma endregion

#pragma region KEY_MANAGER
	KEY_STATE   Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region EVENT_MANAGER
	HRESULT Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize = 0, void* pArg = nullptr);
	HRESULT Event_DestoryObject(class CGameObject* pGameObject);
#pragma endregion


#pragma region SOUND_MANAGER
	void Play_Sound(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, _float fVolume);
	void StopSound(_uint eID);
	void StopAll();
	void SetChannelVolume(_uint eID, _float fVolume);
	void SetPlayeSpeed(_uint eID, _float fSpeedRatio);
	void  Pause(_uint eID);
	_bool Check_IsPlaying(_uint eID);
#pragma endregion

#pragma region PICKING
	void Change_To_LocalSpace(_float4x4 WorldMatrixInverse);
	_bool isPicked_InWorldSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut);
	_bool isPicked_InLocalSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut);
#pragma endregion

#pragma region CULLING
	_bool Is_In_Sight(_float3 vPosition, _float fRadius);
#pragma endregion

private:
	class CGraphic_Device*	m_pGraphic_Device = { nullptr };

	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CRenderer* m_pRenderer = { nullptr };

	/* 여기서부터 우리가 추가한 기능들*/
	class CCollider_Manager* m_pCollider_Manager = { nullptr };
	class CKey_Manager* m_pKey_Manager = { nullptr };
	class CEvent_Manager* m_pEvent_Manager = { nullptr };
	class CUI_Manager* m_pUI_Manager = { nullptr };
	class CSound_Manager* m_pSound_Manager = { nullptr };
	class CPicking* m_pPicking = { nullptr };
	class CCulling_Manager* m_pCulling_Manager = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

END