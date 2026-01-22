#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <vector>
#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/texture.h"
#include "core/Camera.h"
#include "shaders/shader.h"
#include "core/scene.h"

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
//class Scene {
//1    private:
//        int modelIndex = 0;// amount of models
//        int texturesIdex = 0; // amount of textures
//        std::vector<core::Model*> modelCollection; // store pointers
//        std::vector<core::Texture*> textures;
//    public:
//
//
//        void AddModel(core::Model* model) {
//            modelCollection.push_back(model);
//            modelIndex++;
//        }
//
//        void RemoveModel(core::Model* model) {
//            auto it = std::find(modelCollection.begin(), modelCollection.end(), model);
//            if (it != modelCollection.end()) {
//                modelCollection.erase(it);
//                modelIndex--;
//            }
//        }
//
//        void AddTexture(core::Texture* texture) {
//            textures.push_back(texture);
//            texturesIdex++;
//        }
//        void RemoveTexture(core::Texture* & texture) {
//            auto it = std::find(textures.begin(), textures.end(), texture);
//            if (it != textures.end()) {
//                textures.erase(it);
//                texturesIdex--;
//            }
//        }
//
//        void LoadScene(glm::vec3* lightPos, GLuint  modelShaderProgram, GLuint textureShaderProgram) {
//            for (int i = 0; i < modelIndex; i++) {
//
//                glUseProgram(modelShaderProgram);
//                glUniform3fv(lightPosUniform, 1, glm::value_ptr(lightPos));
//                glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.getModelMatrix()));
//                glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(suzanne.getModelMatrix()));
//                suzanne.render(drawMode);
//            }
//            
//        }
//    };

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
    /// ------------------------------------------------
    ///     Window initialization
    /// ------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);// for resizing window

    // CHeck if Glad was loaded
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// controll type

    //Setup platforms
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /// ------------------------------------------------
    ///     GLuint Shaders initialization
    /// ------------------------------------------------
    const GLuint modelVertexShader = generateShader("shaders/modelVertex.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader = generateShader("shaders/fragment.fs", GL_FRAGMENT_SHADER);
    const GLuint textureShader = generateShader("shaders/texture.fs", GL_FRAGMENT_SHADER);
    const GLuint litShader = generateShader("shaders/LitShader.fs", GL_FRAGMENT_SHADER);

    
    /// ------------------------------------------------
	///     Shader Programs initialization
    /// ------------------------------------------------    
	core::Shader modelShaderProgram(modelVertexShader, fragmentShader);
    core::Shader textureShaderProgram(modelVertexShader, fragmentShader);
    core::Shader litShaderProgram(modelVertexShader, fragmentShader);
    
    // Close calls
    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);
    glDeleteShader(litShader);

    /// ------------------------------------------------------------------------
    // Generate Quad meshes for frame Buffers
    /// ------------------------------------------------------------------------
    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({ quad });
	glm::vec3 quadTranslation = { 0, 0, -2.5 };
    glm::vec3 quadScale = { 5, 5, 1 };
    quadModel.translate(quadTranslation);
    quadModel.scale(quadScale);

    /// ------------------------------------------------------------------------
    ///         Model and texture initialization and registration
    /// ------------------------------------------------------------------------
    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Model lightBulbSuzane = core::AssimpLoader::loadModel("models/sz.obj");
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

    /// -----------------------------
    /// Scenes
    /// -----------------------------
    
	std::vector<core::Scene*> scenes;
    
        /// -----------------------------
        /// Scenes 1
        /// -----------------------------
        core::Scene SceneOne;

        SceneOne.addModel(std::make_shared<core::Model>(suzanne));

	    scenes.push_back(&SceneOne);

        /// -----------------------------
        /// Scenes 2
        /// -----------------------------
        core::Scene SceneTwo;

        SceneTwo.addModel(std::make_shared<core::Model>(suzanne));

        scenes.push_back(&SceneTwo);
    

    /// -----------------------------
    /// Camera(s) Data
    /// -----------------------------
    core::Camera mainCamera;

    /// -----------------------------
    /// View  &  Projection
    /// -----------------------------
    glm::mat4 view = mainCamera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);


    /// Uniforms
    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram.ID, "mvpMatrix");
    GLint lightPosUniformModel = glGetUniformLocation(modelShaderProgram.ID, "lightPosition");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram.ID, "mvpMatrix");
    GLint modelMatrix = glGetUniformLocation(textureShaderProgram.ID, "modelMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram.ID, "text");

    GLint lightPosUniform = glGetUniformLocation(litShaderProgram.ID, "lightPosition");
    GLint lightColorUniform = glGetUniformLocation(litShaderProgram.ID, "ambientLightColor");
    GLint lightIntensityUniform = glGetUniformLocation(litShaderProgram.ID, "ambientLightIntensity");
    GLint cameraPositionUniformLitSHader = glGetUniformLocation(litShaderProgram.ID, "cameraPosition");
    /// -----------------------------
    /// Loop Runtime
    /// -----------------------------
    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    float rotationStrength = 100.0f;
    std::string append = "Triangles";

    /// -----------------------------
    /// Light vars Runtime
    /// -----------------------------
    /// 
    glm::vec3 lightDir = glm::vec3(1, 0, 0);
    glm::vec3 lightColor = glm::vec3(1, 0, 0);
    float lightIntensity = .1f;
    glm::vec3 lightbulbSuzana{ 1.0f, 2.0f, 5.0f };
    lightBulbSuzane.position(lightbulbSuzana);

    glm::vec3 suzzaneRotate{ 0.0f, 1.0f, 0.0f };
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // -----------------------------
// ImGui Frame Start
// -----------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // -----------------------------
        // Main Window
        // -----------------------------
        ImGui::Begin("Raw Engine v2", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // FPS Display
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.3f, 1.0f), "FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Separator();

        // ---------- Camera Settings ----------
        if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Enable Rotation", &mainCamera.enableRotation);
            ImGui::SliderFloat("Mouse Sensitivity", &mainCamera.mouseSensitivity, 0.001f, 0.2f, "%.4f");

            ImGui::Text("Position: X: %.2f Y: %.2f Z: %.2f", mainCamera.position.x, mainCamera.position.y, mainCamera.position.z);
        }

        // ---------- Draw Mode ----------
        if (ImGui::CollapsingHeader("Draw Mode", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char* drawModes[] = {
                "Triangles", "Wireframe", "Points", "Line Strip", "Line Loop", "Triangle Strip", "Triangle Fan"
            };
            static int currentDrawMode = 0;

            if (ImGui::Combo("Mode", &currentDrawMode, drawModes, IM_ARRAYSIZE(drawModes))) {
                switch (currentDrawMode) {
                case 0: drawMode = GL_TRIANGLES; append = "Triangles"; break;
                case 1: drawMode = GL_LINES; append = "Wire Frame"; break;
                case 2: drawMode = GL_POINTS; append = "Points"; break;
                case 3: drawMode = GL_LINE_STRIP; append = "Line strips"; break;
                case 4: drawMode = GL_LINE_LOOP; append = "Line loops"; break;
                case 5: drawMode = GL_TRIANGLE_STRIP; append = "Triangle strip"; break;
                case 6: drawMode = GL_TRIANGLE_FAN; append = "Triangle fan"; break;
                }
            }
        }

        // ---------- Application Controls ----------
        ImGui::Separator();
        if (ImGui::Button("Quit Application", ImVec2(-1, 0))) {
            glfwSetWindowShouldClose(window, true);
        }

        ImGui::End();

        // -----------------------------
        // Game Objects Window
        // -----------------------------
        ImGui::Begin("Game Objects", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::CollapsingHeader("Scene Overview", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Camera Position:");
            ImGui::Indent();
            ImGui::Text("X: %.2f", mainCamera.position.x);
            ImGui::Text("Y: %.2f", mainCamera.position.y);
            ImGui::Text("Z: %.2f", mainCamera.position.z);
            ImGui::Unindent();

            ImGui::Text("Current Draw Mode: %s", append.c_str());
        }

        ImGui::End();

        // -----------------------------
        // Light Settings Window
        // -----------------------------
        ImGui::Begin("Light Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat3("Direction", &lightDir.x, -10.0f, 10.0f, "%.2f");
            ImGui::ColorEdit3("Color", &lightColor.x);
            ImGui::SliderFloat("Intensity", &lightIntensity, 0.01f, 1.0f, "%.2f");
        }

        ImGui::End();

        /// -----------------------------
        /// Other procesess
        /// -----------------------------

        processInput(window);
        
        suzanne.rotate(suzzaneRotate, glm::radians(rotationStrength) * static_cast<float>(deltaTime));
        
        mainCamera.CameraMovement(window);
        view = mainCamera.GetViewMatrix();
        
       // view = glm::lookAt(mainCamera.position, mainCamera.lookPivot, mainCamera.up);
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);

        //cameraTarget = cameraPos + cameraDirection;
        //view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            //glfwSetWindowShouldClose(window, true);


        // foreach (Object (or model) obj in scene)  
        //      obj.material.Bind()  // loads the shader + load proper textures in the right slots + set other uniforms (like tint)
        //      set MVP using obj.matrix
        //      TODO later: pass lights
        //      obj.render
        //
        // std::Vector<std:Vector<Model>> Scenes; // initialize scenes.
        // add stuff to scenes a few models
        // int sceneIndex;// use in ImGui to controll scene loading. mybe make a bar scenes and make a list of scene names...
        // a for loop to loead Scenes[sceneIndex] models. Models hold a material.with a shader programm. 
        // We do glUseProgram( model.Material.Shader);
        // glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * quadModel.getModelMatrix()));
        // Inside the material. The material Does:
        //glActiveTexture(GL_TEXTURE0);
        //glUniform1i(textureUniform, 0);
        //glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        // and then we call:
        // model.render(drawMode);
        glUseProgram(textureShaderProgram.ID); // This is a task for the material
        // This line does the whole space transformation from object  > world > camera > screen (more or less)
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * quadModel.getModelMatrix()));
        
        // This is also task of material:
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        
        glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        
        quadModel.render(drawMode);
        glBindVertexArray(0);
        //glActiveTexture(GL_TEXTURE0);

        glUseProgram(litShaderProgram.ID);
        glUniform3fv(lightPosUniform, 1, glm::value_ptr(lightDir));
        glUniform3fv(lightColorUniform, 1, glm::value_ptr(lightColor));
        glUniform1f(lightIntensityUniform, lightIntensity);
        glUniform3fv(cameraPositionUniformLitSHader, 1, glm::value_ptr(mainCamera.position));
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.getModelMatrix()));
        glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(suzanne.getModelMatrix()));
        suzanne.render(drawMode);
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection* view* lightBulbSuzane.getModelMatrix()));
        glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(lightBulbSuzane.getModelMatrix()));
        lightBulbSuzane.render(drawMode);
        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(modelShaderProgram.ID);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}