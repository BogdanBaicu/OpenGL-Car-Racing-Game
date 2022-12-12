#pragma once

#include "components/simple_scene.h"
#include "lab_m1/CarRacing/utils/transform3D.h"
#include "lab_m1/CarRacing/objects/object3D.h"
#include "lab_m1/CarRacing/utils/camera.h"

namespace m1
{
	class CarRacing : public gfxc::SimpleScene
	{
	public:
		struct ViewportArea
		{
			ViewportArea() : x(0), y(0), width(1), height(1) {}
			ViewportArea(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		CarRacing();
		~CarRacing();

		void Init() override;

		Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
		void CreateCube(const char* name, float length, glm::vec3 color);
		void CreateCuboid(const char* name, float length, float height, float width, glm::vec3 color);
		float findArea(float x1, float y1, float x2, float y2, float x3, float y3);
		bool isInside();
		bool isCentreInside();

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		void RenderScene();

	protected:
		ViewportArea miniViewportArea;
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::vec2 carPosition = glm::vec2(1.51, 2.22);
		glm::vec2 carFront; 
		float carAngle = 0;
		bool reverse = false;
		implemented::Camera* camera;
		
	};
}