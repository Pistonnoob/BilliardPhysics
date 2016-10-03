#ifndef EMITTERPTROTOTYPE_H
#define EMITTERPROTOTYPE_H
#include "ParticleEmitter.h"
#include "Particle.h"
#include <algorithm>
class EmitterPrototype :
	public ParticleEmitter
{
private:

	//struct Particle {
	//	float x, y, z, scale;
	//	float r, g, b, rotation;
	//	float velocity;
	//	//Not the true distance but close enough
	//	float cameraDistance;
	//	bool active;

	//	bool operator<(const Particle& that)const {
	//		//Sort in reverse order distance from camera > being optimal
	//		return this->cameraDistance > that.cameraDistance;
	//	}
	//};

	struct by_cameraPos {
		bool operator()(const Particle &left, const Particle &right) {
			return left.cameraDistance > right.cameraDistance;
		}
	};


	float particleDeviationX, particleDeviationY, particleDeviationZ, particleVelocityVariation;
	float particleVelocity;
	float particleSize, particlesPerSecond;

	Particle* particles;
	/*VertexType* vertices;*/

public:
	EmitterPrototype();
	virtual ~EmitterPrototype();
	void ShutdownSpecific();

	bool Initialize(ID3D11Device* device, ID3D11ShaderResourceView* texture, float timeLimit = 10.0f);
	bool UpdateSpecific(float dT, ID3D11DeviceContext* deviceContext);
	void Render(ID3D11DeviceContext * deviceContext, ParticleShaderParameters& emitterParameters, int& amountOfParticles);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

	bool SortParticles();

private:
	bool InitializeEmitter();
	bool InitializeBuffers(ID3D11Device* device);

	void EmittParticles(float dT);
	void UpdateParticles(float dT);
	void KillParticles();


	bool UpdateBuffers(ID3D11DeviceContext* deviceContext);

	void RenderBuffers(ID3D11DeviceContext* deviceContext);
};

#endif