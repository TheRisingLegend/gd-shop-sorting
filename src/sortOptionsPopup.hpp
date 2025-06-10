#ifndef SORTOPTIONSPOPUP_HPP
#define SORTOPTIONSPOPUP_HPP

#include <Geode/Geode.hpp>

class SortOptionsPopup : public geode::Popup<int*, bool*> {
   protected:
    bool setup(int* activeSort, bool* pushOwnedToEnd) override;

   public:
    int* m_activeSort;

    bool* m_pushOwnedToEnd;
    CCMenuItemToggler* m_pushOwnedToEndCheckbox;
    cocos2d::CCMenu* m_checkboxMenu;

    std::vector<CCMenuItemSpriteExtra*> m_sortButtons;

    std::function<void()> m_sortShopItemsCallback;

    void onCheckboxToggle(CCObject* sender);
    CCMenuItemSpriteExtra* createSortOptionButton(std::string const& text, int tag);
    void onSortOption(CCObject* sender);
    void updateSortOptionButtons();

    static SortOptionsPopup* create(int* activeSort, bool* pushOwnedToEnd, std::function<void()> sortShopItemsCallback = nullptr);
};

#endif