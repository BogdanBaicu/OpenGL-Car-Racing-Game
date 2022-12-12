#version 330

// Input
// Get vertex attributes from each location
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 coord;
layout(location=3) in vec3 color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Nume1;

// Output
// Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture;
out vec3 frag_color;


void main()
{
    // Send output to fragment shader
    frag_color = normal;

    // Compute gl_Position
    gl_Position = Projection * View * Model * vec4(position, 1);

}
