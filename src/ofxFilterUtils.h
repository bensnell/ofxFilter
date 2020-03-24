#pragma once

#include "ofMain.h"

void decomposeMat4x4(glm::mat4x4& _mat, glm::vec3& _outPosition, glm::quat& _outQuaternion);
void composeMat4x4(glm::vec3& _position, glm::quat& _quaternion, glm::mat4x4& _outMat);
glm::quat quatConvert(ofQuaternion& _q);

glm::vec3 getTranslation(glm::mat4x4& m);
glm::quat getRotation(glm::mat4x4& m);
glm::vec3 getScale(glm::mat4x4& m);

glm::vec3 getXAxis(glm::mat4x4& a);
glm::vec3 getYAxis(glm::mat4x4& a);
glm::vec3 getZAxis(glm::mat4x4& a);

// Calculate a euler angle, wrapped to the correct dimension,
// given the previous reference euler angle.
// For example, if the previous pitch (refEuler[1]) is -175, and the next
// pitch (euler[1]) is 175, we would assume that the rotation
// took the shortest path possible, in which case the "real" 
// pitch (to output) would be -185.
glm::vec3 getEulerWarped(glm::vec3 euler, glm::vec3 refEuler);