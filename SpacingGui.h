
#include <Cacao.hpp>
#include <stdlib.h>

using cocos2d::extension::CCScale9Sprite;
using namespace cocos2d;
using Cacao::CacTextContainer;

typedef enum {
    kCacNone,
    kCacBezier
} TouchType;

typedef struct {
    float c1_x;
    float c1_y;
    float c2_x;
    float c2_y;
} BezierData;

template<typename T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi)
{
    return v < lo ? lo : hi < v ? hi : v;
}

CCPoint clamp_to_bezier_space(double x, double y) {
    return ccp(clamp(x, -41.9, 41.9), clamp(y, -33.6, 33.6));
}

CCPoint clamp_to_bezier_space(CCPoint p) {
    return clamp_to_bezier_space(p.x, p.y);
}

class SpacingLayer;
class BezierEditor;

constexpr float kControlPointRadius = 2.5;

class BezierCanvas : public CCDrawNode {
 public:
    void _cubicBezierStuff(const CCPoint& origin, const CCPoint& control1, const CCPoint& control2, const CCPoint& destination, unsigned int segments, ccColor4F color) {
        unsigned vertex_count = segments+2;
        CCPoint* vertices = new CCPoint[vertex_count];
        vertices[0] = vertices[1] = origin;

        float t = 0;
        for(unsigned int i = 1; i <= segments; i++)
        {
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

        //drawPolygon(vertices, vertex_count, ccc4FFromccc4B(ccc4(0,0,0,0)), 3, color);
        CC_SAFE_DELETE_ARRAY(vertices);
    }
};



class ControlPoint : public CCDrawNode {
 public:
    static ControlPoint* create(BezierEditor* sl, int ctrl_point) {
        auto pRet = reinterpret_cast<ControlPoint*>(CCDrawNode::create());
        if (pRet) {
            pRet->setUserObject(reinterpret_cast<CCObject*>(sl));
            pRet->setTag(ctrl_point);
            pRet->schedule(schedule_selector(ControlPoint::updateDraw), 0);
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return NULL;
    }

    inline void clampFurther() {
        setPosition(ccp(clamp((double)getPositionX(), -41.9 + kControlPointRadius, 41.9 - kControlPointRadius), clamp((double)getPositionY(), -33.6 + kControlPointRadius, 33.6 - kControlPointRadius)));
    }
    inline BezierEditor* parent() {return reinterpret_cast<BezierEditor*>(getUserObject());}
    inline bool intersectsControl(CCPoint p) {
        return p.getDistance(getPosition()) <= kControlPointRadius;
    }
    void updateDraw(CCObject* sender);
};

class BezierEditor : public CCNode {
 public:
    bool init() override {

        m_background = CCScale9Sprite::create("square02b_small.png");
        m_background->setColor(ccc3(45,45,60));
        m_background->setContentSize(CCSizeMake(175.6,142.4));
        addChild(m_background);


        m_bezierDrawer = reinterpret_cast<BezierCanvas*>(CCDrawNode::create());
        addChild(m_bezierDrawer);

        m_control0 = ControlPoint::create(this, 0);
        m_control0->setPosition(clamp_to_bezier_space(-100,-100) + ccp(kControlPointRadius, kControlPointRadius));
        addChild(m_control0);

        m_control3 = ControlPoint::create(this, 3);
        m_control3->setPosition(clamp_to_bezier_space(100,100) - ccp(kControlPointRadius, kControlPointRadius));
        addChild(m_control3);

        m_control2 = ControlPoint::create(this, 2);
        m_control2->setPosition(clamp_to_bezier_space(100,100) - ccp(kControlPointRadius + 30, kControlPointRadius));
        addChild(m_control2);

        m_control1 = ControlPoint::create(this, 1);
        m_control1->setPosition(clamp_to_bezier_space(-100,-100) + ccp(kControlPointRadius + 30, kControlPointRadius));
        addChild(m_control1);

        updateBezier();

        return true;
    }

    CCPoint getTouchPos(CCPoint p) {
        auto nodespace = ccp(p.x, CCDirector::sharedDirector()->getWinSize().height-p.y);
        nodespace = nodespace - getPosition();
        nodespace = ccp(nodespace.x/2, nodespace.y/2);
        return nodespace;
    }


    bool intersectsEditor(CCPoint p) {
        auto nodespace = ccp(p.x, CCDirector::sharedDirector()->getWinSize().height-p.y);
        nodespace = nodespace - getPosition();
        nodespace = ccp(nodespace.x/2, nodespace.y/2);

        auto clamped = clamp_to_bezier_space(nodespace);
        auto difference = nodespace - clamped;
        if (fabs(difference.x) < 1 && fabs(difference.y) < 1) {
            return true;
        }
        return false;
    }

    void evalNewTouch(CCTouch*);
    void cacTouchMoved(CCTouch*, CCEvent*);
    void updateBezier();
    CAC_CREATE(BezierEditor);
 public:
    SpacingLayer* m_spacingLayer;
    ControlPoint* m_control0;
    ControlPoint* m_control1;
    ControlPoint* m_control2;
    ControlPoint* m_control3;

