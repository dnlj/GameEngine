﻿// Includes
#ifdef _WIN32
	#include <Windows.h>
#endif

// C++ STD
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <chrono>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL
#include <SOIL.h>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Engine
#include <engine/engine.hpp>
#include <engine/util/util.hpp>
#include <engine/TextureFormat.hpp>
#include <engine/Texture.hpp>
#include <engine/Shader.hpp>
#include <engine/ShaderProgram.hpp>
#include <engine/Material.hpp>
#include <engine/Model.hpp>
#include <engine/CubeMap.hpp>

// ImGui
// TODO: #define IMGUI_DISABLE_TEST_WINDOWS
#include <imgui/imgui.h>
// TODO: This is temp until i get around to doing this myself
#include <imgui_test.hpp>

// My includes
#include "Camera.h"

// TODO: Add include statment for glsl to make code more re-usable
// TODO: Need to add a way (in the material format?) to specify channels of textures to use for things. Such as using the r channel for the roughness and the g channel for the metalness and stuff like that.
// TODO: Consider using rapidjson for material format. Its pretty fast.
// TODO: Consider refining the engine namespace into engine::gfx or engine::graphics
// TODO: Look into using a uniform buffer object vs normal uniforms for things common to all shaders (projection matrix, mvp, model, etc)
// TODO: Look into direct_state_access (GL 4.5). Could save on some gl calls such as bind
// TODO: Look into 32bit z/depth buffer. Not supported on default frame buffer or something like that?
// TODO: Look into inverse log and other types of z buffers
// TODO: Add some kind of input handler so you can easily map keys to do stuff
// TODO: Read the unreal notes from siggraph, it covers a lot of this stuff
// TODO: Implement IBL
// TODO: Implement multiple types of lights with attenuation
// TODO: Make sure you are using debug/release assimp and soil, i think currently i have assimp using debug for both... not sure about soil. double check and google
// TODO: Make a camera that is not bad, fps style no need for quaternions.
// TODO: Input handling seems to be broke (at the time of writing this) when vsync is not enabled. Detach input/physics from framerate.
// TODO: Do more testing over the new Model/Texture/Shader/ShaderProgram/Material after you finish implementing the mto make sure they are getting cleaned up and what not


// TODO: Temp while testing
void imguiInit(GLFWwindow *window) {
	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup custom style
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4{0.05f, 0.05f, 0.05f, 0.60f};

	style.FrameRounding = 0.0f;
	style.WindowRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.ChildWindowRounding = 0.0f;

}

