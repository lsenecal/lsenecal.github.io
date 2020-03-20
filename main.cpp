#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Bezier.h"
#include "shader.hpp"
#include <string>

#include <iostream>

using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool nearlyEquals(double a, float b, float c);

int main(void) {

	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	unsigned int w_width = 800;
	unsigned int w_height = 800;

	unsigned int xrange = 10;
	unsigned int yrange = 10;

	window = glfwCreateWindow(w_width, w_height, "Bezier Curve", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint vaoCurve, vaoCP, vaoP;
	glGenVertexArrays(1, &vaoCurve);
	glGenVertexArrays(1, &vaoCP);
	glGenVertexArrays(1, &vaoP);

	GLuint programID = load_shaders("src/vertex_shader.glsl", "src/fragment_shader.glsl");

	std::vector<vec3> CP;
	CP.push_back(vec3(-1.0f, 0.0f, 0.0f));
	CP.push_back(vec3(0.0f, 1.0f, 0.0f));
	CP.push_back(vec3(1.0f, 1.0f, 0.0f));
	CP.push_back(vec3(2.0f, 0.0f, 0.0f));

	std::vector<glm::vec3> data = BezierCurveByBernstein(CP, 50);

	glBindVertexArray(vaoCurve);
	GLuint vertex_buffer_curve;
	glGenBuffers(1, &vertex_buffer_curve);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_curve);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.size(), &(data.data()[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(vaoCP);
	GLuint vertex_buffer_CP;
	glGenBuffers(1, &vertex_buffer_CP);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_CP);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * CP.size(), &(CP.data()[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	mat4 scale = mat4(1.0f);
	scale = glm::scale(scale, vec3(0.4f, 0.4f, 1.0f));

	float size = 1;
	vec2 offset = vec2(0.0, 0.0);

	GLfloat pointSize = 10;
	glPointSize(pointSize);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 440");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	int index = -1;
	bool moving = false;

	while (!glfwWindowShouldClose(window)) {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Control Points");
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			if (!moving) {

				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				for (unsigned int i = 0; i < CP.size(); i++) {

					float x = (CP.at(i).x + xrange/2) * width/xrange;
					float y = (-CP.at(i).y + yrange/2) * height/yrange;

					if (nearlyEquals(xpos, x, pointSize/2) && nearlyEquals(ypos, y, pointSize/2))
					{
						index = i;
						moving = true;
					}
				}
			}
			else {
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);

				CP.at(index).x = (float)(xpos / (width / xrange) - xrange / 2);
				CP.at(index).y = (float)(-ypos / (height / yrange) + yrange / 2);

				glBindVertexArray(vaoCP);
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_CP);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * index, sizeof(glm::vec3), &(CP.at(index)));
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				data = BezierCurveByBernstein(CP, 50);

				glBindVertexArray(vaoCurve);
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_curve);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * data.size(), &(data.data()[0]));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && moving) {
			moving = false;
		}

		if (ImGui::TreeNode("Positions")) {
			for (unsigned int i = 0; i < CP.size(); i++) {
				std::string name = "Point " + std::to_string(i);
				if (ImGui::TreeNode(name.c_str())) {

					std::string x = "x : " + std::to_string(CP.at(i).x);
					std::string y = "y : " + std::to_string(CP.at(i).y);
					ImGui::Text(x.c_str());
					ImGui::Text(y.c_str());
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::Button("Add new point")) {
			vec3 newCP = vec3(0.0f, 0.0f, 0.0f);
			CP.push_back(newCP);

			glBindVertexArray(vaoCP);
			glGenBuffers(1, &vertex_buffer_CP);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_CP);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * CP.size(), &(CP.data()[0]), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			data = BezierCurveByBernstein(CP, 50);

			glBindVertexArray(vaoCurve);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_curve);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * data.size(), &(data.data()[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		ImGui::End();

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			size += 0.05f;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && size > 0.5)
			size -= 0.05f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			offset.y += 0.01f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			offset.y -= 0.01f;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			offset.x -= 0.01f;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			offset.x += 0.01f;

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);

		int size_loc = glGetUniformLocation(programID, "size");
		glUniform1f(size_loc, size);

		int offset_loc = glGetUniformLocation(programID, "offset");
		glUniform2f(offset_loc, offset.x, offset.y);

		int scale_loc = glGetUniformLocation(programID, "scale");
		glUniformMatrix4fv(scale_loc, 1, GL_FALSE, glm::value_ptr(scale));

		glBindVertexArray(vaoCurve);
		glDrawArrays(GL_LINES, 0, data.size());

		glBindVertexArray(vaoCP);
		glDrawArrays(GL_POINTS, 0, CP.size());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vertex_buffer_curve);
	glDeleteBuffers(1, &vertex_buffer_CP);
	glDeleteVertexArrays(1, &vaoP);
	glDeleteVertexArrays(1, &vaoCurve);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
};

bool nearlyEquals(double a, float b, float c) {
	if (abs(a - b) < c)
		return true;
	else
		return false;
};