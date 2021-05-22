#include "BezierEditor.hpp"

using namespace cocos2d;
using namespace cocos2d::extension;

CCPoint clamp_to_bezier_space(double x, double y) {
    return ccp(clamp(x, -41.9, 41.9), clamp(y, -33.6, 33.6));
}

bool BezierEditor::init() {
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

CCPoint BezierEditor::getTouchPos(CCPoint const& p) {
    return (p - getPosition()) / 2;
}

bool BezierEditor::intersectsEditor(CCPoint const& p) {
    auto size = this->m_background->getScaledContentSize();
    auto rect = cocos2d::CCRect {
        this->getPositionX() - size.width / 2, this->getPositionY() - size.height / 2, size.width, size.height
    };

    return rect.containsPoint(p);
}

void BezierEditor::evalNewTouch(CCTouch* touch) {
    auto touchPos = getTouchPos(touch->getLocation());

    if (m_control1->intersectsControl(touchPos))
        m_selectedControl = m_control1;
    else if (m_control2->intersectsControl(touchPos))
        m_selectedControl = m_control2;
    else
        m_selectedControl = nullptr;
}

void BezierEditor::cacTouchMoved(CCTouch* touch, CCEvent* event) {
    auto touchPos = getTouchPos(touch->getLocation());

    if (m_selectedControl) {
        m_selectedControl->setPosition(m_selectedControl->getPosition() + (touchPos - getTouchPos(touch->getPreviousLocation())));
        m_selectedControl->clampControl();
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