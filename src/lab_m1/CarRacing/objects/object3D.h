#pragma once

#include <string>
#include <vector>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

extern std::vector<glm::vec3> treePositions;
extern std::vector<glm::vec3> trackPositions;

namespace object3D
{
	Mesh* CreatePlane(const std::string& name, glm::vec3 corner, float width, float height, glm::vec3 color, bool fill = false);
	Mesh* CreateTrack(const std::string& name, glm::vec3 color, bool fill = false);
}