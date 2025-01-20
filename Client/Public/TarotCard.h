#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CTarotCard :
    public CUI
{
public:
	enum CARDTYPE { ATTACKPOWERUP, BONUSHEART, DOUBLEBULLET, HEAL, SPEEDUP, CARDEND };
private:
	CTarotCard(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTarotCard(const CTarotCard& Prototype);
	virtual ~CTarotCard() = default;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	CARDTYPE Get_CardType() { return m_eType; }
	_bool	 Get_FinishExpand() { return m_bFinishExpand; }
	_bool	 Get_MouseClicked() { return m_bMouseClicked; }
	void	 Set_StartReduce(_bool _b) { m_bStartReduce = _b; }
private:
	void Show();
	void Reduced(_float fTimeDelta);
	void ShowSelected();
	void Compute_MouseClick();			// 클릭한거 검사 , 값 전달
private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT Ready_Components();

	CARDTYPE m_eType = {};
	_bool   m_bFirstActive = { false };
	_bool   m_bFinishExpand = { false };
	_bool   m_bMouseClicked = { false };
	_bool   m_bStartReduce = { false };
	_bool	m_bPlayCardShowSound = { false };
	_bool	m_bPlayCardMouseOnSound= { false };
	_bool	m_bPlayCardSelectSound = { false };
	_bool	m_bPlayCardReduceSound = { false };

	_int   m_iSizeChagneCount= { 0 };
public:
	static CTarotCard* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END