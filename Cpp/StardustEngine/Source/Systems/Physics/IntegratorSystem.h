#pragma once
#include "../../Core/System.h"
#include "../../Core/Coordinator.h"

extern Coordinator g_coordinator;

class IntegratorSystem : public System {
public : 
	void update();
};