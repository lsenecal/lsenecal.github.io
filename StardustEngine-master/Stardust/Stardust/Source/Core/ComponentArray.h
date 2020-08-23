#pragma once
#include "Types.h"
#include <array>
#include <cassert>
#include <unordered_map>

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray
{
private:
	std::array<T, MAX_ENTITIES> _componentArray{};
	std::unordered_map<Entity, size_t> _entityToIndex{};
	std::unordered_map<size_t, Entity> _indexToEntity{};
	size_t _numberOfElementInArray{};

public:

	void insertData(Entity entity, T component)
	{
		assert(_entityToIndex.find(entity) == _entityToIndex.end() && "Try to add component to the same entity more than once.");

		// put new entity at the end of the array
		size_t newIndex = _numberOfElementInArray;
		++_numberOfElementInArray;

		_entityToIndex[entity] = newIndex;
		_indexToEntity[newIndex] = entity;
		_componentArray[newIndex] = component;
	}


	void removeData(Entity entity)
	{
		assert(_entityToIndex.find(entity) != _entityToIndex.end() && "Try to remove an inexistant component.");

		// copy the removed element to the end => maintain density of array
		size_t indexOfRemovedEntity = _entityToIndex[entity];
		size_t indexOfLastElement = _numberOfElementInArray - 1;
		_componentArray[indexOfRemovedEntity] = _componentArray[indexOfLastElement];

		// update maps and 
		Entity entityOfLastElement = _indexToEntity[indexOfLastElement];
		_entityToIndex[indexOfLastElement] = indexOfRemovedEntity;
		_indexToEntity[indexOfRemovedEntity] = entityOfLastElement;

		// remove entity
		_entityToIndex.erase(entity);
		_indexToEntity.erase(indexOfLastElement);
		--_numberOfElementInArray;
	}

	T& getData(Entity entity)
	{
		assert(_entityToIndex.find(entity) != _entityToIndex.end() && "Try to get an inexistant component.");
		return _componentArray[_entityToIndex[entity]];
	}

	void entityDestroyed(Entity entity) override
	{
		if (_entityToIndex.find(entity) != _entityToIndex.end())
			removeData(entity);
	}
};
