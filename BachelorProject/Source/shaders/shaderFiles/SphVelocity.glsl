#version 430 core

#define MAX_FLUID 3072
#define MAX_GLASS 3072
#define MAX_FLUID_TYPES 10
#define MAX_PARTICLE_OBJECTS 10
#define MAX_SPEED 1
#define SORT_ARRAY_SIZE 2*MAX_FLUID
#define MAX_SCENE_X 200
#define MAX_SCENE_Y 200
#define MAX_SCENE_Z 200
#define DELTA_TIME 0.0005
#define GRAVITY_Y -3000
#define VELOCITY_FACTOR 1.0f
#define BOUNCE_DISTANCE 0.5f
#define BOUNCE_VELOCITY_MULTIPLIER 0.5f
#define MAX_PARTICLE_SPEED 0.5f

#define M_PI 3.1415926535897932384626433832795

#define INSERT_VARIABLES_HERE

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

//////////////////////////////////////////////////
//	STORAGE

struct FluidParticle {
	float x, y, z;
	int type;
};


layout(std430, binding = 1) buffer positionsBuf
{
	FluidParticle fluidPositions[MAX_FLUID];
};

layout(std430, binding = 4) buffer detailsBuf
{
	uint numOfParticles;
	uint numOfGlassParticles;
};

layout(std430, binding = 8) buffer simVariablesBuf
{
	float fluidVelocity[3 * MAX_FLUID];
	float fluidAcceleration[3 * MAX_FLUID];
	float fluidSurfaceVector[3 * MAX_FLUID];
	float fluidSurfaceDistance[MAX_FLUID];
};
//
//layout(std430, binding = 9) buffer lookUpTableBuf
//{
//	int indexMap[MAX_SCENE_X * MAX_SCENE_Y * MAX_SCENE_Z];	// array index of neighbours of set particle
//};

//////////////////////////////////////////////////

float Kernel(in float x) {   
	if(x >= 1) return 0.0;
	return 315 * pow(1 - pow(x, 2), 3) / (64 * M_PI);
}

float KernelDerivative(in float x) {  
	if(x >= 1) return 0.0;
	return  -45 * pow(1 - x, 2) / (M_PI);
}

float KernelSecondDerivative(in float x) {  
	if(x >= 1) return 0.0;
	return 45 * (1 - x) / M_PI;         
}


void main(void)
{
	const uint myThreadNumber = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationIndex;
	if(myThreadNumber>=numOfParticles) return;

	const FluidParticle myFluid = fluidPositions[myThreadNumber];

	if(myFluid.type<0) return;

	vec3 pPosition = vec3(myFluid.x, myFluid.y, myFluid.z); 

	vec3 pVelocity = vec3(fluidVelocity[3*myThreadNumber+0], fluidVelocity[3*myThreadNumber+1], fluidVelocity[3*myThreadNumber+2]); 

	//const vec3 pAcceleration = vec3(0,GRAVITY_Y,0);
	const vec3 pAcceleration = vec3(fluidAcceleration[3*myThreadNumber+0], fluidAcceleration[3*myThreadNumber+1] + GRAVITY_Y, fluidAcceleration[3*myThreadNumber+2]); 

	pVelocity = (pVelocity + pAcceleration * DELTA_TIME);
	if(fluidSurfaceDistance[myThreadNumber] < BOUNCE_DISTANCE) {	// BOUNCE
			//	https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector

		const vec3 pSurfVec = vec3(fluidSurfaceVector[3*myThreadNumber+0], fluidSurfaceVector[3*myThreadNumber+1], fluidSurfaceVector[3*myThreadNumber+2]); 
		float dotProduct = dot(pVelocity, pSurfVec);
		if(dotProduct < 0) {	// angle is greater than 90deg, velocity points into a glass
			pVelocity = (pVelocity - 2 * (dotProduct) * pSurfVec) * BOUNCE_VELOCITY_MULTIPLIER;
		}

//		if(length(pVelocity) * DELTA_TIME < MAX_PARTICLE_SPEED * 0.1) {
//			pVelocity = normalize(pVelocity) * MAX_PARTICLE_SPEED * 0.1 / DELTA_TIME;
//		}
	}

	if(length(pVelocity) * DELTA_TIME > MAX_PARTICLE_SPEED) {
		pVelocity = normalize(pVelocity) * MAX_PARTICLE_SPEED / DELTA_TIME;
	}

	pPosition = pPosition + pVelocity * DELTA_TIME;

	fluidVelocity[3*myThreadNumber+0] = pVelocity.x;
	fluidVelocity[3*myThreadNumber+1] = pVelocity.y;
	fluidVelocity[3*myThreadNumber+2] = pVelocity.z;

	fluidPositions[myThreadNumber].x = pPosition.x;
	fluidPositions[myThreadNumber].y = pPosition.y;
	fluidPositions[myThreadNumber].z = pPosition.z;

	//indexMap[sortIndexArray[myThreadNumber]] = 0;
}


/*
in uvec3 gl_NumWorkGroups;
in uvec3 gl_WorkGroupID; 		== contains the index of the workgroup currently being operated on by a compute shader
in uvec3 gl_LocalInvocationID;
in uvec3 gl_GlobalInvocationID	== contains the global index of work item currently being operated on by a compute shader
in uint  gl_LocalInvocationIndex;
in uvec3 gl_WorkGroupSize		== layout
*/