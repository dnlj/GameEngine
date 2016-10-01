// Includes
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
#include <engine/util.hpp>
#include <engine/TextureFormat.hpp>
#include <engine/Texture.hpp>
#include <engine/Shader.hpp>
#include <engine/ShaderProgram.hpp>
#include <engine/Material.hpp>
#include <engine/Model.hpp>

// ImGui
//#define IMGUI_DISABLE_TEST_WINDOWS
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
// TODO: Need to be calling Model/Texture/Shader/ShaderProgram cleanup somewhere after converting them to use the Resource system
// TODO: Do more testing over the new Model/Texture/Shader/ShaderProgram/Material after you finish implementing the mto make sure they are getting cleaned up and what not

// TODO: Temp while testing
void imguiInit(GLFWwindow *window) {
	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, true);
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

	// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}

	// Rendering
	ImGui::Render();
}

void setupWindow(GLFWwindow *&window, std::string title) {
	// GLFW setup
	glfwSetErrorCallback([](int error, const char* desc) {
		std::stringstream stream;
		stream << "GLFW error 0x" 
			<< std::setfill('0') << std::setw(sizeof(int)*2) << std::hex 
			<< error << ": " << desc;
		engine_error(stream.str());
	});

	// Initialize GLFW and create the window
	if (!glfwInit()) { engine_error("GLFW initialization failed."); }

	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);

	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);

	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

	// Window setup
	constexpr bool fullscreen = false;
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

	if (fullscreen) {
		window = glfwCreateWindow(videoMode->width, videoMode->height, title.c_str(), monitor, nullptr);
	} else {
		window = glfwCreateWindow(1280, 720, title.c_str(), nullptr, nullptr);
	}

	if (!window) { engine_error("GLFW failed to create window."); }

	if (!fullscreen) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		glfwSetWindowPos(window, videoMode->width / 2 - width / 2, videoMode->height / 2 - height / 2);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // VSYNC 0=off        x = rate/x        1=rate/1 2=rate/2 etc...
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height){ glViewport(0, 0, width, height); });
	//glfwSetWindowFocusCallback(window, [](GLFWwindow *window, int focus) { printf("Focus: %i\n", focus); });
}

