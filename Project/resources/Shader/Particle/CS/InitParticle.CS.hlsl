#include "../Particle.hlsli"


RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gCounter : register(u1);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    
    if (particleIndex == 0)
    {
        gCounter[0] = 0;
    }
    
    if (particleIndex < kMaxParticles)
    {
        gParticles[particleIndex] = (Particle) 0;
       
    }

}