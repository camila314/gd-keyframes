#pragma once

#include <Cacao.hpp>
#include <stdlib.h>
#include "../SpacingController.h"

using cocos2d::extension::CCScale9Sprite;
using namespace cocos2d;
using Cacao::CacTextContainer;

constexpr float kControlPointRadius = 2.5;

typedef enum { // to be continued
    kCacNone,
    kCacBezier
} TouchType;

template<typename T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi) {
    return v < lo ? lo : hi < v ? hi : v;
}

class SpacingLayer;
class BezierEditor;
class ControlPoint;
class DurationTextDelegate;
class BezierCanvas;


class BezierCanvas : public CCDrawNode {
 public:
    void _cubicBezierStuff(const CCPoint& origin, const CCPoint& control1, const CCPoint& control2, const CCPoint& destination, unsigned int segments, ccColor4F color);
};

class ControlPoint : public CCDrawNode {
 public:
    static ControlPoint* create(BezierEditor* sl, int ctrl_point);
    void clampControl();
    inline BezierEditor* parent() {return reinterpret_cast<BezierEditor*>(getUserObject());}
    inline bool intersectsControl(CCPoint p) {return p.getDistance(getPosition()) <= kControlPointRadius;}
    void updateDraw(CCObject* sender);
};

class BezierEditor : public CCNode {
 public:
    bool init() override;
    CCPoint getTouchPos(CCPoint p);
    bool intersectsEditor(CCPoint p);
    void evalNewTouch(CCTouch*);
    void cacTouchMoved(CCTouch*, CCEvent*);
    void updateBezier();
    inline CCPoint normalize(CCPoint c1) {return (c1 + ccp(39.4, 31.1)) / ccp(78.8, 62.2);}
    inline CCPoint unnormalize(CCPoint c1) {return (c1 * ccp(78.8, 62.2)) - ccp(39.4, 31.1);}
    CAC_CREATE(BezierEditor);
 protected:
    SpacingLayer* m_spacingLayer;
    ControlPoint* m_control0;
    ControlPoint* m_control1;
    ControlPoint* m_control2;
    ControlPoint* m_control3;
    CCScale9Sprite* m_background;
    ControlPoint* m_selectedControl;
    BezierCanvas* m_bezierDrawer;
    friend class SpacingLayer;
};

class DurationTextDelegate : public TextInputDelegate {
 public:
    DurationTextDelegate(SpacingLayer* sl) {m_spacingLayer = sl;}
    void textChanged() override;
 protected:
    SpacingLayer* m_spacingLayer;
};

class SpacingLayer : public Cacao::CacAlertLayer {
 public:
    void onDurationChange(CCObject* sender);
    void onToggle(CCObject* sender);
    void onLeftArrow(CCObject* sender);
    void onRightArrow(CCObject* sender);
    void onClose() override;
    bool alertInit() override;
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void setDuration(float duration);
    void setSubdivide(bool enabled, unsigned divisions);
    void setBezierControls(CCPoint c1, CCPoint c2);

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
    CAC_CREATE(SpacingLayer, CCSize(400, 300));
 protected:
    CAC_PROPERTY(SpacingController*, controller);
    DurationTextDelegate* m_delegate;
    CacTextContainer* m_durationText;
    Slider* m_durationSlider;
    BezierEditor* m_bezierEditor;
    int m_currentTouch;
    TouchType m_currentTouchedObject;
    friend class DurationTextDelegate;

    CacTextContainer* m_subdivideText;
    CCLabelBMFont* m_subdivideLabel;
    CCMenuItemToggler* m_subdivideToggler;
    CCMenuItemSpriteExtra* m_subdivideLeftArrow;
    CCMenuItemSpriteExtra* m_subdivideRightArrow;
};
