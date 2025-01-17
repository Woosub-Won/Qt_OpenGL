#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
uniform sampler2D Tex0;
uniform sampler2D Tex1;

struct LightInfo {
    vec4 Position;
    vec3 La, Ld, Ls;
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka, Kd, Ks;
    float Shininess;
};
uniform MaterialInfo Material;

out vec4 FragColor;

void main() {


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(Light.Position.xyz - Position);
    vec3 viewDir = normalize(-Position);
    vec3 reflectDir = reflect(-lightDir, norm);

    if(!gl_FrontFacing ) {
       norm = -norm;
    }

    // Ambient
    vec3 ambient = Light.La * Material.Ka;

    // Diffuse
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = Light.Ld * Material.Kd * diff;

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.Shininess);
    vec3 specular = Light.Ls * Material.Ks * spec;

    vec4 brickTex = texture(Tex0, TexCoord);
    vec4 mossTex = texture(Tex1, TexCoord);
    vec4 texColor = mix(brickTex, mossTex,  mossTex.a);

    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting, 1.0);
    if(mossTex.a < 0.15){
        //discard;
    }
    else {

    }

}
