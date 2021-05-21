#version 130

uniform struct LightInfo {
    vec4 position;
    vec3 intensity;
} light;

uniform vec4 color;
uniform vec3 eye;

in vec3 position;
out vec4 fragColor;


vec3 CalcPointLight(LightInfo light, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera) {
    float ambientCoff = 0.3f;
    float att = 0.1f;
    vec3 surfaceToLight = normalize(light.position.xyz - surfacePos);
    //AMBIENT COMPONENT
    vec3 ambient = ambientCoff * color.xyz * light.intensity;

    //DIFFUSE COMPONENT
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * color.xyz * light.intensity;

    //SPECULAR COMPONENT
    float specularCoefficient = 1.0f;
    vec3 materialSpecularColor = 0.02f * color.xyz;
    float materialShininess = 10000.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), materialShininess);
    vec3 specular = specularCoefficient * materialSpecularColor * light.intensity;

    //Attenuation
    float distanceToLight = length(light.position.xyz - surfacePos);
    float attenuation = 1.0 / (1.0 + att * pow(distanceToLight, 2));

    return (ambient + attenuation*(diffuse + specular));
}

void main() {
    vec3 surfaceToCamera = normalize(eye - position);
    vec3 normal;
    normal.xy = gl_PointCoord* 2.0 - vec2(1.0);
    float mag = dot(normal.xy, normal.xy);
    if(mag > 1.0)
    discard; // kill pixels outside circle

    normal.z = sqrt(1.0-mag);
    // calculate lighting
    vec3 linearColor = CalcPointLight(light, normal, position, surfaceToCamera);

//    vec3 gamma = vec3(1.0/2.2);
    vec3 gamma = vec3(1.0/2.8);
    fragColor = vec4(pow(linearColor, gamma), color[3]);
}