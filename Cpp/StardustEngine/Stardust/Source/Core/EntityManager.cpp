#include "EntityManager.h"



EntityManager::EntityManager()
{
	// initialize all living entities
	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		_availableEntities.push(entity);
}

EntityManager::~EntityManager() {}

Entity EntityManager::createEntity()
{
	assert(_livingEntitiesNumber < MAX_ENTITIES && "There is too many living entities.");

	// equivalent to Entity id = _availableEntities.peek();
	Entity id = _availableEntities.front();
	_availableEntities.pop();

	// increment number of living entities
	++_livingEntitiesNumber;
	return id;
}

void EntityManager::destroyEntity(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity ID is out of range.");

	// reset bit to zero for the entity index in _signatures
	_signatures[entity].reset();

	// add entity at the end of the queue
	_availableEntities.push(entity);

	// decrement number of living entities
	--_livingEntitiesNumber;
}

void EntityManager::setSignature(Entity entity, Signature signature)
{
	assert(entity < MAX_ENTITIES && "Entity ID is out of range.");
	_signatures[entity] = signature;
}

Signature EntityManager::getSignature(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity ID is out of range.");
	return _signatures[entity];
}
