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

// ImGui
#define IMGUI_DISABLE_TEST_WINDOWS
#include <imgui/imgui.h>
// TODO: This is temp until i get around to doing this myself
#include <imgui_test.hpp>

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
#include <engine/ShaderProgram2.hpp>
#include <engine/Material.hpp>
#include <engine/Material2.hpp>
#include <engine/Model.hpp>
#include <engine/Camera.hpp>

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

void drawScene(const engine::ShaderProgram& program, const engine::ShaderProgram& skyboxProgram, const glm::vec3& lightPosition, const engine::Camera& camera, const engine::Texture& cubemap, const std::vector<engine::Model>& models) {
	auto projection = camera.getProjectionMaterix();

	glm::mat4 model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mvp = projection * camera.getViewMatrix() * model;

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getTexture());
	glUniform1i(program.getUniformLocation("cubeMap"), 3);

	glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(program.getUniformLocation("viewPos"), 1, &camera.getPosition()[0]);
	glUniform3fv(program.getUniformLocation("lightPos"), 1, &lightPosition[0]);

	model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -0.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = projection * camera.getViewMatrix() * model;
	glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
	models[1].render();

	model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -1.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = projection * camera.getViewMatrix() * model;
	glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
	models[2].render();

	model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = projection * camera.getViewMatrix() * model;
	glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
	models[3].render();

	model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f*18.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = projection * camera.getViewMatrix() * model;
	glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
	models[4].render();

	model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp = projection * camera.getViewMatrix() * model;
	glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
	models[5].render();

	model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(10.0f, 0.01f, 0.0f)), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp = projection * camera.getViewMatrix() * model;
	glUniformMatrix4fv(program.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(program.getUniformLocation("modelMatrix"), 1, GL_FALSE, &model[0][0]);
	models[6].render();

	// Skybox
	{
		skyboxProgram.use();

		model = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 mvp = projection * glm::mat4{glm::mat3{camera.getViewMatrix()}} *model;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getTexture());

		glUniformMatrix4fv(skyboxProgram.getUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
		glUniform1i(skyboxProgram.getUniformLocation("cubeMap"), 0);

		models[0].render();
	}
}

