#include "..\Public\Graphic_Device.h"

CGraphic_Device::CGraphic_Device() 
	: m_pDevice(nullptr)
	, m_pSDK(nullptr)
	, m_pSprite(nullptr)
{
}


void CGraphic_Device::RenderText(const _wstring& strText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
	m_pFont[eFontType]->DrawTextW(nullptr, strText.c_str(), (_int)strText.size(), rect, DT_LEFT | DT_TOP, color);
}

void CGraphic_Device::RenderText(const wchar_t& szText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
	m_pFont[eFontType]->DrawTextW(nullptr, &szText , 1  ,rect, DT_LEFT | DT_TOP, color);
}

void CGraphic_Device::RenderTextCenter(const _wstring& strText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
	m_pFont[eFontType]->DrawTextW(nullptr, strText.c_str(), (_int)strText.size(), rect, DT_CENTER | DT_VCENTER, color);
}

void CGraphic_Device::RenderTextCenter(const wchar_t& szText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType)
{
	m_pFont[eFontType]->DrawTextW(nullptr, &szText, 1, rect, DT_CENTER | DT_VCENTER, color);
}

_float2 CGraphic_Device::Get_FontSize(FONT_TYPE _eFont)
{
	D3DXFONT_DESCW FontDesc = {};

	if (FAILED(m_pFont[_eFont]->GetDesc(&FontDesc)))
	{
		MSG_BOX(L"ã���ô� ��Ʈ�� ������ �����ϴ�.");
		return _float2(0.f, 0.f);
	}
		
	
	return _float2(_float(FontDesc.Width), _float(FontDesc.Height));
}


HRESULT CGraphic_Device::Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice)
{
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9		DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// GetDeviceCaps : ��ġ�� ���� ���� ������ �����Ͽ� ���� ������ �Լ�
	// D3DADAPTER_DEFAULT : ������ �������� �⺻ �׷��� ī�带 �ǹ�
	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		MSG_BOX(L"GetDeviceCaps Failed");
		return E_FAIL;
	}

	// ���ؽ� ���μ��� = ���� ��ȯ + ����(���) ����

	DWORD		vp(0);

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;


	D3DPRESENT_PARAMETERS		d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	Set_Parameters(d3dpp, EngineDesc);

	// CreateDevice : ��ġ�� ������ ��ü�� �����ϴ� �Լ�
	// CreateDevice(� �׷��� ī�带 ������ ���ΰ�, � ������ ��ġ�� ������ ���ΰ�, ��ġ�� ����� ������ �ڵ�, ���� ���, ��� ȯ��, ������ ��ü�� ���� ������)
	
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, EngineDesc.hWnd, vp, &d3dpp, &m_pDevice)))
	{
		MSG_BOX(L"CreateDevice Failed");
		return E_FAIL;
	}

	// ��������Ʈ

	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		MSG_BOX(L"CreateSprite Failed");
		return E_FAIL;
	}



	Load_MyPont(L"../Bin/Cult_Resources/Font/dongju.ttf");

	if (FAILED(Create_MyPont()))
		return E_FAIL;


	
	

	*ppDevice = m_pDevice;
	Safe_AddRef(m_pDevice);

	return S_OK;
}

void CGraphic_Device::Render_Begin()
{
	m_pDevice->Clear(0,			// ��Ʈ�� ����
					nullptr,	// ��Ʈ �迭�� ù ��° �ּ�
					D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
					D3DCOLOR_ARGB(255, 0, 0, 0), // �� ������ ����
					1.f,	// ���� ������ �ʱ�ȭ ��
					0);		// ���ٽ� ������ �ʱ�ȭ ��

	m_pDevice->BeginScene();	// ���⼭���� �ĸ� ���ۿ� �׸��� ����
}

void CGraphic_Device::Render_End(HWND hWnd)
{	
	m_pDevice->EndScene();		// �ĸ� ���ۿ� �׸��Ⱑ ���� ����

	// �ĸ� ������ surface�� ���� ������ surface�� ��ȯ
	m_pDevice->Present(nullptr, nullptr, hWnd, nullptr);

	// 1, 2 : D3DSWAPEFFECT_COPY ���� ����
	// 3 : null�� ��� Set_Parameters �Լ� �ȿ� ������ �ڵ�� �ڵ� ����
	// 4 : D3DSWAPEFFECT_COPY ���� ����
}

