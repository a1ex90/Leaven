#version 130

in vec3 vertexPosition;
out vec3 position;

uniform mat4 modelViewMatrix;
uniform mat4 mvp;
uniform vec4 color;
uniform vec4 viewport;
uniform float R;

void main() {
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
    position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    mat4 T = mat4(
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            vertexPosition.x/R, vertexPosition.y/R, vertexPosition.z/R, 1.0/R);

    mat4 PMTt = transpose(mvp * T);

    vec4 r1 = PMTt[0];
    vec4 r2 = PMTt[1];
    vec4 r4 = PMTt[3];
    float r1Dr4T = dot(r1.xyz,r4.xyz)-r1.w*r4.w;
    float r1Dr1T = dot(r1.xyz,r1.xyz)-r1.w*r1.w;
    float r4Dr4T = dot(r4.xyz,r4.xyz)-r4.w*r4.w;
    float r2Dr2T = dot(r2.xyz,r2.xyz)-r2.w*r2.w;
    float r2Dr4T = dot(r2.xyz,r4.xyz)-r2.w*r4.w;

    gl_Position = vec4(-r1Dr4T, -r2Dr4T, gl_Position.z/gl_Position.w*(-r4Dr4T), -r4Dr4T);

    float discriminant_x = r1Dr4T*r1Dr4T-r4Dr4T*r1Dr1T;
    float discriminant_y = r2Dr4T*r2Dr4T-r4Dr4T*r2Dr2T;
    float screen = min(float(viewport.z), float(viewport.w));

    gl_PointSize = sqrt(max(discriminant_x, discriminant_y))*screen/(-r4Dr4T);
}