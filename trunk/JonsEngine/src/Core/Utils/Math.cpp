#include "include/Core/Utils/Math.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace JonsEngine
{
    Mat4 CreatePerspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar)
    {
        return glm::perspective(fov, aspect, zNear, zFar);
    }

}