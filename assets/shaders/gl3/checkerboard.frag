#version 130

//uniform bool uGrid;

//uniform vec4 uColorPrimary;
//uniform vec4 uColorSecondary;

//uniform float uFadeNear;
//uniform float uFadeFar;

in vec2 vTexcoord;
in vec4 cameraSpacePosition;

out vec4 fragColor;

void main()
{
    float uFadeNear = 0.01;
    float uFadeFar = 10;

    vec4 uColorPrimary = vec4(1.0,1.0,1.0,0.7);
    vec4 uColorSecondary = vec4(0.1,0.1,0.1,0.7);

    bool uGrid = false;

    vec2 fw = fwidth(vTexcoord);
	vec2 square = fract(vTexcoord);

    vec4 color;
    if (uGrid)
    {
        if (square.x > 0.99 || square.y > 0.99)
        {
            color = vec4(uColorPrimary);
        }
        else
        {
            color = vec4(uColorSecondary);
        }
	}
	else
	{
	    vec2 p = smoothstep(vec2(0.5), vec2(0.5) + fw, square) + (1.0 - smoothstep(vec2(0.0), vec2(0.0) + fw, square));
    	color = mix(uColorPrimary, uColorSecondary, p.x * p.y + (1.0 - p.x) * (1.0 - p.y));
	}

	float f = 1.0 - smoothstep(uFadeNear, uFadeFar, length(cameraSpacePosition));
    color.a = min(color.a, f);

    fragColor = color;
}
