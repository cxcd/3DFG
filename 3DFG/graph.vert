#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in float height1;
layout(location = 2) in float height2;

out vec4 color;

uniform mat4 MVP;
uniform float weight;

float remap(float v, float inMin, float inMax, float outMin, float outMax) {
  return (v - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
}

void main() {
	// Get new positions
	float newY = mix(height1, height2, weight);
	vec3 finalPos = vec3(vPos.x, newY, vPos.z);

	// Nice effect, brighest at 0.5, 0.5, 0.5
	//float xBias = vPos.x;
	//float yBias = newY;
	//float zBias = vPos.z;
	//float bias = (xBias + yBias + zBias) / 1.5;
	//vec3 finalColor = vec3(0.5 + bias, 0.5 + bias, 0.5 + bias);

	// New effect
	//float xBias = vPos.x;
	//float yBias = newY;
	//float zBias = vPos.z;
	//float bias = -(xBias * xBias + zBias * zBias) + 0.5 + yBias;
	//vec3 finalColor = vec3(0.5 + bias, 0.5 + bias, 0.5 + bias);

	gl_Position = MVP * vec4(finalPos, 1.0);

	// Color based on vertex y position
	vec3 color1 = vec3(0.5, 0.0, 0.7); // Lowest point
	vec3 color2 = vec3(0.9, 0.9, 1.0); // Highest point
	vec3 heightColor = mix(color1, color2, finalPos.y * 2); // change vPos to finalPos

	// test
	//float alpha = remap(gl_Position.z, 0.0, 2.0, 0.8, 0.0);
	//heightColor *= alpha;
	// end test
	color = vec4(heightColor, 1.0);
}