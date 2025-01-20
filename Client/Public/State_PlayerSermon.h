#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerSermon final : public CState
{
	enum SERMON_STATE
	{
		SERMON_START,
		SERMON_LOOP,
		SERMON_END,
		SERMON_STATE_END
	};

private:
	CState_PlayerSermon(class CFsm* pFsm);
	virtual ~CState_PlayerSermon() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	void Check_State();
	void Check_State_Init();

	void Sermon_Start();
	void Sermon_End();

	void Sermon_Start_Init();
	void Sermon_End_Init();

private:
	class CPlayer* m_pPlayer = nullptr;
	SERMON_STATE m_eCurState = SERMON_START;
	SERMON_STATE m_ePreState = SERMON_STATE_END;

public:
	static CState_PlayerSermon* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
