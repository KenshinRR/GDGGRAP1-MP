#version 330 core

//I USED POINT LIGHT HERE by Reblando, Kenshin R.

uniform sampler2D tex0;

uniform sampler2D norm_tex;

uniform vec3 lightPos;

uniform vec3 lightColor;

uniform float ambientStr;

uniform vec3 ambientColor;

uniform vec3 cameraPos;

uniform float specStr;

uniform float specPhong;

uniform float brightness;

//		EXPORT

out vec4 FragColor;

//		IMPORT

in vec2 texCoord;

in vec3 normCoord;

in vec3 fragPos;

in mat3 TBN;

void main()
{
	//get the color of the pixel
	vec4 pixelColor = texture(tex0, texCoord);
	//if the alpha is low enough
	if (pixelColor.a < 0.1){ //0.1 is the cutoff
		//discad this pixel
		discard; //works same as return
		//ignores the rest after this
	}

	//Gets the RGB data of the texture
	vec3 normal = texture(norm_tex, texCoord).rgb;
	//converts
	//RGB -> XYZ
	//0 == -1
	//1 == 1
	normal = normalize(normal * 2.0 - 1.0); //Normal = (2RGB) - 1

	//multiply the normal wih the TBN matrix
	normal = normalize(TBN * normal);

	//vec3 normal = normalize(normCoord);

	vec3 lightVec = lightPos - fragPos;
	vec3 lightDir = normalize(lightVec);

	//POINT LIGHT
	//compute for the attenuation which is the change in light intensity over the change in distance
	float flDistance = length(lightVec); //get the distance between the light and the fragment object
	float linear = 0.09f, quadratic = 0.032f; //constants for the attenuation formula
	float lightIntensity = brightness /(quadratic * flDistance * flDistance + linear * flDistance + 1.0f); //computing the actual intensity or attenuation

	float diff = max(dot(normal,lightDir) , 0.0);

	vec3 diffuse = diff * lightColor;

	vec3 ambientCol = ambientColor * ambientStr;

	vec3 viewDir = normalize(cameraPos - fragPos);

	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);

	vec3 specColor = spec * specStr * lightColor;

	//FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	//multiply the attenuation to the sum of the diffuse, ambientCol, and specColor to implement it
	FragColor = vec4((diffuse + ambientCol + specColor) * lightIntensity,1.0) * texture(tex0, texCoord);
}
