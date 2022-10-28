// Name: Adam Motaouakkil
// ID: 260956145

#include "DAGNode.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "Axis.h"
#include <cmath>

using namespace std;

class Shape;

DAGNode::DAGNode()
{
}

DAGNode::~DAGNode()
{
}

void DAGNode::init()
{
}

void DAGNode::draw(const std::shared_ptr<Program> prog2,
                   const std::shared_ptr<Program> prog,
                   const std::shared_ptr<Shape> shape1,
                   const std::shared_ptr<Shape> shape2,
                   const std::shared_ptr<Shape> shape3,
                   const std::shared_ptr<MatrixStack> MV,
                   float* frameData, std::shared_ptr<MatrixStack> P,
                   const std::shared_ptr<Axis> axis,
                   GLuint vao,
                   int frameI,
                   int debugger) const
{
    // Position and rotation intializations.
    
    float xPos, yPos, zPos, xRot, yRot, zRot = 0;
    
    // Model offsets.
    float xOff = offset.x * 0.01f;
    float yOff = offset.y * 0.01f;
    float zOff = offset.z * 0.01f;
    
    MV->pushMatrix();
    
    // Just draw the frames first.
    MV->translate(glm::vec3(xOff, yOff, zOff));
    if (channels.size() == 6)
    {
        // Populate the necessary channel data.
        xPos = frameData[ frameI + channelDataStartIndex ] * 0.01f;
        yPos = frameData[ frameI + channelDataStartIndex + 1 ] * 0.01f;
        zPos = frameData[ frameI + channelDataStartIndex + 2 ] * 0.01f;
        
        // Apply the transformations per frame.
        MV->translate(xPos, yPos, zPos);
        
        if (channels.at(3).compare("Zrotation") == 0)
        {
            zRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 3 ]);
            xRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 4 ]);
            yRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 5 ]);
            MV->rotate(zRot, 0, 0, 1);
            MV->rotate(xRot, 1, 0, 0);
            MV->rotate(yRot, 0, 1, 0);
        }
        else
        {
            xRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 3 ]);
            yRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 4 ]);
            zRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 5 ]);
            MV->rotate(xRot, 1, 0, 0);
            MV->rotate(yRot, 0, 1, 0);
            MV->rotate(zRot, 0, 0, 1);
        }
    }
    else
    {
        if (channels.at(0).compare("Zrotation") == 0)
        {
            zRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex ]);
            xRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 1 ]);
            yRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 2 ]);
            MV->rotate(zRot, 0, 0, 1);
            MV->rotate(xRot, 1, 0, 0);
            MV->rotate(yRot, 0, 1, 0);
        }
        else
        {
            xRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex ]);
            yRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 1 ]);
            zRot = (float) glm::radians(frameData[ frameI + channelDataStartIndex + 2 ]);
            MV->rotate(xRot, 1, 0, 0);
            MV->rotate(yRot, 0, 1, 0);
            MV->rotate(zRot, 0, 0, 1);
        }
    }
    
    // Draw the axis
    axis->draw(prog2, MV, P, vao);
    
    // Prepare shape drawing
    prog->bind();
    MV->pushMatrix();
    
    // Draw different shapes
    if ( name.compare("Head") == 0)
    {
        MV->scale(0.08f);
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
        shape2->draw(prog);
        MV->popMatrix();
        prog->unbind();
        
    }
    else
    {
        MV->scale(0.03f);
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
        shape3->draw(prog);
        MV->popMatrix();
        prog->unbind();
    }
    
    // Modify the offsets so that you get them for later frames.
    for (int i = 0; i<children.size(); i++)
    {
        if (children.at(i)->name.compare("EndSite") != 0)
        {
            children.at(i)->draw(prog2, prog, shape1, shape2, shape3,
                                 MV, frameData, P, axis,
                                 vao, frameI, debugger);
        }
    }
    MV->popMatrix();
}
