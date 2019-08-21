out vec4 vColor;
flat out uint vPicking;


uniform ivec2 dims;
uniform vec2 dataMin;
uniform vec2 dataMax;
uniform ivec2 domainMin;
uniform ivec2 domainMax;

void main() {
    vColor = in_Color;
    vPicking = in_Picking;

    vec2 nPos = (vec2(in_Time, in_Duration) - dataMin) / (dataMax - dataMin);
    vec2 pPos = nPos * (domainMax - domainMin) + domainMin;
    vec2 ndc = (pPos / dims) * vec2(2.0) - vec2(1.0); 

    gl_Position = vec4(ndc, 0.0, 1.0);
}

