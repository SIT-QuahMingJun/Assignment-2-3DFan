#pragma once

#include "Platform.h"
#include "Model.h"
#include "Transform.h"
#include <chrono>

// For android
#ifdef PLATFORM_ANDROID
    #include <android/asset_manager.h>
#endif

class Fan : public Model {
    public:

        #ifdef PLATFORM_ANDROID
            explicit Fan(AAssetManager* assetMgr);
        #else
            Fan();
        #endif

        ~Fan() override;


        void InitModel() override;
        void Render() override;
        void Resize(int w, int h) override;

        void TouchEventDown(float x, float y) override;
        void TouchEventMove(float x, float y) override;
        void TouchEventRelease(float x, float y) override;
    
    private:
        Transform transform; // provided matrix stack (model/view/projection)

        // Program, VAO
        GLuint programID{}, vao{}, vbo{}, idx{};
        
        // Uniform locations
        GLint uMODELVIEWPROJECTIONMATRIX{-1}, uPartColor{-1};

        // Properties
        float spinAngle = 0.0f; // current blade angle (degrees)
        bool fanOn = true; // toggled by a tap
        float dragBoost = 0.0f; // extra speed while dragging, 0 otherwise
        static constexpr float kBaseSpeed = 1.5f; // deg/frame when ON
        static constexpr float kMaxBoost = 20.0f; // clamp for the drag boost
        static constexpr float kBoostScale = 8.0f; // px/ms -> deg/frame
        static constexpr float kTapThreshold = 12.0f; // px: below this it's a tap
        float lastX{}, lastY{}; // previous touch position
        float movedDistance{}; // total movement since touch-down
        bool dragging{false};    // drag
        int bladeCount{4};    // bladecount
        std::chrono::steady_clock::time_point lastMoveTime{};



        #ifdef PLATFORM_ANDROID
            AAssetManager* mgr = nullptr;
        #endif
        
        // Helper function
        void drawCube(const glm::vec3& colour);
};

