#version 330 core
in vec3 pos;
in vec3 normal;

uniform vec3 camPos;

out vec4 FragColor;

vec3 lightPos = vec3(400.0, 100.0, 200.0);
vec3 lightCol = vec3(1.0, 1.0, 1.0);

void main()
{
	float ambient = 0.30;
	float diffuse = 0.0;
	float specular = 0.0;

	vec3 lightDirection = normalize(lightPos - pos);
	vec3 viewDirection = normalize(camPos - pos);

	float light_dot_normal = dot(lightDirection, normal);
	if (light_dot_normal > 0.0)
	{
		diffuse = light_dot_normal;
		vec3 reflectv = reflect(-lightDirection, normal);
		float reflect_dot_eye = dot(reflectv, viewDirection);
		if (reflect_dot_eye > 0.0)
		{
			float factor = pow(reflect_dot_eye, 8);
			specular = factor;
		}
	}

    float phong = ambient + diffuse + specular;
	// outputs final color
	FragColor = vec4(1.0 * lightCol * phong, 1.0);
	//FragColor = vec4(1.0);
}
