#version 130

in vec3 vertexPosition;
in vec2 aTexcoord;

out vec2 vTexcoord;
out vec4 cameraSpacePosition;

uniform mat4 modelViewMatrix;
uniform mat4 mvp;

void main()
{
	cameraSpacePosition = modelViewMatrix * vec4( vertexPosition, 1.0 );

	vTexcoord = aTexcoord;

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
