in vec4 lPickColor;
in vec4 lColor;
in float lFalloffAlpha;

float fallofPower = 2.0;

void main() {   
    vec4 res = lColor;
    res.a *= pow(lFalloffAlpha, fallofPower);

    PickingData = lPickColor;
    FragData0 = res;
}
