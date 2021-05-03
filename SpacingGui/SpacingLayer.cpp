#include "SpacingGui.h"

bool SpacingLayer::alertInit() {
    m_noElasticity = true;
    m_closeButton->setVisible(false);
    title("Keyframe Spacing");
    m_title->setPosition(m_title->getPosition() + ccp(1, 10));

    m_durationSlider = Slider::create(this, menu_selector(SpacingLayer::onDurationChange), 0.75);
    m_durationSlider->setBarVisibility(false);
    m_durationSlider->setPosition(Cacao::addedPosition(-95.13, 67.2));
    m_mainLayer->addChild(m_durationSlider);

    m_delegate = new DurationTextDelegate(this);
    m_durationText = CacTextContainer::create(CCSizeMake(60, 30), m_delegate, "bigFont.fnt")
                     ->charLimit(6)
                     ->allowedChars("0123456789.ei+")
                     ->placeholder("Num");
    m_durationText->setPosition(Cacao::addedPosition(-51.2, 101.54));
    m_mainLayer->addChild(m_durationText);

    auto durationLabel = CCLabelBMFont::create("Duration:", "goldFont.fnt");
    durationLabel->setPosition(Cacao::addedPosition(-141.8, 101.54));
    durationLabel->setScale(0.75);
    m_mainLayer->addChild(durationLabel);


    m_subdivideToggler = Cacao::createToggler(this, menu_selector(SpacingLayer::onToggle));
    m_subdivideToggler->setPosition(ccp(-172.8,16));
    m_subdivideToggler->setScale(0.8);
    m_buttonMenu->addChild(m_subdivideToggler);

    auto subdivideLabel = CCLabelBMFont::create("Enable\nSubdivision", "bigFont.fnt");
    subdivideLabel->setScale(0.35);
    subdivideLabel->setPosition(Cacao::addedPosition(-122.8, 16));
    m_mainLayer->addChild(subdivideLabel);

    m_subdivideLabel = CCLabelBMFont::create("Subdivisons", "goldFont.fnt");
    m_subdivideLabel->setScale(0.5);
    m_subdivideLabel->setPosition(Cacao::addedPosition(-42.8, 40.8));
    m_mainLayer->addChild(m_subdivideLabel);

    m_subdivideText = CacTextContainer::create(CCSizeMake(37.35, 28), NULL, "bigFont.fnt")
                      ->charLimit(4)
                      ->allowedChars("0123456789")
                      ->placeholder("Num");
    m_subdivideText->setPosition(Cacao::addedPosition(-42.8, 15.8));
    m_mainLayer->addChild(m_subdivideText);


    auto leftArrow = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow->setScale(0.75);
    m_subdivideLeftArrow = CCMenuItemSpriteExtra::create(leftArrow, NULL, this, menu_selector(SpacingLayer::onLeftArrow));
    m_subdivideLeftArrow->setPosition(ccp(-88.8 + 12, 15.8));
    m_buttonMenu->addChild(m_subdivideLeftArrow);

    auto rightArrow = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow->setScale(0.75);
    m_subdivideRightArrow = CCMenuItemSpriteExtra::create(rightArrow, NULL, this, menu_selector(SpacingLayer::onRightArrow));
    m_subdivideRightArrow->setPosition(ccp(3.2 - 12, 15.8));
    m_buttonMenu->addChild(m_subdivideRightArrow);

    m_bezierEditor = BezierEditor::create();
    m_bezierEditor->setPosition(Cacao::addedPosition(61.2 + 31.2, 78.34 - 33.2));
    m_mainLayer->addChild(m_bezierEditor);

    hideSubdivide();
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

void SpacingLayer::setSubdivide(bool enabled, unsigned divisions) {
    m_subdivideToggler->toggle(enabled);
    if (enabled) {
        showSubdivide();
    } else {
        hideSubdivide();
    }

    char ok[16];
    sprintf(ok, "%u", divisions);
    m_subdivideText->text(ok);
}

void SpacingLayer::onClose() {
    char* err;
    float dur = strtof(m_durationText->text(), &err);
    if (*err)
        dur = 0.5;
    char* err2;
    unsigned divisions = strtol(m_subdivideText->text(), &err2, 10);
    if (*err)
        divisions = 0;
    m_controller->updateValues(dur, m_bezierEditor->normalize(m_bezierEditor->m_control1->getPosition()), m_bezierEditor->normalize(m_bezierEditor->m_control2->getPosition()), m_subdivideToggler->_toggled(), divisions);
}

void SpacingLayer::onToggle(CCObject* sender) {
    if (!m_subdivideToggler->_toggled())
        showSubdivide();
    else
        hideSubdivide();
    std::cout << "yeah it works\n";
}

void SpacingLayer::onLeftArrow(CCObject* sender) {
    char* err;
    unsigned long maybe = strtol(m_subdivideText->text(), &err, 10);
    if (!err[0]) {
        if (maybe>0)
            maybe--;
        char ok[16];
        sprintf(ok, "%lu", maybe);
        m_subdivideText->text(ok);
    }
}

void SpacingLayer::onRightArrow(CCObject* sender) {
    char* err;
    unsigned long maybe = strtol(m_subdivideText->text(), &err, 10);
    if (!err[0]) {
        if (maybe<9999)
            maybe++;
        char ok[4];
        sprintf(ok, "%lu", maybe);
        m_subdivideText->text(ok);
    }
}
