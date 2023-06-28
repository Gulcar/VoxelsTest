#include "VoxelRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLT_IMPLEMENTATION
#include <glText/gltext.h>
#include "ChunkManager.h"
#include "Save.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdarg.h>
#include <limits>

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
    glm::mat4 m_otherViewProj; // ce je USE_DEBUG_CAMERA

    bool m_mouseEnabled = true;
    glm::dvec2 m_prevMousePos;
    bool m_lineMode = false;

    uint32_t m_lineVao;
    uint32_t m_lineVbo;
    glm::vec3 m_lineDrawBuffer[64];
    uint32_t m_lineDrawIndex = 0;

    uint32_t m_shadowFbo;
    uint32_t m_shadowTexture;
    uint32_t m_shadowShaderProgram;
    glm::mat4 m_shadowViewProj;
    constexpr int m_shadowMapSize = 8192;

    constexpr float m_moveSpeed = 2.0f;
    constexpr float m_sprintSpeedMult = 2.5f;
    constexpr double m_rotationSpeed = 0.9;


    void OnGlfwResize(GLFWwindow* window, int width, int height)
    {
        if (width == 0 || height == 0)
            return;

        m_windowSize = { width, height };
        glViewport(0, 0, width, height);
    }

    void CalcViewProjMat()
    {
        glm::mat4 view = glm::lookAt(m_camPos, m_camPos + m_camForward, glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)m_windowSize.x / m_windowSize.y, 0.01f, 100.0f);

        m_viewProj = projection * view;

        glm::mat4 oview = glm::lookAt(glm::vec3(0, 50, 0), glm::vec3(0, 0, -0.1f), glm::vec3(0, 1, 0));
        //glm::mat4 oview = glm::lookAt(glm::vec3(2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        m_otherViewProj = projection * oview;
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

        case GLFW_KEY_S:
            if (mods & GLFW_MOD_CONTROL)
                voxr::Save::SaveWorld();
            break;

        case GLFW_KEY_O:
            if (mods & GLFW_MOD_CONTROL)
                voxr::Save::OpenWorld();
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

    void OnGlfwError(int error, const char* desc)
    {
        std::cout << "GLFW ERROR " << error << ": " << desc << "\n";
    }
}

