// Copyright 2021 camden314
#pragma once

#include "../offsets.hpp"

class ConnectorNode : public cocos2d::CCDrawNode {
    protected:
        void updateDraw(CCObject* sender);
        int getIndex();

    public:
        static cocos2d::CCArray* connectorPool;
        
        static ConnectorNode* findFromSource(gd::GameObject* src);
        static ConnectorNode* create(gd::GameObject* dest, gd::GameObject* src);

        gd::GameObject* getDestinationObject();
        gd::GameObject* getSourceObject();

        cocos2d::CCLabelBMFont* getIndexLabel();
        bool getDotEnabled();
        void setDotEnabled(bool set);

        void destroy();
};