#ifndef PLANET_H
#define PLANET_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <string>
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
class Planet{
public:
	Planet(float x0, float y0, float z0, float r0,  std::string address) {
		x = x0;
		y = y0;
		z = z0;
		r = r0;
		data = sphere(x, y, z, r);
		index = sphere_index();
		shaderInit();
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
		shader->use();
		shader->setInt("texture", 0);
	}
	~Planet() {}
	void shaderInit() {
		Shader ourShader("texture.vs", "texture.fs");
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		shader = &ourShader;
	}
	void flush(glm::mat4 projection, glm::mat4 transform, glm::mat4 view) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		shader->use();
		shader->setMat4("projection", projection);
		shader->setMat4("transform", transform);
		shader->setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(-x, -y, -z));
		float angle = 20.0f * 0;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		shader->setMat4("model", model);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 40 * 73 * 6, GL_UNSIGNED_INT, 0);
	}
	void free() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
private:
	float x;
	float y;
	float z;
	float r;
	float* data;
	unsigned int* index;
	unsigned int VBO, VAO, EBO;
	Shader* shader;
	unsigned texture1;
};
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
#endif