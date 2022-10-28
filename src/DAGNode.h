// Name: Adam Motaouakkil
// ID: 260956145

#pragma once
#ifndef DAGNODE_H
#define DAGNODE_H

#include <string>
#include <vector>
#include <memory>
#include "GLSL.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Program;
class MatrixStack;
class Axis;
class Shape;

/**
 * This class manages nodes for a character skeleton specified by a bvh file.
 * TODO: add to this class as needed!
 */
class DAGNode
{
public:
	DAGNode();
	virtual ~DAGNode();	
	void init();
	// Draws the node and its children.  TODO: modify this method signature if needed!
	void draw(const std::shared_ptr<Program> prog2,
              const std::shared_ptr<Program> prog,
              const std::shared_ptr<Shape> shape1,
              const std::shared_ptr<Shape> shape2,
              const std::shared_ptr<Shape> shape3,
              const std::shared_ptr<MatrixStack> MV,
              float* frameData, std::shared_ptr<MatrixStack> P,
              std::shared_ptr<Axis> axis,
              GLuint vao,
              int frameI,
              int debugger) const;

	// name of this node (useful for debugging)
	std::string name;
	// translational offset of this node from its parent
	glm::fvec3 offset;
	// channel names, each will be one of Xposition Yposition Zposition Xrotation Yrotation Zrotation
	std::vector<std::string> channels;
	// parent of this node, or NULL if this node is the root
	DAGNode* parent;
	// children of this node
	std::vector <DAGNode*> children;
	// start index from which this node's channel data can be read from a frame of motion capture data
	int channelDataStartIndex;

private:
};

#endif
