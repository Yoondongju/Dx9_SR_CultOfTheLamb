#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CEye_Controller final : public CGameObject
{
private:
	CEye_Controller(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEye_Controller(const CEye_Controller& Prototype);
	virtual ~CEye_Controller() = default;

public:
	_bool Get_IsFirsDie() { return m_isFirtDie; }
	_bool Get_IsStart() { return m_isStart; }

public:
	virtual HRESULT Initialize_Prototype() override { return true; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<class CBoss_Eye*> m_Eyes;

	_uint m_iBallCount = {};
	_uint m_iChainCount = {};
	_uint m_iFirstPatternNum = {};
	_uint m_iSecondPatternNum = {};

	_float m_fEyeTotalHP = {};
	_float m_fAccTime = {};
	_float m_fPatternDelay = { 0.f };
	_float m_LastTalkDelay = { 0.f };

	_bool m_isFirtDie = { false };
	_bool m_isSecondDIe = { false };
	_bool m_isAllDie = { false };
	_bool m_isStart = { false };
	_bool m_isCameraTurn = { false };

	_bool m_isLastTalk[3] = { false, false, false };

	_wstring m_strBossTalk = {};


private:
	void FirstDiePattern();
	void SecondDiePattern();


public:
	static CEye_Controller* Create(LPDIRECT3DDEVICE9 pGraphic_Device, void* pArg);
	virtual CGameObject* Clone(void* pArg) override { return nullptr; }
	virtual void Free() override;
};

END