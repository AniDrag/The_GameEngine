#version 400 core
uniform vec3 lightPosition; // TODO: set this value
out vec4 FragColor; 
in vec3 fPos;
in vec3 fNor;
in vec2 uv;

void main()
{
    //lightDirection = vec3(2,-3,4);
    // debug: show light direction
   // FragColor = vec4(lightDirection,1);
    // TODO: do actual light calculations (start with diffuse!)                     
    // aligment aka Dot product between normal and light vector.,  -> -> of dot = 1,    |  -> of dot = 0, or negative 1 if <- -> 
    // show normals:                              
   float alighment = max(dot(fNor, lightPosition),0.0);
   
   FragColor = vec4(alighment,alighment,alighment, 1);

   // Collor buffer after framgent shader. rendering the fragments to a Buffer ffame and we use it to add post processong

   /// GL_Texture_0 + i in a for loop can loop tghru n textures
   ///You can subract, add, multiply passes

}