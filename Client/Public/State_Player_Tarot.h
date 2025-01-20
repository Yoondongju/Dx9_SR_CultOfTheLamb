#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_Player_Tarot final : public CState
{
private:
    CState_Player_Tarot(class CFsm* pFsm);
    virtual ~CState_Player_Tarot() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CPlayer*  m_pPlayer = nullptr;
    _bool           m_bEffectOnce = true;

public:
    static CState_Player_Tarot* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END
