#pragma once
#include "Base.h"

BEGIN(Engine)

class CEvent_Manager final : public CBase
{
public:
	enum EVENT_TYPE { DELETE_OBJECT, CREATE_OBJECT, CHANGE_LEVEL, CHANGE_STATE };

	typedef struct
	{
		_uint iLevelIndex;
		_wstring strLayerTag;
		_wstring strPrototypeTag;
		void* pArg = nullptr;

	}CLONE_DESC;

	typedef struct
	{
		EVENT_TYPE				eEven;
		DWORD_PTR				lParam;
		DWORD_PTR				wParam;

		CLONE_DESC*				CloneDesc = { nullptr };
	}EVENT;


private:
	CEvent_Manager(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr, class CUI_Manager* pUIMgr);
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Initialize();
	void update();

public:
	void AddEvent(EVENT tEvent) { m_Eventlist.push_back(tEvent); }

private:
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CUI_Manager*			m_pUI_Manager = { nullptr };



	list<EVENT>					m_Eventlist;
	list<class CGameObject*>	m_Deadlist;




private:
	void Excute(const EVENT tEvent);


public:
	static CEvent_Manager* Create(class CLevel_Manager* pLevelMgr, class CObject_Manager* pObjectMgr ,class CUI_Manager* pUIMgr );
	virtual void Free() override;
};

END
