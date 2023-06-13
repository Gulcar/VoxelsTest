#include "VoxelRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLT_IMPLEMENTATION
#include <glText/gltext.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdarg.h>

// anonymous namespace
namespace
{
	GLFWwindow* m_window;
	glm::ivec2 m_windowSize;

	uint32_t m_shaderProgram;
	uint32_t m_cubeVao;

	glm::vec3 m_camPos = { 0.0f, 0.5f, 2.0f };
	float m_camRot = -PI / 2.0f;
	glm::vec3 m_camForward = {};
	glm::vec3 m_camRight = {};

	constexpr float m_moveSpeed = 1.5f;
	constexpr float m_rotationSpeed = 1.5f;

	void OnGlfwResize(GLFWwindow* window, int width, int height)
	{
		m_windowSize = { width, height };
		glViewport(0, 0, width, height);
	}
}

namespace voxr
{
	void CreateWindow(const char* title, int width, int height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(width, height, title, 0, 0);
		glfwMakeContextCurrent(m_window);

		m_windowSize = { width, height };
		glfwSetFramebufferSizeCallback(m_window, OnGlfwResize);

		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		glViewport(0, 0, width, height);

		std::cout << glGetString(GL_VERSION) << "\n";

		m_shaderProgram = LoadShaderProgram("res/vert.glsl", "res/frag.glsl");

		gltInit();

		glfwSwapInterval(0);

		//

		float cubeVertices[] = {
			// back face
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // bottom-left
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // top-right
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // bottom-right         
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // top-right
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // bottom-left
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // top-left
			// front face
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // bottom-left
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // bottom-right
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-left
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // bottom-left
			// left face
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // top-right
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // bottom-left
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // bottom-right
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // top-right
			// right face
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // top-left
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // bottom-right
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // top-right         
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // bottom-right
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // top-left
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // bottom-left     
			// bottom face
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // top-right
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // top-left
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // bottom-left
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // bottom-left
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // top-right
			// top face
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // top-left
			 0.5f,  0.5f , 0.5f,  0.0f,  1.0f,  0.0f, // bottom-right
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // top-right     
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // bottom-right
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // top-left
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // bottom-left        
		};

		uint32_t cubeVbo;

		glGenVertexArrays(1, &m_cubeVao);
		glBindVertexArray(m_cubeVao);

		glGenBuffers(1, &cubeVbo);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 0));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

		//

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	GLFWwindow* GetWindow()
	{
		return m_window;
	}

	void UpdateCamera(float deltaTime)
	{
		m_camForward = {
			cos(m_camRot),
			0.0f,
			sin(m_camRot),
		};

		m_camRight = {
			cos(m_camRot + PI / 2.0f),
			0.0f,
			sin(m_camRot + PI / 2.0f),
		};

		float speed = 1.4f * deltaTime;

		if (glfwGetKey(m_window, GLFW_KEY_W))
			m_camPos += m_camForward * m_moveSpeed * deltaTime;
		if (glfwGetKey(m_window, GLFW_KEY_S))
			m_camPos -= m_camForward * m_moveSpeed * deltaTime;
		if (glfwGetKey(m_window, GLFW_KEY_D))
			m_camPos += m_camRight * m_moveSpeed * deltaTime;
		if (glfwGetKey(m_window, GLFW_KEY_A))
			m_camPos -= m_camRight * m_moveSpeed * deltaTime;

		if (glfwGetKey(m_window, GLFW_KEY_LEFT))
			m_camRot -= m_rotationSpeed * deltaTime;
		if (glfwGetKey(m_window, GLFW_KEY_RIGHT))
			m_camRot += m_rotationSpeed * deltaTime;
	}

	void DrawCube(const glm::vec3& pos, const glm::vec3 color)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(1.0f / 16.0f));

		glm::mat4 view = glm::lookAt(m_camPos, m_camPos + m_camForward, glm::vec3(0, 1, 0));
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)m_windowSize.x / m_windowSize.y, 0.01f, 100.0f);

		glUseProgram(m_shaderProgram);

		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uView"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uProjection"), 1, GL_FALSE, &projection[0][0]);
		glUniform3fv(glGetUniformLocation(m_shaderProgram, "uColor"), 1, &color.r);

		glBindVertexArray(m_cubeVao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void DrawText(std::string_view text, glm::vec2 pos)
	{
		gltBeginDraw();

		static GLTtext* gltText = gltCreateText();
		gltSetText(gltText, text.data());

		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltDrawText2D(gltText, pos.x, pos.y, 2.0f);

		gltEndDraw();
	}

	void DrawTextF(std::string_view format, glm::vec2 pos, ...)
	{
		va_list args;
		va_start(args, pos);

		char buf[48];

		vsprintf(buf, format.data(), args);

		va_end(args);

		DrawText(buf, pos);
	}

	uint32_t LoadShader(std::string_view source, GLenum type)
	{
		const char* data = source.data();

		uint32_t shader = glCreateShader(type);
		glShaderSource(shader, 1, &data, 0);
		glCompileShader(shader);
		return shader;
	}

	uint32_t LoadShaderProgram(const char* vertShaderFile, const char* fragShaderFile)
	{
		std::ifstream file(vertShaderFile);
		if (file.is_open() == false) file.open(std::string("../../../") + vertShaderFile);
		if (file.is_open() == false)
		{
			std::cout << "failed to open file: " << vertShaderFile << "\n";
			return 0;
		}
		std::stringstream ss;
		ss << file.rdbuf();
		std::string vertexSource = ss.str();
		file.close();

		file.open(fragShaderFile);
		if (file.is_open() == false) file.open(std::string("../../../") + fragShaderFile);
		if (file.is_open() == false)
		{
			std::cout << "failed to open file: " << fragShaderFile << "\n";
			return 0;
		}
		ss.str("");
		ss << file.rdbuf();
		std::string fragmentSource = ss.str();

		uint32_t vertexShader = LoadShader(vertexSource, GL_VERTEX_SHADER);
		uint32_t fragmentShader = LoadShader(fragmentSource, GL_FRAGMENT_SHADER);

		uint32_t program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		int success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			int logLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			char* log = new char[logLength];
			glGetProgramInfoLog(program, logLength, 0, log);
			printf("Failed to link shader!\n%s\n", log);
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return program;
	}
}