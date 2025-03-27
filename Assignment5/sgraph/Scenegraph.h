#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "IScenegraph.h"
#include "SGNode.h"
#include "glm/glm.hpp"
#include "IVertexData.h"
#include "../../include/TextureImage.h"
#include "PolygonMesh.h"
#include <string>
#include <map>
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

    void setImages(map<string,util::TextureImage>& images) { this->images = images; }

    map<string,util::PolygonMesh<VertexAttrib> > getMeshes() {
      return this->meshes;
    }

    map<string,util::TextureImage> getImages() { return this->images; }

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
  };
}
#endif
