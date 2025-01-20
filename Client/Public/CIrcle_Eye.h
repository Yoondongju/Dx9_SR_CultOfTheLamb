#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Command_Circle.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

class CCircle_Eye final :
	public CUI
{
private:
	CCircle_Eye(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCircle_Eye(const CCircle_Eye& Prototype);
	virtual ~CCircle_Eye() = default;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool Get_Pupil_Active() { return m_bPupilActive; }

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	RECT	m_rText = {};
	_float	m_fTimer = {0.f};
	_float	m_fOffsetX = {0.f};
	_float	m_fOffsetY = {0.f};

	_int	m_iBindNum = { };
	_bool	m_bPupilActive = {true};


private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	void Move_Offset();
	
public:
	static CCircle_Eye* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
