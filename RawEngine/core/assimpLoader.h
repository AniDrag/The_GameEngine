#pragma once

#include <string>
#include <vector>
#include <assimp/scene.h>

#include "model.h"
#include "mesh.h"

namespace core {

    class AssimpLoader {
    public:
        static Model loadModel(const std::string& path);

    private:
        static void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);

        static Mesh processMesh(aiMesh* mesh, const aiScene* scene, int meshIndex);

        static void debugMeshInfo(const aiMesh* mesh, int meshIndex);
    };

}