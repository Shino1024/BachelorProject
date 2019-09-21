#include "ParticleObjectCreator.h"

void forceOpenFluid();

void forceOpenGlass();

void forceOpenGlassVectors();

void forceOpenDetails();

void forceOpenObjects();

void weakOpenDetails();

void weakOpenObjects();

void commitFluid();

void commitGlass();

void commitGlassVectors();

void commitDetails();

void commitObjects();

void ParticleObjectCreator::runWorkerThread()
{
	loguru::set_thread_name("ParticleObjectCreator worker");
	LOG_F(INFO, "ParticleObjectCreator::runWorkerThread");

	while (true) {
		LOG_F(INFO, "ParticleObjectCreator::runWorkerThread LOOP");
		// check for new order
		std::unique_lock<std::mutex> lock_ParticleObjectDetails(m_mutex_partObjectDetails);
		while(ParticleObjectCreator::m_ParticleObjectDetaisVector.size() == 0)	m_condVariable_partObjectDetails.wait(lock_ParticleObjectDetails);
		lock_ParticleObjectDetails.unlock();

		// take last element from vector
		ParticleObjectDetais currentDetails = ParticleObjectCreator::m_ParticleObjectDetaisVector[m_ParticleObjectDetaisVector.size()-1];
		ParticleObjectCreator::m_ParticleObjectDetaisVector.pop_back();

		LOG_F(INFO, "New object to create, type: %d", currentDetails.fluidType);


		int numOfParticles = 0;

		// if details is not open request it, we will need it later;
		weakOpenDetails();

		if (currentDetails.fluidType > 0) {
		////////////////////////////////////////////
		//		FLUID

			// open fluid array
			forceOpenFluid();

			float* fluidArray = ParticleData::m_resFluidArray;
			int* fluidTypesArray = ParticleData::m_resFluidTypesArray;

			// create fluid particles
			ParticleObjectCreator::createFluid(currentDetails, fluidArray, fluidTypesArray, numOfParticles);

			ParticleData::m_numOfAddedFluid = numOfParticles;

			// request commit
			commitFluid();

			// we have to update particle datails in sim (it might not be opened)
			forceOpenDetails();
			ParticleData::m_resDetails->numOfParticles += numOfParticles;
			commitDetails();

		}

		else {
		////////////////////////////////////////////
		//		GLASS

			// open arrays
			forceOpenGlass();
			forceOpenGlassVectors();

			float* glassPositions = ParticleData::m_resGlassArray;
			float* glassVectors = ParticleData::m_resGlassVectorsArray;

			// create particles
			ParticleObject mug;
			mug.createMug(currentDetails, glassPositions, glassVectors, numOfParticles);

			ParticleData::m_numOfAddedGlass = numOfParticles;

			// request commit
			commitGlass();
			commitGlassVectors();

			// we have to update particle datails in sim (it might not be opened)
			forceOpenDetails();
			ParticleData::m_resDetails->numOfGlassParticles += numOfParticles;
			commitDetails();

			// add object data to managet, it will send it to GPU
			ParticleObjectManager::addObject(mug);
		}

	
		LOG_F(INFO, "New object added to Simulation: type: %d, particles: %d", currentDetails.fluidType, numOfParticles);

	}
}

