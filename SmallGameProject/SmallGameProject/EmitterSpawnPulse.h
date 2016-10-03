#ifndef EMITTERSPAWNPULSE_H
#define EMITTERSPAWNPULSE_H
#include "ParticleEmitter.h"
#include <vector>
#include <algorithm>
class EmitterSpawnPulse :
	public ParticleEmitter
{
private:
	struct Particle {
		//Variables
		float x, y, z, scale;
		float minScale, maxScale;
		float r, g, b, uCoord;
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
			float x = 0.0f, y = 0.0f;
			node.time += dT;
			//Let it reach the max 6 times during its lifetime
			Algorithm::GetSawtoothWave(x, y, node.time, node.timeCap / 6.0f, node.minScale, node.maxScale);
			node.x = x;
			node.y = y;
			node.active = node.time < node.timeCap;
			return node;
		};
		//void operator()(Particle& element) { element.time += dT; Algorithm::GetEllipse(element.x, element.y, element.time * element.velocity, element.time, element.time); };
	};

	struct sort_by_Y {
		bool operator()(const Particle &left, const Particle &right)const {
			return left.y < right.y;
		}
	};

	float particleTimeLimit;
	float particleSize;
	float height;
	std::vector<Particle> particles;
public:
	EmitterSpawnPulse();
	~EmitterSpawnPulse();

	void ShutdownSpecific();

	bool AddSpawnPulse(float x, float y, float z, float minSize, float maxSize, float r, float g, float b, float time);

	bool Initialize(ID3D11Device* device, ID3D11ShaderResourceView* texture, float timeLimit = 1.0f);
	bool UpdateSpecific(float dT, ID3D11DeviceContext* deviceContext);
	void Render(ID3D11DeviceContext * deviceContext, ParticleShaderParameters& emitterParameters, int& amountOfParticles);

	int GetIndexCount();
	bool SortParticles();

	virtual bool IsCompleted();
private:
	bool InitializeEmitter();
	bool InitializeBuffers(ID3D11Device* device);

	void UpdateParticles(float dT);
	void KillParticles();

	bool UpdateBuffers(ID3D11DeviceContext* deviceContext);

	void RenderBuffers(ID3D11DeviceContext* deviceContext);
};

#endif