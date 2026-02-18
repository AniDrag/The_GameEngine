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
bool ppGrayscale = false;
bool ppInvert = false;

bool ppBloom = false;
float ppBloomStrength = 1;

bool ppPixelize = false;
float ppPixelSize = 512;
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
	const GLuint textureVertexShader = generateShader("shaders/vertex.vs", GL_VERTEX_SHADER);
    const GLuint postProcessFS = generateShader("shaders/PostProcessing.fs", GL_FRAGMENT_SHADER);
    const GLuint blurVertex = generateShader("shaders/blur.vs", GL_VERTEX_SHADER);
    const GLuint blurFragment = generateShader("shaders/blur.fs", GL_FRAGMENT_SHADER);
    const GLuint combineFragment = generateShader("shaders/bloom_combine.fs", GL_FRAGMENT_SHADER);
    const GLuint downsampler = generateShader("shaders/downsample.fs", GL_FRAGMENT_SHADER);
    
    
    /// ------------------------------------------------
	///     Shader Programs initialization
    /// ------------------------------------------------ 
    int success;
    char infoLog[512];
	core::Shader modelShaderProgram(modelVertexShader, fragmentShader);
   // core::Shader textureShaderProgram(modelVertexShader, fragmentShader);
    core::Shader litShaderProgram(modelVertexShader, litShader);
    core::Shader textureShaderProgram(textureVertexShader, textureShader);

    core::Shader postProcessShader(textureVertexShader, postProcessFS);    
    core::Shader blurShader(blurVertex, blurFragment);
    core::Shader combineShader(textureVertexShader, combineFragment);
    core::Shader downsampleShader(textureVertexShader, downsampler);

 
    // Close calls
    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);
    glDeleteShader(litShader);
	glDeleteShader(textureVertexShader);

    glDeleteShader(postProcessFS);
    glDeleteShader(blurVertex);
    glDeleteShader(blurFragment);
    glDeleteShader(combineFragment);
    glDeleteShader(downsampler);

    /// ------------------------------------------------------------------------
    // Generate Quad meshes for image gato
    /// ------------------------------------------------------------------------
    core::Mesh quad = core::Mesh::generateQuad();
    core::Mesh screenQuad = core::Mesh::generateQuad();
	core::Model screenQuadModel({ screenQuad });
    core::Model quadModel({ quad });
	glm::vec3 quadTranslation = { 0, 0, -2.5 };
    glm::vec3 quadScale = { 5, 5, 1 };
    quadModel.translate(quadTranslation);
    quadModel.scale(quadScale);
    

    /// ------------------------------------------------------------------------
    ///         Model and texture initialization and registration
    /// ------------------------------------------------------------------------
    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    //core::Model lightBulbSuzane = core::AssimpLoader::loadModel("models/sz.obj");
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");// CMGaTo_crop.png
    suzanne.material.addAlbedoTexture("textures/CMGaTo_crop.png");
	auto litShaderProgramPtr = std::make_shared<core::Shader>(litShaderProgram); // one attachment here material gets ye worked like butter
    //Even textures loaded

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
   // lightBulbSuzane.position(lightbulbSuzana);

    glm::vec3 suzzaneRotate{ 0.0f, 1.0f, 0.0f };




    //////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    
    GLuint sceneFBO = 0;
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    // Color attachment 0 – HDR scene color
    GLuint colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    // Color attachment 1 – Bright pass (also HDR)
    GLuint brightBuffer;
    glGenTextures(1, &brightBuffer);
    glBindTexture(GL_TEXTURE_2D, brightBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightBuffer, 0);

    // Depth/stencil renderbuffer
    GLuint sceneRBO;
    glGenRenderbuffers(1, &sceneRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_width, g_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);

    // Set draw buffers to both attachments
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR: Scene FBO incomplete!\n");
        return -1;
    }

    GLuint downsampleFBO;
    GLuint downsampleTexture;

    int halfW = g_width / 2;
    int halfH = g_height / 2;

    glGenFramebuffers(1, &downsampleFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, downsampleFBO);

    glGenTextures(1, &downsampleTexture);
    glBindTexture(GL_TEXTURE_2D, downsampleTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, halfW, halfH, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, downsampleTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR: Downsample FBO incomplete!\n");

    // --- Ping-pong FBOs for separable blur ---
    GLuint pingpongFBO[2];
    GLuint pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, halfW, halfH, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    while (!glfwWindowShouldClose(window)) {
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clearing depth buffer and color buffer aka reseting values to draw new frame
        // ---  Render scene into FBO ---
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
        ImGui::Separator();

        // ------------------------------------------------
       // FBO settings Selection
       // ------------------------------------------------
        if (ImGui::CollapsingHeader("Post Processing", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Grayscale", &ppGrayscale);
            ImGui::Checkbox("Invert Colors", &ppInvert);

            ImGui::Checkbox("Bloom", &ppBloom);
            if (ppBloom) {
                if (activeScene) {
                    ImGui::SliderFloat("Bloom Threshold", &activeScene->ppBloomThreshold, 0.0f, 2.0f);
                }
                ImGui::SliderFloat("Bloom Strength", &ppBloomStrength, 0.0f, 2.0f);
            }

            ImGui::Checkbox("Pixelize", &ppPixelize);
            if (ppPixelize)
                ImGui::SliderFloat("Pixel Size", &ppPixelSize, 16.0f, 512.0f);
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
                    0.05f, -1.0f, 1.0f
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
        
        //suzanne.rotate(suzzaneRotate, glm::radians(rotationStrength) * static_cast<float>(deltaTime));
        
        activeScene->render(); 

#pragma region  legacy code


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
#pragma endregion
        glDisable(GL_DEPTH_TEST);
        if (ppBloom) {
            // --- Downsample bright texture ---
            glBindFramebuffer(GL_FRAMEBUFFER, downsampleFBO);
            glViewport(0, 0, g_width / 2, g_height / 2);
            glClear(GL_COLOR_BUFFER_BIT);
            downsampleShader.use();
            downsampleShader.setProperty("uTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, brightBuffer);
            screenQuadModel.render(GL_TRIANGLES);

            // --- Horizontal blur ---
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
            glViewport(0, 0, g_width / 2, g_height / 2);
            glClear(GL_COLOR_BUFFER_BIT);
            blurShader.use();
            blurShader.setProperty("uTexture", 0);
            blurShader.setProperty("uHorizontal", true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, downsampleTexture);
            screenQuadModel.render(GL_TRIANGLES);

            // --- Vertical blur ---
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]);
            glViewport(0, 0, g_width / 2, g_height / 2);      // <-- ADDED
            glClear(GL_COLOR_BUFFER_BIT);
            blurShader.setProperty("uHorizontal", false);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
            screenQuadModel.render(GL_TRIANGLES);

            // --- Combine ---
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, g_width, g_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            combineShader.use();
            combineShader.setProperty("uSceneTexture", 0);
            combineShader.setProperty("uBloomTexture", 1);
            combineShader.setProperty("uBloomIntensity", ppBloomStrength);
            combineShader.setProperty("uGrayscale", ppGrayscale);
            combineShader.setProperty("uInvert", ppInvert);
            combineShader.setProperty("uPixelize", ppPixelize);
            combineShader.setProperty("uPixelSize", ppPixelSize);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, colorBuffer);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
            screenQuadModel.render(GL_TRIANGLES);
        }
        else {
            // No bloom – just post?process the scene directly
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            postProcessShader.use();
            postProcessShader.setProperty("uGrayscale", ppGrayscale);
            postProcessShader.setProperty("uInvert", ppInvert);
            postProcessShader.setProperty("uPixelize", ppPixelize);
            postProcessShader.setProperty("uPixelSize", ppPixelSize);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, colorBuffer);
            postProcessShader.setProperty("text", 0);
            screenQuadModel.render(GL_TRIANGLES);
        }

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);// bind FBO just in case


      /* glBindFramebuffer(GL_FRAMEBUFFER, 0);// bind FBO
       
       // --- Render screen quad with FBO texture ---
       glViewport(0, 0, g_width, g_height);
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       glUseProgram(textureShaderProgram.ID);
       glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_2D, colorBuffer); // FBO color attachment
       screenQuadModel.render(GL_TRIANGLES);
       */
        
       // ImGui rendering always happens after scene is drawn to default framebuffer
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
