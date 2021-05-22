#pragma once

#include "../offsets.hpp"
#include "BezierCanvas.hpp"
#include "ControlPoint.hpp"
#include "SpacingLayer.hpp"

class ControlPoint;
class BezierCanvas;
class SpacingLayer;

class BezierEditor : public cocos2d::CCNode {
    public:
        bool init() override;

        cocos2d::CCPoint getTouchPos(cocos2d::CCPoint const& p);
        bool intersectsEditor(cocos2d::CCPoint const& p);
        void evalNewTouch(cocos2d::CCTouch*);
        void cacTouchMoved(cocos2d::CCTouch*, cocos2d::CCEvent*);
        void updateBezier();

        inline cocos2d::CCPoint normalize(cocos2d::CCPoint c1) {
            return (c1 + ccp(39.4, 31.1)) / ccp(78.8, 62.2);
        }
        inline cocos2d::CCPoint unnormalize(cocos2d::CCPoint c1) {
            return (c1 * ccp(78.8, 62.2)) - ccp(39.4, 31.1);
        }

        MK_CLASS_CREATE(BezierEditor);

 protected:
    SpacingLayer* m_spacingLayer;
    ControlPoint* m_control0;
    ControlPoint* m_control1;
    ControlPoint* m_control2;
    ControlPoint* m_control3;
    cocos2d::extension::CCScale9Sprite* m_background;
    ControlPoint* m_selectedControl;
    BezierCanvas* m_bezierDrawer;
    friend class SpacingLayer;
};
