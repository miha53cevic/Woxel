#version 330
in vec2 pass_texture;

out vec4 Frag_Colour;

uniform sampler2D textureSampler;

void main(void)
{
	vec4 texColor = texture(textureSampler, pass_texture);
	Frag_Colour = texColor;

	if (Frag_Colour.a <= 0.1)
		discard;
}