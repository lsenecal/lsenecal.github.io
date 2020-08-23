#pragma once
#include "ComponentArray.h"
#include "Types.h"
#include <any>
#include <memory>

class ComponentManager
{
private:
	std::unordered_map<const char *, ComponentType> _componentTypes{};
	std::unordered_map<const char *, std::shared_ptr<IComponentArray>> _componentArrays{};
	ComponentType _nextComponentType;

	template <typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
		const char * typeName = typeid(T).name();
		assert(_componentTypes.find(typeName) != _componentTypes.end() && "This component is not registered before use.");
		return std::static_pointer_cast<ComponentArray<T>>(_componentArrays[typeName]);
	}

public:
	template <typename T>
	void registerComponent()
	{
		const char * typeName = typeid(T).name();
		assert(_componentTypes.find(typeName) == _componentTypes.end() && "Try to register component type more than once.");

		// add new type of component and insert it to componant's arrays
		_componentTypes.insert({ typeName, _nextComponentType });
		_componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		++_nextComponentType;
	}

	template <typename T>
	ComponentType getComponentType()
	{
		const char * typeName = typeid(T).name();
		assert(_componentTypes.find(typeName) != _componentTypes.end() && "This component is not registered before use.");

		// return ComponentType of the type T
		return _componentTypes[typeName];
	}

	template <typename T>
	void addComponent(Entity entity, T component)
	{
		// get ComponentArray of T and insert to it the entity and the component
		getComponentArray<T>()->insertData(entity, component);
	}

	template <typename T>
	void removeComponent(Entity entity)
	{
		// get ComponentArray of T and removeData of the entity
		getComponentArray<T>()->removeData(entity);
	}

	template <typename T>
	T& getComponent(Entity entity)
	{
		// get ComponentArray of T and get data of the entity
		return getComponentArray<T>()->getData(entity);
	}

	void entityDestroyed(Entity entity)
	{
		// for each component, notify that the entity is detroyed
		for (auto const & pair : _componentArrays)
			pair.second->entityDestroyed(entity);
	}
};