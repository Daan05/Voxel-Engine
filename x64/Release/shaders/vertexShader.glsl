#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

uniform mat4 mat;

out vec3 pos;
out vec3 normal;

void main()
{
	gl_Position = mat * vec4(aPos, 1.0);

	pos = aPos;
	normal = normalize(aNor);
}
