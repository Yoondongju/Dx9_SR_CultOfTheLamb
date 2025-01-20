#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_EligorLE final : public CState
{
private:
	CState_EligorLE(class CFsm* pFsm);
	virtual ~CState_EligorLE() = default;

public:
	virtual	HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	void Create_Bullet();

private:
	class CBoss_Eligor* m_pEligor = nullptr;
	_uint m_iBulletCount = 0;
	_float m_iAttackTimer = 0.f;
	_bool m_bBulletHere = false;

public:
	static CState_EligorLE* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END

