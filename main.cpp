// replace this code

#include <Cacao.hpp>
#include <iostream>
#include <numeric>
#include "ConnectorNode.h"
#include "SpacingController.h"

using namespace cocos2d;
ModContainer* m;

typedef struct _pog {
    float rotate;
    CCPoint move;
} KeyframeInfo;

std::string createMoveTrigger(CCPoint realpos, CCPoint targetpos, int group, float distance) {
    char out[128];
    sprintf(out, "1,901,2,%f,3,%f,36,1,28,%f,29,%f,51,%d,10,%f", realpos.x, realpos.y-90, targetpos.x, targetpos.y, group, distance);
    //std::cout << "our thing " << out << std::endl;
    return std::string(out);
}

std::string createRotateTrigger(CCPoint realpos, float targetrot, int group, float distance) {
    char out[128];
    sprintf(out, "1,1346,2,%f,3,%f,36,1,68,%f,51,%d,10,%f", realpos.x, realpos.y-90, targetrot, group, distance);
    std::cout << "our thing " << out << std::endl;
    return std::string(out);
}

std::vector<SpacingController*> g_spacingList;
class PhantomEditorUI : public EditorUI {
public:

    void onKeyframe(CCObject* sender) {
        auto so = this->getSelectedObjects();
        if (so->count() > 0) {
            auto ccar = CCArray::create();
            for (int j = 0; j < so->count(); j++) {
                auto i = reinterpret_cast<GameObject*>(so->objectAtIndex(j));
                GameObject* newly = _editorLayer()->addObjectFromString(i->getSaveString());

                ccar->addObject(newly);
                newly->_zOrder() = 314159;


                ConnectorNode* src_connector = ConnectorNode::findFromSource(i);
                if (src_connector) {
                    auto dest = src_connector->getDestinationObject();
                    src_connector->destroy();
                    _editorLayer()->_objectLayer()->addChild(ConnectorNode::create(dest, newly),9<<9);
                    dest->setUserObject(newly);
                }
                auto connector = ConnectorNode::create(newly, i);
                _editorLayer()->_objectLayer()->addChild(connector,9<<9);
                newly->setUserObject(i);

                if (i->_zOrder()==314159) {
                    newly->setFollowingSprite(i->getFollowingSprite());
                } else {
                    connector->setDotEnabled(true);
                    newly->setFollowingSprite(i);
                }
                newly->_type() = 7;

                auto copysprite = CCSprite::createWithSpriteFrame(newly->displayFrame());
                copysprite->setColor(ccc3(255,158,75));
                copysprite->setPosition(ccp(newly->getContentSize().width/2, newly->getContentSize().height/2));
                newly->addChild(copysprite);
            }
            this->deselectAll();
            this->selectObjects(ccar, false);
        }
    }

    std::vector<KeyframeInfo> subdivide(std::vector<KeyframeInfo> frames) {
        std::vector<KeyframeInfo> new_frames;
        for (int index = 0; index<frames.size();index++) {
            KeyframeInfo kinfo = frames[index];

            KeyframeInfo k1 = {.move = ccp(kinfo.move.x/2, kinfo.move.y/2), .rotate = kinfo.rotate/2};
            KeyframeInfo k2 = {.move = ccp(kinfo.move.x/2, kinfo.move.y/2), .rotate = kinfo.rotate/2};
            new_frames.push_back(k1);
            new_frames.push_back(k2);
        }
        return new_frames;
    }

    std::string collapseFrames(GameObject* src) {
        std::string toPaste;

        std::vector<KeyframeInfo> frames;
        //frames.push_back({.move=ccp(0,0),.rotate=0,.index=0});
        auto connection = ConnectorNode::findFromSource(src);
        GameObject* original;
        while (connection) {
            auto kframe = connection->getDestinationObject();
            if (!_editorLayer()->_objects()->containsObject(kframe)) {
                connection->destroy();
                break;
            }
            original = reinterpret_cast<GameObject*>(kframe->getFollowingSprite());

            auto metadata = reinterpret_cast<CCNode*>(kframe->getUserObject());

            _editorLayer()->removeObject(kframe, true);
            if (!metadata)
                break;

            KeyframeInfo info;
            info.move = kframe->getPosition() - metadata->getPosition();
            info.rotate = kframe->getRotation() - metadata->getRotation();

            if (info.move.x!=0 || info.move.y!=0 || info.rotate!=0)
                frames.push_back(info);
            connection->destroy();
            connection = ConnectorNode::findFromSource(kframe);
        }
        auto sController = SpacingController::fromList(g_spacingList, src);

        for (int i=0; i<sController->subdivisions(); i++) {
            frames = subdivide(frames);
        }

        float duration = sController->duration();
        //frames.push_back({.move=ccp(0,0),.rotate=0});
        for (int counter=0; counter<frames.size();counter++) {
            auto kinfo = frames[counter];

            int newGroup = Cacao::uniqueGroupToObject(original, _editorLayer());

            auto bezierSpacing = sController->bezierMultiplierAtRange(counter, frames.size()) ;//* duration;
            auto nextBezierSpacing = sController->bezierMultiplierAtRange(counter+1, frames.size()) ;//* duration;

            float currPosition = counter * ( (300.0f * bezierSpacing) / (frames.size() )) * duration;
            float nextPosition = (counter+1) * ( (300.0f * nextBezierSpacing) / (frames.size() )) * duration;

            std::cout << "Bezier of " << bezierSpacing << "   "
                      << "Next one is " << nextBezierSpacing << "\n";
            if (kinfo.move.x!=0 || kinfo.move.y!=0) {
                toPaste = toPaste + createMoveTrigger(original->getPosition()+ccp(currPosition, 0), kinfo.move, newGroup, (nextPosition - currPosition) / 300.0f) + ";";
            }
            if (kinfo.rotate !=0) {
                toPaste = toPaste + createRotateTrigger(original->getPosition()+ccp(currPosition, 30), kinfo.rotate, newGroup, (nextPosition - currPosition) / 300.0f) + ";";
            }

        }
        return toPaste;

    }
    void onClick(CCObject* sender) {
        std::string toPaste;

        for (int i = 0; i < this->getSelectedObjects()->count(); i++) {
            GameObject* src = reinterpret_cast<GameObject*>(this->getSelectedObjects()->objectAtIndex(i));
            if (src->_zOrder() != 314159 && ConnectorNode::findFromSource(src)) {
                toPaste += this->collapseFrames(src);
            }
        }
        this->deselectAll();
        this->pasteObjects(toPaste);
    }

