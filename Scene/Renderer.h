#pragma once

/**
 * @file    Renderer.h
 * @brief   Controls and manages the models in the scene.
 */

#include "Model.h"

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

#include <vector>

class Renderer {
public:
    static Renderer& Instance();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

#ifdef PLATFORM_ANDROID
    void setAssetManager(AAssetManager* mgr);
#endif

    bool initializeRenderer();
    void resize(int w, int h);
    void render();

    void touchEventDown(float x, float y);
    void touchEventMove(float x, float y);
    void touchEventRelease(float x, float y);

private:
    Renderer() = default;
    ~Renderer();

    void createModels();
    void initializeModels();
    void clearModels();

#ifdef PLATFORM_ANDROID
    AAssetManager* assetMgr = nullptr;
#endif

    std::vector<Model*> models;
};
