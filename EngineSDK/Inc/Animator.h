#pragma once
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CAnimator final
	: public CComponent
{
public:
	typedef struct
	{
		_uint		iStartFrame = {  };
		_uint		iEndFrame = {  };
		_float		fDuration = {  };
		_bool		isLoop = { false };
	}ANIMATION_DESC;

private:
	CAnimator(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAnimator(const CAnimator& Prototype);
	virtual ~CAnimator() = default;

public:
	_wstring Get_AnimationTag() { return m_strAnimationTag; }
	_bool IsEnd_CurrentAnimation() { return m_isEnd; }

	_uint Get_CurrentFrame() { return m_iCurrentFrame; }
	void Set_CurrentFrame(_uint iCurrentFrame) { m_iCurrentFrame = iCurrentFrame; }

	void Start_MoveFrame() { m_isMoveFrame = true; }
	void Stop_MoveFrame() { m_isMoveFrame = false; }

	void Start_CurrentAnimationLoop() { m_pCurrentAnimationDesc->isLoop = false; }
	void Stop_CurrentAnimationLoop() { m_pCurrentAnimationDesc->isLoop = false; }

	LPDIRECT3DBASETEXTURE9 Get_CurrentTexture() { return m_Animation.find(m_strAnimationTag)->second->Get_Texture(m_iCurrentFrame); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Update(_float fTimeDelta);
	void Render(HDC hDC);

public:
	HRESULT Change_Animation(const _wstring& strAnimationTag, _uint iStartFrame = 0);
	HRESULT Add_Animation(const _wstring& strAnimationTag, CTexture* pTexture, _uint iStartFrame, _uint iEndFrame, _float fDuration, _bool isLoop = true);
	CTexture* Find_Animation(const _wstring& strAnimationTag);
	void	Reset_Frame();

private:
	map<const _wstring, CTexture*>				m_Animation;
	map<const _wstring, ANIMATION_DESC*>		m_AnimationDesc;

	CTexture*			m_pCurrentAnimation = { nullptr };
	ANIMATION_DESC*		m_pCurrentAnimationDesc = {};

	_uint					m_iCurrentFrame = {};
	_float				m_fAccTime = {}; // 시간 누적값
	_bool				m_isMoveFrame = { true };
	_bool				m_isEnd = { false };

	_wstring			m_strAnimationTag = {};

public:
	static CAnimator* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END