    void onSpacing(CCObject* sender) {
        std::vector<GameObject*> valid;
        for (int i = 0; i < this->getSelectedObjects()->count(); i++) {
            GameObject* src = reinterpret_cast<GameObject*>(this->getSelectedObjects()->objectAtIndex(i));
            if (src->_zOrder() != 314159 && ConnectorNode::findFromSource(src))
                valid.push_back(src);
        }
        if (valid.size()==1) {
            auto ctrl = SpacingController::fromList(g_spacingList, valid[0]);
            if (ctrl) {
                addChild(ctrl);
                ctrl->show();
            }
        }
    }

    static void h_init(PhantomEditorUI* self) {
        ORIG(h_init, 0xcb50)(self);
        g_spacingList.clear();

        auto menu = CCMenu::create();
        self->addChild(menu);

        auto mybutton = CCSprite::create("GJ_button_05.png");
        mybutton->setScale(0.77);

        auto keyIcon = CCSprite::createWithSpriteFrameName("d_key01_001.png");
        mybutton->addChild(keyIcon);
        keyIcon->setPosition(ccp(mybutton->getContentSize().width/2,mybutton->getContentSize().height/2));
        keyIcon->setScale(0.9);
        mybutton->setChildColor(ccc3(255,57,47));

        auto mitem = CreateMenuItem::create(mybutton, mybutton, self, menu_selector(PhantomEditorUI::onKeyframe));
        menu->addChild(mitem);

        auto dir = CCDirector::sharedDirector();
        float top = dir->getScreenTop()-60.0;
        float rgt = dir->getScreenRight()-24.0;

        menu->setPosition(ccp(rgt, top) + ccp(38 * -3, 0));




        auto menu2 = CCMenu::create();
        self->addChild(menu2);

        auto mybutton2 = CCSprite::create("GJ_button_05.png");
        mybutton2->setScale(0.77);

        auto keyHoleIcon = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        mybutton2->addChild(keyHoleIcon);
        keyHoleIcon->setPosition(ccp(mybutton2->getContentSize().width/2,mybutton2->getContentSize().height/2));
        keyHoleIcon->setScale(0.9);
        mybutton2->setChildColor(ccc3(235,87,87));
        keyHoleIcon->setOpacity(192);

        auto mitem2 = CreateMenuItem::create(mybutton2, mybutton2, self, menu_selector(PhantomEditorUI::onClick));
        menu2->addChild(mitem2);

        menu2->setPosition(ccp(rgt, top) + ccp(38 * -3, -35));




        auto menu3 = CCMenu::create();
        self->addChild(menu3);

        auto mybutton3 = CCSprite::create("GJ_button_04.png");
        mybutton3->setScale(0.77);

        auto spacingIcon = CCSprite::createWithSpriteFrameName("edit_flipXBtn_001.png");
        mybutton3->addChild(spacingIcon);
        spacingIcon->setPosition(ccp(mybutton3->getContentSize().width/2,mybutton3->getContentSize().height/2));
        spacingIcon->setScale(0.9);
        mybutton3->setChildColor(ccc3(255,97,107));
        spacingIcon->setOpacity(192);

        auto mitem3 = CreateMenuItem::create(mybutton3, mybutton3, self, menu_selector(PhantomEditorUI::onSpacing));
        menu3->addChild(mitem3);

        menu3->setPosition(ccp(rgt, top) + ccp(38 * -3, 35 * -2));
    }
};

void inject() {
    m = new ModContainer("Keyframes");
    m->registerHook(getBase()+0xcb50, PhantomEditorUI::h_init);
    m->enable();
}