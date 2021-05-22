#pragma once

#include "../offsets.hpp"
#include "../nodes/BrownAlertDelegate.hpp"
#include "../nodes/InputNode.hpp"
#include <stdlib.h>
#include "../kfnodes/SpacingController.hpp"
#include "BezierCanvas.hpp"

enum TouchType { // to be continued
    kCacNone,
    kCacBezier
};

class SpacingLayer;
class BezierEditor;
class ControlPoint;
class DurationTextDelegate;
class BezierCanvas;

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

    void setup() override;
    void onClose(cocos2d::CCObject*) override;

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

    MK_CLASS_CREATE(SpacingLayer, 400.0f, 275.0f);
};