void forceOpenFluid() {
	if (Simulation::m_reqFluidArray != NO_ORDER) {	// wait for simulation to finish last order
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);
		while (Simulation::m_reqFluidArray != NO_ORDER) {
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
	if (ParticleData::m_resFluidArray == nullptr) {	// wait for wimulation to open array
		// request open 
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);	// take resource mutex
		while (ParticleData::m_resFluidArray == nullptr) {
			// while wanted resource in no opened -> order it and wait
			Simulation::m_reqFluidArray = OPEN;
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
}

void forceOpenGlass() {
	if (Simulation::m_reqGlassArray != NO_ORDER) {	// wait for simulation to finish last order
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);
		while (Simulation::m_reqGlassArray != NO_ORDER) {
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
	if (ParticleData::m_resFluidArray == nullptr) {
		// request open 
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);	// take resource mutex
		while (ParticleData::m_resGlassArray == nullptr) {
			// while wanted resource in no opened -> order it and wait
			Simulation::m_reqGlassArray = OPEN;
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
}

void forceOpenGlassVectors()
{
	if (Simulation::m_reqGlassVectorsArray != NO_ORDER) {	// wait for simulation to finish last order
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);
		while (Simulation::m_reqGlassVectorsArray != NO_ORDER) {
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
	if (ParticleData::m_resGlassVectorsArray == nullptr) {
		// request open 
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);	// take resource mutex
		while (ParticleData::m_resGlassVectorsArray == nullptr) {
			// while wanted resource in no opened -> order it and wait
			Simulation::m_reqGlassVectorsArray = OPEN;
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
}

void forceOpenDetails() {
	if (Simulation::m_reqDetils != NO_ORDER) {	// wait for simulation to finish last order
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);
		while (Simulation::m_reqDetils != NO_ORDER) {
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
	if (ParticleData::m_resDetails == nullptr) {
		std::unique_lock<std::mutex> lock_details(ParticleData::m_ResourceMutex);	// take resource mutex
		while (ParticleData::m_resDetails == nullptr) {
			// while wanted resource in no opened -> order it and wait
			Simulation::m_reqDetils = OPEN;
			ParticleData::m_ResourceCondVariable.wait(lock_details);
		}
		lock_details.unlock();
	}
}

void forceOpenObjects()
{
	if (Simulation::m_reqObjects != NO_ORDER) {	// wait for simulation to finish last order
		std::unique_lock<std::mutex> lock_resource(ParticleData::m_ResourceMutex);
		while (Simulation::m_reqObjects != NO_ORDER) {
			ParticleData::m_ResourceCondVariable.wait(lock_resource);
		}
		lock_resource.unlock();
	}
	if (ParticleData::m_resObjectsArray == nullptr) {
		std::unique_lock<std::mutex> lock_details(ParticleData::m_ResourceMutex);	// take resource mutex
		while (ParticleData::m_resObjectsArray == nullptr) {
			// while wanted resource in no opened -> order it and wait
			Simulation::m_reqObjects = OPEN;
			ParticleData::m_ResourceCondVariable.wait(lock_details);
		}
		lock_details.unlock();
	}
}

void weakOpenDetails() {
	if (ParticleData::m_resDetails == nullptr && Simulation::m_reqDetils == NO_ORDER) {
		Simulation::m_reqDetils = OPEN;
	}
}

void weakOpenObjects()
{
	if (ParticleData::m_resObjectsArray == nullptr && Simulation::m_reqObjects == NO_ORDER) {
		Simulation::m_reqObjects = OPEN;
	}
}

void commitFluid()
{
	Simulation::m_reqFluidArray = COMMIT;
}

void commitGlass()
{
	Simulation::m_reqGlassArray = COMMIT;
}

void commitGlassVectors()
{
	Simulation::m_reqGlassVectorsArray = COMMIT;
}

void commitDetails()
{
	Simulation::m_reqDetils = COMMIT;
}

void commitObjects()
{
	Simulation::m_reqObjects = COMMIT;
}

void ParticleObjectCreator::createFluid(ParticleObjectDetais details, float positions[], int fluidTypes[], int &numOfParts) {

	const float gap = Configuration.FLUID_PARTICLE_BUILD_GAP;

	const float startX = details.lowX;
	const float startY = details.lowY;
	const float startZ = details.lowZ;
	const float endX = details.highX;
	const float endY = details.highY;
	const float endZ = details.highZ;
	const int fluidType = details.fluidType;

	numOfParts = 0;

	for (float oX = startX; oX <= endX; oX += gap) {
		for (float oY = startY; oY <= endY; oY += gap) {
			for (float oZ = startZ; oZ <= endZ; oZ += gap) {
				positions[3 * numOfParts + 0] = oX;
				positions[3 * numOfParts + 1] = oY;
				positions[3 * numOfParts + 2] = oZ;

				fluidTypes[numOfParts] = fluidType;

				numOfParts++;
			}
		}
	}
}

void ParticleObjectCreator::init()
{
	LOG_F(INFO, "Initialize ParticleObjectCreator");
	ParticleObjectCreator::m_workerThread = std::thread(ParticleObjectCreator::runWorkerThread);
	Threads::addThreadToList(&m_workerThread);
}

void ParticleObjectCreator::addObject(ParticleObjectDetais details)
{
	LOG_F(INFO, "New object assign: type: %d", details.fluidType);
	ParticleObjectCreator::m_ParticleObjectDetaisVector.push_back(details);
	ParticleObjectCreator::m_condVariable_partObjectDetails.notify_all();
}
