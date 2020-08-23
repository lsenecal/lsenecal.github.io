#include "MasterPhysics.h"  
#include "constantes.h"
#include <iostream>

extern Coordinator g_coordinator;

MasterPhysics::MasterPhysics()
{
	t_initial = std::chrono::system_clock::now();
	last_update_time = t_initial;

	std::time_t end_time = std::chrono::system_clock::to_time_t(t_initial);

	_gravitySystem = g_coordinator.registerSystem<GravitySystem>();
	_collisionDetectionSystem = g_coordinator.registerSystem<CollisionDetectionSystem>();
	_collisionResolutionSystem = g_coordinator.registerSystem<CollisionResolutionSystem>();
	_integratorSystem = g_coordinator.registerSystem<IntegratorSystem>();

	Signature gravitySignature;
	gravitySignature.set(g_coordinator.getComponentType<Transform>());
	gravitySignature.set(g_coordinator.getComponentType<RigidBody>());

	Signature collisionDetectionSignature;
	collisionDetectionSignature.set(g_coordinator.getComponentType<Transform>());
	//collisionDetectionSignature.set(g_coordinator.getComponentType<Collider>());
	collisionDetectionSignature.set(g_coordinator.getComponentType<ColliderHierarchy>());
	collisionDetectionSignature.set(g_coordinator.getComponentType<RigidBody>());

	Signature collisionResolutionSignature;
	collisionResolutionSignature.set(g_coordinator.getComponentType<Transform>());
	collisionResolutionSignature.set(g_coordinator.getComponentType<RigidBody>());
	//collisionResolutionSignature.set(g_coordinator.getComponentType<Collider>());
	collisionResolutionSignature.set(g_coordinator.getComponentType<ColliderHierarchy>());

	Signature integratorSignature;
	integratorSignature.set(g_coordinator.getComponentType<Transform>());
	integratorSignature.set(g_coordinator.getComponentType<RigidBody>());

	g_coordinator.setSystemSignature<GravitySystem>(gravitySignature);
	g_coordinator.setSystemSignature<CollisionDetectionSystem>(collisionDetectionSignature);
	g_coordinator.setSystemSignature<CollisionResolutionSystem>(collisionResolutionSignature);
	g_coordinator.setSystemSignature<IntegratorSystem>(integratorSignature);
}

MasterPhysics::~MasterPhysics()
{
}

void MasterPhysics::update()
{
	/*float h = (float)physicsInterval; // pas d'integration

	auto current_time = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = current_time - last_update_time;
	float t = (float)elapsed_seconds.count();

	int n = (int)round(t / h); // number of physics update needed since last frame

	if (n != 0)
		last_update_time = std::chrono::system_clock::now(); //TODO last_update += n*step plutôt que now, peut-être ?

	for (int i = 0; i < n; i++) {
		//_gravitySystem->update();
		//_integratorSystem->update();
		//_collisionDetectionSystem->update();
		_collisionResolutionSystem->resolve(_collisionDetectionSystem->update());
	}*/
	
	_collisionResolutionSystem->resolve(_collisionDetectionSystem->update());
}
