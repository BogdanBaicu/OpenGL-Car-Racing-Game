#include "lab_m1/CarRacing/CarRacing.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

CarRacing::CarRacing()
{

}

CarRacing::~CarRacing()
{

}

void CarRacing::Init()
{
	// Meshes
	Mesh* grass = object3D::CreatePlane("grass", glm::vec3(0, 0, 0), 120, 80, glm::vec3(0, 0.6, 0.1), true);
	Mesh* track = object3D::CreateTrack("track", glm::vec3(0.22, 0.22, 0.22), true);
		
	AddMeshToList(grass);
	AddMeshToList(track);
    
    CreateCube("treeCrown",  1.5f, glm::vec3(0.227, 0.372, 0.04));
    CreateCuboid("treeTrunk",  0.5f, 1.f, 0.5f, glm::vec3(0.45, 0.36, 0.26));
    CreateCuboid("car", 2.f, 0.5f, 1.f, glm::vec3(0, 0.5, 1));
    
    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("CustomShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "CarRacing", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "CarRacing", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

	// Sets the resolution of the small viewport
	glm::ivec2 resolution = window->GetResolution();
	miniViewportArea = ViewportArea(20, 20, resolution.x / 5.f, resolution.y / 5.f);

    // Camera
    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void CarRacing::FrameStart()
{
	glClearColor(0.4, 0.7, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void CarRacing::Update(float deltaTimeSeconds)
{
    camera->Set(glm::vec3(carPosition.x - 5 * sin(RADIANS(90 + carAngle)), 2, carPosition.y - 5 * cos(RADIANS(90 + carAngle))), glm::vec3(carPosition.x, 1.5, carPosition.y), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    RenderScene();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    camera->Set(glm::vec3(carPosition.x, 3, carPosition.y), glm::vec3(carPosition.x, 2, carPosition.y), glm::vec3(0, 0, -1));
    projectionMatrix = glm::ortho(-20.f, 20.f, -10.f, 10.f, 0.01f, 200.f);
	// render the scene again, in the new mini viewport
	RenderScene();
}

void CarRacing::FrameEnd()
{
	DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void CarRacing::RenderScene()
{
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(-60, 0, -40);
	RenderMesh(meshes["grass"], shaders["VertexColor"], modelMatrix);
    modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(0, 0.1, 0);
	RenderMesh(meshes["track"], shaders["VertexColor"], modelMatrix);
    float* treePosition;
    for (int i = 0; i < treePositions.size(); i++) {
        treePosition = glm::value_ptr(treePositions[i]);
        modelMatrix = glm::mat4(1);
        modelMatrix = transform3D::Translate(treePosition[0], 1.75, treePosition[2]);
        RenderMesh(meshes["treeCrown"], shaders["CustomShader"], modelMatrix);
        modelMatrix = glm::mat4(1);
        modelMatrix = transform3D::Translate(treePosition[0], 0.5, treePosition[2]);
        RenderMesh(meshes["treeTrunk"], shaders["CustomShader"], modelMatrix);
    }
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(carPosition.x, 0.35, carPosition.y);
    modelMatrix *= transform3D::RotateOY(RADIANS(carAngle));
    RenderMesh(meshes["car"], shaders["CustomShader"], modelMatrix);
    carFront = carPosition + glm::vec2(sin(RADIANS(90 + carAngle)), cos(RADIANS(90 + carAngle)));
}

void CarRacing::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

Mesh* CarRacing::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data.

    // If you look closely in the `Init()` and `Update()`
    // functions, you will see that we have three objects which we load
    // and use in three different ways:
    // - LoadMesh   + LabShader (this lab's shader)
    // - CreateMesh + VertexNormal (this shader is already implemented)
    // - CreateMesh + LabShader (this lab's shader)
    // To get an idea about how they're different from one another, do the
    // following experiments. What happens if you switch the color pipe and
    // normal pipe in this function (but not in the shader)? Now, what happens
    // if you do the same thing in the shader (but not in this function)?
    // Finally, what happens if you do the same thing in both places? Why?

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

void CarRacing::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Get shader location for uniform mat4 "Model"
    int location = glGetUniformLocation(shader->program, "Model");

    // Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(location, 1, 0, glm::value_ptr(modelMatrix));

    // Get shader location for uniform mat4 "View"
    int location2 = glGetUniformLocation(shader->program, "View");

    // Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glUniformMatrix4fv(location2, 1, 0, glm::value_ptr(viewMatrix));

    // Get shader location for uniform mat4 "Projection"
    int location3 = glGetUniformLocation(shader->program, "Projection");

    // Set shader uniform "Projection" to projectionMatrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    glUniformMatrix4fv(location3, 1, 0, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void CarRacing::CreateCube(const char* name, float length, glm::vec3 color)
{
    // Create a simple cube
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-length / 2, -length / 2,  length / 2), color, glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(length / 2, -length / 2,  length / 2), color, glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-length / 2,  length / 2,  length /2), color, glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3(length / 2,  length / 2,  length / 2), color, glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-length / 2, -length / 2, -length / 2), color, glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3(length / 2, -length / 2, -length / 2), color, glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-length / 2,  length / 2, -length / 2), color, glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3(length / 2,  length/ 2, -length / 2), color, glm::vec3(0.1, 0.5, 0.8)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

        CreateMesh(name, vertices, indices);
    }
}

void CarRacing::CreateCuboid(const char* name, float length, float height, float width, glm::vec3 color)
{
    // Create a simple cuboid
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-length / 2, -height / 2,  width / 2), color, glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(length / 2, -height / 2,  width / 2), color, glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-length / 2,  height / 2,  width / 2), color, glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3(length / 2,  height / 2,  width / 2), color, glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-length / 2, -height / 2, -width / 2), color, glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3(length / 2, -height / 2, -width / 2), color, glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-length / 2,  height / 2, -width / 2), color, glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3(length / 2,  height / 2, -width / 2), color, glm::vec3(0.1, 0.5, 0.8)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

        CreateMesh(name, vertices, indices);
    }
}

