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
	view->show();
}

void SpacingController::updateValues(float duration, CCPoint c1, CCPoint c2) {
	m_duration = duration;
	m_bezierControl1 = c1;
	m_bezierControl2 = c2;
}

float SpacingController::bezierMultiplierAtRange(float index, float segments) {
	//std::cout << x << "\n";

	float x = index / segments;
	if (x>=1)
		x = 1;// - (1.0/segments);

    float t = 0;
    CCPoint currentVertex = ccp(0,0);
    for(unsigned int i = 1; i <= segments*2; i++) {
    	CCPoint vertex;
        if (i== segments*2) {
            vertex.x = 1;
            vertex.y = 1;
        } else {
            vertex.x = 3.0f * powf(1 - t, 2) * t * m_bezierControl1.x + 3.0f * (1 - t) * t * t * m_bezierControl2.x + t * t * t;
            vertex.y = 3.0f * powf(1 - t, 2) * t * m_bezierControl1.y + 3.0f * (1 - t) * t * t * m_bezierControl2.y + t * t * t;
            t += 1.0f / (segments*2);
        }

        if (fabs(x - vertex.x) < fabs(x - currentVertex.x))
        	currentVertex = vertex;
    }

    //Cacao::printGeometry(currentVertex);
    //auto ret = x/(currentVertex.y > 0.1 ? currentVertex.y : 0.01);
    auto ret = 1+(x - currentVertex.y);
    return ret!=ret ? 0.0 : ret;
}
