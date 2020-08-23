#pragma once
#include "System.h"
#include "Types.h"
#include <cassert>
#include <memory>
#include <unordered_map>

class SystemManager
{
private:
	std::unordered_map<const char *, Signature> _signatures{};
	std::unordered_map<const char *, std::shared_ptr<System>> _systems{};

public:
	template<typename T>
	std::shared_ptr<T> registerSystem()
	{
		const char * typeName = typeid(T).name();
		assert(_systems.find(typeName) == _systems.end() && "This system is already defined.");

		auto system = std::make_shared<T>();
		_systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void setSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();
		assert(_systems.find(typeName) != _systems.end() && "Try to use system before his definition.");
		_signatures.insert({ typeName, signature });
	}

	void entityDestroyed(Entity entity)
	{
		// for each system, erase the entity in the set
		for (auto const & pair : _systems)
			pair.second->_entities.erase(entity);
	}

	void entitySignatureChanged(Entity entity, Signature entitySignature)
	{
		// for each system, change signature of the entity
		for (auto const& pair : _systems)
		{
			auto const& name = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = _signatures[name];

			if ((entitySignature & systemSignature) == systemSignature)
				system->_entities.insert(entity);
			else
				system->_entities.erase(entity);
		}
	}
};