void run() {
	// Register resource directories
	// NOTE: These will need to be updated when running the project on a different machine
	engine::ResourcePath::AddResourceDir("Shaders", "D:/OpenGL Projects/GameEngine/GLEngineProject/resources/shaders/"); // This is the "resources/shaders/" folder that is included in git
	engine::ResourcePath::AddResourceDir("Models", "D:/OpenGL Projects/Models/"); // This is where the models are stored. Not included in git.
	engine::ResourcePath::AddResourceDir("Textures", "D:/OpenGL Projects/Resources/Textures/"); // This is where the texture bitmaps are stored. Not included in git.
	engine::ResourcePath::AddResourceDir("Textures", "D:/OpenGL Projects/GameEngine/GLEngineProject/resources/textures/"); // This is the "resources/textures/" folder that is included in git
	engine::ResourcePath::AddResourceDir("CubeMaps", "D:/OpenGL Projects/Resources/CubeMaps/"); // This is where the cubemap bitmaps are stored. Not included in git.
	engine::ResourcePath::AddResourceDir("CubeMaps", "D:/OpenGL Projects/GameEngine/GLEngineProject/resources/cubemaps/"); // This is the "resources/cubemaps/" folder that is included in git
	engine::ResourcePath::AddResourceDir("Programs", "D:/OpenGL Projects/GameEngine/GLEngineProject/resources/programs/"); // This is the "resources/programs/" folder that is included in git

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
		engine::Shader::load("Shaders:vertex_brdf_ibl.glsl", GL_VERTEX_SHADER),
		engine::Shader::load("Shaders:fragment_brdf_ibl.glsl", GL_FRAGMENT_SHADER),
	});
	
	engine::ShaderProgram skyboxProgram({
		engine::Shader::load("Shaders:vertex_skybox.glsl", GL_VERTEX_SHADER),
		engine::Shader::load("Shaders:fragment_skybox.glsl", GL_FRAGMENT_SHADER),
	});


	// Camera
	engine::Camera camera{glm::radians(60.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f};
	camera.setPosition(glm::vec3(20.0f, 10.0f, 0.0f));
	camera.lookAt({0, 5, 0});

	
	// Load a CubeMap
	//engine::Texture cubeMap = engine::Texture::load("CubeMaps:split_sides.cube");
	//engine::Texture cubeMap = engine::Texture::load("CubeMaps:horizontal_cross.cube");
	//engine::Texture cubeMap = engine::Texture::load("CubeMaps:horizontal_line.cube");
	//engine::Texture cubeMap = engine::Texture::load("CubeMaps:vertical_cross.cube");
	//engine::Texture cubeMap = engine::Texture::load("CubeMaps:vertical_line.cube");
	engine::Texture cubeMap = engine::Texture::load("CubeMaps:stormydays_large.cube");

	// Load some meshes for testing
	std::vector<engine::Model> models;
	models.emplace_back(engine::Model::load("Models:_my_models/skybox.obj", 10.0f, 1.0f));
	models.emplace_back(engine::Model::load("Models:shaderBallNoCrease/shaderBall.obj", 0.025f, 2.0f));
	models.emplace_back(engine::Model::load("Models:_my_models/hoola.obj", 2.0f, 5.0f));
	models.emplace_back(engine::Model::load("Models:sailor_giveaway/sailor.obj", 1.5f, 2.0f));
	models.emplace_back(engine::Model::load("Models:_my_models/hatman2.obj", 1.0f, 2.0f));
	models.emplace_back(engine::Model::load("Models:_my_models/Backdrop/backdrop.obj", 0.1f, 3.5f));
	models.emplace_back(engine::Model::load("Models:_my_models/uvplane.obj", 10.0f, 1.0f));

	//////////
	for (auto& model : models) {
		model.tempSetupGLStuff(program);
	}
	/////////
	engine::Texture tile_a = engine::Texture::load("Textures:old_tiles/old_tiles_a.tex");
	engine::Texture tile_n = engine::Texture::load("Textures:old_tiles/old_tiles_n.tex");
	engine::Texture tile_r = engine::Texture::load("Textures:old_tiles/old_tiles_r.tex");

	// Material Testing
	program.use(); // TODO: if this isnt here the first call to mat.loadParameters causes a GL_INVALID_OPERATION. figure out why
	program.loadProgramUniforms();
	engine::Material mat{program};
	mat.setParameter("albedoMap", tile_a);
	mat.setParameter("normalMap", tile_n);
	mat.setParameter("roughnessMap", tile_r);

	// Render to Cubemap testing
	int faceSize = 256;
	GLuint frameBuffer;
	GLuint cubeMapTexture;
	GLuint depthTexture;
	{// Setup cubeMapTexture
	
		// Generate and bind the ccubemap texture
		glGenTextures(1, &cubeMapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		
		// Setup texture parameters
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
		// Setup our faces
		for (int face = 0; face < 6; ++face) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		}
	
		// Unbind our cubemap texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	{ // Setup Depth
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, faceSize, faceSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	{// Setup frameBuffer
		// Generate frame buffer
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		// Do any setup here
	
		// Unbind our frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


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

		// Models and Skybox
		for (int i = 0; i < 1; ++i){

			program.use();

			glUniform1f(program.getUniformLocation("metalness"), metalness);
			glUniform1f(program.getUniformLocation("intensity"), intensity);
			
			mat.loadParameters();

			if (i > 0) {
				// TODO: Shouldnt the camera be orthogonal for this?

				glViewport(0, 0, faceSize, faceSize);
				glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i-1, cubeMapTexture, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

				camera.setFOV(glm::radians(90.0f));
				camera.setAspect(1.0f);
				switch (i) {
					case 1:
						camera.lookAt({1.0f, 0.0f, 0.0f});
						break;
					case 2:
						camera.lookAt({-1.0f, 0.0f, 0.0f});
						break;
					case 3:
						camera.lookAt({0.0f, 1.0f, 0.0f});
						break;
					case 4:
						camera.lookAt({0.0f, -1.0f, 0.0f});
						break;
					case 5:
						camera.lookAt({0.0f, 0.0f, 1.0f});
						break;
					case 6:
						camera.lookAt({0.0f, 0.0f,-1.0f});
						break;
				}
			} else {
				glViewport(0, 0, width, height);
				camera.setFOV(glm::radians(70.0f));
				camera.setAspect(static_cast<float>(width) / static_cast<float>(height));
			}

			// OpenGL drawing
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawScene(program, skyboxProgram, lightPosition, camera, cubeMap, models);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Imgui
		runMenu(width, height, metalness, intensity);

		// Buffers and events
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Check for close
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) { glfwSetWindowShouldClose(window, GL_TRUE); }
	}

	//{ // Save out screenshot of current frame buffer on exit
	//	int width = faceSize;
	//	int height = faceSize;
	//	std::vector<glm::u8vec3> img;
	//	img.resize(width * height);
	//	
	//	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	//
	//	for (int i=0; i < 6; ++i) {
	//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapTexture, 0);
	//		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &img[0].r);
	//
	//		// Flip the image vertically
	//		for (int x = 0; x < width; ++x) {
	//			for (int y = 0; y < height / 2; ++y) {
	//				size_t idx1 = x + y * width;
	//				size_t idx2 = (height-y) * width - width + x;
	//				auto temp = img[idx1];
	//				img[idx1] = img[idx2];
	//				img[idx2] = temp;
	//			}
	//		}
	//
	//		// Save to disk
	//		std::string fileName = "face";
	//		fileName += std::to_string(i);
	//		fileName += ".bmp";
	//		SOIL_save_image(fileName.c_str(), SOIL_SAVE_TYPE_BMP, width, height, 3, &img[0].r);
	//	}
	//}

	glDeleteTextures(1, &cubeMapTexture);
	glDeleteTextures(1, &depthTexture);
	glDeleteFramebuffers(1, &frameBuffer);

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