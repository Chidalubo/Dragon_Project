#version 150
in vec3 fragColour;
out vec4 outColour;

void main()
{
    outColour = vec4(fragColour, 1.0f);
}
