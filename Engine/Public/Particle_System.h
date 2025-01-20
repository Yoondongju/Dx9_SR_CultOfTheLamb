#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CTexture;

class ENGINE_DLL CParticle_System abstract : public CGameObject
{
public:
	struct PARTICLE_INFO
	{
		_wstring strTextureTag;		// 가지고 올 파티클 텍스처 태그
		_uint	 iMaxParticle;		// 최대로 가질 수 있는 파티클 수
	};

protected:
	CParticle_System(LPDIRECT3DDEVICE9 pGraphic_Device);
	CParticle_System(const CParticle_System& Prototype);
	virtual ~CParticle_System() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fDeltaTime) override;
	virtual void	Update(_float fDeltaTime) override;
	virtual void	Late_Update(_float fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	_bool Is_Empty();
	_bool Is_Dead();

protected:
	virtual void Reset();
	virtual void Reset_Particle(ATTRIBUTE& pAttribute) PURE;
	virtual void Add_Particle();
	virtual void Set_RenderState();
	virtual void Reset_RenderState();
	virtual void Remove_Dead_Particle();

private:
	HRESULT Ready_Components();

protected:
	_float3 m_vOriginPos = { 0.f,0.f,0.f };
	_float  m_fScale = 0.f;					// 모든 파티클의 크기
	class CTexture* m_pTextureCom = nullptr;
	LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
	list<ATTRIBUTE> m_Particles;			// 파티클 속성의 리스트

	PARTICLE_INFO m_Particle_Info = {};	
	DWORD m_dwVBSize = 0;					// 버텍스 버퍼가 보관할 수 있는 파티클 수 실제 파티클 시스템 내의 파티클 수와는 독립적
	DWORD m_dwVBOffset = 0;
	DWORD m_dwVBBatchSize = 0;

	_uint m_iMaxParticle = 0;				// 가질 수 있는 최대 파티클
	_float m_fParticleTimer = 0.f;          // 새로운 파티클이 추가되는 비율

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;

};
END
