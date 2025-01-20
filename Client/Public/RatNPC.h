#pragma once
#include "Client_Defines.h"
#include "LandObject.h"
#include <TalkPanelUI.h>

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CRatNPC :
    public CLandObject
{
public:
	typedef struct
	{
		_float3        vPos;
		LANDOBJECT_DESC tLandObjectDesc;
	}Rat_INFO;
private:
	CRatNPC(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRatNPC(const CRatNPC& Prototype);
	virtual ~CRatNPC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	_float  Culculate_Distance_with_Player();
	void    Sound_Play();

private:
	CAnimator* m_pAnimatorCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	Rat_INFO m_tInfo;
	_bool   m_bDirection = { false }; //연출 중인지 아닌지 확인용 변수
	_bool   m_bFinishTalk = { false };
	_bool   m_bActive = { false };
	_bool   m_bStartSound = { false };

	_int    m_iFirstEnter = { 1 };
	_int    m_iSoundNum = { 0 };
public:
	static CRatNPC* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
