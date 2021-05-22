#pragma once

#include "../offsets.hpp"
#include "SpacingLayer.hpp"

class SpacingLayer;

class DurationTextDelegate : public gd::TextInputDelegate {
    protected:
        SpacingLayer* m_spacingLayer;

    public:
        DurationTextDelegate(SpacingLayer* sl) {m_spacingLayer = sl;}
        void textChanged(gd::CCTextInputNode*) override;
};

