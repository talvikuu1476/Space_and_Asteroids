#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <stb_image.h>
#include <Camera.h>
#include <Model.h>
#include <filesystem>

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadCubemap(vector<std::string> faces);

//screen
const unsigned int screenWidth = 1400;
const unsigned int screenHeight = 800;

//time difference between the current frame and the last frame
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//camera
Camera camera(glm::vec3(-600.0f, 0.0f, -150.0f));
bool firstMouse = true;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;

//light
glm::vec3 lightPos(-500.0f, 500.0f, -500.0f);
glm::vec3 lightColour(7.5f, 7.5f, 7.5f);

//update window size
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//update frame rate
void updateWindowTitle(GLFWwindow* window, float& lastTime, int& frameCount) {
	double currentTime = glfwGetTime();
	frameCount++;

	if (currentTime - lastTime >= 0.5) {
		std::ostringstream title;
		title << "Planet with Asteroids  ||  FPS: " << frameCount;
		glfwSetWindowTitle(window, title.str().c_str());

		frameCount = 0;
		lastTime = currentTime;
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; //reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

string getPath(const string& filename) {
	filesystem::path fullPath = filesystem::current_path() / filename;
	return fullPath.string();
}

int main()
{
	glfwInit();
	//set OpenGL version as 3.3 (major & minor)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//use core-profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//set the time of sampling
	glfwWindowHint(GLFW_SAMPLES, 8);

	//create a window object
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Planet with Asteroids  ||  FPS: ", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//check if glad has been successflly initialised
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialise GLAD" << endl;
		return -1;
	}

	//set the window is focused
	glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused)
		{
			if (focused) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPos(window, screenWidth / 2.0f, screenHeight / 2.0f);
			}
			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		});

	//set the mouse's default position
	if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window, screenWidth / 2.0, screenHeight / 2.0);
	}

	/*
		configure global OpenGL settings
	*/
	//enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	//enable MSAA
	glEnable(GL_MULTISAMPLE);

	/*
		build and compile shader programs
	*/
	string planetVertex = getPath("Shaders/planet.vertex");
	string planetFragment = getPath("Shaders/planet.fragment");
	string asteroidsVertex = getPath("Shaders/asteroids.vertex");
	string asteroidsFragment = getPath("Shaders/asteroids.fragment");
	string skyboxVertex = getPath("Shaders/skybox.vertex");
	string skyboxFragment = getPath("Shaders/skybox.fragment");
	string screenVertex = getPath("Shaders/screen.vertex");
	string screenFragment = getPath("Shaders/screen.fragment");

	Shader planetShader(planetVertex.c_str(), planetFragment.c_str());
	Shader asteroidsShader(asteroidsVertex.c_str(), asteroidsFragment.c_str());
	Shader skyboxShader(skyboxVertex.c_str(), skyboxFragment.c_str());
	Shader screenShader(screenVertex.c_str(), screenFragment.c_str());

	/*
		load textures
	*/
	string planetPath = getPath("Resources/models/planet/planet.obj");
	string rockPath = getPath("Resources/models/rock/rock.obj");

	Model planet("Resources/models/planet/planet.obj");
	Model rock(rockPath);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	float screenVertices[] = 
	{
		//positions    //texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	//load skybox texture
	string right = getPath("Resources/textures/skybox/right.png");
	string left = getPath("Resources/textures/skybox/left.png");
	string top = getPath("Resources/textures/skybox/top.png");
	string bottom = getPath("Resources/textures/skybox/bottom.png");
	string front = getPath("Resources/textures/skybox/front.png");
	string back = getPath("Resources/textures/skybox/back.png");

	vector<std::string> faces {right, left, top, bottom, front, back};
	unsigned int cubemapTexture = loadCubemap(faces);

	//generate a large list of random model transformation matrices
	unsigned int amount = 10000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(static_cast<unsigned int>(glfwGetTime())); //initialize random seed
	float radius = 500.0;
	float offset = 100.0f;

	//generate random asteroid objects
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);

		//displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; //keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		//scale between 0.1 and 0.25f
		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.1);
		model = glm::scale(model, glm::vec3(scale));

		//add random rotation around a randomly picked rotation axis vector
		float rotAngle = static_cast<float>((rand() % 360));
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		//add to list of matrices
		modelMatrices[i] = model;
	}
	
	//generate random asteroids rotation speed
	vector<float> rotationSpeeds(amount);
	for (auto& speed : rotationSpeeds)
	{
		speed = (rand() % 100) / 10.0f;
	}

	//asteroids rotation speed VBO
	unsigned int rotationVBO;
	glGenBuffers(1, &rotationVBO);
	glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float), rotationSpeeds.data(), GL_STATIC_DRAW);

	//planet VBO
	unsigned int planetVBO;
	glGenBuffers(1, &planetVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planetVBO);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	//asteroids VAO
	for (unsigned int i = 0; i < rock.meshes.size(); i++)
	{
		unsigned int asteroidsVAO = rock.meshes[i].VAO;
		glBindVertexArray(asteroidsVAO);

		//vertex attributes
		GLsizei vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
		glEnableVertexAttribArray(7);
		glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
		glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);

		//update attributes once per instance
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}

	//skybox VAO & VBO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	unsigned int screenVAO, screenVBO;
	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	//MSAA FBO
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int textureColourBufferMultiSampled;
	glGenTextures(1, &textureColourBufferMultiSampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColourBufferMultiSampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, screenWidth, screenHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColourBufferMultiSampled, 0);

	//(multisampled) RBO for depth and stencil attachments
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//second post-processing FBO
	unsigned int intermediateFBO;
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
	//colour attachment texture
	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	float planetRotationSpeed = 2.5f;

	glfwMakeContextCurrent(window); // 绑定 OpenGL 上下文

	float lastTime = glfwGetTime();
	int frameCount = 0;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	/*-
		main render loop
	-*/
	while (!glfwWindowShouldClose(window))
	{
		//per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		updateWindowTitle(window, lastTime, frameCount);

		processInput(window);

		/*-
			render
		-*/
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		//set the background colour
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//clear colour buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		//process transforms
		glm::mat4 model, view, projection;

		model = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.1f, 10000.0f);

		asteroidsShader.use();

		asteroidsShader.setMat4("view", camera.GetViewMatrix());
		asteroidsShader.setMat4("projection", projection);
		asteroidsShader.setFloat("uTime", currentFrame * 10.0f);

		planetShader.use();
		planetShader.setMat4("view", camera.GetViewMatrix());
		planetShader.setMat4("projection", projection);

		//rendering planet
		float planetRotationAngle = currentFrame * planetRotationSpeed;

		model = glm::mat4(1.0f); //reset as identity matrix
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, glm::radians(planetRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate along y-axis
		model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
		glm::mat3 modelMatrix = glm::mat3(transpose(inverse(model)));
		planetShader.setMat4("model", model);
		planetShader.setMat3("modelMatrix", modelMatrix);

		planetShader.setFloat("material.shininess", 64.0f);

		planetShader.setVec3("light.position", lightPos);
		planetShader.setVec3("light.ambient", glm::vec3(0.1f));
		planetShader.setVec3("light.diffuse", glm::vec3(1.0f));
		planetShader.setVec3("light.specular", glm::vec3(0.0f));
		planet.Draw(planetShader);

		//rendering asteroids
		asteroidsShader.use();
		asteroidsShader.setInt("material.texture_diffuse1", 0);
		asteroidsShader.setFloat("material.shininess", 64.0);

		asteroidsShader.setVec3("light.lightPos", lightPos);
		asteroidsShader.setVec3("light.ambient", glm::vec3(0.1f));
		asteroidsShader.setVec3("light.diffuse", glm::vec3(0.8f));
		asteroidsShader.setVec3("light.specular", glm::vec3(0.2f));

		asteroidsShader.setVec3("cameraPos", camera.Position);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);

		//rendering rocks
		for (unsigned int i = 0; i < rock.meshes.size(); i++)
		{
			glBindVertexArray(rock.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.meshes[i].indices.size()),
				GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  //change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); //remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); //switch back to default depth function 

		//blit multisampled buffers to normal colourbuffer of intermediate FBO 
		//image is stored in screenTexture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //switch back to default FBO

		//render screen
		screenShader.use();
		glBindVertexArray(screenVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//check if evens have been triggered, and swap colour buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//clear pre-allocated resources
	delete[] modelMatrices;

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &screenVAO); 
	glDeleteBuffers(1, &rotationVBO);
	glDeleteBuffers(1, &planetVBO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &screenVBO);
	glDeleteFramebuffers(1, &FBO);
	glDeleteFramebuffers(1, &intermediateFBO);
	glDeleteRenderbuffers(1, &RBO);

	glfwTerminate();

	return 0;
}



