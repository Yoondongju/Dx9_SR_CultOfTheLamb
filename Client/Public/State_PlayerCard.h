#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerCard final : public CState
{
	enum CARD_STATE
	{
		CARD_START,
		CARD_LOOP,
		CARD_END,
		CARD_STATE_END
	};

private:
	CState_PlayerCard(class CFsm* pFsm);
	virtual ~CState_PlayerCard() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	void Check_State();
	void Check_State_Init();

	void Card_Start();
	void Card_Loop();
	void Card_End();

	void Card_Start_Init();
	void Card_Loop_Init();
	void Card_End_Init();

private:
	class CPlayer* m_pPlayer = nullptr;
	CARD_STATE m_eCurState = CARD_START;
	CARD_STATE m_ePreState = CARD_STATE_END;

public:
	static CState_PlayerCard* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END


