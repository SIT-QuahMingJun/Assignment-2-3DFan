#define LOG_TAG "Fan3D" // For Android if needed

#include <glm/gtc/type_ptr.hpp>
#include "Fan.h"
#include "ShaderHelper.h"
#include <cmath>
#include <algorithm>

namespace {

    // Vertex
    constexpr GLfloat kPositions[] = {
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f
    };

    // Shades
    constexpr GLfloat kShades[] = {
        1.00f, 1.00f, 1.00f,
        0.95f, 0.95f, 0.95f,
        0.85f, 0.85f, 0.85f,
        0.55f, 0.55f, 0.55f,
        0.70f, 0.70f, 0.70f,
        0.65f, 0.65f, 0.65f,
        0.60f, 0.60f, 0.60f,
        0.60f, 0.60f, 0.60f
    };

    // Index
    constexpr GLushort kIndices[] = {
        0, 3, 1,  3, 2, 1,
        7, 4, 6,  4, 5, 6,
        4, 0, 5,  0, 1, 5,
        3, 7, 2,  7, 6, 2,
        1, 2, 5,  2, 6, 5,
        3, 0, 7,  0, 4, 7
    };

}

#ifdef PLATFORM_ANDROID
    Fan::Fan(AAssetManager* assetMgr): mgr(assetMgr) {}
#else
    Fan::Fan() = default;
#endif

Fan::~Fan()
{
    glDeleteBuffers(1, &idx);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}

void Fan::TouchEventDown(float x, float y)
{
    lastX = x; lastY = y;
    movedDistance = 0.0f;
    dragBoost = 0.0f;
    lastMoveTime = std::chrono::steady_clock::now();
    this->dragging = true;
}

void Fan::TouchEventMove(float x, float y)
{
    // Calculate dx, dy, distance, elapsed time and drag velocity.
    const float dx = x - lastX, dy = y - lastY;

    // Calculate Swipe Speed
    float dist = std::sqrt(dx*dx + dy*dy);
    this->movedDistance += dist;

    float dtMs = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - this->lastMoveTime).count();   // Delta time

    // Just in case dt = 0, prevent in case dtMs == 0 rofl and division_by_zero error
    if (dtMs > 0.0f){
        float velocity = dist / dtMs; // pixels per millisecond
        this->dragBoost = std::min(velocity * kBoostScale, kMaxBoost);  // More pixels per ms if swipe is large, limited to kMaxBoost
        LOGI("Drag velocity %.3f px/ms, boost %.3f", velocity, dragBoost);
    }
    
    // Update current variables
    this->lastX = x;
    this->lastY = y;
    this->lastMoveTime = std::chrono::steady_clock::now();
}

void Fan::TouchEventRelease(float /*x*/, float /*y*/)
{
    // Toggle fan if movement was below the tap threshold.
    if (this->movedDistance < this->kTapThreshold){
        fanOn = !fanOn;
        LOGI("Fan %s", fanOn ? "ON" : "OFF");
    }
    this->dragging = false;
}

void Fan::InitModel(){
    #ifdef PLATFORM_ANDROID
    this->programID = ShaderHelper::buildProgramFromAssets(mgr, "shader/FanVertex.glsl", "shader/FanFragment.glsl");
    #else
    this->programID = ShaderHelper::buildProgramFromFile("FanVertex.glsl", "FanFragment.glsl");
    #endif
    
    if (programID == 0) {
        LOGE("Fan: Failed to create shader programs... GG");
        return;
    }

    uMODELVIEWPROJECTIONMATRIX = glGetUniformLocation(programID, "MODELVIEWPROJECTIONMATRIX");
    uPartColor = glGetUniformLocation(programID, "PARTCOLOR");
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    constexpr GLsizeiptr positionBytes = sizeof(kPositions);
    constexpr GLsizeiptr shadeBytes = sizeof(kShades);
    constexpr GLsizeiptr totalBytes = positionBytes + shadeBytes;

    // Allocate the whole VBO without supplying initial data.
    glBufferData(GL_ARRAY_BUFFER, totalBytes, nullptr, GL_STATIC_DRAW);

    // Upload array buffer
    glBufferSubData(GL_ARRAY_BUFFER,0,positionBytes,kPositions);

    // Upload shades color
    glBufferSubData(GL_ARRAY_BUFFER, positionBytes, shadeBytes, kShades);

    // Enable attribute=0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void*>(0));

    // Enable attribute=1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const void*>(positionBytes));

    // Bind indices
    glGenBuffers(1, &idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);

    // BIND VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    transform.TransformInit();

    LOGI("Fan::InitModel done");
}