// TODO: Temp while testing
void imguiTest(GLFWwindow *window) {
	static bool show_test_window = true;
	static bool show_another_window = false;
	static ImVec4 clear_color = ImColor(0, 0, 0);

	ImGui_ImplGlfwGL3_NewFrame();
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window, this time using an explicit Begin/End pair
	if (show_another_window) {
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello");
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
}

double getNextDeltaTime() {
	using seconds = std::chrono::duration<double>;
	static auto last = std::chrono::high_resolution_clock::now();

	auto current = std::chrono::high_resolution_clock::now();
	auto dt = std::chrono::duration_cast<seconds>(current - last).count();
	
	last = current;

	return dt;
}

void runMenu(int width, int height, float& metalness, float& intensity) {
	ImGui_ImplGlfwGL3_NewFrame();
	{
		constexpr float padding = 10.0f;

		ImGui::SetNextWindowPos({padding, padding}, ImGuiSetCond_Once);
		ImGui::SetNextWindowCollapsed(true, ImGuiSetCond_Once);
		if (ImGui::Begin("Settings", nullptr, {300.0f, height - padding * 2}, -1.0f, ImGuiWindowFlags_NoMove)) {

			if (ImGui::CollapsingHeader("Shader Sliders", "shader sliders", true, true)) {
				ImGui::SliderFloat("metalness", &metalness, 0.0f, 1.0f);
				ImGui::SliderFloat("intensity", &intensity, 0.0f, 200.0f);
			}

			if (ImGui::CollapsingHeader("Instructions")) {
				ImGui::BulletText("Hold middle mouse to move");
				ImGui::BulletText("Press F to toggle light position");
				ImGui::BulletText("mouse = look");
				ImGui::BulletText("wasd = move");
				ImGui::BulletText("ctrl = down");
				ImGui::BulletText("space = up");
			}
		}

		ImGui::End();

		ImGui::Render();
	}
}

void run() {
	// Register resource directories
	engine::ResourcePath::AddResourceDir("Shader", "shaders/");
	engine::ResourcePath::AddResourceDir("Model", "D:/OpenGL Projects/Models/");
	engine::ResourcePath::AddResourceDir("Texture", "D:/OpenGL Projects/Textures/");

	//engine::CubeMap::loadCubeMap("Texture:CubeMaps/single_sides.cube");
	//getchar();
	//return;

	// Setup the window
	static char *windowTitle = "Window Title";
	GLFWwindow* window = engine::getWindow();

	// Initialize glLoadGen
	engine::util::initializeOpenGL();

	// Display OpenGL info
	engine::util::printGLInfo();

	// Get the window width and height
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// OpenGL settings
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	// Program
	engine::ShaderProgram program({
		engine::Shader::loadShader("Shader:vertex_brdf_ibl.glsl", GL_VERTEX_SHADER),
		engine::Shader::loadShader("Shader:fragment_brdf_ibl.glsl", GL_FRAGMENT_SHADER),
	});
	
	engine::ShaderProgram skyboxProgram({
		engine::Shader::loadShader("Shader:vertex_skybox.glsl", GL_VERTEX_SHADER),
		engine::Shader::loadShader("Shader:fragment_skybox.glsl", GL_FRAGMENT_SHADER),
	});


	// Camera
	Camera camera;
	camera.setPosition(glm::vec3(0.0f, 8.0f, 0.0f));
	glm::mat4 projMatrix = glm::perspective(glm::radians(60.0f), (float)width/(float)height, 0.1f, 1000.0f); // TODO: Move inside the camera class and add fov, nearz, farz as members with getters/setters

	
	// Load a CubeMap
	//engine::CubeMap cubeMap = engine::CubeMap::loadCubeMap("Texture:CubeMaps/split_sides.cube");
	//engine::CubeMap cubeMap = engine::CubeMap::loadCubeMap("Texture:CubeMaps/horizontal_cross.png");
	//engine::CubeMap cubeMap = engine::CubeMap::loadCubeMap("Texture:CubeMaps/horizontal_line.png");
	//engine::CubeMap cubeMap = engine::CubeMap::loadCubeMap("Texture:CubeMaps/vertical_cross.png");
	//engine::CubeMap cubeMap = engine::CubeMap::loadCubeMap("Texture:CubeMaps/vertical_line.png");
	engine::CubeMap cubeMap = engine::CubeMap::loadCubeMap("Texture:CubeMaps/stormydays_large.png");

	// Load some meshes for testing
	engine::Model ball = engine::Model::loadModel("Model:shaderBallNoCrease/shaderBall.obj", 0.025f, 2.0f);
	engine::Model hoola = engine::Model::loadModel("Model:_my_models/hoola.obj", 2.0f, 5.0f);
	engine::Model sailor = engine::Model::loadModel("Model:sailor_giveaway/sailor.obj", 1.5f, 2.0f);
	engine::Model skybox = engine::Model::loadModel("Model:_my_models/skybox.obj", 10.0f, 1.0f);
	engine::Model hatman2 = engine::Model::loadModel("Model:_my_models/hatman2.obj", 1.0f, 2.0f);
	engine::Model uvplane = engine::Model::loadModel("Model:_my_models/uvplane.obj", 10.0f, 1.0f);
	engine::Model backdrop = engine::Model::loadModel("Model:_my_models/Backdrop/backdrop.obj", 0.1f, 3.5f);

	//////////
	ball.tempSetupGLStuff(program);
	hoola.tempSetupGLStuff(program);
	sailor.tempSetupGLStuff(program);
	skybox.tempSetupGLStuff(program);
	hatman2.tempSetupGLStuff(program);
	uvplane.tempSetupGLStuff(program);
	backdrop.tempSetupGLStuff(program);
	/////////
	
	engine::TextureFormat albedoFormat;
	engine::TextureFormat normalFormat;
	normalFormat.useGammaCorrection = false;

	// Tile
	engine::Texture albedo_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/old_tiles/old_tiles_a.tga", albedoFormat);
	engine::Texture normal_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/old_tiles/old_tiles_n.tga", normalFormat);
	engine::Texture roughness_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/old_tiles/old_tiles_r.tga", normalFormat);

	// Clay Brick
	//engine::Texture albedo_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/clay_brick/clay_brick_a.tga", albedoFormat);
	//engine::Texture normal_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/clay_brick/clay_brick_n.tga", normalFormat);
	//engine::Texture roughness_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/clay_brick/clay_brick_r.tga", normalFormat);
	
	// Old Wooden Planks
	//engine::Texture albedo_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/old_wooden_planks/old_wooden_planks_a.tga", albedoFormat);
	//engine::Texture normal_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/old_wooden_planks/old_wooden_planks_n.tga", normalFormat);
	//engine::Texture roughness_tex = engine::Texture::loadTexture("Texture:s_schulz_mat_pack_free_dl/old_wooden_planks/old_wooden_planks_r.tga", normalFormat);

	// Error Checker
	//engine::Texture albedo_tex = engine::Texture::loadTexture("Texture:error_checker/error_checker_basecolor.png", albedoFormat);
	//engine::Texture normal_tex = engine::Texture::loadTexture("Texture:error_checker/error_checker_normal_fixed.png", normalFormat);
	//engine::Texture roughness_tex = engine::Texture::loadTexture("Texture:error_checker/error_checker_roughness.png", normalFormat);
	
	//engine::Texture albedo_tex = engine::Texture::loadTexture("Texture:test.jpg", albedoFormat);

	// Material Testing
	program.use(); // TODO: if this isnt here the first call to mat.loadParameters causes a GL_INVALID_OPERATION. figure out why
	program.loadProgramUniforms();
	engine::Material mat{program};
	mat.setParameter("albedoMap", albedo_tex);
	mat.setParameter("normalMap", normal_tex);
	mat.setParameter("roughnessMap", roughness_tex);


	engine::util::checkGLErrors();


	// Setup lighting and material properties
	glm::vec3 lightPosition = glm::vec3(0.0f);
	float metalness = 0.0f; // 0.0 = 0.04 because we lerp between 0.04 and texture value
	float intensity	= 1.0f;

	// ImGui setup
	imguiInit(window);

	// Main game loop
	while (!glfwWindowShouldClose(window)) {
		double dt = getNextDeltaTime();
		
		{ // Light input
			static bool toggle = true;
			static bool lastPressed = false;

			bool framePressed = glfwGetKey(window, GLFW_KEY_F) == 1;

			if (framePressed && !lastPressed) {
				toggle = !toggle;
				std::cout << "Update light position: " << std::boolalpha << toggle << std::endl;
			}

			if (toggle) {
				lightPosition = camera.getPosition();
			}

			lastPressed = framePressed;
		}
		
		
		camera.handleInput(window, static_cast<float>(dt));


		// OpenGL drawing
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 mvp = projMatrix * camera.getViewMatrix() * model;


		// Models
		{
			program.use();

			glUniform1f(program.getUniformLocation("metalness"), metalness);
			glUniform1f(program.getUniformLocation("intensity"), intensity);
			
			mat.loadParameters();

			// TODO: need to handle this cubemap texture properly
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getCubeMapTexture());
			glUniform1i(program.getUniformLocation("cubeMap"), 3);
			
			glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniform3fv(program.getUniformLocation("viewPos"), 1, &camera.getPosition()[0]);
			glUniform3fv(program.getUniformLocation("lightPos"), 1, &lightPosition[0]);
			
			model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -0.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			mvp = projMatrix * camera.getViewMatrix() * model;
			glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
			ball.render();
			
			model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -1.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			mvp = projMatrix * camera.getViewMatrix() * model;
			glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
			hoola.render();
			
			model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			mvp = projMatrix * camera.getViewMatrix() * model;
			glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
			sailor.render();
			
			model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			mvp = projMatrix * camera.getViewMatrix() * model;
			glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
			hatman2.render();
			
			model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			mvp = projMatrix * camera.getViewMatrix() * model;
			glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
			backdrop.render();
			
			model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(10.0f, 0.01f, 0.0f)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			mvp = projMatrix * camera.getViewMatrix() * model;
			glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
			uvplane.render();
		}

		// Skybox
		{
			skyboxProgram.use();

			model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 mvp = projMatrix * glm::mat4{glm::mat3{camera.getViewMatrix()}} *model;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getCubeMapTexture());

			glUniformMatrix4fv(skyboxProgram.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			glUniform1i(skyboxProgram.getUniformLocation("cubeMap"), 0);

			skybox.render();
		}


		// Imgui
		runMenu(width, height, metalness, intensity);


		// Buffers and events
		glfwSwapBuffers(window);
		glfwPollEvents();


		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) { glfwSetWindowShouldClose(window, GL_TRUE); }
	}

	engine::util::checkGLErrors();
}

