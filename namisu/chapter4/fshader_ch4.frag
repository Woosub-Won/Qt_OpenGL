#version 400

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

in mat3 toObjectLocal;

uniform sampler2D ColorMapTex;
uniform sampler2D NormalMapTex;
uniform sampler2D DiffuseMapTex;

struct LightInfo {
    vec4 Position; // Light position in eye coords.
    vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

// 출력
out vec4 FragColor;

void main() {
    // Normal Map에서 법선 벡터 가져오기
    vec3 normalMap = texture(NormalMapTex, TexCoord).rgb * 2.0 - 1.0; // [-1,1] 범위로 변환
    vec3 normal = normalize(toObjectLocal * normalMap); // Tangent Space -> Eye Space

    // 조명 및 뷰 방향 계산
    vec3 lightDir = normalize(vec3(Light.Position) - vec3(Position)); // Eye-space에서의 조명 방향
    vec3 viewDir = normalize(-vec3(Position)); // Eye-space에서의 뷰 방향
    vec3 reflectDir = reflect(-lightDir, normal); // 반사 벡터 계산

    // Ambient 성분
    vec3 ambient = Light.Intensity * Material.Ka * texture(ColorMapTex, TexCoord).rgb;

    // Diffuse 성분 (조명과 법선의 내적)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = Light.Intensity * texture(DiffuseMapTex, TexCoord).rgb * diff;

    // Specular 성분 (뷰 벡터와 반사 벡터의 내적)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.Shininess);
    vec3 specular = Light.Intensity * Material.Ks * spec;

    // 최종 색상 계산
    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0); // 최종 색상 출력
}
