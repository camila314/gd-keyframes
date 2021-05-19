#pragma once

#include "../offsets.hpp"
#include <vector>

class SpacingController : public cocos2d::CCNode {
	protected:
		int m_uuid;
		cocos2d::CCPoint m_bezierControl1;
		cocos2d::CCPoint m_bezierControl2;
		float m_duration;
		bool m_subdivisionEnabled;
		unsigned m_subdivisions;

	public:
		static SpacingController* create(gd::GameObject* ob);
		static SpacingController* fromList(std::vector<SpacingController*>& vec, gd::GameObject* ob);
		
		bool init(gd::GameObject* ob);
		void show();

		inline int uuid() { return m_uuid; }
		inline float duration() { return m_duration; }

		void updateValues(float duration, cocos2d::CCPoint c1, cocos2d::CCPoint c2, bool enabled, unsigned divisions);
		unsigned subdivisions();
		float bezierMultiplierAtRange(float index, float segments);
};