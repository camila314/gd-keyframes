#pragma once

#include "SpacingLayer.hpp"
#include "DurationTextDelegate.hpp"
#include "BezierEditor.hpp"

using namespace gd;
using namespace cocos2d;
using namespace cocos2d::extension;

void SpacingLayer::setup() {
    m_durationSlider = Slider::create(this, menu_selector(SpacingLayer::onDurationChange), 0.75);
    m_durationSlider->setBarVisibility(false);
    m_durationSlider->setPosition(Cacao::addedPosition(-95.13, 67.2));
    this->m_pLayer->addChild(m_durationSlider);

    m_delegate = new DurationTextDelegate(this);
    m_durationText = InputNode::create(60.0f, "Num", "0123456789.ei+"_s);
    m_durationText->setInputCallback([this](const char* txt) -> void {
        if (txt && strlen(txt))
            try {
                auto f = std::stof(std::string(txt));

                this->m_durationSlider->setValue(f / 10);
            } catch (...) {}
    });
    m_durationText->getInputNode()->setMaxLabelLength(6);
    m_durationText->setPosition(Cacao::addedPosition(-51.2, 101.54));
    this->m_pLayer->addChild(m_durationText);

    auto durationLabel = CCLabelBMFont::create("Duration:", "goldFont.fnt");
    durationLabel->setPosition(Cacao::addedPosition(-141.8, 101.54));
    durationLabel->setScale(0.75);
    this->m_pLayer->addChild(durationLabel);

    m_subdivideToggler = gd::CCMenuItemToggler::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
        this,
        menu_selector(SpacingLayer::onToggle)
    );
    m_subdivideToggler->setPosition(ccp(-172.8,16));
    m_subdivideToggler->setScale(0.8);
    this->m_pButtonMenu->addChild(m_subdivideToggler);

    auto subdivideLabel = CCLabelBMFont::create("Enable\nSubdivision", "bigFont.fnt");
    subdivideLabel->setScale(0.35);
    subdivideLabel->setPosition(Cacao::addedPosition(-122.8, 16));
    this->m_pLayer->addChild(subdivideLabel);

    m_subdivideLabel = CCLabelBMFont::create("Subdivisons", "goldFont.fnt");
    m_subdivideLabel->setScale(0.5);
    m_subdivideLabel->setPosition(Cacao::addedPosition(-42.8, 40.8));
    this->m_pLayer->addChild(m_subdivideLabel);

    m_subdivideText = InputNode::create(37.35f, "Num", "0123456789"_s);
    m_subdivideText->getInputNode()->setMaxLabelLength(4);
    m_subdivideText->setPosition(Cacao::addedPosition(-42.8, 15.8));
    this->m_pLayer->addChild(m_subdivideText);

    auto leftArrow = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow->setScale(0.75);
    m_subdivideLeftArrow = CCMenuItemSpriteExtra::create(leftArrow, this, menu_selector(SpacingLayer::onLeftArrow));
    m_subdivideLeftArrow->setPosition(ccp(-88.8 + 12, 15.8));
    this->m_pButtonMenu->addChild(m_subdivideLeftArrow);

    auto rightArrow = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow->setScale(0.75);
    m_subdivideRightArrow = CCMenuItemSpriteExtra::create(rightArrow, this, menu_selector(SpacingLayer::onRightArrow));
    m_subdivideRightArrow->setPosition(ccp(3.2 - 12, 15.8));
    this->m_pButtonMenu->addChild(m_subdivideRightArrow);

    m_bezierEditor = BezierEditor::create();
    m_bezierEditor->setPosition(Cacao::addedPosition(61.2 + 31.2, 78.34 - 33.2));
    this->m_pLayer->addChild(m_bezierEditor);

    hideSubdivide();
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
    if (touch->getID() != m_currentTouch)
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
    sprintf(ok, "%.3f", newscale * 10);
    m_durationText->setString(ok);
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
    m_subdivideText->setString(ok);
}

void SpacingLayer::onClose(cocos2d::CCObject* pSender) {
    char* err;
    float dur = strtof(m_durationText->getString(), &err);
    if (*err)
        dur = 0.5;
    char* err2;
    unsigned divisions = strtol(m_subdivideText->getString(), &err2, 10);
    if (*err)
        divisions = 0;
    m_controller->updateValues(dur, 
        m_bezierEditor->normalize(m_bezierEditor->m_control1->getPosition()), 
        m_bezierEditor->normalize(m_bezierEditor->m_control2->getPosition()), 
        m_subdivideToggler->isOn(), 
    divisions);

    BrownAlertDelegate::onClose(pSender);
}

void SpacingLayer::onToggle(CCObject* sender) {
    if (!m_subdivideToggler->isOn())
        showSubdivide();
    else
        hideSubdivide();
}

void SpacingLayer::onLeftArrow(CCObject* sender) {
    char* err;
    unsigned long maybe = strtol(m_subdivideText->getString(), &err, 10);
    if (!err[0]) {
        if (maybe>0)
            maybe--;
        char ok[16];
        sprintf(ok, "%lu", maybe);
        m_subdivideText->setString(ok);
    }
}

void SpacingLayer::onRightArrow(CCObject* sender) {
    char* err;
    unsigned long maybe = strtol(m_subdivideText->getString(), &err, 10);
    if (!err[0]) {
        if (maybe<9999)
            maybe++;
        char ok[4];
        sprintf(ok, "%lu", maybe);
        m_subdivideText->setString(ok);
    }
}
