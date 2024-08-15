#pragma once

#include <SFML/Graphics.hpp>

namespace bmath {
    float max(float x, float y) {
        if (x > y) return x;
        return y;
    }

    float min(float x, float y) {
        if (x > y) return y;
        return x;
    }

    float abs(float x) {
        if (x < 0) return -x;
        return x;
    }

    /**
     * @brief Checks if the point q lines on the line segment pr
     * 
     * @param p line start
     * @param q point
     * @param r line end
     * @return true if q lies on pr
     * @return false if q doesn't
     */
    bool onSegment(sf::Vector2f p, sf::Vector2f q, sf::Vector2f r) {
        if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
            q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) {
                return true;
        }
        return false;
    }

    /**
     * @brief Find the orientation of the points p, q, and r
     * 
     * @param p point
     * @param q point
     * @param r point
     * @return int 0 if they're colinear, 1 if clockwise, 2 if counterclockwise
     */
    int orientation(sf::Vector2f p, sf::Vector2f q, sf::Vector2f r) {
        int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

        if (val == 0) return 0;
        return (val > 0)? 1 : 2;
    }
}