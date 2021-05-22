#pragma once

#include "../offsets.hpp"

typedef struct _pog {
    float rotate;
    cocos2d::CCPoint move;
} KeyframeInfo;

class EditorUI_hook : public gd::EditorUI {
    protected:
        static inline bool (__thiscall* init)(EditorUI*, gd::GJGameLevel*);
        static bool __fastcall initHook(EditorUI*, edx_t, gd::GJGameLevel*);

        void onKeyframe(cocos2d::CCObject*);
        std::vector<KeyframeInfo> subdivide(std::vector<KeyframeInfo>);
        std::string collapseFrames(gd::GameObject*);
        void onClick(cocos2d::CCObject*);
        void onSpacing(cocos2d::CCObject*);

    public:
        static bool loadHook() {
            return MH_CreateHook(
                (PVOID)(gd::base + 0x76310),
                as<LPVOID>(EditorUI_hook::initHook),
                as<LPVOID*>(&EditorUI_hook::init)
            ) == MH_OK;
        }
};
