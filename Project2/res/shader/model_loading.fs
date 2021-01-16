#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

//纹理采样器
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;


// 定向光源
struct DirLight {
    bool on;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 点光源
struct PointLight {
    bool on;
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float c;
    float l;
    float q;
};

// 聚光
struct SpotLight {
    bool on;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float c;
    float l;
    float q;
};

uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLights[6];
uniform SpotLight spotLight;



// 计算定向光
vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 normal, vec3 diff, vec3 spec)
{
    if(!light.on) {
        return vec3(0.0);
    }
    

    // 漫反射
    vec3 lightDir = normalize(-light.direction);
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    
    // 镜面反射
    vec3 halfway = normalize(lightDir + viewDir);
    float spceularFactor = pow(max(dot(halfway, normal), 0.0f), 32);

    // 计算环境光，漫反射光和镜面光
    vec3 ambient = diff * light.ambient;
    vec3 diffuse = diff * light.diffuse * diffuseFactor;
    vec3 specular = spec * light.specular * spceularFactor;
    
    return ambient + diffuse + specular;
}


// 计算点光源
vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 viewDir, vec3 normal, vec3 diff, vec3 spec)
{
    if(!light.on) {
        return vec3(0.0);
    }
    // 漫反射
    vec3 lightDir = normalize(light.position - fragPos);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    // 镜面反射
    vec3 halfway = normalize(lightDir + viewDir);
    float spceularFactor = pow(max(dot(halfway, normal), 0.0f), 32);
    // 距离和衰减
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.c + light.l * d + light.q * d * d);
    
    
    // 计算环境光，漫反射光和镜面光
    vec3 ambient = light.ambient * diff;
    vec3 diffuse = light.diffuse * diffuseFactor * diff;
    vec3 specular = light.specular * spceularFactor * spec;
    return (ambient + diffuse + specular) * attenuation;
}

// 计算聚光
vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 viewDir, vec3 normal, vec3 diff, vec3 spec)
{
    if(!light.on) {
        return vec3(0.0);
    }
    // 漫反射
    vec3 lightDir = normalize(light.position - fragPos);
    float diffuseFactor = max(dot(normal, lightDir), 0.0); 
    // 镜面反射
    vec3 halfway = normalize(lightDir + viewDir);
    float spceularFactor = pow(max(dot(halfway, normal), 0.0f), 32);
    // 距离和衰减
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.c + light.l * d + light.q * d * d);
    // 聚光强度
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // 计算环境光，漫反射光和镜面光
    vec3 ambient = light.ambient * diff;
    vec3 diffuse = light.diffuse * diffuseFactor * diff;
    vec3 specular = light.specular * spceularFactor * spec;
    // 乘聚光强度为了避免光照过强，平滑聚光边缘
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}

void main()
{    
    //从法线贴图范围[0,1]获取法线
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // 将法线向量转换为范围[-1,1]
    normal = normalize(normal * 2.0f - 1.0f);
    //引入切线到世界空间变换
    normal = normalize(fs_in.TBN * normal);
    
    //从光照贴图里面读取漫反射和镜面反射纹理
    vec3 diffuse = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    //镜面光贴图的纹理怎么只有R分量，真是奇怪
    vec3 specular = vec3(texture(texture_specular1,fs_in.TexCoords).r,texture(texture_specular1,fs_in.TexCoords).r,texture(texture_specular1,fs_in.TexCoords).r);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    //布林—冯

    // 计算定向光
    vec3 result = CalcDirLight(dirLight, viewDir, normal,  diffuse, specular);
    // 计算点光源
    for (int i = 0; i < 6; i++) {
        result += CalcPointLight(pointLights[i], fs_in.FragPos, viewDir, normal,  diffuse, specular);
    }
    // 计算聚光
    result += CalcSpotLight(spotLight,  fs_in.FragPos, viewDir, normal,  diffuse, specular);
    FragColor = vec4(result, 1.0);
   
}