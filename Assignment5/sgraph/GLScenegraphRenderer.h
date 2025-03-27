#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include "glm/gtc/type_ptr.hpp"
#include <stack>
#include <iostream>
#include <vector>
using namespace std;

namespace sgraph
{
    /**
     * This visitor implements drawing the scene graph using OpenGL
     */
    class GLScenegraphRenderer : public SGNodeVisitor
    {
    private:
        stack<glm::mat4> &modelview;
        util::ShaderLocationsVault shaderLocations;
        map<string, util::ObjectInstance *> objects;
        map<string, util::TextureImage> textures;
        vector<util::Light> lights;
        int maxLights;

        /**
         * Create and store texture IDs for each texture
         */
        class TextureManager
        {
        public:
            TextureManager() {}

            // Create texture IDs for all textures in the map
            void createTextureIDs(map<string, util::TextureImage> &textures)
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
            void bindTexture(const string &textureName, GLenum textureUnit = GL_TEXTURE0)
            {
                if (textureIDs.find(textureName) != textureIDs.end())
                {
                    glActiveTexture(textureUnit);
                    glBindTexture(GL_TEXTURE_2D, textureIDs[textureName]);
                }
            }

            // Check if a texture exists
            bool hasTexture(const string &textureName)
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
            map<string, GLuint> textureIDs;
        };

        TextureManager textureManager;

    public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         */
        GLScenegraphRenderer(
            stack<glm::mat4> &mv,
            map<string, util::ObjectInstance *> &os,
            map<string, util::TextureImage> &txs,
            util::ShaderLocationsVault &shaderLocations) : modelview(mv), objects(os), textures(txs), shaderLocations(shaderLocations)
        {
            this->maxLights = 10; // Must match MAXLIGHTS in the shader

            // Create texture IDs
            textureManager.createTextureIDs(textures);

            for (map<string, util::ObjectInstance *>::iterator it = objects.begin(); it != objects.end(); it++)
            {
                cout << "Mesh with name: " << it->first << endl;
            }
        }

        /**
         * @brief Set the lights to use for rendering
         */
        void setLights(const vector<util::Light> &lights)
        {
            this->lights = lights;
        }

        /**
         * @brief Recur to the children for drawing
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            for (int i = 0; i < groupNode->getChildren().size(); i = i + 1)
            {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf
         */
        void visitLeafNode(LeafNode *leafNode)
        {
            // send modelview matrix to GPU
            glUniformMatrix4fv(
                shaderLocations.getLocation("modelview"),
                1,
                GL_FALSE,
                glm::value_ptr(modelview.top()));

            // Calculate normal matrix
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelview.top())));
            glUniformMatrix4fv(
                shaderLocations.getLocation("normalmatrix"),
                1,
                GL_FALSE,
                glm::value_ptr(glm::mat4(normalMatrix)));

            // Texture matrix (identity by default)
            glm::mat4 texMatrix(1.0f);
            glUniformMatrix4fv(
                shaderLocations.getLocation("texturematrix"),
                1,
                GL_FALSE,
                glm::value_ptr(texMatrix));

            // Get the material
            util::Material mat = leafNode->getMaterial();

            // Send material to shader
            if (shaderLocations.getLocation("material.ambient") >= 0)
            {
                glUniform3fv(shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(glm::vec3(mat.getAmbient())));
                glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(glm::vec3(mat.getDiffuse())));
                glUniform3fv(shaderLocations.getLocation("material.specular"), 1, glm::value_ptr(glm::vec3(mat.getSpecular())));
                glUniform1f(shaderLocations.getLocation("material.shininess"), mat.getShininess());
            }

            // For backward compatibility
            glUniform4fv(shaderLocations.getLocation("vColor"), 1, glm::value_ptr(mat.getAmbient()));

            // Send light information
            if (shaderLocations.getLocation("numLights") >= 0)
            {
                glUniform1i(shaderLocations.getLocation("numLights"), min((int)lights.size(), maxLights));

                for (int i = 0; i < min((int)lights.size(), maxLights); i++)
                {
                    string prefix = "light[" + to_string(i) + "].";

                    glUniform3fv(shaderLocations.getLocation(prefix + "ambient"), 1, glm::value_ptr(lights[i].getAmbient()));
                    glUniform3fv(shaderLocations.getLocation(prefix + "diffuse"), 1, glm::value_ptr(lights[i].getDiffuse()));
                    glUniform3fv(shaderLocations.getLocation(prefix + "specular"), 1, glm::value_ptr(lights[i].getSpecular()));
                    glUniform4fv(shaderLocations.getLocation(prefix + "position"), 1, glm::value_ptr(lights[i].getPosition()));

                    // For spotlights
                    if (lights[i].getSpotCutoff() > 0.0f)
                    {
                        glUniform4fv(shaderLocations.getLocation(prefix + "spotDirection"), 1, glm::value_ptr(lights[i].getSpotDirection()));
                        glUniform1f(shaderLocations.getLocation(prefix + "spotCutoff"), cos(glm::radians(lights[i].getSpotCutoff())));
                        glUniform1i(shaderLocations.getLocation(prefix + "type"), 2); // 2 = SPOT
                    }
                    else if (lights[i].getPosition().w == 0.0f)
                    {
                        glUniform1i(shaderLocations.getLocation(prefix + "type"), 1); // 1 = DIRECTIONAL
                    }
                    else
                    {
                        glUniform1i(shaderLocations.getLocation(prefix + "type"), 0); // 0 = POINT
                    }
                }
            }

            // Handle texture
            if (shaderLocations.getLocation("useTexture") >= 0)
            {
                string texName = leafNode->getTexture().getName();
                if (texName != "" && textureManager.hasTexture(texName))
                {
                    glUniform1i(shaderLocations.getLocation("useTexture"), 1);
                    glUniform1i(shaderLocations.getLocation("image"), 0); // Use texture unit 0
                    textureManager.bindTexture(texName, GL_TEXTURE0);
                }
                else
                {
                    glUniform1i(shaderLocations.getLocation("useTexture"), 0);
                }
            }

            // Draw the object
            objects[leafNode->getInstanceOf()]->draw();
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            if (transformNode->getChildren().size() > 0)
            {
                transformNode->getChildren()[0]->accept(this);
            }
            modelview.pop();
        }

        void visitScaleTransform(ScaleTransform *scaleNode)
        {
            visitTransformNode(scaleNode);
        }

        void visitTranslateTransform(TranslateTransform *translateNode)
        {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode)
        {
            visitTransformNode(rotateNode);
        }

        ~GLScenegraphRenderer()
        {
            textureManager.cleanup();
        }
    };
}

#endif // _GLSCENEGRAPHRENDERER_H_