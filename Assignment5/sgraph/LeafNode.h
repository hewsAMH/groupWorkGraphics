#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractSGNode.h"
#include "glm/glm.hpp"
#include "Light.h"
#include "Material.h"
#include "../../include/TextureImage.h"
#include "SGNodeVisitor.h"

#include <map>
#include <stack>
#include <string>
using namespace std;

namespace sgraph {

/**
 * This node represents the leaf of a scene graph. It is the only type of node that has
 * actual geometry to render.
 * \author Amit Shesh
 */
class LeafNode : public AbstractSGNode {
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
  protected:
    string objInstanceName;
    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;
    /**
     * The light associated with the object instance at this leaf
     */
    util::Light light;
    /**
     * The image/texture associated with the object instance at this leaf
     */
    util::TextureImage image;

  public:
    LeafNode(
        const string& instanceOf, util::Material& material, util::Light& light, util::TextureImage& image, const string& name,
        sgraph::IScenegraph* graph
    )
        : AbstractSGNode(name, graph), objInstanceName(instanceOf), material(material), light(light), image(image) {}

    LeafNode(const string& instanceOf, const string& name, sgraph::IScenegraph* graph)
        : AbstractSGNode(name, graph), objInstanceName(instanceOf) {}

    ~LeafNode() {}

    /*
     *Set the material of each vertex in this object
     */
    void setMaterial(const util::Material& mat) { material = mat; }

    /*
     *Set the light of each vertex in this object
     */
    void setLight(const util::Light& lt) { this->light = lt; }

    /*
     *Set the image/texture of each vertex in this object
     */
    void setTexture(const util::TextureImage& img) { this->image = img; }

    /*
     * gets the material
     */
    util::Material getMaterial() { return material; }

    /*
     * gets the light
     */
    util::Light getLight() { return this->light; }

    /*
     * gets the image/texture
     */
    util::TextureImage getTexture() { return this-> image; }

    /**
     * Get the name of the instance this leaf contains
     *
     * @return string
     */
    string getInstanceOf() { return this->objInstanceName; }

    /**
     * Get a copy of this node.
     *
     * @return SGNode*
     */

    SGNode* clone() {
        LeafNode* newclone = new LeafNode(this->objInstanceName, material, light, image, name, scenegraph);
        return newclone;
    }

    /**
     * Visit this node.
     *
     */
    void accept(SGNodeVisitor* visitor) { visitor->visitLeafNode(this); }
};
} // namespace sgraph
#endif
