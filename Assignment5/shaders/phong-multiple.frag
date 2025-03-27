#version 330

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec4 spotDirection;
    float spotCutoff;
    int type; // 0 = POINT, 1 = DIRECTIONAL, 2 = SPOT
};

in vec3 fNormal;
in vec4 fPosition;
in vec4 fTexCoord;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light[MAXLIGHTS];
uniform int numLights;
uniform vec4 vColor;

/* texture */
uniform sampler2D image;
uniform bool useTexture;

out vec4 fColor;

void main()
{
    vec3 lightVec, viewVec, reflectVec;
    vec3 normalView;
    vec3 ambient, diffuse, specular;
    float nDotL, rDotV;
    float attenuation;

    // Start with zero color
    vec3 color = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < numLights && i < MAXLIGHTS; i++)
    {
        // Default attenuation (no attenuation)
        attenuation = 1.0;
        
        // Calculate light direction based on light type
        if (light[i].type == 1) { // DIRECTIONAL
            // Directional light: use negative position
            lightVec = normalize(-light[i].position.xyz);
        }
        else { // POINT or SPOT
            // Point/spot light: calculate direction to light
            lightVec = normalize(light[i].position.xyz - fPosition.xyz);
            
            // Calculate distance for attenuation
            float distance = length(light[i].position.xyz - fPosition.xyz);
            attenuation = 1.0 / (1.0 + 0.01 * distance + 0.001 * distance * distance);
            
            // For spotlight, check if we're in the cone
            if (light[i].type == 2) // SPOT
            {
                float spotCosine = dot(-lightVec, normalize(light[i].spotDirection.xyz));
                
                if (spotCosine < light[i].spotCutoff)
                {
                    // Outside the spotlight cone
                    attenuation = 0.0;
                }
                else
                {
                    // Inside the spotlight cone, apply falloff
                    attenuation *= pow(spotCosine, 8.0); // Using 8.0 as spotExponent
                }
            }
        }

        // Get the normalized normal
        normalView = normalize(fNormal);
        nDotL = dot(normalView, lightVec);

        // Calculate view vector (towards camera)
        viewVec = normalize(-fPosition.xyz);

        // Calculate reflection vector
        reflectVec = reflect(-lightVec, normalView);
        reflectVec = normalize(reflectVec);

        // Calculate reflection dot view for specular
        rDotV = max(dot(reflectVec, viewVec), 0.0);

        // Calculate lighting components
        ambient = material.ambient * light[i].ambient;
        diffuse = material.diffuse * light[i].diffuse * max(nDotL, 0.0);
        
        if (nDotL > 0.0)
            specular = material.specular * light[i].specular * pow(rDotV, material.shininess);
        else
            specular = vec3(0.0, 0.0, 0.0);
        
        // Add this light's contribution with attenuation
        color += attenuation * (ambient + diffuse + specular);
    }

    // Set the output color
    fColor = vec4(color, 1.0);
    
    // If no lights, fallback to material color
    if (numLights == 0) {
        fColor = vColor;
    }
    
    // Apply texture if enabled
    if (useTexture)
    {
        fColor *= texture(image, fTexCoord.st);
    }
}