#version 440 core
out vec4 vertColour;	//output colour of vertex
in vec2 textureCoordinate; //tex coords from vertex shader
in vec3 normals;
in vec3 fragmentPosition;
in vec3 lightColour;

uniform sampler2D aTex;		//uniform holding texture info from main programme
uniform vec3 lightPosition; //uniform holding the light position
uniform vec3 viewPosition;	//uniform holding the viewing position


void main()
{
	//ambient component
	float ambientCoefficient = 0.3f;
	vec3 ambient = ambientCoefficient * lightColour;
	
	//diffuse component
	float diffuseCoefficient = 0.8f;
	//normalise normal vectors (reset them as unit vectors)
	vec3 nNormal = normalize(normals);
	//calculate the light direction from the light position and the fragment position
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
	
	//determine the dot product of normal direction and light direction
	float diffuseStrength = max(dot(nNormal, lightDirection), 0.0f);
	//combine this with the light colour
    vec3 diffuse = diffuseCoefficient * diffuseStrength * lightColour;
	
	//specular component
	float specularCoefficient = 0.7f;
	int specularShininess = 64;
	//set and normalise the viewing direction
	vec3 viewDirection = normalize(viewPosition - fragmentPosition);
	//set direction of reflection, i.e. towards the light position
    vec3 reflectionDirection = reflect(-lightDirection, nNormal);  
	
	//calculate the specular component to be added to the fragment colour
	//dot product between view direction and reflect direction, i.e. how much you might see at your position
	//the shininess of the material is added here
	float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), specularShininess);
    vec3 specular = specularCoefficient * specularStrength * lightColour; 
	
	vec4 textureColour = texture(aTex, textureCoordinate);
	
	//apply ambient and diffuse components with colour contributed by texture
	vertColour = (vec4((ambient+diffuse+specular),1.0) * textureColour);
	
}