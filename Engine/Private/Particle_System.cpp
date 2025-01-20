#include "Particle_System.h"
#include "GameInstance.h"

CParticle_System::CParticle_System(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CGameObject(pGraphic_Device)
{
}

CParticle_System::CParticle_System(const CParticle_System& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CParticle_System::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_System::Initialize(void* pArg)
{
    // 포인터 스프라이트를 저장하기 위한 버텍스 버퍼를 만들고 텍스처를 만드는 등
    // 장치 의존적인 초기화 작업 처리
    if (FAILED(m_pGraphic_Device->CreateVertexBuffer(
        m_dwVBSize * sizeof(VTXPARTICLE),
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
        D3DFVF_XYZ | D3DFVF_DIFFUSE,
        D3DPOOL_DEFAULT,
        &m_pVB,
        0)))
        return E_FAIL;


    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_System::Priority_Update(_float fDeltaTime)
{
}

void CParticle_System::Update(_float fDeltaTime)
{
}

void CParticle_System::Late_Update(_float fDeltaTime)
{
}

HRESULT CParticle_System::Render()
{
	if (!m_Particles.empty())
	{
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		m_pGraphic_Device->SetTransform(D3DTS_WORLD, &I);

		Set_RenderState();

		m_pTextureCom->Bind_Texture();
		m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, sizeof(VTXPARTICLE));

		if (m_dwVBOffset >= m_dwVBSize)
			m_dwVBOffset = 0;

		VTXPARTICLE* vtx = 0;

		if (FAILED(m_pVB->Lock(
			m_dwVBOffset * sizeof(VTXPARTICLE),
			m_dwVBBatchSize * sizeof(VTXPARTICLE),
			(void**)&vtx,
			m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
			return E_FAIL;

		DWORD numParticlesInBatch = 0;

		for (auto it = m_Particles.begin(); it != m_Particles.end(); ++it)
		{
			if (it->bIsAlive)
			{
				vtx->vPosition = it->vPosition;
				vtx->vColor = (D3DCOLOR)it->Color;
				vtx++;

				numParticlesInBatch++;

				if (numParticlesInBatch == m_dwVBBatchSize)
				{
					m_pVB->Unlock();

					if (FAILED(m_pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize)))
					{
						int a = 0;
					}

					m_dwVBOffset += m_dwVBBatchSize;

					if (m_dwVBOffset >= m_dwVBSize)
						m_dwVBOffset = 0;

					if (FAILED(m_pVB->Lock(m_dwVBOffset * sizeof(VTXPARTICLE), m_dwVBBatchSize * sizeof(VTXPARTICLE), (void**)&vtx, m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
					{
						int a = 0;
					}

					numParticlesInBatch = 0;
				}
			}
		}

		m_pVB->Unlock();

		if (numParticlesInBatch)
		{
			if (FAILED(m_pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticlesInBatch)))
			{
				int a = 0;
			}
		}

		m_dwVBOffset += m_dwVBBatchSize;


		Reset_RenderState();
	}

	return S_OK;
}

_bool CParticle_System::Is_Empty()
{
	// 파티클이 들어있는지 아닌지 넘겨줌
	return m_Particles.empty();
}

_bool CParticle_System::Is_Dead()
{
	// 파티클 리스트를 순회하면서 모든 파티클이 죽은 상태면 true return
	for (auto it = m_Particles.begin(); it != m_Particles.end(); ++it)
	{
		if (it->bIsAlive)
			return false;
	}
	return true;
}

void CParticle_System::Reset()
{
	// 파티클을 리셋
	for (auto iter = m_Particles.begin(); iter != m_Particles.end(); iter++)
		Reset_Particle((*iter));
}

void CParticle_System::Add_Particle()
{
	ATTRIBUTE Attribute;
	// 예의상 한번 ZeroMemory랑 비슷
	Reset_Particle(Attribute);

	m_Particles.push_back(Attribute);
}

void CParticle_System::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE, Convert<_float, DWORD>(m_fScale));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSIZE_MIN, Convert<_float, DWORD>(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_A, Convert<_float, DWORD>(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_B, Convert<_float, DWORD>(0.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALE_C, Convert<_float, DWORD>(1.0f));
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CParticle_System::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CParticle_System::Remove_Dead_Particle()
{
	for (auto iter = m_Particles.begin(); iter != m_Particles.end();)
	{
		if (!iter->bIsAlive)
			iter = m_Particles.erase(iter);

		else
			++iter;
	}
}

HRESULT CParticle_System::Ready_Components()
{
    if (FAILED(__super::Add_Component(0, m_Particle_Info.strTextureTag, TEXT("Com_Texture"), (CComponent**)(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

void CParticle_System::Free()
{
    __super::Free();
    Safe_Release(m_pVB);
    Safe_Release(m_pTextureCom);
}
