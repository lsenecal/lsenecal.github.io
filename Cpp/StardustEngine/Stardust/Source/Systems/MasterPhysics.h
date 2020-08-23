#pragma once
#include "Physics/GravitySystem.h"
#include "Physics/CollisionDetectionSystem.h"
#include "Physics/CollisionResolutionSystem.h"
#include "Physics/IntegratorSystem.h"
#include "../Core/Types.h"
#include "../Core/Coordinator.h"
#include "../Components/Collider.h"
#include "../Components/Physics.h"
#include "../Components/Transform.h"
#include <chrono>
#include <ctime>

class MasterPhysics {
public:
	MasterPhysics();
	~MasterPhysics();
	void update();
private:
	std::shared_ptr<GravitySystem> _gravitySystem;
	std::shared_ptr<CollisionDetectionSystem> _collisionDetectionSystem;
	std::shared_ptr<CollisionResolutionSystem> _collisionResolutionSystem;
	std::shared_ptr<IntegratorSystem> _integratorSystem;
	std::chrono::time_point<std::chrono::system_clock> t_initial;
	std::chrono::time_point<std::chrono::system_clock> last_update_time;
};