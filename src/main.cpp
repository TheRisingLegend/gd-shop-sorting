#include <Geode/Geode.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/modify/GJShopLayer.hpp>
#include <tuple>

#include "sortOptionsPopup.hpp"

using namespace geode::prelude;

// int $modify(SortingShopLayer, GJShopLayer)::Fields::m_sortType = 0;

class $modify(SortingShopLayer, GJShopLayer) {
    struct Fields {
        /* static */ int m_sortType;
        std::vector<std::tuple<int, CCMenuItemSpriteExtra*, int, int, bool>> m_shopItems;  // (original index, item, price, unlock type, is owned)
        ListButtonBar* m_container;
    };

    void sortShopItems() {
        // always sort to default first to ensure consistent ordering for other sorting methods
        std::sort(m_fields->m_shopItems.begin(), m_fields->m_shopItems.end(), [](const auto& a, const auto& b) {
            return std::get<0>(a) < std::get<0>(b);
        });

        if (m_fields->m_sortType == 1) {  // Price: Increasing
            std::sort(m_fields->m_shopItems.begin(), m_fields->m_shopItems.end(), [](const auto& a, const auto& b) {
                bool aOwned = std::get<4>(a);
                bool bOwned = std::get<4>(b);
                return aOwned == bOwned ? std::get<2>(a) < std::get<2>(b) : aOwned < bOwned;
            });
        } else if (m_fields->m_sortType == 2) {  // Price: Decreasing
            std::sort(m_fields->m_shopItems.begin(), m_fields->m_shopItems.end(), [](const auto& a, const auto& b) {
                bool aOwned = std::get<4>(a);
                bool bOwned = std::get<4>(b);
                return aOwned == bOwned ? std::get<2>(a) > std::get<2>(b) : aOwned < bOwned;
            });
        } else if (m_fields->m_sortType == 3) {  // Type
            std::sort(m_fields->m_shopItems.begin(), m_fields->m_shopItems.end(), [this](const auto& a, const auto& b) {
                bool aOwned = std::get<4>(a);
                bool bOwned = std::get<4>(b);
                return aOwned == bOwned ? unlockTypeToTypeId(std::get<3>(a)) < unlockTypeToTypeId(std::get<3>(b)) : aOwned < bOwned;
            });
        }

        populateShop();
    }

    // Sets the sort order when sorting by type
    int unlockTypeToTypeId(int unlockType) {
        switch (unlockType) {
            case 1:  // Cube
                return 0;
            case 4:  // Ship
                return 1;
            case 5:  // Ball
                return 2;
            case 6:  // Ufo
                return 3;
            case 7:  // Wave
                return 4;
            case 8:  // Robot
                return 5;
            case 9:  // Spider
                return 6;
            case 13:  // Swing
                return 7;
            case 14:  // Jetpack
                return 8;
            case 2:  // Color 1
                return 9;
            case 3:  // Color 2
                return 10;
            case 10:  // Streak
                return 11;
            case 15:  // Ship Fire
                return 12;
            case 11:  // Death Effect
                return 13;
            case 12:  // Special
                return 14;
            default:
                return 15;  // Unknown
        }
    }

    // Adds sorted items to the shop
    void populateShop() {
        std::vector<CCMenu*> menus;
        for (auto page : CCArrayExt<ListButtonPage*>(m_fields->m_container->m_pages)) {
            auto menu = static_cast<CCMenu*>(page->getChildren()->objectAtIndex(0));
            if (menu) {
                menus.push_back(menu);
                menu->removeAllChildren();
            }
        }

        for (int i = 0; i < m_fields->m_shopItems.size(); i++) {
            CCMenuItemSpriteExtra* item = std::get<1>(m_fields->m_shopItems[i]);
            item->setPosition({-127.5f + (i % 4) * 85, -29.5f - static_cast<int>((i % 8) / 4) * 75});
            menus[i / 8]->addChild(item);
        }
    }

    void onSortButton(CCObject* sender) {
        auto popup = SortOptionsPopup::create(&m_fields->m_sortType, [this]() {
            this->sortShopItems();
        });
        if (popup) {
            popup->setID("sort-options-popup");
            popup->setZOrder(101);  // to be above the icons
            this->addChild(popup);
        } else {
            log::error("Failed to create SortOptionsPopup.");
        }
    }

    bool init(ShopType p0) {
        if (!GJShopLayer::init(p0)) return false;

        // Extract all items from the shop
        // This mess is to traverse the node tree
        m_fields->m_container = getChildByType<ListButtonBar>(0);
        if (!m_fields->m_container) return true;

        auto GSM = GameStatsManager::sharedState();
        for (int i = 0; i < GSM->m_storeItemArray->count(); i++) {
            auto storeItem = static_cast<GJStoreItem*>(GSM->m_storeItemArray->objectAtIndex(i));
            if (storeItem->m_shopType != p0) continue;

            CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(m_shopItems->objectForKey(storeItem->m_index));

            m_fields->m_shopItems.push_back(std::make_tuple(i, item, storeItem->m_price, storeItem->m_unlockType, GSM->isStoreItemUnlocked(storeItem->m_index)));
        }

        sortShopItems();

        // Add sort button in bottom right
        auto sortButtonMenu = CCMenu::create();
        sortButtonMenu->setID("sort-button-container");
        sortButtonMenu->setPosition({0, 0});
        sortButtonMenu->setZOrder(10);
        this->addChild(sortButtonMenu);

        auto sortButtonSprite = CircleButtonSprite::createWithSpriteFrameName("GJ_sortIcon_001.png");
        auto sortButton = CCMenuItemSpriteExtra::create(
            sortButtonSprite,
            this,
            menu_selector(SortingShopLayer::onSortButton));
        sortButton->setID("sort-button");
        sortButton->setPosition({sortButtonMenu->getContentSize().width - 30, 30});
        sortButtonMenu->addChild(sortButton);

        return true;
    }
};