double bench() {
	typedef std::chrono::high_resolution_clock clock;

	clock::time_point start = clock::now();

	// NOTE: Don't forget to change the if statment to true in main to enable benching.
	{ // NOTE: Bench code goes here.
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/_my_models/teapot_01.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/crytek-sponza/sponza.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/_my_models/Backdrop/backdrop.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/_my_models/sphere.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/sailor_giveaway/sailor.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/_my_models/sphereSmooth.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/ShaderBall.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/_my_models/plane32x32.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/shaderBallNoCrease/shaderBall.obj");
		//engine::meshManager::loadMesh("D:/OpenGL Projects/Models/_my_models/lightModel.obj");
	} // End bench code

	clock::time_point end = clock::now();

	clock::duration dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

	double time = dur.count() / 1000000000.0;
	return time;
}

int main(int argc, char* argv[]) {
	std::atexit(engine::_atExit);

	#ifdef _WIN32
		MoveWindow(GetConsoleWindow(), 0, 0, 800, 700, true);
	#endif

	// Code for benching
	if (false) { 
		std::vector<double> times{};
		double avg = 0;

		for (int i = 1; i <= 10; i++) {
			times.push_back(bench());
		}

		for (int i = 0; i < times.size(); i++) {
			std::cout << "Time " << i << ": " << times[i] << std::endl;
			avg += times[i];
		}

		std::cout << "Average Time: " << avg / times.size() << std::endl;
	}

	run();

	return 0;
}