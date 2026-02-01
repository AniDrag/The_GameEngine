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
    // Generate Quad meshes for image gato
    /// ------------------------------------------------------------------------
    core::Mesh quad = core::Mesh::generateQuad();
    core::Mesh screenQuadModel = core::Mesh::generateQuad();
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
    core::Texture cmgtGatoTexture("textures/dragonTexture.png");// CMGaTo_crop.png
    suzanne.material.addAlbedoTexture("textures/dragonTexture.png");
	auto litShaderProgramPtr = std::make_shared<core::Shader>(litShaderProgram);
    suzanne.attachShader(litShaderProgramPtr);


   


    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

    /// -----------------------------
    /// Scenes
    /// -----------------------------
    
	std::vector<core::Scene*> scenes;
    static int activeSceneIndex = 0;
    core::Scene* activeScene = nullptr;

    

    
        /// -----------------------------
        /// Scenes 1
        /// -----------------------------
        core::Scene SceneOne(window);

        // Assign shader to models before adding to scene
        auto suzanneModel1 = std::make_shared<core::Model>(suzanne);
        SceneOne.addModel(suzanneModel1);


	    scenes.push_back(&SceneOne);

        /// -----------------------------
        /// Scenes 2
        /// -----------------------------
        core::Scene SceneTwo(window);


        auto suzanneModel2 = std::make_shared<core::Model>(suzanne);
        SceneTwo.addModel(suzanneModel2);
        SceneTwo.addModel(suzanneModel1);
        glm::vec3 positionSuzi = glm::vec3(-2.0f, 0.0f, 0.0f);
		suzanneModel1->position(positionSuzi);

        scenes.push_back(&SceneTwo);

        if (!scenes.empty()) {
            // clamp index to prevent out-of-range access
            if (activeSceneIndex >= scenes.size()) activeSceneIndex = int(scenes.size() - 1);
            activeScene = scenes[activeSceneIndex];
        }
    

    /// -----------------------------
    /// Camera(s) Data
    /// -----------------------------
  //  core::Camera mainCamera;

    /// -----------------------------
    /// View  &  Projection
    /// -----------------------------
   // glm::mat4 view = mainCamera.GetViewMatrix();
   // glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);


    /// Uniforms
  //  GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram.ID, "mvpMatrix");
  //  GLint lightPosUniformModel = glGetUniformLocation(modelShaderProgram.ID, "lightPosition");
  //  GLint textureModelUniform = glGetUniformLocation(textureShaderProgram.ID, "mvpMatrix");
  //  GLint modelMatrix = glGetUniformLocation(textureShaderProgram.ID, "modelMatrix");
  //  GLint textureUniform = glGetUniformLocation(textureShaderProgram.ID, "text");
  //
  //  GLint lightPosUniform = glGetUniformLocation(litShaderProgram.ID, "lightPosition");
  //  GLint lightColorUniform = glGetUniformLocation(litShaderProgram.ID, "ambientLightColor");
  //  GLint lightIntensityUniform = glGetUniformLocation(litShaderProgram.ID, "ambientLightIntensity");
  //  GLint cameraPositionUniformLitSHader = glGetUniformLocation(litShaderProgram.ID, "cameraPosition");
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
 //  glm::vec3 lightDir = glm::vec3(1, 0, 0);
 //  glm::vec3 lightColor = glm::vec3(1, 0, 0);
 //  float lightIntensity = .1f;
   glm::vec3 lightbulbSuzana{ 1.0f, 2.0f, 5.0f };
    lightBulbSuzane.position(lightbulbSuzana);

    glm::vec3 suzzaneRotate{ 0.0f, 1.0f, 0.0f };




    //////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    GLuint sceneFBO = 0;
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    // color texture that will contain the rendered scene
    GLuint colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_width, g_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    // depth+stencil renderbuffer for the scene FBO
    GLuint sceneRBO = 0;
    glGenRenderbuffers(1, &sceneRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_width, g_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);

    // draw buffers and completeness
    GLenum drawBuf = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &drawBuf);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR::SCENE_FRAMEBUFFER:: Framebuffer is not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (!glfwWindowShouldClose(window)) {
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clearing depth buffer and color buffer aka reseting values to draw new frame
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
        glViewport(0, 0, g_width, g_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region ImGui region



// -----------------------------
// ImGui Frame Start
// -----------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Scene Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // ------------------------------------------------
        // FPS
        // ------------------------------------------------
        ImGui::TextColored(
            ImVec4(0.2f, 1.0f, 0.3f, 1.0f),
            "FPS: %.1f",
            ImGui::GetIO().Framerate
        );
        ImGui::Separator();

        // ------------------------------------------------
        // Scene Selection
        // ------------------------------------------------
        if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char* sceneNames[] = { "Scene One", "Scene Two" };
            if (activeScene) {

                if (ImGui::Combo("Active Scene", &activeSceneIndex, sceneNames, IM_ARRAYSIZE(sceneNames))) {
                    activeScene = scenes[activeSceneIndex];
                }

                ImGui::Text(
                    "Resolution: %.0f x %.0f",
                    activeScene->screenSize.x,
                    activeScene->screenSize.y
                );
            }
        }

        // ------------------------------------------------
        // Camera
        // ------------------------------------------------
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (activeScene) {
                auto& cam = activeScene->sceneCamera;

                ImGui::Checkbox("Enable Rotation", &cam.enableRotation);
                ImGui::SliderFloat("Mouse Sensitivity", &cam.mouseSensitivity, 0.001f, 0.2f, "%.4f");

                ImGui::Separator();
                ImGui::Text("Position");
                ImGui::DragFloat3("##CamPos", &cam.position.x, 0.05f);
            }
        }

        // ------------------------------------------------
        // Lighting
        // ------------------------------------------------
        if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::Text("Directional Light");
            if (activeScene) {

                ImGui::DragFloat3(
                    "Direction",
                    &activeScene->lightDirection.x,
                    0.05f, -10.0f, 10.0f
                );

                ImGui::ColorEdit3(
                    "Color",
                    &activeScene->lightColor.x
                );

                ImGui::SliderFloat(
                    "Intensity",
                    &activeScene->lightIntensity,
                    0.01f, 1.0f
                );
            }
        }

        // ------------------------------------------------
        // Render Settings
        // ------------------------------------------------
        if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char* drawModes[] = {
                "Triangles", "Wireframe", "Points",
                "Line Strip", "Line Loop",
                "Triangle Strip", "Triangle Fan"
            };

            static int currentDrawMode = 0;

            if (activeScene) {
                if (ImGui::Combo("Draw Mode", &currentDrawMode, drawModes, IM_ARRAYSIZE(drawModes))) {
                    switch (currentDrawMode) {
                    case 0: activeScene->drawMode = GL_TRIANGLES; break;
                    case 1: activeScene->drawMode = GL_LINES; break;
                    case 2: activeScene->drawMode = GL_POINTS; break;
                    case 3: activeScene->drawMode = GL_LINE_STRIP; break;
                    case 4: activeScene->drawMode = GL_LINE_LOOP; break;
                    case 5: activeScene->drawMode = GL_TRIANGLE_STRIP; break;
                    case 6: activeScene->drawMode = GL_TRIANGLE_FAN; break;
                    }
                }
            }
        }

        // ------------------------------------------------
        // Application
        // ------------------------------------------------
        ImGui::Separator();
        if (ImGui::Button("Quit Application", ImVec2(-1, 0))) {
            glfwSetWindowShouldClose(window, true);
        }

        ImGui::End();