void CGraphic_Device::Set_Parameters(D3DPRESENT_PARAMETERS & d3dpp, const ENGINE_DESC& EngineDesc)
{
	d3dpp.BackBufferWidth  = EngineDesc.iWinSizeX;
	d3dpp.BackBufferHeight = EngineDesc.iWinSizeY;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount  = 1;
	
	d3dpp.MultiSampleType    = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// D3DSWAPEFFECT_DISCARD : ���� ü�� ���
	// D3DSWAPEFFECT_FLIP : ���� �ϳ��� �������鼭 ����ϴ� ���
	// D3DSWAPEFFECT_COPY : ���� ���۸��� ������ ���� ���
	
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// ��ġ�� ����� ������ �ڵ�
	d3dpp.hDeviceWindow = EngineDesc.hWnd;

	d3dpp.Windowed = TRUE;	// â ��� �Ǵ� ��üȭ�� ��� ���� ����(TRUE : â ���)

	d3dpp.EnableAutoDepthStencil = TRUE;		// ���ٽ� ���ۿ� ���� ������ ���� ����
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	

	// ��ü ȭ�� ��� �� ����� ������� ���� ����
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	// ������� �ÿ��� ���� ����
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

HRESULT CGraphic_Device::Create_MyPont()
{
	D3DXFONT_DESCW		tFontInfo;
	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));

	tFontInfo.Height = 18;
	tFontInfo.Width = 7;
	tFontInfo.Weight = FW_BOLD;
	//tFontInfo.CharSet = HANGEUL_CHARSET;
	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont[FONT_TYPE::SMALL])))
	{
		MSG_BOX(L"Create smallFont Indirect Failed");
		return E_FAIL;
	}

	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));
	tFontInfo.Height = 24;
	tFontInfo.Width = 11;
	tFontInfo.Weight = FW_BOLD;
	//tFontInfo.CharSet = HANGEUL_CHARSET;
	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont[FONT_TYPE::MEDIUM])))
	{
		MSG_BOX(L"Create MediumFont Indirect Failed");
		return E_FAIL;
	}

	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));
	tFontInfo.Height = 35;
	tFontInfo.Width = 16;
	tFontInfo.Weight = FW_HEAVY;
	//tFontInfo.CharSet = HANGEUL_CHARSET;
	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont[FONT_TYPE::BIG])))
	{
		MSG_BOX(L"Create BigFont Indirect Failed");
		return E_FAIL;
	}


	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));
	tFontInfo.Height = 50;
	tFontInfo.Width = 23;
	tFontInfo.Weight = FW_HEAVY;
	//tFontInfo.CharSet = HANGEUL_CHARSET;
	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont[FONT_TYPE::VERY_BIG])))
	{
		MSG_BOX(L"Create VeryBigFont Indirect Failed");
		return E_FAIL;
	}

	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));
	tFontInfo.Height = 70;
	tFontInfo.Width = 35;
	tFontInfo.Weight = FW_HEAVY;
	//tFontInfo.CharSet = HANGEUL_CHARSET;
	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont[FONT_TYPE::ULTRA_BIG])))
	{
		MSG_BOX(L"Create VeryBigFont Indirect Failed");
		return E_FAIL;
	}
	return S_OK;
}

void CGraphic_Device::Load_MyPont(const wchar_t* filePath)
{
	int result = AddFontResourceW(filePath);
	if (result == 0)
		MSG_BOX(TEXT("Fail_PONT_LOAD"));

}

CGraphic_Device * CGraphic_Device::Create(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice)
{
	CGraphic_Device*		pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Init_Device(EngineDesc, ppDevice)))
	{
		MSG_BOX(TEXT("Failed to Created : CGraphic_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	for (_uint i = 0; i < FONT_TYPE::FONT_END; i++)
	{
		Safe_Release(m_pFont[i]);
	}
	


	Safe_Release(m_pSprite);

	if (0 != Safe_Release(m_pDevice))
		MSG_BOX(TEXT("Failed to Release Com-Instance"));

	Safe_Release(m_pSDK);

	__super::Free();

}
