#include "assimpLoader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace core {
    //Job of loading the Mesh to a ?Scene
    int gMeshIndex = 0;

    // ---------------- MODEL LOAD ----------------
    Model AssimpLoader::loadModel(const std::string& path) {
        Assimp::Importer import;

        const aiScene* scene = import.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenNormals |
            aiProcess_CalcTangentSpace
        );

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            printf("Error: %s\n", import.GetErrorString());
            return Model({});
        }

        std::string directory = path.substr(0, path.find_last_of('/'));
        std::vector<Mesh> meshes;
        gMeshIndex = 0; 
        processNode(scene->mRootNode, scene, meshes);
        return Model(meshes);
    }

    // ---------------- NODE PROCESS ----------------
    void AssimpLoader::processNode(aiNode *node, const aiScene *scene, std::vector<Mesh>& meshes) {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, gMeshIndex++));
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, meshes);
        }
    }

    // ---------------- MESH PROCESS ----------------
    Mesh AssimpLoader::processMesh(aiMesh* mesh, const aiScene* scene, int meshIndex) {
        debugMeshInfo(mesh, meshIndex);

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        vertices.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // ---------------- UV ----------------
            glm::vec2 uv(0.0f);

            if (mesh->mTextureCoords[0])
            {
                uv.x = mesh->mTextureCoords[0][i].x;
                uv.y = mesh->mTextureCoords[0][i].y;
            }

            // ---------------- NORMAL ----------------
            glm::vec3 normal = mesh->HasNormals()
                ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                : glm::vec3(0.0f);


            // ---------------- TANGENT SPACE ----------------
            glm::vec3 tangent(1.0f, 0.0f, 0.0f);
            glm::vec3 bitangent(0.0f, 1.0f, 0.0f);

            if (mesh->HasTangentsAndBitangents())
            {
                tangent = glm::vec3(
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z
                );

                bitangent = glm::vec3(
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z
                );
            }

            // ---------------- VERTEX ----------------
            vertices.emplace_back(
                glm::vec3(
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z
                ),
                normal,
                uv,
                tangent,
                bitangent
            );
        }

        // ---------------- INDICES ----------------
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Mesh(vertices, indices);
    }

    // ---------------- DEBUG INFO ----------------

    void AssimpLoader::debugMeshInfo(const aiMesh* mesh, int meshIndex)
    {
        std::cout << "\n===== Mesh [" << meshIndex << "] Debug Info =====\n";

        std::cout << "Vertices: " << mesh->mNumVertices << "\n";
        std::cout << "Faces: " << mesh->mNumFaces << "\n";

        bool hasNormals = mesh->HasNormals();
        bool hasUVs = mesh->mTextureCoords[0] != nullptr;
        bool hasTBN = mesh->HasTangentsAndBitangents();

        std::cout << "Normals: " << (hasNormals ? "YES" : "NO") << "\n";
        std::cout << "UV0: " << (hasUVs ? "YES" : "NO") << "\n";
        std::cout << "Tangents/Bitangents: " << (hasTBN ? "YES" : "NO") << "\n";

        if (!hasNormals)
            std::cerr << "[WARN] Missing normals\n";

        if (!hasUVs)
            std::cerr << "[WARN] Missing UVs\n";

        if (!hasTBN)
            std::cerr << "[WARN] Missing tangents/bitangents\n";
    }
}