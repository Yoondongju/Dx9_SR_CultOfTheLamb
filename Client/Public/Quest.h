#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CBase;
END

class CQuest final : public CBase
{
	DECLARE_SINGLETON(CQuest)

public:
	struct tCurQuest
	{
		_wstring strCurQuestName = L"";
		_wstring strAdditialnalName = L"";
		_bool	 bCurQuestSucceed = false;	

	};

private:
	CQuest();
	virtual ~CQuest() = default;

public:
	tCurQuest	Get_CurQuest() { return m_tMyCurQuest; }


public:
	HRESULT	Initialize_Prototype();
	HRESULT Initialize();
	void Update(_float fTimeDelta);

	void	Add_Quset(const _wstring& strQuestName )
	{
		m_mapQuest.emplace(make_pair(strQuestName, false));
	}
	

	void    Succeed_Quest(const _wstring& strQuestName)
	{
		auto iter = m_mapQuest.find(strQuestName);
		if (iter != m_mapQuest.end()) {
			iter->second = true;

			// 현재 퀘스트와 성공한 퀘스트가 동일한지 확인하고 업데이트
			if (m_tMyCurQuest.strCurQuestName == strQuestName) {
				m_tMyCurQuest.bCurQuestSucceed = true;
			}
		}
	}


	bool	IsSucceed_Quest(const _wstring& strQuestName)
	{
		return m_mapQuest.find(strQuestName)->second;	
	}

	HRESULT  AssignCurQuest(const _wstring& strQuestName, const _wstring& _AddiName)
	{
		auto& iter = m_mapQuest.find(strQuestName);

		if (m_mapQuest.end() == iter)
			return E_FAIL;


		m_tMyCurQuest.strCurQuestName = iter->first;
		m_tMyCurQuest.strAdditialnalName = _AddiName;
		m_tMyCurQuest.bCurQuestSucceed = iter->second;


		return S_OK;
	}






private:
	map<_wstring, _bool>		m_mapQuest;
	tCurQuest					m_tMyCurQuest = {};




public:
	virtual void Free() override;
};

