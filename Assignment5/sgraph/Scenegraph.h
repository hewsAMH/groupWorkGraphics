#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "IScenegraph.h"
#include "SGNode.h"
#include "LeafNode.h"
#include "GroupNode.h"
#include "TransformNode.h"
#include "glm/glm.hpp"
#include "IVertexData.h"
#include "../../include/TextureImage.h"
#include "PolygonMesh.h"
#include <string>
#include <map>
#include <vector>
using namespace std;

namespace sgraph {

  /**
 * A specific implementation of this scene graph. This implementation is still independent
 * of the rendering technology (i.e. OpenGL)
 * \author Amit Shesh
 */

  class Scenegraph: public IScenegraph {
    /**
     * The root of the scene graph tree
     */
  protected:
    SGNode *root;
    map<string,util::PolygonMesh<VertexAttrib> > meshes;
    map<string,util::TextureImage> images;
    map<string,string> meshPaths;
    map<string,string> imagePaths;

    /**
     * A map to store the (name,node) pairs. A map is chosen for efficient search
     */
    map<string,SGNode *> nodes;
    
  public:
    Scenegraph() {
      root = NULL;
    }

    ~Scenegraph() {
      dispose();
    }

    void dispose() {
      if (root!=NULL) {
          delete root;
          root = NULL;
      }
    }

    /**
     * Set the root of the scenegraph, and then pass a reference to this scene graph object
     * to all its node. This will enable any node to call functions of its associated scene graph
     * \param root
     */
    void makeScenegraph(SGNode *root) {
      this->root = root;
      if (root!=NULL) {
        this->root->setScenegraph(this);
      }
    }

    void addNode(const string& name, SGNode *node) {
      nodes[name]=node;
    }

    SGNode *getRoot() {
      return root;
    }

    map<string, SGNode *> getNodes() {
      return nodes;
    }

    void setMeshes(map<string,util::PolygonMesh<VertexAttrib> >& meshes) {
      this->meshes = meshes;
    }

    void setImages(map<string,util::TextureImage>& images) { 
      this->images = images; 
    }

    map<string,util::PolygonMesh<VertexAttrib> > getMeshes() {
      return this->meshes;
    }

    map<string,util::TextureImage> getImages() { 
      return this->images; 
    }

    void setMeshPaths(map<string,string>& meshPaths) {
      this->meshPaths = meshPaths;
    }

    void setImagePaths(map<string,string>& imagePaths) {
      this->imagePaths = imagePaths;
    }

    map<string,string> getMeshPaths() {
      return this->meshPaths;
    }

    map<string,string> getImagePaths() {
      return this->imagePaths;
    }
    
    /**
     * Get all lights in the scene, transformed to view space
     * @param viewMatrix the view matrix
     * @return vector of lights in view space
     */
    vector<util::Light> getAllLightsInViewSpace(const glm::mat4& viewMatrix) {
        vector<util::Light> lightsInViewSpace;
        
        // Collect lights from all nodes
        collectLightsFromNode(root, glm::mat4(1.0f), viewMatrix, lightsInViewSpace);
        
        return lightsInViewSpace;
    }

  private:
    /**
     * Helper method to transform a light to a new coordinate system
     */
    util::Light transformLight(const util::Light& light, const glm::mat4& matrix) {
        util::Light result = light;
        
        // Transform position
        glm::vec4 pos = light.getPosition();
        if (pos.w != 0.0f) {
            // Point or spot light - transform position
            glm::vec4 transformedPos = matrix * pos;
            result.setPosition(transformedPos);
        } else {
            // Directional light
            glm::vec3 dirVec(pos.x, pos.y, pos.z);
            // For simplicity, just transform without normal matrix
            glm::vec3 transformedDir = glm::mat3(matrix) * dirVec;
            transformedDir = glm::normalize(transformedDir);
            result.setDirection(transformedDir.x, transformedDir.y, transformedDir.z);
        }
        
        // Transform spotlight direction if applicable
        if (light.getSpotCutoff() > 0.0f) {
            glm::vec4 spotDir = light.getSpotDirection();
            glm::vec3 spotDirVec(spotDir.x, spotDir.y, spotDir.z);
            // Transform direction
            glm::vec3 transformedSpotDir = glm::mat3(matrix) * spotDirVec;
            transformedSpotDir = glm::normalize(transformedSpotDir);
            result.setSpotDirection(transformedSpotDir.x, transformedSpotDir.y, transformedSpotDir.z);
        }
        
        return result;
    }

    /**
     * Helper method to collect lights recursively from the scene graph
     */
    void collectLightsFromNode(SGNode* node, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, vector<util::Light>& lights) {
        if (!node) return;
        
        // Check if this is a leaf node with a light
        LeafNode* leafNode = dynamic_cast<LeafNode*>(node);
        if (leafNode) {
            const util::Light& nodeLight = leafNode->getLight();
            
            // Check if the light has any non-zero components
            if (glm::length(nodeLight.getAmbient()) > 0.0f || 
                glm::length(nodeLight.getDiffuse()) > 0.0f || 
                glm::length(nodeLight.getSpecular()) > 0.0f) {
                
                // Transform the light to view space
                util::Light transformedLight = transformLight(nodeLight, viewMatrix * modelMatrix);
                lights.push_back(transformedLight);
            }
        }
        
        // Process group nodes
        GroupNode* groupNode = dynamic_cast<GroupNode*>(node);
        if (groupNode) {
            for (SGNode* child : groupNode->getChildren()) {
                collectLightsFromNode(child, modelMatrix, viewMatrix, lights);
            }
        }
        
        // Process transform nodes
        TransformNode* transformNode = dynamic_cast<TransformNode*>(node);
        if (transformNode) {
            glm::mat4 newModelMatrix = modelMatrix * transformNode->getTransform();
            if (!transformNode->getChildren().empty()) {
                collectLightsFromNode(transformNode->getChildren()[0], newModelMatrix, viewMatrix, lights);
            }
        }
    }
  };
}
#endif