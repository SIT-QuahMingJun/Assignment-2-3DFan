/**
 * @file    Renderer.cpp
 * @brief   Implements the shared scene renderer (singleton pattern).
 */

#define LOG_TAG "Renderer"

#include "Platform.h"
#include "Renderer.h"
#include "Fan.h"

Renderer& Renderer::Instance()
{
    static Renderer instance;
    return instance;
}

Renderer::~Renderer()
{
    clearModels();
}

#ifdef PLATFORM_ANDROID
void Renderer::setAssetManager(AAssetManager* mgr)
{
    this->assetMgr = mgr;
}
#endif

bool Renderer::initializeRenderer() {
    #ifdef PLATFORM_ANDROID
        if (this->assetMgr == nullptr) return false;
    #endif

    // Create and init fan model.
    createModels();
    initializeModels();
    return true;
}

void Renderer::resize(int w, int h)
{
    for (Model* model : models) model->Resize(w, h);
}

void Renderer::render()
{
    for (Model* model : models) model->Render();
}

void Renderer::touchEventDown(float x, float y)
{
    for (Model* model : models) model->TouchEventDown(x, y);
}

void Renderer::touchEventMove(float x, float y)
{
    for (Model* model : models) model->TouchEventMove(x, y);
}

void Renderer::touchEventRelease(float x, float y)
{
    for (Model* model : models) model->TouchEventRelease(x, y);
}

void Renderer::createModels()
{
    clearModels();

#ifdef PLATFORM_ANDROID
    models.push_back(new Fan(assetMgr));
#else
    models.push_back(new Fan());
#endif
}

void Renderer::initializeModels() {
    for (Model* model : models) model->InitModel();
}

void Renderer::clearModels()
{
    for (Model* model : this->models) {
        delete model;
    }

    models.clear();
}
