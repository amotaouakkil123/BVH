// Name: Adam Motaouakkil
// ID: 260956145

#include "Axis.h"
#include <iostream>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Axis::Axis()
{
}

Axis::~Axis()
{
}


void Axis::draw(const std::shared_ptr<Program> prog2, std::shared_ptr<MatrixStack> MV, std::shared_ptr<MatrixStack> P, GLuint vao) const
{
    prog2->bind();
    MV->pushMatrix();
    MV->scale(2);
    glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
    glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
    glUniform3f(prog2->getUniform("col"), 1, 0, 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 0, 2);
    MV->popMatrix();
    
    MV->pushMatrix();
    MV->scale(2);
    glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
    glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
    glUniform3f(prog2->getUniform("col"), 0, 1, 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 2, 2);
    MV->popMatrix();
    
    MV->pushMatrix();
    MV->scale(2);
    glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
    glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
    glUniform3f(prog2->getUniform("col"), 0, 0, 1);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 4, 2);
    MV->popMatrix();
    prog2->unbind();
}
