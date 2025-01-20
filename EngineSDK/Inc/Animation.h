#pragma once
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final
	: public CComponent
{
public:
	typedef struct
	{
		_uint		iStartFrame;
		_uint		iEndFrame;
		_float		fDuration;

	}ANIMATION_DESC;

private:
	CAnimation(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CAnimation() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Update(_float fTimeDelta);
	void Render(HDC hDC);

public:
	HRESULT Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame);
	HRESULT Add_Animation(const _wstring& strAnimationTag, CTexture* pTexture, _uint iStartFrame, _uint iEndFrame, _float fDuration);
	CTexture* Find_Animation(const _wstring& strAnimationTag);

private:
	map<const _wstring, CTexture*>				m_Animation;
	map<const _wstring, ANIMATION_DESC*>		m_AnimationDesc;

	CTexture*			m_pCurrentAnimation = { nullptr };
	ANIMATION_DESC*		m_pCurrentAnimationDesc = {};
	int					m_iCurrentFrame = {};

	float				m_fAccTime = {}; // 시간 누적값

public:
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END