void run() {
	// Register resource directories
	engine::ResourcePath::AddResourceDir("Shader", "shaders/");
	engine::ResourcePath::AddResourceDir("Model", "D:/OpenGL Projects/Models/");
	engine::ResourcePath::AddResourceDir("Texture", "D:/OpenGL Projects/Textures/");

	// Setup the window
	static char *windowTitle = "Window Title";
	GLFWwindow *window = nullptr;
	setupWindow(window, windowTitle);

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
	// TODO: use resource paths
	engine::ShaderProgram program({
		engine::Shader::loadShader("Shader:vertex_brdf_ibl.glsl", GL_VERTEX_SHADER),
		engine::Shader::loadShader("Shader:fragment_brdf_ibl.glsl", GL_FRAGMENT_SHADER),
	});
	program.use();

	// Camera
	Camera camera;
	camera.setPosition(glm::vec3(0.0f, 8.0f, 0.0f));
	glm::mat4 projMatrix = glm::perspective(glm::radians(60.0f), (float)width/(float)height, 0.1f, 1000.0f); // TODO: Move inside the camera class and add fov, nearz, farz as members with getters/setters

	
	//GLuint cubeMap;
	//{
	//	glGenTextures(1, &cubeMap);
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	//
	//	std::vector<std::string> cubemapTextures {
	//		"D:/OpenGL Projects/Textures/cubemaps/PiazzaDelPopolo1/negx.jpg",
	//		"D:/OpenGL Projects/Textures/cubemaps/PiazzaDelPopolo1/negy.jpg",
	//		"D:/OpenGL Projects/Textures/cubemaps/PiazzaDelPopolo1/negz.jpg",
	//		"D:/OpenGL Projects/Textures/cubemaps/PiazzaDelPopolo1/posx.jpg",
	//		"D:/OpenGL Projects/Textures/cubemaps/PiazzaDelPopolo1/posy.jpg",
	//		"D:/OpenGL Projects/Textures/cubemaps/PiazzaDelPopolo1/posz.jpg",
	//	};
	//
	//	std::vector<GLuint> cubemapEnums {
	//		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	//		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	//		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	//		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	//		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	//		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	//	};
	//
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
	//
	//	int width, height;
	//	unsigned char* image;  
	//	for(GLuint i = 0; i < 6; i++) {
	//		image = SOIL_load_image(cubemapTextures[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	//		glTexImage2D(cubemapEnums[i], 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//		SOIL_free_image_data(image);
	//	}
	//}
	//
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	//glUniform1i(program.getUniformLocation("cubeMap"), 2);


	// Load some meshes for testing
	engine::Model ball = engine::Model::loadModel("Model:shaderBallNoCrease/shaderBall.obj", 0.025f, 2.0f);
	engine::Model hoola = engine::Model::loadModel("Model:_my_models/hoola.obj", 2.0f, 5.0f);
	engine::Model sailor = engine::Model::loadModel("Model:sailor_giveaway/sailor.obj", 1.5f, 2.0f);
	engine::Model hatman2 = engine::Model::loadModel("Model:_my_models/hatman2.obj", 1.0f, 2.0f);
	engine::Model backdrop = engine::Model::loadModel("Model:_my_models/Backdrop/backdrop.obj", 0.1f, 3.5f);
	engine::Model uvplane = engine::Model::loadModel("Model:_my_models/uvplane.obj", 10.0f, 1.0f);

	//////////
	ball.tempSetupGLStuff(program);
	hoola.tempSetupGLStuff(program);
	sailor.tempSetupGLStuff(program);
	hatman2.tempSetupGLStuff(program);
	backdrop.tempSetupGLStuff(program);
	uvplane.tempSetupGLStuff(program);
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
	
	//albedo_tex = engine::Texture::loadTexture("Texture:test.jpg", albedoFormat);
	// Material Testing
	program.loadProgramUniforms();
	engine::Material mat{program};
	mat.setParameter("albedoMap", albedo_tex);
	mat.setParameter("normalMap", normal_tex);
	mat.setParameter("roughnessMap", roughness_tex);
	mat.loadParameters();
	
	
	engine::util::checkGLErrors();

	// If we dont have this the cam spazzes at the start depending on where you cursor was
	glfwSetCursorPos(window, 0.0, 0.0);

	// Setup lighting and material properties
	glm::vec3 lightPosition = glm::vec3(0.0f);
	float roughness = 0.5f;
	float metalness = 0.0f; // 0.0 = 0.04 because we lerp between 0.04 and texture value
	float intensity	= 2.0f;

	// ImGui setup
	imguiInit(window);

	// Main game loop
	while (!glfwWindowShouldClose(window)) {
		// TODO: Move this fps stuff into a function
		static int frameCount = 0;
		static double frameTotal = 0.0;
		static double lastFrameTime	= 0.0;

		double dt = glfwGetTime() - lastFrameTime;
		lastFrameTime = glfwGetTime();
		frameTotal += dt;
		frameCount += 1;

		if (frameTotal >= 1.0f) {
			std::string title(windowTitle);
			title += " FPS: " + std::to_string((double)frameCount / frameTotal);
			title += " FT: " + std::to_string(frameTotal / (double)frameCount);

			glfwSetWindowTitle(window, title.c_str());

			frameTotal = 0.0;
			frameCount = 0;
		}

		if (dt > 1.0f) {
			dt = 1.0f;
		}
		
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


		{
			static float lastRoughness = roughness;
			static float lastMetalness = metalness;
			static float lastIntensity = intensity;
			static float inc = 0.3f;
			static float inc2 = 5.0f;
			
			if (glfwGetKey(window, GLFW_KEY_UP)) {
				roughness += (float)(inc * dt);
			}

			if (glfwGetKey(window, GLFW_KEY_DOWN)) {
				roughness -= (float)(inc * dt);
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT)) {
				metalness -= (float)(inc * dt);
			}

			if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
				metalness += (float)(inc * dt);
			}

			if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN)) {
				intensity -= (float)(inc2 * dt);
			}

			if (glfwGetKey(window, GLFW_KEY_PAGE_UP)) {
				intensity += (float)(inc2 * dt);
			}

			roughness = std::min(1.0f, std::max(0.0f, roughness));
			metalness = std::min(1.0f, std::max(0.0f, metalness));
			intensity = std::max(0.0f, intensity);

			if ((lastRoughness != roughness) || (lastMetalness != metalness) || (lastIntensity != intensity)) {
				std::cout << "roughness: " << std::fixed << std::setprecision(6) << roughness
				<< "\tmetalness: " << std::fixed << std::setprecision(6) << metalness
				<< "\tintensity: " << std::fixed << std::setprecision(6) << intensity << '\n';
			}

			lastRoughness = roughness;
			lastMetalness = metalness;
			lastIntensity = intensity;
		}

		//glUniform1f(program.getUniformLocation("roughness"), roughness); // NOTE: Disabled currently since we are using a texture, left incase we want to enabled later for testing (Will also need to enable in shader)
		glUniform1f(program.getUniformLocation("metalness"), metalness);
		glUniform1f(program.getUniformLocation("intensity"), intensity);
		

		camera.handleInput(window, (float)dt);


		// OpenGL drawing
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program.use();

		glUniform3fv(program.getUniformLocation("viewPos"), 1, &camera.getPosition()[0]);
		glUniform3fv(program.getUniformLocation("lightPos"), 1, &lightPosition[0]);

		mat.loadParameters();

		glm::mat4 model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 mvp = projMatrix * camera.getViewMatrix() * model;
		glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);

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


		// Imgui
		imguiTest(window);


		// Buffers and events
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {glfwSetWindowShouldClose(window, GL_TRUE);}
	}


	// TODO: Make an engine::cleanup function to encapsualte all these 
	engine::Texture::cleanup();
	engine::Model::cleanup();
	engine::Shader::cleanup();

	ImGui::Shutdown();

	glfwDestroyWindow(window);
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
	#ifdef _WIN32
		MoveWindow(GetConsoleWindow(), 0, 0, 800, 700, true);
	#endif

	if (false) { // NOTE: set to true to bench
		// Code for benching
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

	glfwTerminate();
	return 0;
}