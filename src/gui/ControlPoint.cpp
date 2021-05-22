#pragma once

#include "ControlPoint.hpp"

using namespace cocos2d;

void ControlPoint::clampControl() {
    setPosition(
        ccp(clamp((double)getPositionX(), -41.9 + kControlPointRadius, 41.9 - kControlPointRadius),
        clamp((double)getPositionY(), -33.6 + kControlPointRadius, 33.6 - kControlPointRadius))
    );
}

ControlPoint* ControlPoint::create(BezierEditor* sl, int ctrl_point) {
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