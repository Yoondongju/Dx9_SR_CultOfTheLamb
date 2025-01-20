
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"
#include "MonologueUI.h"
#include "GameInstance.h"
#include "Texture.h"
#include "Camera.h"

RECT CLevel_Logo::m_rtStartBindBox;
RECT CLevel_Logo::m_rtNameBindBox;
RECT CLevel_Logo::m_rtExitBindBox;

CLevel_Logo::CLevel_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device }
{
	RECT StartBindBox = {};
	StartBindBox.left = 130;
	StartBindBox.top = (LONG)(g_iWinSizeY / 2.f - 30);
	StartBindBox.right = 200;
	StartBindBox.bottom = (LONG)(StartBindBox.top + 30);
	m_rtStartBindBox = StartBindBox;


	RECT NameBindBox = {};
	NameBindBox.left = 130;
	NameBindBox.top = (LONG)(StartBindBox.bottom + 30);
	NameBindBox.right = 200;
	NameBindBox.bottom = (LONG)(NameBindBox.top + 30);
	m_rtNameBindBox = NameBindBox;

	RECT ExitBindBox = {};
	ExitBindBox.left = 130;
	ExitBindBox.top = (LONG)(NameBindBox.bottom + 30);
	ExitBindBox.right = 200;
	ExitBindBox.bottom = (LONG)(ExitBindBox.top + 30);
	m_rtExitBindBox = ExitBindBox;
}

HRESULT CLevel_Logo::Initialize()
{
	if (!g_isFirstLoading)
	{
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());
		pCamera->Start_FadeIn(1.f);

		pCamera->Set_IsEnding(false);
	}

	if (FAILED(Ready_Layer_Camera()))			// ī�޶� ���� �����Ǵ½��� 
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Ladyer_UI()))
		return E_FAIL;

	m_strCrewNameArray[0] = TEXT("������");
	m_strCrewNameArray[1] = TEXT("�ڳ���");
	m_strCrewNameArray[2] = TEXT("������");
	m_strCrewNameArray[3] = TEXT("����");

	m_strCrewExplainArray[0] = TEXT("�����ӿ�ũ\n�Ϲݸ���\n���� �� ��������\n���� : ��ٸ��� ��, ������\n");
	m_strCrewExplainArray[1] = TEXT("�÷��̾�\n�߰����� : ���ÿ�\n�߰����� : ������\n�߰����� : ����\n����Ʈ");
	m_strCrewExplainArray[2] = TEXT("����\n���� �ý���\nUI\n���� : ����");
	m_strCrewExplainArray[3] = TEXT("NPC\n����\nUI\nī�޶�");

	CGameInstance::Get_Instance()->PlayBGM(TEXT("BGM_Intro.wav"), g_fBgmVolume);
	
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_DUNGEON0))))
			return;
	}

	if (m_fTime <= 3.f)			// 3�ʿ� ���� ���İ� ǥ��
	{
		m_fTime += fTimeDelta;

		m_iAlpha = _uint((m_fTime / 3.f) * 255);

		if (m_iAlpha > 255)
			m_iAlpha = 255;
	}
	else
		m_iAlpha = 255;
	
	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());

	if (nullptr != pCamera)
	{
		if (pCamera->Get_IsShowCrew())
		{
			pCamera->Start_FadeIn();

			if (m_fCrewAlpha < 1.f)
			{
				m_fCrewAlpha += fTimeDelta;
			}
			else
			{
				m_fCrewPos += 80.f * fTimeDelta;
				m_fCrewAlpha = 1.f;
			}
		}
	}

}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("�ΰ����Դϴ�."));

	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());

	if (nullptr != pCamera)
	{
		if (pCamera->Get_IsShowCrew())
		{
			Render_Crew();
		}
		else
		{
			m_pGameInstance->RenderText(L"�׽���", &m_rtStartBindBox, D3DCOLOR_ARGB(m_iAlpha, 0, 255, 0), FONT_TYPE::MEDIUM);
			m_pGameInstance->RenderText(L"������", &m_rtNameBindBox, D3DCOLOR_ARGB(m_iAlpha, 0, 255, 0), FONT_TYPE::MEDIUM);
			m_pGameInstance->RenderText(L"������", &m_rtExitBindBox, D3DCOLOR_ARGB(m_iAlpha, 0, 255, 0), FONT_TYPE::MEDIUM);
		}
	}


	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera()
{
	 CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float3(0.f, 0.f, -15.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = D3DXToRadian(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera"),&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround()
{

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround_Player"))))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Ladyer_UI()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Title"),
		TEXT("Prototype_GameObject_Title"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Button"),
		TEXT("Prototype_GameObject_Button"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_Logo::Render_Crew()
{
	m_rtCrew.left = (LONG)(g_iWinSizeX*0.5f - 130);
	m_rtCrew.top = (LONG)(g_iWinSizeY - m_fCrewPos);
	m_rtCrew.right = (LONG)(g_iWinSizeX * 0.5f + 130);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 70 );

	//������
	m_pGameInstance->RenderTextCenter(m_strCrewNameArray[0], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::ULTRA_BIG);

	m_rtCrew.top = (LONG)(m_rtCrew.bottom + 30);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 150);
	m_pGameInstance->RenderTextCenter(m_strCrewExplainArray[0], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::MEDIUM);

	//�ڳ���
	m_rtCrew.top = (LONG)(m_rtCrew.bottom + 40);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 110);
	m_pGameInstance->RenderTextCenter(m_strCrewNameArray[1], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::ULTRA_BIG);

	m_rtCrew.top = (LONG)(m_rtCrew.bottom + 30);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 150);
	m_pGameInstance->RenderTextCenter(m_strCrewExplainArray[1], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::MEDIUM);
	
	//������
	m_rtCrew.top = (LONG)(m_rtCrew.bottom + 40);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 110);
	m_pGameInstance->RenderTextCenter(m_strCrewNameArray[2], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::ULTRA_BIG);

	m_rtCrew.top = (LONG)(m_rtCrew.bottom + 30);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 150);
	m_pGameInstance->RenderTextCenter(m_strCrewExplainArray[2], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::MEDIUM);

	//����
	m_rtCrew.top = (LONG)(m_rtCrew.bottom + 40);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 110);
	m_pGameInstance->RenderTextCenter(m_strCrewNameArray[3], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::ULTRA_BIG);

	m_rtCrew.top = (LONG)(m_rtCrew.bottom + 30);
	m_rtCrew.bottom = (LONG)(m_rtCrew.top + 150);
	m_pGameInstance->RenderTextCenter(m_strCrewExplainArray[3], &m_rtCrew, D3DCOLOR_ARGB(_uint(255 * 1), 255, 255, 255), FONT_TYPE::MEDIUM);

	if (m_rtCrew.bottom < 0.f)
	{
		CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());
		pCamera->Set_IsShowCrew(false);
		pCamera->Start_FadeOut();
	}


}

CLevel_Logo * CLevel_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
