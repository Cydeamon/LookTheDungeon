#version 460 core
out vec4 FragColor;

layout (location = 0) in vec2 TexCoord;
layout (location = 1) in vec4 MeshColor;
layout (location = 2) in vec3 Position;
layout (location = 3) in vec3 Normal;

uniform sampler2D TextureDiffuse1;
uniform bool UseTexture;
uniform vec3 ViewPos;

void main()
{
    bool blinn = false;
    vec4 textureColor = texture(TextureDiffuse1, TexCoord);
    vec4 color;
    vec3 lightPos = vec3(1000, 1000, 1000);

    if (UseTexture)
        color = mix(MeshColor, textureColor.rgba, textureColor.a);
    else
        color = MeshColor;

    // Ambient light
    vec4 ambient = 0.8 * color;

    // Diffuse light
    vec3 lightDir = normalize(lightPos - Position);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec4 diffuse = diff * color;

    // Specular
    float spec = 0.1;
    vec3 viewDir = normalize(ViewPos - Position);

    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }

    vec4 specular = vec4(0.3) * spec; // assuming bright white light color
    FragColor = ambient + diffuse + specular;
    FragColor.a = 1;
}