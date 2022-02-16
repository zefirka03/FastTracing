~~vertex~~
#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

out vec2 tx_cord;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    tx_cord = tex_coord;
}


~~fragment~~
#version 430 core

in vec2 tx_cord;
uniform sampler2D tex;

out vec4 color;


vec3 LessThan(vec3 f, float value)
{
	return vec3(
		(f.x < value) ? 1.0f : 0.0f,
		(f.y < value) ? 1.0f : 0.0f,
		(f.z < value) ? 1.0f : 0.0f);
}

vec3 LinearToSRGB(vec3 rgb)
{
	rgb = clamp(rgb, 0.0f, 1.0f);

	return mix(
		pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
		rgb * 12.92f,
		LessThan(rgb, 0.0031308f)
	);
}

vec3 SRGBToLinear(vec3 rgb)
{
	rgb = clamp(rgb, 0.0f, 1.0f);

	return mix(
		pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
		rgb / 12.92f,
		LessThan(rgb, 0.04045f)
	);
}

vec3 ACESFilm(vec3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}


void main() {
    vec3 clr = texture(tex, tx_cord).rgb;

	clr *= 0.5f;
	clr = ACESFilm(clr);
	clr = LinearToSRGB(clr);

    color = vec4(clr, 1);
}
