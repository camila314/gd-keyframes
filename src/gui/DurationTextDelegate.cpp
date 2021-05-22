#pragma once

#include "DurationTextDelegate.hpp"

void DurationTextDelegate::textChanged(gd::CCTextInputNode*) {
    char const* newtext = m_spacingLayer->m_durationText->getInputNode()->getString();

    char* err;
    float maybe = fmin(1.0, strtof(newtext, &err) / 10.0f);
    if (!(*err)) {
        m_spacingLayer->m_durationSlider->setValue(maybe);
    }
}
