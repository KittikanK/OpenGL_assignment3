#version 330

out vec4 colour;

in vec4 vColor;
in vec3 vNormal;
in vec3 vPosition;


uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main()
{
    float ambientStrength = 0.5f;


    vec3 lightDir = normalize(lightPosition - vPosition);
    vec3 normal = normalize(vNormal);
    float diffuse = max(dot(lightDir, normal), 0.0);

    float specularStrength = 0.2f;
    vec3 viewDir = normalize(cameraPosition - vPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectDir), 0.0),8);
    float specular = specAmount * specularStrength;

    colour = vColor * lightColor * (diffuse+ambientStrength+specular); 
}