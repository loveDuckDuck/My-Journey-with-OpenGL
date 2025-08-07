#version 330 core

// Final color output of the fragment
out vec4 FragColor;



// Input data from vertex shader (tangent space lighting)
in VS_OUT {
    vec3 FragPos;        // Fragment position in world space
    vec2 TexCoords;      // Texture coordinates
    vec3 TangentLightPos; // Light position transformed to tangent space
    vec3 TangentViewPos;  // View/camera position in tangent space
    vec3 TangentFragPos;  // Fragment position in tangent space
} fs_in;

// Surface normal from vertex shader (world space)
in vec3 Normal;
in vec3 FragPos;

// Light and camera positions in world space
uniform vec3 lightPos;
uniform vec3 viewPos;

// Spotlight structure definition
struct SpotLight {
    vec3 position;      // Position of the spotlight in world space
    vec3 direction;     // Direction the spotlight is pointing
    vec3 ambient;       // Ambient light color contribution
    vec3 diffuse;       // Diffuse light color (main illumination)
    vec3 specular;      // Specular light color (reflections)
    float cutOff;       // Inner cone angle (cosine of angle)
    float outerCutOff;  // Outer cone angle for soft edges
    float constant;     // Constant attenuation factor
    float linear;       // Linear distance attenuation factor
    float quadratic;    // Quadratic distance attenuation factor
};
uniform SpotLight spotLight;

// Material properties structure
struct Material {
    vec3 ambient;       // How much ambient light the material reflects
    vec3 diffuse;       // Main surface color
    vec3 specular;      // Specular reflection color
    float shininess;    // Shininess exponent (higher = more focused highlights)
};
uniform Material material;

// Function to calculate spotlight contribution using Phong lighting model
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Calculate direction from fragment to light source
    vec3 lightDir = normalize(light.position - fragPos);
    
    // DIFFUSE SHADING
    // Calculate how much the surface faces the light (Lambert's law)
    float diff = max(dot(normal, lightDir), 0.0);
    
    // SPECULAR SHADING
    // Calculate reflection direction for specular highlights
    vec3 reflectDir = reflect(-lightDir, normal);
    // Calculate specular intensity using Phong reflection model
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // DISTANCE ATTENUATION
    // Light gets weaker with distance using quadratic formula
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * (distance * distance));
    
    // SPOTLIGHT INTENSITY CALCULATION
    // Check if fragment is within the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    // Create smooth falloff at cone edges
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // COMBINE ALL LIGHTING COMPONENTS
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    
    // Apply attenuation and spotlight intensity to all components
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    // Return combined lighting result
    return (ambient + diffuse + specular);
}

void main()
{
    // Calculate view direction in tangent space for proper normal mapping
    vec3 viewDir=normalize(viewPos-FragPos);
    
    // LIGHTING CALCULATION
    // Note: There's a bug here - using world space Normal instead of tangent space normal
    // !Should probably be: CalcSpotLight(spotLight, normal, fs_in.FragPos, viewDir);
    vec3 result = CalcSpotLight(spotLight, Normal, FragPos, viewDir);
    
    // Output final color with full alpha
    FragColor = vec4(result, 1.0);
}