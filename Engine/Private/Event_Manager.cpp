#include "../Public/Event_Manager.h"
#include "../Public/GameObject.h"

#include "../Public/Level_Manager.h"
#include "../Public/Object_Manager.h"
#include "../Public/UI_Manager.h"

#include "../Public/Level.h"
#include "../Public/UI.h"

CEvent_Manager::CEvent_Manager(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr, class CUI_Manager* pUIMgr)
	: m_pLevel_Manager(pLevelMgr)
	, m_pObject_Manager(pObjectMgr)
	, m_pUI_Manager(pUIMgr)
{
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

void CEvent_Manager::update()
{
	if (m_Deadlist.size() > 0)
	{
		for (auto& pDeadObject : m_Deadlist)
			Safe_Release(pDeadObject);
		m_Deadlist.clear();
	}

	for (auto& Event = m_Eventlist.begin(); Event != m_Eventlist.end();)
	{

		Excute(*Event);

		if (nullptr != Event->CloneDesc)
		{
			Safe_Delete(Event->CloneDesc->pArg);
			Safe_Delete(Event->CloneDesc);
		}
			
		Event = m_Eventlist.erase(Event);
	}
	m_Eventlist.clear();
}



void CEvent_Manager::Excute(const EVENT tEvent)
{
	switch ((UINT)tEvent.eEven)
	{
	case (UINT)EVENT_TYPE::CREATE_OBJECT:
	{
		// lParam : 안씀
		// wParam : 안씀
		// pArg   : Clone_Desc 정보
		const CLONE_DESC* CloneDesc = tEvent.CloneDesc;

		m_pObject_Manager->Add_CloneObject_ToLayer(CloneDesc->iLevelIndex, CloneDesc->strLayerTag, CloneDesc->strPrototypeTag, CloneDesc->pArg);
	}
	break;

	case (UINT)EVENT_TYPE::DELETE_OBJECT:
	{
		// lParam : Object adress
		// wParam: Group Type
		// Object를 Dead상태로 변경
		// 삭제예정 오브젝트들을 모아둔다.

		CGameObject* pDeadObj = (CGameObject*)tEvent.lParam;

		if (!pDeadObj->Get_Dead())
		{
			pDeadObj->Set_Dead(true);
			m_Deadlist.push_back(pDeadObj);
		}


		if (nullptr != dynamic_cast<CUI*>(pDeadObj))	// UI란 소리고
		{
			m_pUI_Manager->SetFocusUI(nullptr);
		}

	}
	break;


	case (UINT)EVENT_TYPE::CHANGE_LEVEL:
	{
		// lParam:   Level Index
		// wParam :  Next Scene Type
		m_pLevel_Manager->Change_Level((_uint)tEvent.lParam, (CLevel*)tEvent.wParam);
		m_pUI_Manager->SetFocusUI(nullptr);
	}
	break;

	case (UINT)EVENT_TYPE::CHANGE_STATE:
	{
		// lParam : AI
		// wParam : Next Type

	}
	break;
	}
}



CEvent_Manager* CEvent_Manager::Create(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr, class CUI_Manager* pUIMgr)
{
	CEvent_Manager* pInstance = new CEvent_Manager(pLevelMgr, pObjectMgr, pUIMgr);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CEvent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pUI_Manager);


	for (auto& pDeadObject : m_Deadlist)
		Safe_Release(pDeadObject);
	m_Deadlist.clear();

	m_Eventlist.clear();
}



