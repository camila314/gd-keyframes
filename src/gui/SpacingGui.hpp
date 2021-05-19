#pragma once

#include "../offsets.hpp"
#include "../nodes/BrownAlertDelegate.hpp"
#include "../nodes/InputNode.hpp"
#include <stdlib.h>
#include "../kfnodes/SpacingController.hpp"

constexpr float kControlPointRadius = 2.5;

enum TouchType { // to be continued
    kCacNone,
    kCacBezier
};

template<typename T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi) {
    return v < lo ? lo : hi < v ? hi : v;
}

class SpacingLayer;
class BezierEditor;
class ControlPoint;
class DurationTextDelegate;
class BezierCanvas;

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

class BezierEditor : public cocos2d::CCNode {
    public:
        bool init() override;

        cocos2d::CCPoint getTouchPos(cocos2d::CCPoint p);
        bool intersectsEditor(cocos2d::CCPoint p);
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

class DurationTextDelegate : public gd::TextInputDelegate {
    protected:
        SpacingLayer* m_spacingLayer;

    public:
        DurationTextDelegate(SpacingLayer* sl) {m_spacingLayer = sl;}
        void textChanged(gd::CCTextInputNode*) override;
};

class SpacingLayer : public BrownAlertDelegate {
 protected:
    CAC_PROPERTY(SpacingController*, controller);
    DurationTextDelegate* m_delegate;
    InputNode* m_durationText;
    gd::Slider* m_durationSlider;
    BezierEditor* m_bezierEditor;
    int m_currentTouch;
    TouchType m_currentTouchedObject;
    friend class DurationTextDelegate;

    InputNode* m_subdivideText;
    cocos2d::CCLabelBMFont* m_subdivideLabel;
    gd::CCMenuItemToggler* m_subdivideToggler;
    gd::CCMenuItemSpriteExtra* m_subdivideLeftArrow;
    gd::CCMenuItemSpriteExtra* m_subdivideRightArrow;

 public:
    void onDurationChange(CCObject* sender);
    void onToggle(CCObject* sender);
    void onLeftArrow(CCObject* sender);
    void onRightArrow(CCObject* sender);
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void setDuration(float duration);
    void setSubdivide(bool enabled, unsigned divisions);
    void setBezierControls(cocos2d::CCPoint c1, cocos2d::CCPoint c2);

    inline void hideSubdivide() {
        m_subdivideText->setVisible(false);
        m_subdivideLabel->setVisible(false);
        m_subdivideRightArrow->setVisible(false);
        m_subdivideLeftArrow->setVisible(false);
    }

    inline void showSubdivide() {
        m_subdivideText->setVisible(true);
        //m_subdivideLabel->setVisible(true);
        m_subdivideRightArrow->setVisible(true);
        m_subdivideLeftArrow->setVisible(true);
    }

    MK_CLASS_CREATE(SpacingLayer, CCSize(400, 300));
};