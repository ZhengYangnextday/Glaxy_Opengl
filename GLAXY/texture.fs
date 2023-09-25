#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	//FragColor = vec4(1.0f, 0.2f, 0.5f, 1.0f);
	FragColor = texture(texture1, TexCoord);
}