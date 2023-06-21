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
    glm::vec2 m_camRot = {};
    glm::vec3 m_camForward = {};
    glm::vec3 m_camRight = {};

    glm::mat4 m_viewProj;

    bool m_mouseEnabled = true;
    glm::dvec2 m_prevMousePos;
    bool m_lineMode = false;

    constexpr float m_moveSpeed = 1.5f;
    constexpr double m_rotationSpeed = 0.9;


    void OnGlfwResize(GLFWwindow* window, int width, int height)
    {
        m_windowSize = { width, height };
        glViewport(0, 0, width, height);
    }

    void CalcViewProjMat()
    {
        glm::mat4 view = glm::lookAt(m_camPos, m_camPos + m_camForward, glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)m_windowSize.x / m_windowSize.y, 0.01f, 100.0f);

        m_viewProj = projection * view;
    }

    void OnGlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (action != GLFW_PRESS) return;

        switch (key)
        {
        case GLFW_KEY_F: 
            m_lineMode = !m_lineMode;
            glPolygonMode(GL_FRONT_AND_BACK, m_lineMode ? GL_LINE : GL_FILL);
            break;

        case GLFW_KEY_ESCAPE:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_mouseEnabled = false;
            break;
        }
    }

    void OnGlfwMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        if (action != GLFW_PRESS) return;

        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
        case GLFW_MOUSE_BUTTON_RIGHT:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_mouseEnabled = true;
            glfwGetCursorPos(m_window, &m_prevMousePos.x, &m_prevMousePos.y);
            break;
        }
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

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(m_window, &m_prevMousePos.x, &m_prevMousePos.y);

        glfwSetKeyCallback(m_window, OnGlfwKey);
        glfwSetMouseButtonCallback(m_window, OnGlfwMouseButton);

        //

        float cubeVertices[] = {
            // back face
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 1.0f, // bottom-left
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 1.0f, // top-right
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 1.0f, // bottom-right         
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 1.0f, // top-right
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 1.0f, // bottom-left
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 1.0f, // top-left
            // front face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f, // bottom-left
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f, // bottom-right
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f, // top-right
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1.0f, // bottom-left
            // left face
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-left
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-right
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            // right face
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-left
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-right
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right         
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-right
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-left
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-left     
            // bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-left
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-left
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-left
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-right
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            // top face
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-left
             0.5f,  0.5f , 0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-right
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right     
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-right
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-left
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-left        
        };

        uint32_t cubeVbo;

        glGenVertexArrays(1, &m_cubeVao);
        glBindVertexArray(m_cubeVao);

        glGenBuffers(1, &cubeVbo);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float) * 0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float) * 6));

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
        glm::mat4 rotMat(1.0f);
        rotMat = glm::rotate(rotMat, m_camRot.x, glm::vec3(0, -1, 0));
        rotMat = glm::rotate(rotMat, m_camRot.y, glm::vec3(-1, 0, 0));
        m_camForward = rotMat * glm::vec4(0, 0, -1, 1);

        rotMat = glm::rotate(rotMat, PI / 2.0f, glm::vec3(0, -1, 0));
        m_camRight = rotMat * glm::vec4(0, 0, -1, 1);

        if (glfwGetKey(m_window, GLFW_KEY_W))
            m_camPos += m_camForward * m_moveSpeed * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_S))
            m_camPos -= m_camForward * m_moveSpeed * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_D))
            m_camPos += m_camRight * m_moveSpeed * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_A))
            m_camPos -= m_camRight * m_moveSpeed * deltaTime;

        if (glfwGetKey(m_window, GLFW_KEY_E))
            m_camPos.y += m_moveSpeed * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_Q))
            m_camPos.y -= m_moveSpeed * deltaTime;

        if (m_mouseEnabled)
        {
            glm::dvec2 mousePos;
            glfwGetCursorPos(m_window, &mousePos.x, &mousePos.y);

            glm::dvec2 deltaMouse = mousePos - m_prevMousePos;
            m_prevMousePos = mousePos;

            m_camRot += deltaMouse / 1000.0 * m_rotationSpeed;
            m_camRot.y = glm::clamp(m_camRot.y, -PI / 2.1f, PI / 2.1f);
        }

        CalcViewProjMat();
    }

    void DrawCube(const glm::vec3& pos, const glm::vec3 color)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, glm::vec3(1.0f / 16.0f));

        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));

        glUseProgram(m_shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_viewProj[0][0]);
        glUniformMatrix3fv(glGetUniformLocation(m_shaderProgram, "uNormalMat"), 1, GL_FALSE, &normalMat[0][0]);

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

    void DrawChunk(const Chunk& chunk, const glm::vec3& pos)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);

        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));

        glUseProgram(m_shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_viewProj[0][0]);
        glUniformMatrix3fv(glGetUniformLocation(m_shaderProgram, "uNormalMat"), 1, GL_FALSE, &normalMat[0][0]);

        glBindVertexArray(chunk.GetVao());
        glDrawArrays(GL_TRIANGLES, 0, chunk.GetNumVertices());

    }

    uint32_t LoadShader(std::string_view source, GLenum type)
    {
        const char* data = source.data();

        uint32_t shader = glCreateShader(type);
        glShaderSource(shader, 1, &data, 0);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            int logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            char* log = new char[logLength];
            glGetShaderInfoLog(shader, logLength, 0, log);
            printf("Failed to compile shader!\n%s\n", log);
        }

        return shader;
    }

    uint32_t LoadShaderProgram(const char* vertShaderFile, const char* fragShaderFile)
    {
        std::ifstream file(vertShaderFile);
        if (file.is_open() == false) file.open(std::string("../../") + vertShaderFile);
        if (file.is_open() == false) file.open(std::string("../") + vertShaderFile);
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
        if (file.is_open() == false) file.open(std::string("../../") + fragShaderFile);
        if (file.is_open() == false) file.open(std::string("../") + fragShaderFile);
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
