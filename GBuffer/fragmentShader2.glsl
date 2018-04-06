#version 330 core

in vec2 Position;

out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;

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
	vec2 texCoord=(Position+vec2(1.0))/2;
	vec3 fragPos=texture(gPosition,texCoord).xyz;
	vec3 normal=texture(gNormal,texCoord).xyz;
	vec4 objColor=texture(gColor,texCoord);

	color+=objColor * vec4(getLightColor(light,normal,fragPos),1.0);
	color+=objColor * vec4(getLightColor(light2,normal,fragPos),1.0);


}