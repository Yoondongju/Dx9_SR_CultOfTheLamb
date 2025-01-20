#include "../Public/TalkPanelUI.h"
#include "GameInstance.h"

#include "QuestOrnamentUI.h"
#include "Player.h"

#include "MonologueUI.h"
#include "Camera.h"

#include "Boss_DeathCat.h"


CTalkPanelUI::CTalkPanelUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CTalkPanelUI::CTalkPanelUI(const CTalkPanelUI& Prototype)
	: CUI{ Prototype }
{
	

}

HRESULT CTalkPanelUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTalkPanelUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Speech()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	TALK_DESC* pDesc = reinterpret_cast<TALK_DESC*>(pArg);

	m_fX = pDesc->UIDesc.m_vPos.x;
	m_fY = pDesc->UIDesc.m_vPos.y;

	m_fSizeX = pDesc->UIDesc.m_vSize.x;
	m_fSizeY = pDesc->UIDesc.m_vSize.y;
	
	m_strNameOpponent = pDesc->strNameOpponent;
	m_strLayerOpponent = pDesc->strLayerOpponent;

	m_rtStrRange = {
				(LONG)(m_fX - (m_fSizeX / 2.f) + 40.f),
				(LONG)(m_fY - (m_fSizeY / 2.f) + 100.f),
				(LONG)(m_fX + (m_fSizeX / 2.f)),
				(LONG)(m_fY + (m_fSizeY / 2.f))
	};



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* ������������� ������ش�. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);



	UI_DESC Desc = {};
	Desc.m_vPos =
	{
		m_fX + (m_fSizeX / 2.f) - 50.f ,
		m_fY + (m_fSizeY / 2.f) - 55.f 
	};
	Desc.m_vSize = { 30.f,30.f };

	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TalkIconUI"));
	if (nullptr != pPrototype)	// �����Ѵٸ�
	{
		CUI* pArrowtUI = dynamic_cast<CUI*>(pPrototype->Clone(&Desc)); // ������Ÿ���� �±׸��� ���� ��ü�� �� Ÿ���� �����Ҽ������� ����.
	
		m_childUI_List.push_back(pArrowtUI);
		pArrowtUI->SetParentUI(this);
	}

	
	m_bActivate = pDesc->UIDesc.bActivate;

	return S_OK;
}

void CTalkPanelUI::Priority_Update(_float fTimeDelta)
{	
	if (m_bTalkEnd)
	{
		if (m_fAlpha > 0.f)
		{
			m_fAlpha -= 0.04f;
		}
					
		if (m_fAlpha < 0.f)
		{
			m_fAlpha = 0.f;
			m_bActivate = false;

			CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());
			pCam->Target_to_Player();
			pCam->Zoom_Out(2.f);

				
			if (L"Layer Boss" == m_strLayerOpponent)
			{
				CMonologueUI::MONOLOGUE_DESC Desc = {};
				Desc.strText = m_strNameOpponent;
				Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - (m_strNameOpponent.size() * 17.f)),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 300.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
				};

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
					MSG_BOX(TEXT("Can't Create MonologueUI"));
			}
			

			//m_pGameInstance->Event_DestoryObject(this); //  ������ ��Ȱ��ȭ ���� ����غ��� �ϴ� ��Ȱ��ȭ�Ұ� �׷��� ���� �� ��ȭ�� ������ ����Ҽ��־�

		}
	}


	

	if (!m_bActivate)
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}


void CTalkPanelUI::Update(_float fTimeDelta)
{	
	if (!m_bActivate)
		return;

	m_fTime += fTimeDelta;
	
	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}
}

void CTalkPanelUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CTalkPanelUI::Render()
{
	if (!m_bActivate)
		return S_OK;


	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();


	

	if (true == TalkEnd_Opponent())
	{
		// ��ȭ�� ������
		LEVELID eCurLevel = (LEVELID)m_pGameInstance->Get_LevelIndex();

		switch (eCurLevel)
		{
		case Client::LEVEL_STATIC:
			break;
		case Client::LEVEL_LOADING:
			break;
		case Client::LEVEL_LOGO:
			break;
		case Client::LEVEL_GAMEPLAY:
			break;
		case Client::LEVEL_DUNGEON0:
			if (m_iCurTalkOrder >= m_vecSpeech[eCurLevel].size())		//  ��ȭ�߰��Ҷ� 1�����߰��ϸ� ��������Ű�����?��
			{
				m_bTalkEnd = true;
			}
			break;
		case Client::LEVEL_DUNGEON1:
			break;
		case Client::LEVEL_DUNGEON2:
			break;
		case Client::LEVEL_DUNGEON3:
		{
		
			CGameObject* pAim = m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_Boss", 0);			// 0�� ���� 1�� ������
			CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

			
		
			if (!m_bCheckFlag_CreateAim)
			{
				m_bActivate = false;

				CGameObject* pDeathCat = m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_Boss", 1);
				dynamic_cast<CBoss_DeathCat*>(pDeathCat)->Change_State(CBoss_DeathCat::DISAPPEAR);

				m_vecSpeech[eCurLevel].push_back(L"���δ��� �� ���� �ʸ� ���ַ� �Դ�, ��, ���� ������ ���� ������ ���� ���̴�.");
				m_vecSpeech[eCurLevel].push_back(L"���� ���δ��� ���� ��� ���� ���ƴ�.�׸��� �ʴ� ���� ���� ���� �� ���� �غ��ض�, ��. ���ݺ��� ���� ������ ���۵ȴ�.");

				pCam->Target_to(pAim);

				m_bCheckFlag_CreateAim = true;
			}
		
			if (!m_bCheckFlag_CamZoom)
			{
				if (pCam->IsFinishZoomIn())
				{					
					m_bActivate = true;
					m_strNameOpponent = L"����";
		
					m_bCheckFlag_CamZoom = true;
				}
			}


			
 			if (m_iCurTalkOrder >= m_vecSpeech[eCurLevel].size())		//  ��ȭ�߰��Ҷ� 1�����߰��ϸ� ��������Ű�����?��
			{
				m_bTalkEnd = true;
			}
					

		}
			break;
		case Client::LEVEL_END:
			break;
		default:
			break;
		}
	}
	else
	{
		m_fAlpha = 1.f;
		m_bTalkEnd = false;
	}
	
	
	

	



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}


