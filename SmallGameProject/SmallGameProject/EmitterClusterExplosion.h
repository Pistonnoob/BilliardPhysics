#ifndef EMITTERCLUSTEREXPLOSION_H
#define EMITTERCLUSTEREXPLOSION_H
#include "ParticleEmitter.h"
#include <vector>
#include <algorithm>
class EmitterClusterExplosion :
	public ParticleEmitter
{
private:
	struct Particle {
		//Variables
		float originX, originZ;
		float x, y, z, scale;
		float r, g, b, uCoord;
		float dX, dZ;
		//Not the true distance but close enough
		float timeCap;
		float time;
		bool active;
		bool operator<(const Particle& that)const {
			//Sort in reverse order distance from camera > being optimal
			return this->y < that.y;
		}
	};

	struct Particle_Update {
		Particle_Update(float deltaTime) : dT{ deltaTime } { }
		float dT;
		Particle operator()(Particle node) {
			float x = 0, y = 0;
			node.time += dT;
			node.x = node.originX + node.dX * node.time;
			node.z = node.originZ + node.dZ * node.time;
			node.r = 1.0f - pow(node.time / node.timeCap, 2);
			node.g = 0.8f - node.time / node.timeCap;
			return node;
		};
	};


	float particleTimeLimit;
	float velocity;
	float velocityDeviation;
	float height;
	float particleSize;

	DirectX::XMFLOAT3 spawningPos;
	float spawningRadius;

	std::vector<Particle> particles;

public:
	EmitterClusterExplosion();
	~EmitterClusterExplosion();

	void ShutdownSpecific();

	bool Initialize(ID3D11Device* device, ID3D11ShaderResourceView* texture, float timeLimit, float spawnRadius, int particleCount);
	bool UpdateSpecific(float dT, ID3D11DeviceContext* deviceContext);
	void Render(ID3D11DeviceContext * deviceContext, ParticleShaderParameters& emitterParameters, int& amountOfParticles);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

	bool SortParticles();
private:
	bool InitializeEmitter(float spawnRadius);
	bool InitializeBuffers(ID3D11Device* device);

	void UpdateParticles(float dT);
	void KillParticles();


	bool UpdateBuffers(ID3D11DeviceContext* deviceContext);

	void RenderBuffers(ID3D11DeviceContext* deviceContext);
};

#endif