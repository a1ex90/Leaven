#version 130

in vec3 vertexPosition;

out vec3 position;

uniform mat4 modelViewMatrix;
uniform mat4 mvp;
uniform float ps;

void main()
{
	position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    gl_PointSize = ps;
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
