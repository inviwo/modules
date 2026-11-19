
const float PI = 3.14159265358979323846;

int nemoCoeffIndex(float d) {
    return int(floor((200*log((6 + d)/6.))/173.));
}

const vec4 nemoCoeffs[8] = vec4[](
 vec4(-0.5020485620454235, 1.0064965390207923, -0.026022656770173513, 0.00011339152867423767),
 vec4(-0.040511845501916374, 0.9188060606908754, -0.02326253117084194, 0.0002693122897196264),
 vec4(4.947890418847749, 0.38650206094726314, -0.0035014310967430335, 0.000015357339276672554),
 vec4(9.435867702868881, 0.19363090075034473, -0.0006519616483171304, 9.953310338937843e-7),
 vec4(15.290389876996707, 0.10380091532715348, -0.000176565679655483, 1.2741608192830535e-7),
 vec4(24.71897593205658, 0.03985744167034901, -0.000027506296541509556, 8.466529994835795e-9),
 vec4(33.024019406608886, 0.015991187545708305, -3.9489148091425246e-6, 5.183556638826113e-10),
 vec4(40.021437807532486, 0.007584806553897775, -4.797526641657579e-7, 2.882092803741341e-11));

// This will map a depth in meter [0, 6000] to a texture coordinate [0,1]. 
float mapDepthToTexture(float depth) {
    int index = clamp(nemoCoeffIndex(depth), 0, 7);
    vec4 c = nemoCoeffs[index];

	const float nemoZdim = 75;
    return (c.x + depth * c.y + depth*depth*c.z + depth*depth*depth*c.w) / nemoZdim;
}


float linearMap(float val, vec2 from, vec2 to) {
    return (val - from.x) / (from.y - from.x) * (to.y - to.x) + to.x;
}

vec3 rThetaPhiToLatLonDepth(vec3 rThetaPhi) {
    float lat = rThetaPhi.z * 180 / PI - 90;
    float lon = -rThetaPhi.y * 180 / PI;
	float depth = linearMap(1 - rThetaPhi.x, vec2(0, 0.2), vec2(0, 1));
	return vec3(lat, lon, depth);
}

vec2 orcaLatLonToTex(float latDeg, float lonDeg) {
	
	vec2 ij = orcaLatLonToIJ(latDeg, lonDeg);

    float x = (ij.x + 0.5) / 4322;
    float y = (ij.y + 0.5) / 3059;

    return vec2(x,y);
}

// Take a texture pos [0,1] return a pos [-1,1]
vec3 textureSamplePosToSpherePos(vec3 p) {
    return 2.0 * p - vec3(1.0);
}

vec3 cartesianToSpherical(vec3 p) {
    float r = length(p);
    float theta = atan(p.y, p.x);           // azimuth, [-π, π]
    float phi = atan(length(p.xy), p.z);    // polar angle, [0, π]

    return vec3(r, theta, phi);
}

vec3 mapPositionToTexture(vec3 position, float zZoom = 1.0) {
	vec3 spherePos = textureSamplePosToSpherePos(position);
	vec3 rThetePhi = cartesianToSpherical(spherePos);
	vec3 latLonDepth = rThetaPhiToLatLonDepth(rThetePhi);
	vec2 texXY = orcaLatLonToTex(latLonDepth.x, latLonDepth.y);

	float z = mapDepthToTexture(latLonDepth.z * 6000.0 / zZoom);

	return vec3(texXY.x, texXY.y, z);
}