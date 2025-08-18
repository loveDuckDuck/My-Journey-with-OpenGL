#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

out vec3 Normal;
out vec3 FragPos; 

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Calculate fragment position 📍
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    // Pass texture coordinates 🗺️
    vs_out.TexCoords = aTexCoords;
    
    // Calculate normal matrix 📐
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    // Tangent in tangent space 🧭
    vec3 T = normalize(normalMatrix * aTangent);
    // Normal in tangent space ⬆️
    vec3 N = normalize(normalMatrix * aNormal);
    // Re-orthogonalize T vector 🔄
    T = normalize(T - dot(T, N) * N);
    // Calculate bitangent vector 🟦
    vec3 B = cross(N, T);
    
    // Create TBN matrix 🧮
    mat3 TBN = transpose(mat3(T, B, N));    
    // Light position in tangent space 💡
    vs_out.TangentLightPos = TBN * lightPos;
    // View position in tangent space 👀
    vs_out.TangentViewPos  = TBN * viewPos;
    // Fragment in tangent space 📌
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    
    // Calculate world space normal 🌐
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    // Calculate world space frag position 🌎
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Final clip space position 👓
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}