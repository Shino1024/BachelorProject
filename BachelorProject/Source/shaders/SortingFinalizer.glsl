#version 430 core

#define NUM_THREADS 1
#define MAX_FLUID 8192
#define MAX_SCENE_X 200
#define MAX_SCENE_Y 200
#define MAX_SCENE_Z 200
#define SORT_ARRAY_SIZE 2*MAX_FLUID

#define INSERT_VARIABLES_HERE


layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;


//////////////////////////////////////////////////
//	STORAGE

layout(std430, binding = 9) buffer sortingHelpBuf
{
	uint sortIndexArray[SORT_ARRAY_SIZE];
	uint originalIndex[SORT_ARRAY_SIZE];
	float	CPY_Positions[3 * MAX_FLUID];
	float	CPY_Velocity[3 * MAX_FLUID];
	int		CPY_FluidTypes[MAX_FLUID];
};

layout(std430, binding = 1) buffer positionsBuf
{
	float fluidPositions[3*MAX_FLUID];
};

layout(std430, binding = 2) buffer partFluidTypeBuf
{
	int particleFluidType[MAX_FLUID];
};


layout(std430, binding = 6) buffer detailsBuf
{
	uint numOfParticles;
	uint numOfGlassParticles;
};

layout(std430, binding = 3) buffer glassPositionsBuf
{
	float glassPositions[3*MAX_FLUID];
};

uint getCellIndex(in float pX, in float pY, in float pZ);

void main(void)
{
	const uint myThreadNumber = gl_WorkGroupID.x * gl_WorkGroupSize.x
			+ gl_LocalInvocationIndex;
	//if(particleFluidType[myThreadNumber] != 0) {
		sortIndexArray[myThreadNumber] = getCellIndex(fluidPositions[3*myThreadNumber + 0]
											, fluidPositions[3*myThreadNumber + 1]
											, fluidPositions[3*myThreadNumber + 2]);
	//}
}
/*
in uvec3 gl_NumWorkGroups;
in uvec3 gl_WorkGroupID; 		== contains the index of the workgroup currently being operated on by a compute shader
in uvec3 gl_LocalInvocationID;
in uvec3 gl_GlobalInvocationID	== contains the global index of work item currently being operated on by a compute shader
in uint  gl_LocalInvocationIndex;
in uvec3 gl_WorkGroupSize		== layout
*/