namespace voxr
{
    void CreateWindow(const char* title, int width, int height)
    {
        glfwSetErrorCallback(OnGlfwError);

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
        m_shadowShaderProgram = LoadShaderProgram("res/shadowVert.glsl", "res/shadowFrag.glsl");

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

        glGenVertexArrays(1, &m_lineVao);
        glBindVertexArray(m_lineVao);

        glGenBuffers(1, &m_lineVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_lineDrawBuffer), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 0));

        //

        glGenFramebuffers(1, &m_shadowFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFbo);

        glGenTextures(1, &m_shadowTexture);
        glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowMapSize, m_shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowTexture, 0);
        glDrawBuffer(GL_NONE); // oboje na none ker ta framebuffer ne bo imel barv
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Failed to create shadow map framebuffer!\n";

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

        glm::vec3 moveForward = m_camForward;
        glm::vec3 moveRight = m_camRight;

        if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL))
        {
            moveForward.y = 0.0f;
            moveForward = glm::normalize(moveForward);
            moveRight.y = 0.0f;
            moveRight = glm::normalize(moveRight);
        }

        if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT))
        {
            moveForward *= m_sprintSpeedMult;
            moveRight *= m_sprintSpeedMult;
        }

        if (glfwGetKey(m_window, GLFW_KEY_W))
            m_camPos += moveForward * m_moveSpeed * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_S))
            m_camPos -= moveForward * m_moveSpeed * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_D))
            m_camPos += moveRight * m_moveSpeed * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_A))
            m_camPos -= moveRight * m_moveSpeed * deltaTime;

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

        UpdateCameraFrustum(m_camPos, m_camForward, m_camRight, 0.01f, 25.0f, (float)m_windowSize.x / m_windowSize.y);
    }

    const glm::vec3& GetCameraPos()
    {
        return m_camPos;
    }

    void SetCameraPos(const glm::vec3& pos)
    {
        m_camPos = pos;
    }

    const glm::vec3& GetCameraForward()
    {
        return m_camForward;
    }

    const glm::vec2& GetCameraRot()
    {
        return m_camRot;
    }

    void SetCameraRot(const glm::vec2& rot)
    {
        m_camRot = rot;
    }

    void DrawCube(const glm::vec3& pos, const glm::vec3 color)
    {
#if USE_DEBUG_CAMERA
        glViewport(m_windowSize.x / 3, m_windowSize.y / 3, m_windowSize.x * 2 / 3, m_windowSize.y * 2 / 3);
#endif
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, glm::vec3(1.0f / 16.0f));

        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));

        glUseProgram(m_shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_viewProj[0][0]);
        glUniformMatrix3fv(glGetUniformLocation(m_shaderProgram, "uNormalMat"), 1, GL_FALSE, &normalMat[0][0]);
        glUniform3fv(glGetUniformLocation(m_shaderProgram, "uCameraPos"), 1, &m_camPos[0]);
        glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uShadowViewProj"), 1, GL_FALSE, &m_shadowViewProj[0][0]);

        glBindVertexArray(m_cubeVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

#if USE_DEBUG_CAMERA
        glViewport(0, 0, m_windowSize.x / 3, m_windowSize.y / 3);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_otherViewProj[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glViewport(0, 0, m_windowSize.x, m_windowSize.y);
#endif
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
#if USE_DEBUG_CAMERA
        glViewport(m_windowSize.x / 3, m_windowSize.y / 3, m_windowSize.x * 2 / 3, m_windowSize.y * 2 / 3);
#endif

        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);

        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));

        glUseProgram(m_shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_viewProj[0][0]);
        glUniformMatrix3fv(glGetUniformLocation(m_shaderProgram, "uNormalMat"), 1, GL_FALSE, &normalMat[0][0]);
        glUniform3fv(glGetUniformLocation(m_shaderProgram, "uCameraPos"), 1, &m_camPos[0]);
        glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uShadowViewProj"), 1, GL_FALSE, &m_shadowViewProj[0][0]);

        glBindVertexArray(chunk.GetVao());
        glDrawArrays(GL_TRIANGLES, 0, chunk.GetNumVertices());

#if USE_DEBUG_CAMERA
        glViewport(0, 0, m_windowSize.x / 3, m_windowSize.y / 3);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_otherViewProj[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, chunk.GetNumVertices());

        glViewport(0, 0, m_windowSize.x, m_windowSize.y);
