#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/texture.h"
#include "core/Camera.h"

//#define MAC_CLION
#define VSTUDIO

#ifdef MAC_CLION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#ifdef VSTUDIO
// Note: install imgui with:
//     ./vcpkg.exe install imgui[glfw-binding,opengl3-binding]
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

int g_width = 800;
int g_height = 600;
GLenum drawMode = GL_TRIANGLES;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow* window,
    int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
}

std::string readFileToString(const std::string& filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        printf("Could not open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

GLuint generateShader(const std::string& shaderPath, GLuint shaderType) {
    printf("Loading shader: %s\n", shaderPath.c_str());
    const std::string shaderText = readFileToString(shaderPath);
    const GLuint shader = glCreateShader(shaderType);
    const char* s_str = shaderText.c_str();
    glShaderSource(shader, 1, &s_str, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
    }
    return shader;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Setup platforms
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    const GLuint modelVertexShader = generateShader("shaders/modelVertex.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader = generateShader("shaders/fragment.fs", GL_FRAGMENT_SHADER);
    const GLuint textureShader = generateShader("shaders/texture.fs", GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];
    const unsigned int modelShaderProgram = glCreateProgram();
    glAttachShader(modelShaderProgram, modelVertexShader);
    glAttachShader(modelShaderProgram, fragmentShader);
    glLinkProgram(modelShaderProgram);
    glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(modelShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }
    const unsigned int textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, modelVertexShader);
    glAttachShader(textureShaderProgram, textureShader);
    glLinkProgram(textureShaderProgram);
    glGetProgramiv(textureShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textureShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);

    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({ quad });
    quadModel.translate(glm::vec3(0, 0, -2.5));
    quadModel.scale(glm::vec3(5, 5, 1));

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
        clearColor.g, clearColor.b, clearColor.a);
    core::Camera mainCamera;
    //glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //mainCamera.forward = glm::normalize(mainCamera.position-mainCamera.lookPivot);
    //glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    //glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    //glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    //glm::vec3 right; 

    //VP
    glm::mat4 view = glm::lookAt(mainCamera.position, mainCamera.lookPivot, mainCamera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);

    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");

    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    float rotationStrength = 100.0f;
    std::string append = "Triangles";
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Raw Engine v2", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        if (ImGui::Button("Print test")) {
            printf("Button pressed\n");
        }
        
        std::string wireName = "Draw Mode: " + append;
        if (ImGui::Button(wireName.c_str())) {
            if (drawMode == GL_TRIANGLES) {
                drawMode = GL_LINES;
                append = "Wire Frame";
            }
            else if (drawMode == GL_LINES) {
                drawMode = GL_POINTS;
                append = "Points";
            }
            else if (drawMode == GL_POINTS) {
                drawMode = GL_LINE_STRIP;
                append = "Line strips";
            }
            else if (drawMode == GL_LINE_STRIP) {
                drawMode = GL_LINE_LOOP;
                append = "Line loops";
            }
            else if (drawMode == GL_LINE_LOOP) {
                drawMode = GL_TRIANGLE_STRIP;
                append = "Triangel strip";
            }
            else if (drawMode == GL_TRIANGLE_STRIP) {
                drawMode = GL_TRIANGLE_FAN;
                append = "Triangel fan";
            }
            else {
                drawMode = GL_TRIANGLES; 
                append = "Triangles";
            }
            printf(" Wireframe button pressed: %s\n", append.c_str());
        }

        //Quit Application
        if (ImGui::Button("Quit Application")) {
            glfwSetWindowShouldClose(window, true);  
        }

        ImGui::End();

        // === SECOND WINDOW ===
        ImGui::Begin("Game Objects", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Scene Overview");
        ImGui::Separator();
        ImGui::Text("Camera Position: X: %.2f, Y: %.2f, Z: %.2f", mainCamera.position.x, mainCamera.position.y, mainCamera.position.z);
        ImGui::Text("Draw Mode: %s", drawMode == GL_TRIANGLES ? "TRIANGLES" : "LINES");

        ImGui::End();

        processInput(window);
        suzanne.rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));

        // Recalculate the direction vectors ONCE per frame
        mainCamera.forward = glm::normalize(mainCamera.lookPivot - mainCamera.position);
        mainCamera.right = glm::normalize(glm::cross(mainCamera.forward, mainCamera.globalUp));
        mainCamera.up = glm::normalize(glm::cross(mainCamera.right, mainCamera.forward));

        // W/S - forward/backward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            mainCamera.position += 0.01f * mainCamera.forward;
            printf("Cam y:%f\n", mainCamera.position.z);
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            mainCamera.position -= 0.01f * mainCamera.forward;
            printf("Cam y:%f\n", mainCamera.position.z);
        }

        // A/D - strafe left/right
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            mainCamera.position -= 0.01f * mainCamera.right;
            printf("Cam y:%f\n", mainCamera.position.x);
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            mainCamera.position += 0.01f * mainCamera.right;
            printf("Cam y:%f\n", mainCamera.position.x);
        }
        

        // Q/E - up/down
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            mainCamera.position += 0.01f * mainCamera.up;
            printf("Cam y:%f\n", mainCamera.position.y);
        }
        else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            mainCamera.position -= 0.01f * mainCamera.up;
            printf("Cam y:%f\n", mainCamera.position.y);
        }

        // Update lookPivot and view matrix
        mainCamera.lookPivot = mainCamera.position + mainCamera.forward;
        view = glm::lookAt(mainCamera.position, mainCamera.lookPivot, mainCamera.up);
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);

        //cameraTarget = cameraPos + cameraDirection;
        //view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            //glfwSetWindowShouldClose(window, true);

        glUseProgram(textureShaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * quadModel.getModelMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        quadModel.render(drawMode);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);

        glUseProgram(modelShaderProgram);
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.getModelMatrix()));
        suzanne.render(drawMode);
        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(modelShaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}