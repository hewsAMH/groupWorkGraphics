#ifndef __TEXTURE_UTILS_H__
#define __TEXTURE_UTILS_H__

#include "../../include/TextureImage.h"
#include <map>
#include <string>
#include <glad/glad.h>

namespace util
{

    // Create and store texture IDs for each texture
    class TextureManager
    {
    public:
        TextureManager() {}

        // Create texture IDs for all textures in the map
        void createTextureIDs(std::map<std::string, util::TextureImage> &textures)
        {
            for (auto it = textures.begin(); it != textures.end(); ++it)
            {
                if (textureIDs.find(it->first) == textureIDs.end())
                {
                    GLuint textureID;
                    glGenTextures(1, &textureID);
                    glBindTexture(GL_TEXTURE_2D, textureID);

                    // Set texture parameters for mipmapping
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    // Upload texture data
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, it->second.getWidth(), it->second.getHeight(),
                                 0, GL_RGB, GL_UNSIGNED_BYTE, it->second.getImage());

                    // Generate mipmaps
                    glGenerateMipmap(GL_TEXTURE_2D);

                    textureIDs[it->first] = textureID;
                }
            }
        }

        // Bind a texture by name
        void bindTexture(const std::string &textureName, GLenum textureUnit = GL_TEXTURE0)
        {
            if (textureIDs.find(textureName) != textureIDs.end())
            {
                glActiveTexture(textureUnit);
                glBindTexture(GL_TEXTURE_2D, textureIDs[textureName]);
            }
        }

        // Check if a texture exists
        bool hasTexture(const std::string &textureName)
        {
            return textureIDs.find(textureName) != textureIDs.end();
        }

        // Clean up textures
        void cleanup()
        {
            for (auto it = textureIDs.begin(); it != textureIDs.end(); ++it)
            {
                glDeleteTextures(1, &it->second);
            }
            textureIDs.clear();
        }

    private:
        std::map<std::string, GLuint> textureIDs;
    };

} // namespace util

#endif // __TEXTURE_UTILS_H__