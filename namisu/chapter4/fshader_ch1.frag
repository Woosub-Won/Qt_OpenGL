#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex1;

struct LightInfo {
    vec4 Position; // Light position in eye coords
    vec3 Intensity; // Ambient, Diffuse, Specular intensity
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;

layout(location = 0) out vec4 FragColor;

void phongModel(vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec) {
    // Normalize the normal vector
    vec3 n = normalize(norm);
    // Compute the light direction
    vec3 s = normalize(vec3(Light.Position) - pos);
    // Compute the view direction
    vec3 v = normalize(-pos);
    // Compute the reflection direction
    vec3 r = reflect(-s, n);
    // Compute ambient component
    vec3 ambient = Light.Intensity * Material.Ka;
    // Compute diffuse component (Lambertian)
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
    // Compute specular component (Phong reflection)
    float rDotV = max(dot(r, v), 0.0);
    vec3 specular = vec3(0.0);
    if (sDotN > 0.0) {
        specular = Light.Intensity * Material.Ks * pow(rDotV, Material.Shininess);
    }

    // Output ambient + diffuse and specular components
    ambAndDiff = ambient + diffuse;
    spec = specular;
}

void main() {
    vec3 ambAndDiff, spec;
    // Fetch the Texture color
    vec4 texColor = texture(Tex1, TexCoord);
    // Call Phong model
    phongModel(Position, Normal, ambAndDiff, spec);
    // Combine results with texture color
    FragColor = vec4(TexCoord, 1.0, 1.0);
}
