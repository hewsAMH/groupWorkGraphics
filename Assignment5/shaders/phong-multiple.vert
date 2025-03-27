#version 330

in vec4 vPosition;
in vec4 vNormal;
in vec4 vTexCoord;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalmatrix;
uniform mat4 texturematrix;

out vec3 fNormal;
out vec4 fPosition;
out vec4 fTexCoord;

void main()
{
    // Transform vertex position to view space
    fPosition = modelview * vPosition;
    gl_Position = projection * fPosition;

    // Transform normal to view space
    vec4 tNormal = normalmatrix * vNormal;
    fNormal = normalize(tNormal.xyz);

    // Pass texture coordinates
    fTexCoord = texturematrix * vTexCoord;
}