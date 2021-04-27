#include "SpacingGui.h"

void DurationTextDelegate::textChanged() {
    char const* newtext = m_spacingLayer->m_durationText->text();

    char* err;
    float maybe = fmin(1.0, strtof(newtext, &err)/10.0f);
    if (!(*err)) {
        m_spacingLayer->m_durationSlider->setValue(maybe);
    }
}
