#include "SpacingGui.h"

bool SpacingLayer::alertInit() {
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

bool SpacingLayer::ccTouchBegan(CCTouch* touch, CCEvent* event) {
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

void SpacingLayer::ccTouchMoved(CCTouch* touch, CCEvent* event) {
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

void SpacingLayer::onDurationChange(CCObject* sender) {
    float newscale = m_durationSlider->getValue();

    char ok[16];
    sprintf(ok, "%.3f", newscale*10);
    m_durationText->text(ok);
}

void SpacingLayer::setDuration(float duration) {
    m_durationSlider->setValue(fmin(10.0, duration/10));
    onDurationChange(this);
}

void SpacingLayer::setBezierControls(CCPoint c1, CCPoint c2) {
    m_bezierEditor->m_control1->setPosition(m_bezierEditor->unnormalize(c1));
    m_bezierEditor->m_control1->clampControl();
    m_bezierEditor->m_control2->setPosition(m_bezierEditor->unnormalize(c2));
    m_bezierEditor->m_control2->clampControl();
    m_bezierEditor->updateBezier();
}

void SpacingLayer::onClose() {
    char* err;
    float dur = strtof(m_durationText->text(), &err);
    if (*err)
        dur = 0.5;
    m_controller->updateValues(dur, m_bezierEditor->normalize(m_bezierEditor->m_control1->getPosition()), m_bezierEditor->normalize(m_bezierEditor->m_control2->getPosition()));
}
