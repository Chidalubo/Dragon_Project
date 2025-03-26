#version 150
in vec3 aPos;
in vec3 colour;
in vec3 normal;

out vec3 fragColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 per;

void main()
{
   gl_Position = per * view * model * vec4(aPos, 1.0);
   fragColour = colour + (normal * vec3(0.9));
}
