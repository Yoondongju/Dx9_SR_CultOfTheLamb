#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CShader;
END


BEGIN(Client)
class CFadeInOut :
    public CUI
{
private:
	CFadeInOut(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFadeInOut(const CFadeInOut& Prototype);
	virtual ~CFadeInOut() = default;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	void Start_FadeIn(_float fFadeTime = 3.f) 
	{ 
		if (m_bStart_FadeOut == false)
		{
			m_iShaderNum = 2;
			m_fFadeTime = fFadeTime;
			m_bStart_FadeIn = true;
		}
	}
	void Start_FadeOut(_float fFadeTime = 3.f)
	{
		if (m_bStart_FadeIn == false)
		{
			m_iShaderNum = 2;
			m_fFadeTime = fFadeTime;
			m_bStart_FadeOut = true;
		}
	}
	void Start_Blink()
	{
		if (m_bBlink == false)
		{
			m_iShaderNum = 3;
			m_bBlink = true;
		}
	}
	_float Get_FadeAlpha() { return m_fAlpha; }

	_bool Get_IsFadeIn() { return m_bStart_FadeIn; }
	_bool Get_IsFadeOut() { return m_bStart_FadeOut; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:

	HRESULT Ready_Components();
	void FadeIn(_float fTimeDelta);
	void FadeOut(_float fTimeDelta);
	void Blink(_float fTimeDelta);

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	_bool m_bStart_FadeIn = { false };
	_bool m_bStart_FadeOut = { false };
	_bool m_bStart_FadeInOut = { false };
	_bool m_bBlink = { false };

	_uint m_iShaderNum = { 2 };

	_float m_fAlpha = {0.f};
	_float m_fColor = { 0.f };
	_float m_fFadeTime = { 3.f };

	_bool m_bFadeSet = { true };

	_float m_fFadeInOutDelayTime = { 0.f };
	_float m_fBlinkTime = { 0.f };


public:
	static CFadeInOut* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