bool CTalkPanelUI::TalkEnd_Opponent()
{
	_uint iCurLevel = m_pGameInstance->Get_LevelIndex();

	RECT rt = {
					(LONG)(m_fX - (m_strNameOpponent.size() * 15.f)),
					(LONG)(m_fY - 100.f),
					(LONG)(m_fX + 400),
					(LONG)(m_fY + (m_fSizeY / 2.f))
	};
	m_pGameInstance->RenderText(m_strNameOpponent, &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 255, 0), FONT_TYPE::BIG);



	static _uint iszCount = 0;

	if (m_iCurTalkOrder < m_vecSpeech[iCurLevel].size())
	{
	
		_float2	vFontSize = m_pGameInstance->Get_FontSize(FONT_TYPE::MEDIUM);


		_int iCount = 0;

		_int iCurX = m_rtStrRange.left;
		_int iCurY = m_rtStrRange.top;

		for (_uint i = 0; i < iszCount; ++i)
		{
			if (m_vecSpeech[iCurLevel][m_iCurTalkOrder][i] == L'\n' || iCount > 20)
			{
				iCurX = (_int)m_rtStrRange.left;
				iCurY += (_int)vFontSize.y + 5;

				iCount = 0;

				if (m_vecSpeech[iCurLevel][m_iCurTalkOrder][i] == L'\n')
					continue;
			}

			RECT rt = {
				iCurX,
				iCurY,
				m_rtStrRange.right,
				m_rtStrRange.bottom,
			};

			m_pGameInstance->RenderText(m_vecSpeech[iCurLevel][m_iCurTalkOrder][i], &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 255, 255), FONT_TYPE::MEDIUM);

			++iCount;
			iCurX += _int(vFontSize.x * 2);
		}


		if (iszCount < m_vecSpeech[iCurLevel][m_iCurTalkOrder].size())
		{
			if (m_fTime > 0.05f)
			{
				iszCount++;
				m_fTime = 0.f;
			}		
		}
		else
		{
			if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
			{
				++m_iCurTalkOrder;
				iszCount = 0;			// iszCount �ʱ�ȭ
				if(m_pGameInstance->Check_IsPlaying(SOUND_UI)==false)
 					m_pGameInstance->Play_Sound(TEXT("UI_DialogueNext.wav"), SOUND_UI, g_fEffectVolume);
			}
		}
	}
	else
	{
		iszCount = 0;
		return true;		
	}

	return false;
}



void CTalkPanelUI::Init_TargetPos(_float fTimeDelta)
{
}


HRESULT CTalkPanelUI::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Talk"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CTalkPanelUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CTalkPanelUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT CTalkPanelUI::Ready_Speech()
{
	m_vecSpeech->reserve(LEVELID::LEVEL_END);



	for (_uint i = 0; i < LEVELID::LEVEL_END; i++)
	{
		switch (i)
		{
		case Client::LEVEL_STATIC:
			break;
		case Client::LEVEL_LOADING:
		{
			m_vecSpeech[i].push_back(L"Loading ��ٸ��� �����\n�׽�Ʈ 123456789");
		}
			break;
		case Client::LEVEL_LOGO:
			break;
		case Client::LEVEL_GAMEPLAY:
		{
	
		}
			break;
		case Client::LEVEL_DUNGEON0:
			m_vecSpeech[i].push_back(L"�� �̸��� ��Ÿ��");
			break;
		case Client::LEVEL_DUNGEON1:
			break;
		case Client::LEVEL_DUNGEON2:
			break;
		case Client::LEVEL_DUNGEON3:
		{
			m_vecSpeech[i].push_back(L"����� ���̿�,\n���� �� ������ ������ �����̴϶�.");
			m_vecSpeech[i].push_back(L"���� �������� ���� ��ȹ�� ���� ������ ���̴�.\n �׷��� �װ� ������� �´ٰ�..\n�װ� ���� ��ȹ�� ������ �� �ִٰ� �����ϴ���?");
			m_vecSpeech[i].push_back(L"����ֱ�,������ ���� ���� ������ ����\n������ ����ؾ� �� ���̴�.");
			m_vecSpeech[i].push_back(L"�װ� ���� ������ ������ ���̴�.\n����, ���Ͷ�!");
		}
			break;
		case Client::LEVEL_END:
			break;
		default:
			break;
		}
	}
	




	return S_OK;
}



void CTalkPanelUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CTalkPanelUI"));
}

CTalkPanelUI* CTalkPanelUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTalkPanelUI* pInstance = new CTalkPanelUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTalkPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* ������ü�� ã�Ƽ� ������ü�� Clone�Լ��� ȣ���Ѵ�.  */
CGameObject* CTalkPanelUI::Clone(void* pArg)
{
	CTalkPanelUI* pInstance = new CTalkPanelUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTalkPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalkPanelUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
