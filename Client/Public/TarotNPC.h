#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CTarotNPC :
    public CLandObject
{
public:
	typedef struct
	{
		_float3        vPos;
		LANDOBJECT_DESC tLandObjectDesc;
	}TAROTNPC_INFO;
private:
	CTarotNPC(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTarotNPC(const CTarotNPC& Prototype);
	virtual ~CTarotNPC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_CanSelectCard() { return m_bCanSelcetCard; }
private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	void Calculate_CanSelcetCard();
	void Compute_PlayerFisrtCard();
private:
	TAROTNPC_INFO m_tInfo;
	CAnimator* m_pAnimatorCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class CTarotPanel* m_pTarotPanel = { nullptr };
	_bool	m_bCanSelcetCard = { false };			//카드 뽑기 안내 UI;
	_bool	m_bPlayerPickedCard = { false };


public:
	static CTarotNPC* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END