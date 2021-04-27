#pragma once
#include <Cacao.hpp>
#include <vector>
using namespace cocos2d;

class SpacingController : public CCNode {
 public:
	static SpacingController* create(GameObject* ob);
	static SpacingController* fromList(std::vector<SpacingController*>& vec, GameObject* ob);
	bool init(GameObject* ob);
	void show();
	inline int uuid() {return m_uuid;}
	inline float duration() {return m_duration;}
	void updateValues(float duration, CCPoint c1, CCPoint c2);
	float bezierMultiplierAtRange(float index, float segments);
 protected:
 	int m_uuid;
 	CCPoint m_bezierControl1;
  	CCPoint m_bezierControl2;
  	float m_duration;
};