#pragma endregion

        /// -----------------------------
        /// Other procesess
        /// -----------------------------

        processInput(window);
        
        suzanne.rotate(suzzaneRotate, glm::radians(rotationStrength) * static_cast<float>(deltaTime));
        
        activeScene->render();
        //mainCamera.CameraMovement(window);
        //view = mainCamera.GetViewMatrix();
        //
       //// view = glm::lookAt(mainCamera.position, mainCamera.lookPivot, mainCamera.up);
        //projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);

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
        //glUseProgram(textureShaderProgram.ID); // This is a task for the material
        //// This line does the whole space transformation from object  > world > camera > screen (more or less)
        //glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * quadModel.getModelMatrix()));
        //
        //// This is also task of material:
        //glActiveTexture(GL_TEXTURE3);
        //glUniform1i(textureUniform, 3);        
        //glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        //
        //quadModel.render(drawMode);
        //glBindVertexArray(0);
        //glActiveTexture(GL_TEXTURE0);

        //glUseProgram(litShaderProgram.ID);
        //glUniform3fv(lightPosUniform, 1, glm::value_ptr(lightDir));
        //glUniform3fv(lightColorUniform, 1, glm::value_ptr(lightColor));
        //glUniform1f(lightIntensityUniform, lightIntensity);
        //glUniform3fv(cameraPositionUniformLitSHader, 1, glm::value_ptr(mainCamera.position));
        //glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.getModelMatrix()));
        //glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(suzanne.getModelMatrix()));
        //suzanne.render(drawMode);
        //glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection* view* lightBulbSuzane.getModelMatrix()));
        //glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(lightBulbSuzane.getModelMatrix()));
        //lightBulbSuzane.render(drawMode);
        //glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, g_width, g_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(litShaderProgram.ID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        screenQuadModel.render(activeScene->drawMode);

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