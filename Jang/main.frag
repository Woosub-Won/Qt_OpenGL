#version 400
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex1;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

layout( location = 0 ) out vec4 FragColor;

void phongModel( vec3 pos, vec3 norm,
                out vec3 ambAndDiff, out vec3 spec ) {
    vec3 n = normalize( norm );
    vec3 s = normalize( vec3(LightPosition) - pos );
    vec3 v = normalize(vec3(-pos));
    vec3 h = normalize( v + s );
    ambAndDiff = LightIntensity * ( Ka + Kd * max( dot(s, n), 0.0 ));
    spec = LightIntensity * (Ks * pow( max( dot(h, v), 0.0 ), Shininess ));
}

void main() {
    vec3 ambAndDiff, spec;
    vec4 texColor = texture( Tex1, TexCoord );
    phongModel(Position, Normal, ambAndDiff, spec);
    FragColor = vec4(ambAndDiff, 1.0) * texColor + vec4(spec, 1.0);
}
