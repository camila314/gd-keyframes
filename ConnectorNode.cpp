// Copyright 2021 camden314
#include "ConnectorNode.h"

CCArray* ConnectorNode::connectorPool = nullptr;

int ConnectorNode::getIndex() {
    auto dest = getDestinationObject();
    auto loopitem = dest;
    int counter = -1;
    while (loopitem && loopitem->getUserObject() != dest) {
        counter += 1;
        loopitem = reinterpret_cast<GameObject*>(loopitem->getUserObject());
    }
    return counter;
}
void ConnectorNode::destroy() {
    ConnectorNode::connectorPool->removeObject(this);
    this->removeFromParentAndCleanup(true);
}
void ConnectorNode::updateDraw(CCObject* sender) {
    this->clear();
    if (!LEL->_objects()->containsObject(getSourceObject())) {
        this->destroy();

        if (LEL->_objects()->containsObject(getDestinationObject()))
            LEL->removeObject(getDestinationObject(), true);
        return;
    }
    if (!LEL->_objects()->containsObject(getDestinationObject())) {
        this->setVisible(false);
    } else {
        this->setVisible(true);
    }

    getIndexLabel()->setString(CCString::createWithFormat("%i", getIndex())->getCString(), true);
    getIndexLabel()->setPosition(getDestinationObject()->getPosition() + ccp(0, getDestinationObject()->getContentSize().height));
    this->drawSegment(
        getDestinationObject()->getPosition(), 
        getSourceObject()->getPosition(), 
        1,
        ccc4FFromccc3B(ccc3(240, 151, 43)));
    if (this->getDotEnabled()) {
        this->drawSegment(
            getSourceObject()->getPosition(), 
            getSourceObject()->getPosition(), 
            3, 
            ccc4FFromccc3B(ccc3(255, 16, 16)));
    }
}
ConnectorNode* ConnectorNode::create(GameObject* dest, GameObject* src) {
    auto pret = reinterpret_cast<ConnectorNode*>(CCDrawNode::create());
    auto storage = CCArray::create();
    storage->addObject(dest);
    storage->addObject(src);

    auto indexLabel = CCLabelBMFont::create("0", "goldFont.fnt");
    indexLabel->setScale(0.7);
    storage->addObject(indexLabel);
    pret->addChild(indexLabel);

    pret->setUserObject(storage);
    pret->schedule(schedule_selector(ConnectorNode::updateDraw), 0);

    if (!ConnectorNode::connectorPool) {
        ConnectorNode::connectorPool = CCArray::create();
        ConnectorNode::connectorPool->retain();
    }
    ConnectorNode::connectorPool->addObject(pret);
    return pret;
}

GameObject* ConnectorNode::getDestinationObject() {
    return reinterpret_cast<GameObject*>(
        reinterpret_cast<CCArray*>(getUserObject())
        ->objectAtIndex(0));
}
GameObject* ConnectorNode::getSourceObject() {
    return reinterpret_cast<GameObject*>(
        reinterpret_cast<CCArray*>(getUserObject())
        ->objectAtIndex(1));
}

CCLabelBMFont* ConnectorNode::getIndexLabel() {
    return reinterpret_cast<CCLabelBMFont*>(
        reinterpret_cast<CCArray*>(getUserObject())
        ->objectAtIndex(2));
}

ConnectorNode* ConnectorNode::findFromSource(GameObject* src) {
    if (!ConnectorNode::connectorPool)
        return nullptr;
    for (int i=0; i<ConnectorNode::connectorPool->count(); i++) {
        auto cn = reinterpret_cast<ConnectorNode*>(ConnectorNode::connectorPool->objectAtIndex(i));
        if (cn->getSourceObject() == src)
            return cn;
    }
    return nullptr;
}

bool ConnectorNode::getDotEnabled() {
    return reinterpret_cast<uintptr_t>(this->getUserData())>0;
}
void ConnectorNode::setDotEnabled(bool set) {
    this->setUserData(reinterpret_cast<void*>(set ? 0x2 : 0x0));
}