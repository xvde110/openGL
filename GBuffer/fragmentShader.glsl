#version 330 core


layout (location = 1) out vec3 gPosition;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec3 gColor;

out vec4 color;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D wood;

//¾Û¹âµÆ
struct SpotLight{
	vec3 ambient;
	vec3 lightPos;
	vec3 lightColor;
	vec3 direction;
	float cutoff;
	float outercutoff;
};
uniform SpotLight light;
uniform SpotLight light2;
vec3 getLightColor(SpotLight light,vec3 Normal,vec3 FragPos){
	vec3 fragColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.lightPos - FragPos);

	float theta = dot(lightDir, normalize(-light.direction));

	float distance=length(light.lightPos-FragPos);
	float attenuation=1.0f/(1.0+0.07*distance+0.017*distance*distance);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * attenuation * light.lightColor;
	float epsilon = light.cutoff - light.outercutoff;
	float intensity = clamp((theta - light.outercutoff) / epsilon,0.0, 1.0);
	fragColor=light.ambient+diffuse*intensity;
	return fragColor;
}

void main()
{
	
	color=vec4(0.0);
	
	vec4 objectColor=texture(wood,TexCoord);
	color+=objectColor*vec4(getLightColor(light,Normal,Position),1.0); 
	color+=objectColor*vec4(getLightColor(light2,Normal,Position),1.0); 

	
	gPosition=Position;
	gNormal=Normal;
	gColor=texture(wood,TexCoord).rgb;
	
}