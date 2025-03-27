Lights and Textures: 
For this assignment we worked together to add texture and lighting to the courtyard scene. The implementation allows lights to be attached to any node in the scene graph, 
transforming with the node's coordinate system, and uses per-fragment (Phong) shading for realistic lighting effects. All objects in the scene have material properties defined 
with varying ambient, diffuse, and specular components, creating visually distinct shiny and diffuse surfaces. The scene includes at least two point light sources at strategic 
locations illuminating the entire environment, plus one narrow-angle spotlight (20-30 degrees) that creates a clearly visible lighting effect. Texture mapping is applied to all
objects using PPM image files, with mipmapping enabled for quality rendering, and at least two different texture images are used throughout the scene. The parser supports
commands for defining lights, materials, texture loading, and texture assignment to scene elements. To divide the work Aidan got the ball rolling by creating the worspace and 
added functions to our existing codebase to support lighting and texture/image mapping in LeafNode.h, IScenegraph.h, Scenegraph.h, and ScenegraphImporter.h. I then picked up 
where they left off finished / cleaned up the implementation for these functions and edited the courtyard-scene-commands.txt to include the new commands for lighting and texture. 
We worked together to debug the code and ensure that the lighting and texture effects were working correctly using github collaboration. 

image downloads:
https://filesamples.com/formats/ppm
https://www.cs.cornell.edu/courses/cs664/2003fa/images/ 
https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.vecteezy.com%2Ffree-photos%2Fgrass-texture-seamless&psig=AOvVaw1rUgehsJvhttMnJL2BxYvJ&ust=1743204248023000&source=images&cd=vfe&opi=89978449&ved=0CBcQjhxqFwoTCNiC4oK0q4wDFQAAAAAdAAAAABA_
https://www.google.com/url?sa=i&url=https%3A%2F%2Fcommons.wikimedia.org%2Fwiki%2FFile%3ARed_brick_wall_texture.JPG&psig=AOvVaw1rUgehsJvhttMnJL2BxYvJ&ust=1743204248023000&source=images&cd=vfe&opi=89978449&ved=0CBcQjhxqFwoTCNiC4oK0q4wDFQAAAAAdAAAAABBI
https://www.google.com/url?sa=i&url=https%3A%2F%2Ffreestocktextures.com%2F&psig=AOvVaw1rUgehsJvhttMnJL2BxYvJ&ust=1743204248023000&source=images&cd=vfe&opi=89978449&ved=0CBcQjhxqFwoTCNiC4oK0q4wDFQAAAAAdAAAAABBR

we used this file converter to convert jpg to ppm:
https://convertio.co/ppm-converter/