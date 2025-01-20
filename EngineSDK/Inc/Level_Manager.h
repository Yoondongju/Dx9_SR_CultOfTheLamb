#pragma once

#include "Base.h"

/* 1. ���� �Ҵ��� ������ �ּҸ� ��� �ִ´�. */
/* 2. ���� �Ҵ��� ������ �����Ѵ�. */
/* 3. ���ο� ������ ��ü �� ���� ������ �ı����ֳ�. + ���� ������ �ڿ�(���ҽ���, ��ü��) �� �����Ѵ�. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	void Set_LevelIndex(_uint iLevelIndex) { m_iLevelIndex = iLevelIndex; }
	_uint Get_LevelIndex() { return m_iLevelIndex; }

	_uint Get_StageIndex() { return m_iStageIndex; }
	void Set_StageIndex(_uint iStageIndex) { m_iStageIndex = iStageIndex; }

	class CLevel* Get_CurLevel() { return m_pCurrentLevel; }


public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	HRESULT Start_StageChange(_int iNextStageIndex);

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iLevelIndex = {};
	_uint					m_iStageIndex = {};

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END