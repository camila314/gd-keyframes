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
    inline CCPoint normalize(CCPoint c1) {return ccp( (c1.x/(m_background->getContentSize().width/4 - kControlPointRadius*2)) + 0.51285, (c1.y/(m_background->getContentSize().height/4 - kControlPointRadius*2)) + 0.51634 ) + ccp(0.5,0.5);}
    inline CCPoint unnormalize(CCPoint c1) {return ccp(((c1.x - 0.51285) - 0.5) * (m_background->getContentSize().width/4 - kControlPointRadius*2),((c1.y - 0.51634) - 0.5) * (m_background->getContentSize().height/4 - kControlPointRadius*2));}
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
    void onClose() override;
    bool alertInit() override;
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void setDuration(float duration);
    void setBezierControls(CCPoint c1, CCPoint c2);
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
};