#endif
    }

    void DrawLine(const glm::vec3& a, const glm::vec3& b)
    {
        m_lineDrawBuffer[m_lineDrawIndex] = a;
        m_lineDrawBuffer[m_lineDrawIndex + 1] = b;
        m_lineDrawIndex += 2;

        if (m_lineDrawIndex == (sizeof(m_lineDrawBuffer) / sizeof(m_lineDrawBuffer[0])))
            SubmitDrawLines();
    }
    
    void SubmitDrawLines()
    {
        if (m_lineDrawIndex == 0)
            return;

        glBindVertexArray(m_lineVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_lineDrawIndex, m_lineDrawBuffer);

        glm::mat4 model(1.0f);

        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);
        glUniform3fv(glGetUniformLocation(m_shaderProgram, "uCameraPos"), 1, &m_camPos[0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_viewProj[0][0]);

#if USE_DEBUG_CAMERA
        glViewport(m_windowSize.x / 3, m_windowSize.y / 3, m_windowSize.x * 2 / 3, m_windowSize.y * 2 / 3);
#endif

        glDrawArrays(GL_LINES, 0, m_lineDrawIndex);

#if USE_DEBUG_CAMERA
        glViewport(0, 0, m_windowSize.x / 3, m_windowSize.y / 3);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uViewProj"), 1, GL_FALSE, &m_otherViewProj[0][0]);
        glDrawArrays(GL_LINES, 0, m_lineDrawIndex);

        glViewport(0, 0, m_windowSize.x, m_windowSize.y);
#endif

        m_lineDrawIndex = 0;
    }

    void ShadowPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFbo);
        glViewport(0, 0, m_shadowMapSize, m_shadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);

        // https://learnopengl.com/Guest-Articles/2021/CSM

        std::array<glm::vec3, 8> frustumCorners = voxr::GetCamFrustumCorners();

        for (int i = 4; i < 8; i++)
        {
            const glm::vec3& near = frustumCorners[i - 4];
            const glm::vec3& far = frustumCorners[i];
            
            constexpr float shadowDistance = 6.0f;

            frustumCorners[i] = glm::normalize(far - near) * shadowDistance + near;
        }

        glm::vec3 frustumCenter = {};
        for (const auto& p : frustumCorners)
            frustumCenter += p;
        frustumCenter /= (float)frustumCorners.size();
        frustumCenter.y = 0.0f;

        constexpr glm::vec3 lightDir = glm::vec3(0.5f, -1.5f, -0.7f); // isto kot v shaderju
        glm::mat4 view = glm::lookAt(frustumCenter - lightDir * 10.0f, frustumCenter, glm::vec3(0, 1, 0));

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();

        float worldMinX = std::numeric_limits<float>::max();
        float worldMaxX = std::numeric_limits<float>::lowest();
        float worldMinZ = std::numeric_limits<float>::max();
        float worldMaxZ = std::numeric_limits<float>::lowest();

        for (const auto& c : frustumCorners)
        {
            glm::vec4 p = view * glm::vec4(c, 1.0);

            minX = glm::min(minX, p.x);
            maxX = glm::max(maxX, p.x);
            minY = glm::min(minY, p.y);
            maxY = glm::max(maxY, p.y);

            worldMinX = glm::min(worldMinX, c.x);
            worldMaxX = glm::max(worldMaxX, c.x);
            worldMinZ = glm::min(worldMinZ, c.z);
            worldMaxZ = glm::max(worldMaxZ, c.z);
        }

        glm::mat4 proj = glm::ortho(minX, maxX, minY, maxY, 1.0f, 50.0f);
        m_shadowViewProj = proj * view;

        glUseProgram(m_shadowShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(m_shadowShaderProgram, "uViewProj"), 1, GL_FALSE, &m_shadowViewProj[0][0]);

        for (int z = 0; z < ChunkManager::width; z++)
        {
            for (int x = 0; x < ChunkManager::width; x++)
            {
                glm::vec3 pos = glm::vec3(Chunk::worldWidth * x, 0.0f, Chunk::worldWidth * z);
                pos.x -= ChunkManager::width / 2 * Chunk::worldWidth;
                pos.z -= ChunkManager::width / 2 * Chunk::worldWidth;
                pos += glm::vec3(1.0f / 16.0f / 2.0f);
                pos += ChunkManager::GetCenterChunkPos();

                if (pos.x + Chunk::worldWidth / 2.0f < worldMinX ||
                    pos.x - Chunk::worldWidth / 2.0f > worldMaxX ||
                    pos.z + Chunk::worldWidth / 2.0f < worldMinZ ||
                    pos.z - Chunk::worldWidth / 2.0f > worldMaxZ)
                {
                    continue;
                }

                Chunk* chunk = ChunkManager::GetChunk(x, z);

                glm::mat4 model(1.0f);
                model = glm::translate(model, pos);

                glUniformMatrix4fv(glGetUniformLocation(m_shadowShaderProgram, "uModel"), 1, GL_FALSE, &model[0][0]);

                glBindVertexArray(chunk->GetVao());
                glDrawArrays(GL_TRIANGLES, 0, chunk->GetNumVertices());
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowSize.x, m_windowSize.y);
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

            std::exit(1);
        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }
}
