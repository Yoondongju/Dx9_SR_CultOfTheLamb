#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_EligorBurp final : public CState
{
private:
	CState_EligorBurp(class CFsm* pFsm);
	virtual ~CState_EligorBurp() = default;

public:
	virtual	HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	void Create_Bat();
	//void Create_Bullet();

private:
	class CBoss_Eligor* m_pEligor = nullptr;
	_bool m_bBatHere = false;

public:
	static CState_EligorBurp* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END
