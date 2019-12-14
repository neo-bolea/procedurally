#version 330 core
out vec4 FragColor;

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight 
{
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight 
{    
    vec3 position;
    
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;  
};  
#define NR_POINT_LIGHTS 4  

struct SpotLight 
{    
    vec3 position;
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float constant;
    float linear;
    float quadratic;

	float cutOff;
    float outerCutOff;
};  

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fFragPos;

uniform DirLight uDirLight;
uniform PointLight uPointLights[NR_POINT_LIGHTS];
uniform SpotLight uSpotLight;
uniform Material uMaterial;

uniform vec3 uViewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(fNormal);
    vec3 viewDir = normalize(uViewPos - fFragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(uDirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(uPointLights[i], norm, fFragPos, viewDir);    
    // phase 3: Spot light
    result += CalcSpotLight(uSpotLight, norm, fFragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(uMaterial.diffuse, fTexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(uMaterial.diffuse, fTexCoord));
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, fTexCoord));
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(uMaterial.diffuse, fTexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(uMaterial.diffuse, fTexCoord));
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, fTexCoord));

    return (ambient + diffuse + specular) * attenuation;
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
   vec3 LightDir = normalize(light.position - fFragPos);
    
    // check if uLighting is inside the spotuLight cone
	float theta     = dot(LightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   
    // diffuse 
    vec3 norm = normalize(fNormal);
    float diff = max(dot(norm, LightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-LightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // attenuation
    float distance    = length(light.position - fFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	// combine results
	vec3 ambient  = light.ambient  * vec3(texture(uMaterial.diffuse, fTexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(uMaterial.diffuse, fTexCoord));
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, fTexCoord));

	diffuse  *= intensity;
	specular *= intensity;
        
    return (ambient + diffuse + specular) * attenuation;
}