#pragma once

/**
 * @file    Model.h
 * @brief   Base interface for every renderable model.
 */
class Model {
public:
    Model() = default;
    virtual ~Model() = default;

    virtual void InitModel() = 0;
    virtual void Render() = 0;
    virtual void Resize(int w, int h) {}

    // Touch Interface
    virtual void TouchEventDown(float x, float y) {}
    virtual void TouchEventMove(float x, float y) {}
    virtual void TouchEventRelease(float x, float y) {}
};
