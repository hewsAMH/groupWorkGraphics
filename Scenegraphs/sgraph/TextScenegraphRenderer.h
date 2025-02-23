//Abigail Hews | Aidan Johansson | Assignment 3 | Feb 2025

#ifndef _TEXTSCENEGRAPHRENDERER_H_
#define _TEXTSCENEGRAPHRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <sstream>
#include <string>

namespace sgraph {
   //class for rendering a scenegraph to a string
    class TextScenegraphRenderer: public SGNodeVisitor {
        public:
            TextScenegraphRenderer() : currentLevel(0) {}

            //getter for the string output of the visitor
            string getOutput() {
                return output.str();
            }

            //visits each group node and adds its children to the output
            void visitGroupNode(GroupNode *groupNode) {
                //add indentation and node name
                addIndentedLine(groupNode->getName());
                
                //visit all children w/ increased indentation
                currentLevel++;
                for (int i = 0; i < groupNode->getChildren().size(); i++) {
                    groupNode->getChildren()[i]->accept(this);
                }
                currentLevel--;
            }

            //visits and adds leaf nodes
            void visitLeafNode(LeafNode *leafNode) {
                addIndentedLine(leafNode->getName());
            }

            //visits transform nodes +  all child nodes
            void visitTransformNode(TransformNode *transformNode) {
                addIndentedLine(transformNode->getName());
                
                //visits child w/ indentation
                currentLevel++;
                if (transformNode->getChildren().size() > 0) {
                    transformNode->getChildren()[0]->accept(this);
                }
                currentLevel--;
            }

            //handles all transform node types
            void visitScaleTransform(ScaleTransform *scaleNode) {
                visitTransformNode(scaleNode);
            }

            void visitTranslateTransform(TranslateTransform *translateNode) {
                visitTransformNode(translateNode);
            }

            void visitRotateTransform(RotateTransform *rotateNode) {
                visitTransformNode(rotateNode);
            }

        private:
            stringstream output;
            int currentLevel;

            //helper to add indentation and node name to output (formatting)
            void addIndentedLine(const string& nodeName) {
                //starting indentation
                for (int i = 0; i < currentLevel; i++) {
                    output << "   ";
                }
                //add node name
                output << "- " << nodeName << "\n";
            }
    };
}

#endif