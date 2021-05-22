#pragma once

#include "../offsets.hpp"

class BezierCanvas : public cocos2d::CCDrawNode {
    public:
        void _cubicBezierStuff(
            const cocos2d::CCPoint& origin,
            const cocos2d::CCPoint& control1,
            const cocos2d::CCPoint& control2,
            const cocos2d::CCPoint& destination,
            unsigned int segments,
            cocos2d::ccColor4F color
        );
};
