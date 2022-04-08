#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"

#include <Model.h>
#include "Terrain.h"

#include<string>
#include <iostream>
#include <numeric>

#include "Water.h"
#include "WaterFB.h"



// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void RenderScene(Shader terrainShader, Shader normalShader, Terrain Terrain, unsigned int heightMap, int pass, glm::vec3 skyCol, Water water, glm::mat4 model, glm::mat4 view, glm::mat4 proj, GLuint texOutput);
unsigned int loadTexture(char const * path);
//unsigned int loadTexture2(char const * path);


// camera
Camera camera(glm::vec3(260,150,300));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int terrainVAO;
unsigned int waterVAO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool normalMap = false;
bool fillTri = true;

float waveStr = 0.01f;
float flow = 0.f;
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IMAT3907", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_CLIP_DISTANCE0);
	glCullFace(GL_BACK);

	int texWidth = 512, texHeight = 512;
	GLuint texOut;
	glGenTextures(1, &texOut);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texOut);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texWidth, texHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	// simple vertex and fragment shader - add your own tess and geo shader
	Shader terrainShader("..\\shaders\\Terrain\\plainVert.vs", "..\\shaders\\Terrain\\plainFrag.fs","..\\shaders\\Terrain\\plainGeo.gs","..\\shaders\\Terrain\\TCS.tcs","..\\shaders\\Terrain\\TES.tes");
	Shader normalShader("..\\shaders\\norm\\normPlainVert.vs", "..\\shaders\\norm\\normPlainFrag.fs", "..\\shaders\\norm\\normPlainGeo.gs", "..\\shaders\\norm\\normTCS.tcs", "..\\shaders\\norm\\normTES.tes");
	Shader waterShader("..\\shaders\\Water\\waterVert.vs", "..\\shaders\\Water\\waterFrag.fs");

	unsigned int heightMap = 0;
	//Terrain Constructor ; number of grids in width, number of grids in height, gridSize
	Terrain terrain(30, 30,60);
	terrainVAO = terrain.getVAO();

	WaterFB waterfb(SCR_WIDTH, SCR_HEIGHT);
	

	Water water(3000, 3000, 6000);
	waterVAO = water.getVAO();

	unsigned int dudvMap = loadTexture("..\\Resources\\Water\\WaterDUDV.png");
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, dudvMap);

	unsigned int normalMap = loadTexture("..\\Resources\\Water\\WaterNorm.png");
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, normalMap);

	glm::vec3 skyCol = glm::vec3(0.7f, 0.7f, 0.7f);


	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(skyCol.x, skyCol.y, skyCol.z, 1.f);
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		waterfb.bindReflection();
		float dist = 2 * (camera.Position.y - water.getHeight());
	
		camera.Position.y -= dist;
		camera.InvertPitch();
		camera.updateCameraVectors();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.f);
		view = camera.GetViewMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderScene(terrainShader, normalShader, terrain, heightMap, 1, skyCol, water, model, view, projection, texOut);
	
		camera.Position.y += dist;
		camera.InvertPitch();
		camera.updateCameraVectors();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.f);
		view = camera.GetViewMatrix();
		waterfb.bindRefraction();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderScene(terrainShader, normalShader, terrain, heightMap, 2, skyCol, water, model, view, projection, texOut);
	
		waterfb.unbindFB();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderScene(terrainShader, normalShader, terrain, heightMap, 3, skyCol, water, model, view, projection, texOut);
	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
		waterShader.use();
		waterShader.setMat4("model", model);
		waterShader.setMat4("view", view);
		waterShader.setMat4("projection", projection);
		waterShader.setVec3("camPos", camera.Position);
		waterShader.setInt("reflectTex", 7);
		waterShader.setInt("refractTex", 8);
		waterShader.setInt("depthMap", 0);
		waterShader.setInt("dudvMap", 10);
		waterShader.setInt("normMap", 11);
		waterShader.setVec3("lightPos", glm::vec3(0.1f, -1.0f, 0.2f));
		waterShader.setVec3("lightCol", glm::vec3(0.7f, 0.3f, 0.5f));
	
		flow += waveStr * deltaTime;
		if (flow >= 1.f)  flow = 0.f;
		waterShader.setFloat("flow", flow);
		glBindVertexArray(waterVAO);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glDisable(GL_BLEND);
		//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)	normalMap = !normalMap;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)	fillTri = !fillTri;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}





// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
		std::cout << "Loaded texture at path: " << path << " width " << width << " id " << textureID <<  std::endl;

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		
	}

	return textureID;
}

void RenderScene(Shader terrainShader, Shader normalShader, Terrain Terrain, unsigned int heightMap, int pass, glm::vec3 skyCol, Water water, glm::mat4 model, glm::mat4 view, glm::mat4 proj, GLuint texOutput)
{
	terrainShader.use();
	terrainShader.setMat4("model", model);
	terrainShader.setMat4("view", view);
	terrainShader.setMat4("projection", proj);
	terrainShader.setVec3("camPos", camera.Position);
	terrainShader.setVec3("sky", skyCol);
	terrainShader.setInt("heightMap", heightMap);
	terrainShader.setInt("octaves", 50);
	terrainShader.setInt("gridSize", 10);
	terrainShader.setFloat("scale", 70.f);

	switch (pass)
	{
	case 1:
		terrainShader.setVec4("plane", glm::vec4(0, 1, 0, -water.getHeight() + 0.5f));
		break;
	case 2:
		terrainShader.setVec4("plane", glm::vec4(0, -1, 0, water.getHeight() + 0.5f));
		break;
	case 3:
		terrainShader.setVec4("plane", glm::vec4(0, 1, 0, 0));
		break;
	}

	GLint lightPos, ambLight, DiffLight, SpecLight;
	lightPos = glGetUniformLocation(terrainShader.ID, "light.pos");
	ambLight = glGetUniformLocation(terrainShader.ID, "light.amb");
	DiffLight = glGetUniformLocation(terrainShader.ID, "light.diff");
	SpecLight = glGetUniformLocation(terrainShader.ID, "light.spec");
	glUniform3f(lightPos, 0.1f, -0.3f, 0.2f);
	glUniform3f(ambLight, 0.7f, 0.4f, 0.5f);
	glUniform3f(DiffLight, 0.3f, 0.6f, 0.5f);
	glUniform3f(SpecLight, 0.2f, 0.1f, 0.15f);

	glBindVertexArray(terrainVAO);

	if (fillTri) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_PATCHES, 0, Terrain.getSize());

	normalShader.use();
	normalShader.setMat4("model",model);
	normalShader.setMat4("view",view);
	normalShader.setMat4("projection",proj);
	normalShader.setVec3("camPos", camera.Position);
	normalShader.setInt("heightMap", heightMap);
	normalShader.setInt("octaves", 50);
	normalShader.setFloat("scale", 70.f);
	if (normalMap)
	{
		normalShader.setInt("gridSize", 10);
		normalShader.setFloat("length", 1.f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_PATCHES, 0, Terrain.getSize());
	}
	
}





