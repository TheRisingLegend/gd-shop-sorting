#ifndef SORTOPTIONSPOPUP_HPP
#define SORTOPTIONSPOPUP_HPP

#include <Geode/Geode.hpp>

class SortOptionsPopup : public geode::Popup<int*> {
   protected:
    bool setup(int* activeSort) override;

   public:
    int* m_activeSort;
    std::vector<CCMenuItemSpriteExtra*> m_sortButtons;

    std::function<void()> m_sortShopItemsCallback;

    CCMenuItemSpriteExtra* createSortOptionButton(std::string const& text, int tag);
    void onSortOption(CCObject* sender);
    void updateSortOptionButtons();

    static SortOptionsPopup* create(int* activeSort, std::function<void()> sortShopItemsCallback = nullptr);
};

#endif