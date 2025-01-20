#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CTexture;

class ENGINE_DLL CParticle_System abstract : public CGameObject
{
public:
	struct PARTICLE_INFO
	{
		_wstring strTextureTag;		// ������ �� ��ƼŬ �ؽ�ó �±�
		_uint	 iMaxParticle;		// �ִ�� ���� �� �ִ� ��ƼŬ ��
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
	_float  m_fScale = 0.f;					// ��� ��ƼŬ�� ũ��
	class CTexture* m_pTextureCom = nullptr;
	LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
	list<ATTRIBUTE> m_Particles;			// ��ƼŬ �Ӽ��� ����Ʈ

	PARTICLE_INFO m_Particle_Info = {};	
	DWORD m_dwVBSize = 0;					// ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ �� ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������
	DWORD m_dwVBOffset = 0;
	DWORD m_dwVBBatchSize = 0;

	_uint m_iMaxParticle = 0;				// ���� �� �ִ� �ִ� ��ƼŬ
	_float m_fParticleTimer = 0.f;          // ���ο� ��ƼŬ�� �߰��Ǵ� ����

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;

};
END
