#include "GuiSystem.h"
#include "../Core/Coordinator.h"
#include "../Components/Transform.h"
#include "../Components/Physics.h"
#include "../Components/Collider.h"
#include "glm/gtc/matrix_transform.hpp"
#include <algorithm>

extern Coordinator g_coordinator;

void GuiSystem::init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 440");
}

void GuiSystem::initializeNewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GuiSystem::configure()
{
	ImGui::Begin("StarDust", NULL, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		configureMenuBar();
		ImGui::EndMenuBar();
	}
	ImGui::Text("Application average %.1f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
	
	ImGui::Begin("Entities", NULL);

	int i = 1;
	for (Entity e : _entities) {
		std::string s = "Entity " + std::to_string(i);
		if (ImGui::TreeNode(s.c_str()))
		{
			if (ImGui::TreeNode("Transform"))
			{
				if (g_coordinator.getComponent<ColliderHierarchy>(e).colliders.at(0).type != Static) {
					glm::vec3 & position = g_coordinator.getComponent<Transform>(e).position;
					glm::vec3 & rotation = g_coordinator.getComponent<Transform>(e).rotation;
					glm::vec3 & scale = g_coordinator.getComponent<Transform>(e).scale;

					if (ImGui::TreeNode("Position")) {
						ImGui::SliderFloat(("x" + std::to_string(i)).c_str(), &(position.x), -10.0f, 10.0f);
						ImGui::SliderFloat(("y" + std::to_string(i)).c_str(), &(position.y), -10.0f, 10.0f);
						ImGui::SliderFloat(("z" + std::to_string(i)).c_str(), &(position.z), -10.0f, 10.0f);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Rotation")) {
						ImGui::SliderFloat(("x" + std::to_string(i)).c_str(), &(rotation.x), 0.0f, 360.0f);
						ImGui::SliderFloat(("y" + std::to_string(i)).c_str(), &(rotation.y), 0.0f, 360.0f);
						ImGui::SliderFloat(("z" + std::to_string(i)).c_str(), &(rotation.z), 0.0f, 360.0f);

						glm::mat4 rotX = glm::mat4(1.0f);
						glm::mat4 rotY = glm::mat4(1.0f);
						glm::mat4 rotZ = glm::mat4(1.0f);
						rotX = glm::rotate(rotX, glm::radians(g_coordinator.getComponent<Transform>(e).rotation.x), glm::vec3(1, 0, 0));
						rotY = glm::rotate(rotY, glm::radians(g_coordinator.getComponent<Transform>(e).rotation.y), glm::vec3(0, 1, 0));
						rotZ = glm::rotate(rotZ, glm::radians(g_coordinator.getComponent<Transform>(e).rotation.z), glm::vec3(0, 0, 1));

						g_coordinator.getComponent<Transform>(e).orientation[0] = glm::vec3(rotZ * rotY * rotX * glm::vec4(1, 0, 0, 1));
						g_coordinator.getComponent<Transform>(e).orientation[1] = glm::vec3(rotZ * rotY * rotX * glm::vec4(0, 1, 0, 1));
						g_coordinator.getComponent<Transform>(e).orientation[2] = glm::vec3(rotZ * rotY * rotX * glm::vec4(0, 0, 1, 1));

						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Scale")) {
						ImGui::SliderFloat(("x" + std::to_string(i)).c_str(), &(scale.x), 0.0f, 10.0f);
						ImGui::SliderFloat(("y" + std::to_string(i)).c_str(), &(scale.y), 0.0f, 10.0f);
						ImGui::SliderFloat(("z" + std::to_string(i)).c_str(), &(scale.z), 0.0f, 10.0f);

						// update collider to fit object size

						if (g_coordinator.getComponent<Collider>(e).name == sphere) {
							g_coordinator.getComponent<Collider>(e).SphereCollider->radius = std::max(scale.x, std::max(scale.y, scale.z));
						}
						else if (g_coordinator.getComponent<Collider>(e).name == box) {
							g_coordinator.getComponent<Collider>(e).BoxCollider->size = scale;
						}

						ImGui::TreePop();
					}
				}
				else
					ImGui::Text("Entity is Static");
				ImGui::TreePop();
			}
			
			/*std::string name = g_coordinator.getComponent<Collider>(e).name + " Collider";

			if (ImGui::TreeNode(name.c_str()))
			{
				if (g_coordinator.getComponent<Collider>(e).name == sphere) {
					ImGui::SliderFloat("radius", &(g_coordinator.getComponent<Collider>(e).SphereCollider->radius), 0.0f, 10.0f);
				}
				else if (g_coordinator.getComponent<Collider>(e).name == box) {
					if (ImGui::TreeNode("Size")) {
						ImGui::SliderFloat(("x" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).BoxCollider->size.x), -20.0f, 20.0f);
						ImGui::SliderFloat(("y" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).BoxCollider->size.y), -20.0f, 20.0f);
						ImGui::SliderFloat(("z" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).BoxCollider->size.z), -20.0f, 20.0f);
						ImGui::TreePop();
					}
				}

				if (ImGui::TreeNode("Center")) {

					if (g_coordinator.getComponent<Collider>(e).name == sphere) {
						ImGui::SliderFloat(("x" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).SphereCollider->center.x), -20.0f, 20.0f);
						ImGui::SliderFloat(("y" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).SphereCollider->center.y), -20.0f, 20.0f);
						ImGui::SliderFloat(("z" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).SphereCollider->center.z), -20.0f, 20.0f);
					}
					else if (g_coordinator.getComponent<Collider>(e).name == box) {
						ImGui::SliderFloat(("x" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).BoxCollider->center.x), -20.0f, 20.0f);
						ImGui::SliderFloat(("y" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).BoxCollider->center.y), -20.0f, 20.0f);
						ImGui::SliderFloat(("z" + std::to_string(i)).c_str(), &(g_coordinator.getComponent<Collider>(e).BoxCollider->center.z), -20.0f, 20.0f);
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}*/

			ImGui::TreePop();
		}
		i++;
	}

	ImGui::End();

}

void GuiSystem::update()
{
	initializeNewFrame();
	configure();
	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiSystem::end()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}





void GuiSystem::configureMenuBar()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open...", "Ctrl+O")) {}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {}
		if (ImGui::MenuItem("Close", "Ctrl+W")) { glfwSetWindowShouldClose(glfwGetCurrentContext(), true); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Scale"))
	{
		if (ImGui::MenuItem("less")) {
			ImGuiStyle *style = &ImGui::GetStyle(); style->ScaleAllSizes(0.5f);
			ImGui::GetIO().FontGlobalScale = 1.0;
		}
		if (ImGui::MenuItem("more")) {
			ImGuiStyle *style = &ImGui::GetStyle(); style->ScaleAllSizes(2.0f);
			ImGui::GetIO().FontGlobalScale = 2.0;
		}
		ImGui::EndMenu();
	}
}

