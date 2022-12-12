#include "object3D.h"
#include "lab_m1/CarRacing/CarRacing.h"

#include <vector>
#include <iostream>

#include "core/engine.h"
#include "utils/gl_utils.h"

std::vector<glm::vec3> treePositions;
std::vector<glm::vec3> trackPositions;

Mesh* object3D::CreatePlane(
    const std::string& name,
    glm::vec3 corner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, 0, height), color),
        VertexFormat(corner + glm::vec3(0, 0, height), color)
    };

    Mesh* plane = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        plane->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    plane->InitFromData(vertices, indices);
    return plane;
}

Mesh* object3D::CreateTrack(
    const std::string& name,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 d, p;
    glm::vec3 up = glm::vec3(0, 1, 0);
    std::vector<glm::vec3> trackPoints = {
        glm::vec3(-47.28, 0, -2.36),
        glm::vec3(-46.71, 0, -6.14),
        glm::vec3(-45.91, 0, -9.92),
        glm::vec3(-43.16, 0, -13.36),
        glm::vec3(-36.86, 0, -15.76),
        glm::vec3(-32.74, 0, -13.7),
        glm::vec3(-26.78, 0, -13.7),
        glm::vec3(-21.86, 0, -18.05),
        glm::vec3(-17.73, 0, -22.86),
        glm::vec3(-10.63, 0, -28.25),
        glm::vec3(-2.39, 0, -29.16),
        glm::vec3(6.09, 0, -29.28),
        glm::vec3(12.96, 0, -27.79),
        glm::vec3(22.7, 0, -25.38),
        glm::vec3(27.28, 0, -21.83),
        glm::vec3(29.68, 0, -16.45),
        glm::vec3(33.12, 0, -11.87),
        glm::vec3(38.16, 0, -10.03),
        glm::vec3(43.77, 0, -9.81),
        glm::vec3(46.75, 0, -5.45),
        glm::vec3(46.41, 0, 3.94),
        glm::vec3(42.74, 0, 10.12),
        glm::vec3(35.52, 0, 13.33),
        glm::vec3(27.74, 0, 14.59),
        glm::vec3(20.98, 0, 12.87),
        glm::vec3(14.57, 0, 9.44),
        glm::vec3(9.07, 0, 5.66),
        glm::vec3(1.51, 0, 2.22),
        glm::vec3(-7.31, 0, 4.51),
        glm::vec3(-13.15, 0, 8.52),
        glm::vec3(-17.39, 0, 12.99),
        glm::vec3(-23.23, 0, 16.77),
        glm::vec3(-28.04, 0, 19.17),
        glm::vec3(-32.85, 0, 18.71),
        glm::vec3(-37.55, 0, 17.45),
        glm::vec3(-40.64, 0, 15.51),
        glm::vec3(-44.08, 0, 11.61),
        glm::vec3(-45.45, 0, 6.69),
        glm::vec3(-46.71, 0, 1.76) 
    };
    std::vector<glm::vec3> innerTrackPoints, outterTrackPoints; 
    
    for (int i = 0; i < trackPoints.size() - 1; i++) {
        d = trackPoints[i + 1] - trackPoints[i];
        p = glm::cross(d, up);
        p = glm::normalize(p);
        innerTrackPoints.push_back(trackPoints[i] + 2.f * p);
        outterTrackPoints.push_back(trackPoints[i] - 3.f * p);
        treePositions.push_back(trackPoints[i] + 4.f * p);
        treePositions.push_back(trackPoints[i] - 5.f * p);
        trackPositions.push_back(trackPoints[i] + 2.f * p);
        trackPositions.push_back(trackPoints[i] - 3.f * p);
    }
    {
        d = trackPoints[0] - trackPoints[trackPoints.size() - 1];
        p = glm::cross(d, up);
        p = glm::normalize(p);
        innerTrackPoints.push_back(trackPoints[trackPoints.size() - 1] + 2.f * p);
        outterTrackPoints.push_back(trackPoints[trackPoints.size() - 1] - 3.f * p);
        treePositions.push_back(trackPoints[trackPoints.size() - 1] + 3.f * p);
        treePositions.push_back(trackPoints[trackPoints.size() - 1] - 4.f * p);
        trackPositions.push_back(trackPoints[trackPoints.size() - 1] + 2.f * p);
        trackPositions.push_back(trackPoints[trackPoints.size() - 1] - 3.f * p);
    }
    d = trackPoints[1] - trackPoints[0];
    p = glm::cross(d, up);
    p = glm::normalize(p);
    trackPositions.push_back(trackPoints[0] + 2.f * p);
    trackPositions.push_back(trackPoints[0] - 3.f * p);

    std::vector<VertexFormat> vertices = {};
    for (int i = 0; i < trackPoints.size(); i++) {
        vertices.push_back(VertexFormat(innerTrackPoints[i], color));
        vertices.push_back(VertexFormat(outterTrackPoints[i], color));
    }
    
    Mesh* track = new Mesh(name);
    std::vector<unsigned int> indices = {};
    
    for (int i = 0; i < trackPoints.size() - 1; i++) {
        indices.push_back(2 * i);
        indices.push_back(2 * i + 1);
        indices.push_back(2 * i + 2);
        indices.push_back(2 * i + 1);
        indices.push_back(2 * i + 2);
        indices.push_back(2 * i + 3);
    }
    indices.push_back(2 * (trackPoints.size() - 1));
    indices.push_back(2 * (trackPoints.size() - 1) + 1);
    indices.push_back(0);
    indices.push_back(2 * (trackPoints.size() - 1) + 1);
    indices.push_back(0);
    indices.push_back(1);

    if (!fill) {
        track->SetDrawMode(GL_LINE_LOOP);
    }
    

    track->InitFromData(vertices, indices);
    return track;
}

