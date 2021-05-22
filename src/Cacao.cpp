#include "offsets.hpp"

using namespace cocos2d;
using namespace gd;

template <typename T>
std::vector<T> ccToVec(cocos2d::CCArray* arr) {
    std::vector<T> vec;
    for (int i = 0; i < arr->count(); i++) {
        vec.push_back(reinterpret_cast<T>(arr->objectAtIndex(i)));
    }
    return vec;
}

std::vector<int> collapseGroups(GameObject* object) {
    std::vector<int> gs;

    for (int i = 0; i < object->getGroupIDCount(); i++)
        gs.push_back(object->getGroupID(i));

    return gs;
}

int Cacao::uniqueGroupToObjects(CCArray* objects, LevelEditorLayer* lel) {
    std::vector<int> g;
    for (auto obj : ccToVec<GameObject*>(lel->getAllObjects())) {
        auto ig = collapseGroups(obj);
        g.insert(g.end(), ig.begin(), ig.end());
    }
    std::set<int> inclusiveG;
    for (int group : g) {
        if (std::count(g.begin(), g.end(), group) >= objects->count()) {
            inclusiveG.insert(group);
        }
    }
    std::set<int> uniqueG = inclusiveG;
    for (auto obj : ccToVec<GameObject*>(lel->getAllObjects())) {
        if (objects->containsObject(obj) || obj->getZOrder()==314159) {
            continue;
        }
        for (int group : inclusiveG) {
            auto bad_groups = collapseGroups(obj);
            if (std::count(bad_groups.begin(), bad_groups.end(), group)) {
                uniqueG.erase(group);
            }
        }
    }
    if (uniqueG.size()) {
        return *uniqueG.begin();
    } else {
        int newG = lel->getNextFreeGroupID(lel->getAllObjects());
        for (auto obj : ccToVec<GameObject*>(objects)) {
            obj->addToGroup(newG);
        }
        return newG;
    }
}