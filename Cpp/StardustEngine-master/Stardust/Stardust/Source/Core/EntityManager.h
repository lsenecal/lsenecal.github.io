#pragma once
#include "Types.h"
#include <array>
#include <cassert>
#include <queue>

class EntityManager
{
private:
	std::queue<Entity> _availableEntities{};
	std::array<Signature, MAX_ENTITIES> _signatures{};
	uint32_t _livingEntitiesNumber{};

public:
	EntityManager();
	~EntityManager();

	Entity createEntity();
	void destroyEntity(Entity entity);
	void setSignature(Entity entity, Signature signature);
	Signature getSignature(Entity entity);
};

