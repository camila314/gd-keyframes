#include "SpacingGui.h"

void BezierCanvas::_cubicBezierStuff(const CCPoint& origin, const CCPoint& control1, const CCPoint& control2, const CCPoint& destination, unsigned int segments, ccColor4F color) {
    unsigned vertex_count = segments+2;
    CCPoint* vertices = new CCPoint[vertex_count];
    vertices[0] = vertices[1] = origin;

    float t = 0;
    for(unsigned int i = 1; i <= segments; i++) {
        if (i==segments) {
            vertices[i].x = destination.x;
            vertices[i].y = destination.y;
        } else {
            vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
            vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
            t += 1.0f / segments;
        }
        drawSegment(vertices[i-1], vertices[i], 3, color);
    }

    CC_SAFE_DELETE_ARRAY(vertices);
}