void Fan::Resize(int w, int h)
{
    if (h <= 0) h = 1;

    glViewport(0, 0, w, h);
    const float aspect = static_cast<float>(w) / static_cast<float>(h);

    transform.TransformSetMatrixMode(PROJECTION_MATRIX);
    transform.TransformLoadIdentity();
    transform.TransformSetPerspective(glm::radians(60.0f), aspect, 0.01f, 1000.0f, 0.0f);   // FOV 60 DEG, ASPECT: WIDTH / HEIGHT/ NEAR = 0.01, FAR = 1000, VIEW=I
    
    transform.TransformSetMatrixMode(VIEW_MATRIX);
    transform.TransformLoadIdentity();
}

void Fan::drawCube(const glm::vec3& colour)
{
    const glm::mat4* mvp = transform.TransformGetModelViewProjectionMatrix();   // Get mvp
    glUniformMatrix4fv(uMODELVIEWPROJECTIONMATRIX, 1, GL_FALSE, glm::value_ptr(*mvp));  // Update mvp
    glUniform3fv(uPartColor, 1, glm::value_ptr(colour));    // Update color
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);   // Draw elements
}

void Fan::Render()
{
    if (programID == 0 || vao == 0) return;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    glBindVertexArray(vao);

    transform.TransformSetMatrixMode(MODEL_MATRIX);
    transform.TransformLoadIdentity();

    // Shared world transform.
    transform.TransformTranslate(0.0f, 0.8f, -8.0f);
    transform.TransformRotate(glm::radians(20.0f), 0.0f, 1.0f, 0.0f);

    // Base.
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.0f, -2.6f, 0.0f);
    transform.TransformScale(1.6f, 0.25f, 0.8f);
    drawCube(glm::vec3(0.45f, 0.28f, 0.12f));   // Brown
    transform.TransformPopMatrix();

    // Pole.
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.0f, -1.21f, 0.0f);
    transform.TransformScale(0.15f, 2.53f, 0.15f);
    drawCube(glm::vec3(0.55f, 0.55f, 0.58f));   // Grey
    transform.TransformPopMatrix();

    // Hub.
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.0f, 0.2f, 0.0f);
    transform.TransformScale(0.3f, 0.3f, 0.3f);
    drawCube(glm::vec3(0.20f, 0.20f, 0.22f));   // Dark Grey
    transform.TransformPopMatrix();

    // Note, max 5 blades
    const glm::vec3 bladeColours[] = {
        {1.0f, 0.0f, 0.0f},  // red
        {0.0f, 0.0f, 1.0f},  // blue
        {1.0f, 0.5f, 0.0f},  // orange
        {0.0f, 1.0f, 0.0f},   // green
        {1.0f, 1.0f, 0.0f}   // yellow
    };

    // Animating the blades
    if (this->fanOn) spinAngle+= (kBaseSpeed + dragBoost);
    if (this->spinAngle >= 360.0f) spinAngle -=360.0f;  // Ensure spinAngle does not exceed 360 deg

    // Decaying
    if (this->dragging == false && this->dragBoost > 0.0f) {
        this->dragBoost *= 0.98f;
        if (dragBoost < 0.01f) dragBoost = 0.0f;    // Prevent almost never stopping fan
    }

    const float deltaAngle = 360.0f / static_cast<float>(bladeCount);

    // For i in 0..3
    for (int i = 0; i < this->bladeCount; ++i) {
        transform.TransformPushMatrix();
        transform.TransformTranslate(0.0f, 0.2f, 0.15f);
        transform.TransformRotate(glm::radians(spinAngle + i * deltaAngle), 0.0f, 0.0f, 1.0f);
        transform.TransformTranslate(0.0f, 0.55f, 0.0f);
        transform.TransformScale(0.22f, 0.8f, 0.05f);
        drawCube(bladeColours[i]);
        transform.TransformPopMatrix();
    }

    glBindVertexArray(0);
    glUseProgram(0);
}


