#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <random>

#include <shader.h>

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);

constexpr unsigned int WIDTH = 1024;
constexpr unsigned int HEIGHT = 800;

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to load glfw!\n";
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "RadixSort", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create glfw window!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	Shader screenShader;
	screenShader.Compile("shaders/screenVertex.glsl", "shaders/screenFragment.glsl");

	float screenQuad[] = {
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		1.0f,1.0f,0.0f
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad), screenQuad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	
	//seting up compute shader
	Shader computeShader;
	computeShader.CompileCompute("shaders/radixCompute.glsl");


	//COMPUTE SHADER

	std::random_device dev;
	std::mt19937 rnd{ dev() };
	std::uniform_int_distribution<int> dist{ 1,100000 };

	auto gen = [&]() {
		return dist(rnd);
		};

	/* std::vector<int> vec(10);
    std::generate(vec.begin(), vec.end(), gen);*/

	std::vector<int> test{ 5,2,7,0,1,6,};


	unsigned int ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, test.size() * sizeof(int), test.data(),GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	unsigned int ssbo_out;
	glGenBuffers(1, &ssbo_out);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_out);
	glBufferData(GL_SHADER_STORAGE_BUFFER, test.size() * sizeof(int), nullptr, GL_DYNAMIC_READ);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_out);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	std::vector<int> results(test.size());

	
	computeShader.use();
	glDispatchCompute(1, 1, 1);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_out);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, results.size() * sizeof(int), results.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	std::cout << "Wyniki z GPU: ";
	for (int val : results) 
	{
		std::cout << val << ", ";
	}
	std::cout << std::endl;
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindVertexArray(VAO);
		screenShader.use();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
