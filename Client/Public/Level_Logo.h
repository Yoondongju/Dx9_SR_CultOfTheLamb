#pragma once

#include "Client_Defines.h"
#include "Level.h"


BEGIN(Engine)
class CTexture;
END


BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Logo() = default;

public:
	_int Get_LogoAlpha() { return m_iAlpha; }


public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	static RECT	m_rtStartBindBox;
	static RECT	m_rtNameBindBox;
	static RECT	m_rtExitBindBox;

	RECT		m_rtCrew;

	_float		m_fTime = { 0.f };
	_float		m_fCrewAlpha{ 0.f };
	_float		m_fCrewPos = {};

	_int		m_iAlpha = {0};


	_wstring	m_strCrewNameArray[4];
	_wstring	m_strCrewExplainArray[4];

private:
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Ladyer_UI();

	void Render_Crew();

public:
	static CLevel_Logo* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;


	friend class CLogoButtonUI;
};

END
