#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 sf;
uniform mat4 pb;

void main()
{
	gl_Position = projection * view * pb * sf * vec4(aPos[0], -aPos[2], aPos[1], 1.0f);
	//gl_Position = vec4(aPos[0], -aPos[2], aPos[1], 1.0f);
	ourColor = vec3(0.3, 0.3, 0.3);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}