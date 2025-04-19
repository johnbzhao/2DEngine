#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream> // 添加日志输出

class Camera {
public:
    glm::vec3 position;
    float zoom;

    Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f), float startZoom = 1.0f)
        : position(startPosition), zoom(startZoom) {}

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        float left = -aspectRatio * zoom;
        float right = aspectRatio * zoom;
        float bottom = -1.0f * zoom;
        float top = 1.0f * zoom;

        // 正交投影矩阵
        glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

        // 视图变换矩阵（根据摄像机位置平移）
        glm::mat4 view = glm::translate(glm::mat4(1.0f), -position);

        // 返回投影矩阵和视图矩阵的组合
        return projection * view;
    }

    void move(glm::vec3 offset) {
        position += offset;
    }

    void setZoom(float zoomLevel) {
        // 限制 zoom 的范围在 0.1 到 10.0 之间
        zoom = glm::clamp(zoomLevel, 0.1f, 20.0f);
    }

    void adjustZoomToFit(float maxWidth, float maxHeight) {
        if (maxWidth <= 0 || maxHeight <= 0) {
            return;
        }
        zoom = glm::clamp(glm::max(maxWidth, maxHeight), 0.1f, 20.0f);
    }
};

#endif