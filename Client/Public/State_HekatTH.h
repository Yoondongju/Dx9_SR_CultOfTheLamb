#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatTH final : public CState
{
private:
    CState_HekatTH(class CFsm* pFsm);
    virtual ~CState_HekatTH() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Hekat*  m_pHekat = { nullptr };
    _float              m_fEffect_Delay_Timer = 0.f;
    _float              m_fEffect_Timer = 0.f;
    _float              m_fNearEffect_Timer = 0.f;
    _float              m_fCameraTimer = 0.f;
    _float              m_fZoom = 0.f;
    _bool               m_bCameraZoom = true;
    _bool               m_bCameraZoom2 = true;
    _bool               m_bCameraZoom3 = true;
    _bool               m_bEffect_Once = true;

public:
    static CState_HekatTH* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END

