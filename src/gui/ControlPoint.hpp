#pragma once

#include "../offsets.hpp"
#include "BezierEditor.hpp"

constexpr float kControlPointRadius = 2.5f;

class BezierEditor;

class ControlPoint : public cocos2d::CCDrawNode {
    public:
        static ControlPoint* create(BezierEditor* sl, int ctrl_point);
        void clampControl();
        inline BezierEditor* parent() {return reinterpret_cast<BezierEditor*>(getUserObject());}
        inline bool intersectsControl(cocos2d::CCPoint p) {
            return p.getDistance(getPosition()) <= kControlPointRadius;
        }
        void updateDraw(CCObject* sender);
};

