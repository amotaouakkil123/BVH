// Name: Adam Motaouakkil
// ID: 260956145

#pragma once
#ifndef AXIS_H
#define AXIS_H

#include <string>
#include <vector>
#include <memory>
#include "GLSL.h"
#include "MatrixStack.h"

class Program;

class Axis
{
public:
    Axis();
    virtual ~Axis();
    void draw(const std::shared_ptr<Program> prog2, std::shared_ptr<MatrixStack> MV, std::shared_ptr<MatrixStack> P, GLuint vao) const;
private:
};

#endif
