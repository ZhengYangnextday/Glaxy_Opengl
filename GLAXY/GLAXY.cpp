// Presentation.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include "shader.h"
#include "camera.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
#define PI 3.1415926
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void changeColor(GLFWwindow* window);
float* sphere(float x, float y, float z, float r);
unsigned int* sphere_index();
typedef struct color {
	float red;
	float green;
	float blue;
	float alpha;
}color;
color list[] = { {1, 0, 0, 1},  {0, 1, 0, 1}, {0, 0, 1, 1} };
class Planet {
public:
	Planet(float x0, float y0, float z0, float r0, std::string address, float self_time = 0, float public_time = 0) {
		x = x0;
		y = y0;
		z = z0;
		r = r0;
		st = self_time;
		pt = public_time;
		data = sphere(x, y, z, r);
		index = sphere_index();
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 73 * 5 * 41 * sizeof(float), data, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 73 * 6 * 40 * sizeof(unsigned int), index, GL_STATIC_DRAW);

		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, 73 * 5 * 11 * sizeof(float), sphere(0, 0, 0, 0.5), GL_STATIC_DRAW);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		shader.Init("texture.vs", "texture.fs");
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(address.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		shader.use();
		shader.setInt("texture", 0);
	}
	~Planet() {}
	void flush(glm::mat4* projection, glm::mat4* view, float currentFrame) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		shader.use();
		shader.setMat4("projection", *projection);
		shader.setMat4("view", *view);
		glm::mat4 sf;//自转
		sf = glm::translate(sf, glm::vec3(-x, -y, -z));
		sf = glm::rotate(sf, glm::radians(currentFrame / st), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("sf", sf);
		glm::mat4 pb;//公转
		pb = glm::translate(pb, glm::vec3(x, y, z));
		pb = glm::rotate(pb, glm::radians(currentFrame / pt), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("pb", pb);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 40 * 73 * 6, GL_UNSIGNED_INT, 0);
	}
	void free() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	Shader shader;
private:
	float st;
	float pt;
	float x;
	float y;
	float z;
	float r;
	float* data;
	unsigned int* index;
	unsigned int VBO, VAO, EBO;
	unsigned int texture1;
};
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_WIDTH, "GLAXY Demo", NULL, NULL);//宽度，高度标题
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//初始化窗口
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//初始化Glad
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glEnable(GL_DEPTH_TEST);
	//准备操作结束
	//
	//
	//
	Planet earth(3, 3, 0, 1, "Source/earth.jpg",1, 365);
	Planet sun(0, 0, 0, 2, "Source/sun.jpg", 1, 1);
	//
	//
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//
		//
		processInput(window);
		changeColor(window);
		//
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		earth.flush(&projection, &view, currentFrame * 30);
		sun.flush(&projection, &view, currentFrame);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// ------------------------------------------------------------------------
	earth.free();
	glfwTerminate();
	return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
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
}
void changeColor(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		glClearColor(list[0].red, list[0].green, list[0].blue, list[0].alpha);
		//glClear(GL_COLOR_BUFFER_BIT);
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		glClearColor(list[1].red, list[1].green, list[1].blue, list[1].alpha);
		//glClear(GL_COLOR_BUFFER_BIT);
	}
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		glClearColor(list[2].red, list[2].green, list[2].blue, list[2].alpha);
		//glClear(GL_COLOR_BUFFER_BIT);
	}
	else {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
float* sphere(float x, float y, float z, float r) {
	float* data = new float[73 * 41 * 5];
	//总共11层，每层72个点（5度一分）, 每个点对应5个值，前三个表示具体坐标，后两位表示渲染位置
	//73原因是方便渲染器渲染
	float h = r;
	for (int i = 0; i < 41; i++) {
		float r_now = sqrt(abs(r * r - h * h));
		for (int j = 0; j < 73; j++) {
			data[i * 73 * 5 + j * 5] = x + r_now * cos(2 * PI * (j % 72) * 5.0 / 360);
			data[i * 73 * 5 + j * 5 + 1] = y + r_now * sin(2 * PI * (j % 72) * 5.0 / 360);
			data[i * 73 * 5 + j * 5 + 2] = z + h;
			data[i * 73 * 5 + j * 5 + 3] = 1.0 / 72 * j;
			data[i * 73 * 5 + j * 5 + 4] = 1.0 / 40 * i;
		}
		h -= r / 20;
	}
	return data;
}
unsigned int* sphere_index() {
	unsigned int* index = new unsigned int[40 * 73 * 6];
	unsigned int cnt = 0;
	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 73; j++) {
			index[cnt] = i * 73 + j % 73;
			index[cnt + 1] = i * 73 + (j + 1) % 73;
			index[cnt + 2] = (i + 1) * 73 + j % 73;
			index[cnt + 3] = (i + 1) * 73 + j % 73;
			index[cnt + 4] = (i + 1) * 73 + (j + 1) % 73;
			index[cnt + 5] = i * 73 + (j + 1) % 73;
			cnt += 6;
		}
	}
	return index;
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
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}