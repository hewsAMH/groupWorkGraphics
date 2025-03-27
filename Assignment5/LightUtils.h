#ifndef __LIGHT_UTILS_H__
#define __LIGHT_UTILS_H__

#include "../include/Light.h"
#include <glm/glm.hpp>

namespace util
{

    // Function to transform a light to a new coordinate system
    inline Light transformLight(const Light &light, const glm::mat4 &matrix)
    {
        Light result = light;

        // Transform position
        glm::vec4 pos = light.getPosition();
        if (pos.w != 0)
        { // Point or spot light
            glm::vec4 transformedPos = matrix * pos;
            result.setPosition(transformedPos);
        }
        else
        { // Directional light
            // For directional lights, transform direction using normal matrix
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(matrix)));
            glm::vec3 transformedDir = normalMatrix * glm::vec3(pos);
            result.setDirection(transformedDir.x, transformedDir.y, transformedDir.z);
        }

        // Transform spotlight direction if applicable
        if (light.getSpotCutoff() > 0)
        {
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(matrix)));
            glm::vec3 transformedSpotDir = normalMatrix * glm::vec3(light.getSpotDirection());
            result.setSpotDirection(transformedSpotDir.x, transformedSpotDir.y, transformedSpotDir.z);
        }

        return result;
    }

    // Helper function to determine if a light is a spotlight
    inline bool isSpotlight(const Light &light)
    {
        return light.getSpotCutoff() > 0;
    }

    // Helper function to determine if a light is directional
    inline bool isDirectional(const Light &light)
    {
        return light.getPosition().w == 0;
    }

} // namespace util

#endif // __LIGHT_UTILS_H__