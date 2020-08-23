#include "GuiSystem.h"

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

