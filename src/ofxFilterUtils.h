#pragma once

#include "ofMain.h"

void decomposeMat4x4(glm::mat4x4& _mat, glm::vec3& _outPosition, glm::quat& _outQuaternion);
void composeMat4x4(glm::vec3& _position, glm::quat& _quaternion, glm::mat4x4& _outMat);
glm::quat quatConvert(ofQuaternion& _q);
glm::vec3 getTranslation(glm::mat4x4& a);
glm::vec3 getXAxis(glm::mat4x4& a);
glm::vec3 getYAxis(glm::mat4x4& a);
glm::vec3 getZAxis(glm::mat4x4& a);