#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_PlayerPickAxe final : public CState
{
private:
	CState_PlayerPickAxe(class CFsm* pFsm);
	virtual ~CState_PlayerPickAxe() = default;

public:
	virtual HRESULT Initialize(_uint iStateNum) override;
	virtual HRESULT	Start_State() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	End_State() override;

private:
	class CPlayer* m_pPlayer = nullptr;
	_float		   m_fParticleTimer = 0.f;

	_bool m_isCrash = { false };
	_bool m_bIsParticle = true;

public:
	static CState_PlayerPickAxe* Create(class CFsm* pFsm, _uint iStateNum);
	virtual void Free() override;
};
END