    CCScale9Sprite* m_background;
    ControlPoint* m_selectedControl;
    BezierCanvas* m_bezierDrawer;
};
class DurationTextDelegate : public TextInputDelegate {
 public:
    DurationTextDelegate(SpacingLayer* sl) {
        m_spacingLayer = sl;
    }
    void textChanged() override;
 protected:
    SpacingLayer* m_spacingLayer;
};

class SpacingLayer : public Cacao::CacAlertLayer {
 public:
    void onDurationChange(CCObject* sender) {
        float newscale = m_durationSlider->getValue();
        
        char ok[16];
        sprintf(ok, "%.3f", newscale*10);
        m_durationText->text(ok);
    }
    bool alertInit() override {
        m_noElasticity = true;
        m_closeButton->setVisible(false);
        title("Keyframe Spacing");
        m_title->setPosition(m_title->getPosition() + ccp(1, 10));

        m_durationSlider = Slider::create(this, menu_selector(SpacingLayer::onDurationChange), 0.75);
        m_durationSlider->setBarVisibility(false);
        m_durationSlider->setPosition(Cacao::addedPosition(-95.13, 48));
        m_mainLayer->addChild(m_durationSlider);

        m_delegate = new DurationTextDelegate(this);
        m_durationText = CacTextContainer::create(CCSizeMake(60, 30), m_delegate, "bigFont.fnt")
                         ->charLimit(6)
                         ->allowedChars("0123456789.ei+")
                         ->placeholder("Num");
        m_durationText->setPosition(Cacao::addedPosition(-51.2, 82.34));
        m_mainLayer->addChild(m_durationText);

        auto durationLabel = CCLabelBMFont::create("Duration:", "goldFont.fnt");
        durationLabel->setPosition(Cacao::addedPosition(-113.6 - 28.2, 82.34));
        durationLabel->setScale(0.75);
        m_mainLayer->addChild(durationLabel);

        m_bezierEditor = BezierEditor::create();
        m_bezierEditor->setPosition(Cacao::addedPosition(61.2 + 31.2, 78.34 - 33.2));
        m_mainLayer->addChild(m_bezierEditor);
        return true;
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        m_currentTouch = touch->getID();

        if (m_bezierEditor->intersectsEditor(convertToNodeSpace(touch->getLocation()))) {
            m_bezierEditor->m_selectedControl = nullptr;
            m_bezierEditor->evalNewTouch(touch);
            m_currentTouchedObject = kCacBezier;
        } else {
            m_currentTouchedObject = kCacNone;
        }
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        if (touch->getID()!=m_currentTouch)
            return;
        switch (m_currentTouchedObject) {
            case kCacBezier:
                m_bezierEditor->cacTouchMoved(touch, event);
                break;
            default:
                break;
        }
    }
    CAC_CREATE(SpacingLayer, CCSize(400, 300));
 protected:
    DurationTextDelegate* m_delegate;
    CacTextContainer* m_durationText;
    Slider* m_durationSlider;
    BezierEditor* m_bezierEditor;
    int m_currentTouch;
    TouchType m_currentTouchedObject;
    friend class DurationTextDelegate;
};

void DurationTextDelegate::textChanged() {
    char const* newtext = m_spacingLayer->m_durationText->text();

    char* err;
    float maybe = fmin(1.0, strtof(newtext, &err)/10.0f);
    if (!(*err)) {
        m_spacingLayer->m_durationSlider->setValue(maybe);
        std::cout << maybe << "\n";
    }
}

void ControlPoint::updateDraw(CCObject* sender) {
    clear();
    ccColor4F col;
    switch (getTag()) {
        case 0:
            col = ccc4FFromccc3B(ccc3(255, 255, 255));
            break;
        case 1:
            col = ccc4FFromccc3B(ccc3(255, 16, 135));
            break;
        case 2:
            col = ccc4FFromccc3B(ccc3(16, 135, 255));
            break;
        case 3:
            col = ccc4FFromccc3B(ccc3(205, 205, 205));
            break;
        default:
            col = ccc4FFromccc3B(ccc3(255, 255, 255));
    }
    drawSegment(getPosition(), getPosition(), kControlPointRadius * 2, col);
}


void BezierEditor::evalNewTouch(CCTouch* touch) {
    auto touchPos = getTouchPos(touch->getLocation());
    if (m_control1->intersectsControl(touchPos))
        m_selectedControl = m_control1;
    else if(m_control2->intersectsControl(touchPos))
        m_selectedControl = m_control2;
    else
        m_selectedControl = nullptr;
}

void BezierEditor::cacTouchMoved(CCTouch* touch, CCEvent* event) {
    auto touchPos = getTouchPos(touch->getLocation());
    if (m_selectedControl) {
        m_selectedControl->setPosition(m_selectedControl->getPosition() + (touchPos - getTouchPos(touch->getPreviousLocation())));
        m_selectedControl->clampFurther();
        updateBezier();
    }
}

void BezierEditor::updateBezier() {
    m_bezierDrawer->clear();
    auto secondaryLine = ccc4FFromccc3B(ccc3(150, 150, 150));
    auto primaryLine = ccc4FFromccc3B(ccc3(20, 20, 20));
    m_bezierDrawer->drawSegment(
        m_control0->getPosition() + m_control0->getPosition(),
        m_control1->getPosition() + m_control1->getPosition(),
        1,
        secondaryLine
    );
    m_bezierDrawer->drawSegment(
        m_control2->getPosition() + m_control2->getPosition(),
        m_control3->getPosition() + m_control3->getPosition(),
        1,
        secondaryLine
    );

    m_bezierDrawer->_cubicBezierStuff(
        m_control0->getPosition() + m_control0->getPosition(),
        m_control1->getPosition() + m_control1->getPosition(),
        m_control2->getPosition() + m_control2->getPosition(),
        m_control3->getPosition() + m_control3->getPosition(),
        50,
        primaryLine
    );
}