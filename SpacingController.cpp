#include "SpacingController.h"
#include "SpacingGui/SpacingGui.h"

SpacingController* SpacingController::create(GameObject* ob) {
    auto pRet = new SpacingController();
    if (pRet && pRet->init(ob)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

SpacingController* SpacingController::fromList(std::vector<SpacingController*>& vec, GameObject* ob) {
    for (auto& i : vec) {
        if (i->uuid() == ob->_uuid()) {
            return i;
        }
    }
    auto ctrl = SpacingController::create(ob);
    ctrl->retain();
    vec.push_back(ctrl);
    return ctrl;
}

bool SpacingController::init(GameObject* ob) {
    m_uuid = ob->_uuid();
    m_bezierControl1 = ccp(0.258355, 0);
    m_bezierControl2 = ccp(0.741645, 1);
    m_duration = 0.5;
    return true;
}

void SpacingController::show() {
    auto view = SpacingLayer::create()->controller(this);
    view->setDuration(m_duration);
    view->setBezierControls(m_bezierControl1, m_bezierControl2);
    std::cout << m_subdivisionEnabled << " enabled\n";
    view->setSubdivide(m_subdivisionEnabled, m_subdivisions);
    view->show();
}

void SpacingController::updateValues(float duration, CCPoint c1, CCPoint c2, bool enabled, unsigned subdivisions) {
    m_duration = duration;
    m_bezierControl1 = c1;
    m_bezierControl2 = c2;
    m_subdivisionEnabled = enabled;
    m_subdivisions = subdivisions;
}

float SpacingController::bezierMultiplierAtRange(float index, float segments) {
    //std::cout << x << "\n";

    float x = index / segments;
    if (x>=1)
        return 1;
    if (x==0)
        return 0;
    float t = 0;
    CCPoint currentVertex = ccp(0,0);
    while (t <= 1) {
        CCPoint vertex;
        if (0) {
            vertex.x = 1;
            vertex.y = 1;
        } else {
            vertex.x = 3.0f * powf(1 - t, 2) * t * m_bezierControl1.x + 3.0f * (1 - t) * t * t * m_bezierControl2.x + t * t * t;
            vertex.y = 3.0f * powf(1 - t, 2) * t * m_bezierControl1.y + 3.0f * (1 - t) * t * t * m_bezierControl2.y + t * t * t;
            t += 1.0f / (segments*2);
        }

        if (fabs(x - vertex.y) < fabs(x - currentVertex.y) || currentVertex.y==0)
            currentVertex = vertex;
    }
    return currentVertex.x;
}

unsigned SpacingController::subdivisions() {
    if (m_subdivisionEnabled)
        return m_subdivisions;
    return 0;
}
