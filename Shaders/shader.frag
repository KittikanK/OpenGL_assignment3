#version 330

out vec4 colour;

in vec4 vColor;
in vec3 vNormal;
in vec3 vPosition;
in vec2 TexCoord;


uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform sampler2D texture2D;

vec4 pointLight(vec4 tt)
{
    
    vec3 lightVector = lightPosition - vPosition;
    float dist = length(lightVector);
    float a = 0.39f;
    float b = 0.0001f;
    float inten = 1/(a*dist*dist * b*dist+1);
    

     
    float ambientStrength = 0.5f;

    //diffuse
    vec3 lightDir = normalize(lightVector);
    vec3 normal = normalize(vNormal);
    float diffuse = max(dot(lightDir, normal), 0.0);

    //specular
    float specularStrength = 0.2f;
    vec3 viewDir = normalize(cameraPosition - vPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectDir), 0.0),8);
    float specular = specAmount * specularStrength;

    diffuse = diffuse * inten;
    specular = specular * inten;
                                        
    return tt * vColor  *   lightColor  *  (ambientStrength+diffuse+specular); 
}

vec4 direcLight(vec4 tt)
{
    float ambientStrength = 0.5f;
    
    //diffuse
    //vec3 lightDir = normalize(lightPosition);
    vec3 lightDir = normalize(vec3(vPosition.x-1,vPosition.y+1,vPosition.z+1));
    vec3 normal = normalize(vNormal);
    float diffuse = max(dot(lightDir, normal), 0.0);

    //specular
    float specularStrength = 0.2f;
    vec3 viewDir = normalize(cameraPosition - vPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectDir), 0.0),8);
    float specular = specAmount * specularStrength;
      
    return  tt * vColor  *   lightColor  *  (ambientStrength+diffuse+specular);
}

vec4 spotLight(vec4 tt)
{
    float innerCone = 0.95f;
    float outerCone = 0.90f;


    float ambientStrength = 0.5f;

    
    //diffuse
    vec3 lightDir = normalize(lightPosition - vPosition);
    vec3 normal = normalize(vNormal);
    float diffuse = max(dot(normal,lightDir), 0.0);

    //specular
    float specularStrength = 0.2f;
    vec3 viewDir = normalize(cameraPosition - vPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectDir), 0.0),8);
    float specular = specAmount * specularStrength;
    
    float angle = dot(vec3(0.0f,-1.0f,0.0f), -lightDir);
    float inten = clamp((angle - outerCone)/(innerCone - outerCone),0.0f,1.0f);

    diffuse = diffuse * inten;
    specular = specular * inten;

    return tt * vColor  *   lightColor  *  (ambientStrength+diffuse+specular);
}

vec4 noneLight(vec4 tt)
{
    return tt * vColor;
}

void main()
{
    vec4 tt = texture(texture2D, TexCoord);
    colour = pointLight(tt);  
}