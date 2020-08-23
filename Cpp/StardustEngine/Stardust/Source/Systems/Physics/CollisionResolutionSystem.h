#pragma once
#include <vector>
#include "../../Core/System.h"
#include "../../Core/Types.h"
#include "CollisionData.h"

class CollisionResolutionSystem : public System {
public:
	void resolve(std::vector<CollisionReport> pairs);
};