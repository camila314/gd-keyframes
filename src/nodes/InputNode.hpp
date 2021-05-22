#pragma once

#include "../offsets.hpp"

class InputNode : public cocos2d::CCNode, gd::TextInputDelegate {
    protected:
        using inp_func = std::function<void(const char*)>;

        cocos2d::extension::CCScale9Sprite* m_pBG;
        gd::CCTextInputNode* m_pInput;
        inp_func m_pTypeCallback;

        bool init(float, float, const char*, const char*, const std::string &, int);
        bool init(float, const char*, const char*, const std::string &, int);
        bool initMulti(float, float, const char*, const char*, const std::string &, int);

        virtual void textChanged(gd::CCTextInputNode*) override;

    public:
        static InputNode* create(
            float width,
            const char* placeholder,
            const char* fontFile,
            const std::string & filter,
            int limit
        );
        static InputNode* create(
            float width,
            const char* placeholder,
            const std::string & filter,
            int limit
        );
        static InputNode* create(
            float width,
            const char* placeholder,
            const std::string & filter
        );
        static InputNode* create(
            float width,
            const char* placeholder,
            const char* fontFile
        );
        static InputNode* create(
            float width,
            const char* placeholder
        );

        // don't remember if this even works lol
        // the second float is height, rest are
        // the same as normal
        static InputNode* createMulti(float, float, const char*, const char*, const std::string &, int);
        static InputNode* createMulti(float, float, const char*, const char*, int);

        inline void setInputCallback(inp_func f) { this->m_pTypeCallback = f; }

        gd::CCTextInputNode* getInputNode();

        void setString(const char*);
        const char* getString();
};