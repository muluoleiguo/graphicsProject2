#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

//���������
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;


// �����Դ
struct DirLight {
    bool on;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// ���Դ
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

// �۹�
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



// ���㶨���
vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 normal, vec3 diff, vec3 spec)
{
    if(!light.on) {
        return vec3(0.0);
    }
    

    // ������
    vec3 lightDir = normalize(-light.direction);
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    
    // ���淴��
    vec3 halfway = normalize(lightDir + viewDir);
    float spceularFactor = pow(max(dot(halfway, normal), 0.0f), 32);

    // ���㻷���⣬�������;����
    vec3 ambient = diff * light.ambient;
    vec3 diffuse = diff * light.diffuse * diffuseFactor;
    vec3 specular = spec * light.specular * spceularFactor;
    
    return ambient + diffuse + specular;
}


// ������Դ
vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 viewDir, vec3 normal, vec3 diff, vec3 spec)
{
    if(!light.on) {
        return vec3(0.0);
    }
    // ������
    vec3 lightDir = normalize(light.position - fragPos);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    // ���淴��
    vec3 halfway = normalize(lightDir + viewDir);
    float spceularFactor = pow(max(dot(halfway, normal), 0.0f), 32);
    // �����˥��
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.c + light.l * d + light.q * d * d);
    
    
    // ���㻷���⣬�������;����
    vec3 ambient = light.ambient * diff;
    vec3 diffuse = light.diffuse * diffuseFactor * diff;
    vec3 specular = light.specular * spceularFactor * spec;
    return (ambient + diffuse + specular) * attenuation;
}

// ����۹�
vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 viewDir, vec3 normal, vec3 diff, vec3 spec)
{
    if(!light.on) {
        return vec3(0.0);
    }
    // ������
    vec3 lightDir = normalize(light.position - fragPos);
    float diffuseFactor = max(dot(normal, lightDir), 0.0); 
    // ���淴��
    vec3 halfway = normalize(lightDir + viewDir);
    float spceularFactor = pow(max(dot(halfway, normal), 0.0f), 32);
    // �����˥��
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.c + light.l * d + light.q * d * d);
    // �۹�ǿ��
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // ���㻷���⣬�������;����
    vec3 ambient = light.ambient * diff;
    vec3 diffuse = light.diffuse * diffuseFactor * diff;
    vec3 specular = light.specular * spceularFactor * spec;
    // �˾۹�ǿ��Ϊ�˱�����չ�ǿ��ƽ���۹��Ե
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}

void main()
{    
    //�ӷ�����ͼ��Χ[0,1]��ȡ����
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // ����������ת��Ϊ��Χ[-1,1]
    normal = normalize(normal * 2.0f - 1.0f);
    //�������ߵ�����ռ�任
    normal = normalize(fs_in.TBN * normal);
    
    //�ӹ�����ͼ�����ȡ������;��淴������
    vec3 diffuse = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    //�������ͼ��������ôֻ��R�������������
    vec3 specular = vec3(texture(texture_specular1,fs_in.TexCoords).r,texture(texture_specular1,fs_in.TexCoords).r,texture(texture_specular1,fs_in.TexCoords).r);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    //���֡���

    // ���㶨���
    vec3 result = CalcDirLight(dirLight, viewDir, normal,  diffuse, specular);
    // ������Դ
    for (int i = 0; i < 6; i++) {
        result += CalcPointLight(pointLights[i], fs_in.FragPos, viewDir, normal,  diffuse, specular);
    }
    // ����۹�
    result += CalcSpotLight(spotLight,  fs_in.FragPos, viewDir, normal,  diffuse, specular);
    FragColor = vec4(result, 1.0);
   
}