float CarRacing::findArea(float x1, float y1, float x2, float y2, float x3, float y3)
{
    return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

bool CarRacing::isInside()
{
    for (int i = 0; i < trackPositions.size() - 2; i++)
    {
        float* firstPoint = glm::value_ptr(trackPositions[i]);
        float* secondPoint = glm::value_ptr(trackPositions[i + 1]);
        float* thirdPoint = glm::value_ptr(trackPositions[i + 2]);
        float area1 = findArea(carFront.x, carFront.y, firstPoint[0], firstPoint[2], secondPoint[0], secondPoint[2]);
        float area2 = findArea(carFront.x, carFront.y, firstPoint[0], firstPoint[2], thirdPoint[0], thirdPoint[2]);
        float area3 = findArea(carFront.x, carFront.y, secondPoint[0], secondPoint[2], thirdPoint[0], thirdPoint[2]);
        float area4 = findArea(firstPoint[0], firstPoint[2], secondPoint[0], secondPoint[2], thirdPoint[0], thirdPoint[2]);
        if (abs(area1 + area2 + area3 - area4) <= 0.1)
            return true;
    }
    return false;
}

bool CarRacing::isCentreInside()
{
    for (int i = 0; i < trackPositions.size() - 2; i++)
    {
        float* firstPoint = glm::value_ptr(trackPositions[i]);
        float* secondPoint = glm::value_ptr(trackPositions[i + 1]);
        float* thirdPoint = glm::value_ptr(trackPositions[i + 2]);
        float area1 = findArea(carPosition.x, carPosition.y, firstPoint[0], firstPoint[2], secondPoint[0], secondPoint[2]);
        float area2 = findArea(carPosition.x, carPosition.y, firstPoint[0], firstPoint[2], thirdPoint[0], thirdPoint[2]);
        float area3 = findArea(carPosition.x, carPosition.y, secondPoint[0], secondPoint[2], thirdPoint[0], thirdPoint[2]);
        float area4 = findArea(firstPoint[0], firstPoint[2], secondPoint[0], secondPoint[2], thirdPoint[0], thirdPoint[2]);
        if (abs(area1 + area2 + area3 - area4) <= 0.1)
            return true;
    }
    return false;
}

void CarRacing::OnInputUpdate(float deltaTime, int mods)
{
    reverse = false;
    if (window->KeyHold(GLFW_KEY_W) && !window->MouseHold(GLFW_MOUSE_BUTTON_2) && isInside())
    {
        carPosition.y -= 10 * deltaTime * sin(RADIANS(carAngle));
        carPosition.x += 10 * deltaTime * cos(RADIANS(carAngle));
    }
    if (window->KeyHold(GLFW_KEY_X) && window->KeyHold(GLFW_KEY_W) && !window->MouseHold(GLFW_MOUSE_BUTTON_2) && isInside())
    {
        carPosition.y -= 10 * deltaTime * sin(RADIANS(carAngle));
        carPosition.x += 10 * deltaTime * cos(RADIANS(carAngle));
    }
    if (window->KeyHold(GLFW_KEY_S) && !window->MouseHold(GLFW_MOUSE_BUTTON_2) && isCentreInside())
    {
        reverse = true;
        carPosition.y += 5 * deltaTime * sin(RADIANS(carAngle));
        carPosition.x -= 5 * deltaTime * cos(RADIANS(carAngle));
    }
    if (window->KeyHold(GLFW_KEY_A) && !window->MouseHold(GLFW_MOUSE_BUTTON_2))
    {
        if(!reverse)
            carAngle += 100 * deltaTime;
        else
            carAngle -= 100 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D) && !window->MouseHold(GLFW_MOUSE_BUTTON_2))
    {
        if (!reverse)
            carAngle -= 100 * deltaTime;
        else
            carAngle += 100 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_SPACE) && window->KeyHold(GLFW_KEY_A) && !window->MouseHold(GLFW_MOUSE_BUTTON_2))
    {
        carAngle += 100 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_SPACE) && window->KeyHold(GLFW_KEY_D) && !window->MouseHold(GLFW_MOUSE_BUTTON_2))
    {
        carAngle -= 100 * deltaTime;
    }
}

void CarRacing::OnKeyPress(int key, int mods)
{

}

void CarRacing::OnKeyRelease(int key, int mods)
{

}

void CarRacing::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    
}

void CarRacing::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{

}

void CarRacing::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}

void CarRacing::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{

}

void CarRacing::OnWindowResize(int width, int height)
{

}