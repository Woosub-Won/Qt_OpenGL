#version 400
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
uniform sampler2D Tex1;
struct LightInfo {
    vec4 Position; // Light position in eye coords.
    vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;
struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;
layout( location = 0 ) out vec4 FragColor;

void phongModel( vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec ) {
    vec3 n = normalize(norm);
    vec3 s = normalize(vec3(Light.Position) - pos); // Light direction
    vec3 v = normalize(-pos); // View direction (eye is at origin)
    vec3 r = reflect(-s, n); // Reflected light direction

    // Ambient component
    vec3 ambient = Light.Intensity * Material.Ka;

    // Diffuse component
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;

    // Specular component
    vec3 specular = vec3(0.0);
    if (sDotN > 0.0) {
        specular = Light.Intensity * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
    }

    ambAndDiff = ambient + diffuse;
    spec = specular;
}

void main() {
    vec3 ambAndDiff, spec;
    vec4 texColor = texture( Tex1, TexCoord );
    phongModel(Position, Normal, ambAndDiff, spec);
    FragColor = vec4(ambAndDiff, 1.0) * texColor + vec4(spec, 1.0);
}
