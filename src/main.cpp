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
        std::vector<std::tuple<int, CCMenuItemSpriteExtra*, int, int>> m_shopItems;  // (original index, item, price, unlock type)
        std::vector<cocos2d::CCMenu*> m_pages;
    };

    void sortShopItems() {
        // always sort to default first to ensure consistent ordering for other sorting methods
        std::sort(m_fields->m_shopItems.begin(), m_fields->m_shopItems.end(), [](const auto& a, const auto& b) {
            return std::get<0>(a) < std::get<0>(b);
        });

        if (m_fields->m_sortType == 1) {  // Price: Increasing

            // only sort items that have not been purchased yet
            std::vector<std::tuple<int, CCMenuItemSpriteExtra*, int, int>> temp;
            for (const auto& item : m_fields->m_shopItems)
                if (std::get<2>(item) != -1)
                    temp.push_back(item);

            std::sort(temp.begin(), temp.end(), [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });

            // append the remaining items so they can be displayed at the end
            for (const auto& item : m_fields->m_shopItems)
                if (std::get<2>(item) == -1)
                    temp.push_back(item);

            m_fields->m_shopItems = std::move(temp);

        } else if (m_fields->m_sortType == 2) {  // Price: Decreasing

            // only sort items that have not been purchased yet
            std::vector<std::tuple<int, CCMenuItemSpriteExtra*, int, int>> temp;
            for (const auto& item : m_fields->m_shopItems)
                if (std::get<2>(item) != -1)
                    temp.push_back(item);

            std::sort(temp.begin(), temp.end(), [](const auto& a, const auto& b) {
                return std::get<2>(a) > std::get<2>(b);
            });

            // append the remaining items so they can be displayed at the end
            for (const auto& item : m_fields->m_shopItems)
                if (std::get<2>(item) == -1)
                    temp.push_back(item);

            m_fields->m_shopItems = std::move(temp);
        }

        else if (m_fields->m_sortType == 3) {  // Type

            // separate already purchased items from not yet purchased items
            std::vector<std::tuple<int, CCMenuItemSpriteExtra*, int, int>> owned;
            std::vector<std::tuple<int, CCMenuItemSpriteExtra*, int, int>> notOwned;

            for (const auto& item : m_fields->m_shopItems) {
                if (std::get<2>(item) == -1) {
                    owned.push_back(item);
                } else {
                    notOwned.push_back(item);
                }
            }

            std::sort(owned.begin(), owned.end(), [this](const auto& a, const auto& b) {
                return unlockTypeToTypeId(std::get<3>(a)) < unlockTypeToTypeId(std::get<3>(b));
            });

            std::sort(notOwned.begin(), notOwned.end(), [this](const auto& a, const auto& b) {
                return unlockTypeToTypeId(std::get<3>(a)) < unlockTypeToTypeId(std::get<3>(b));
            });

            // combine the two lists, all the not yet purchased items first
            m_fields->m_shopItems.clear();
            m_fields->m_shopItems.insert(m_fields->m_shopItems.end(), notOwned.begin(), notOwned.end());
            m_fields->m_shopItems.insert(m_fields->m_shopItems.end(), owned.begin(), owned.end());
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
        for (auto& page : m_fields->m_pages) {
            page->removeAllChildren();
        }

        for (int i = 0; i < m_fields->m_shopItems.size(); i++) {
            CCMenuItemSpriteExtra* item = std::get<1>(m_fields->m_shopItems[i]);
            item->setPosition({-127.5f + (i % 4) * 85, -29.5f - ((i % 8) / 4) * 75});
            m_fields->m_pages[i / 8]->addChild(item);
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

    // Set some common IDs
    void addIDs() {
        auto children = this->getChildren();
        for (auto child : CCArrayExt<CCNode*>(children)) {
            if (auto bar = typeinfo_cast<ListButtonBar*>(child)) {
                bar->setID("shop-container");
                break;
            }
        }
        ListButtonBar* container = static_cast<ListButtonBar*>(this->getChildByID("shop-container"));
        container->m_scrollLayer->m_extendedLayer->setID("shop-page-container");
    }

    bool init(ShopType p0) {
        if (!GJShopLayer::init(p0)) return false;

        addIDs();

        // Extract all items from the shop
        // This mess is to traverse the node tree
        auto pages_temp = this->getChildByIDRecursive("shop-page-container")->getChildren();  // ListButtonPage
        int originalIndex = 0;                                                                // to be able to sort back to default order
        for (auto page : CCArrayExt<CCNode*>(pages_temp)) {
            auto listPage = typeinfo_cast<ListButtonPage*>(page);
            if (!listPage) continue;

            cocos2d::CCMenu* ccmenu = static_cast<cocos2d::CCMenu*>(page->getChildren()->objectAtIndex(0));
            m_fields->m_pages.push_back(ccmenu);

            auto pageItems = ccmenu->getChildren();
            for (auto itemNode : CCArrayExt<CCNode*>(pageItems)) {
                if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(itemNode)) {
                    // Get the item type
                    GJItemIcon* icon = typeinfo_cast<GJItemIcon*>(item->getChildren()->objectAtIndex(0));  // this should be fine, can't think of any reason anyone would add another child here
                    int unlockType = static_cast<int>(icon->m_unlockType);

                    // Get the price if the item hasn't been purchased yet
                    int price = -1;
                    for (auto child : CCArrayExt<CCNode*>(icon->getChildren())) {
                        if (auto label = typeinfo_cast<CCLabelBMFont*>(child)) {
                            std::string labelText = label->getString();
                            labelText.erase(std::remove(labelText.begin(), labelText.end(), ','), labelText.end());
                            price = std::stoi(labelText);
                        }
                    }

                    log::debug("Found item {} with price {} and unlock type {}", originalIndex, price, unlockType);
                    m_fields->m_shopItems.push_back(std::make_tuple(originalIndex++, item, price, unlockType));
                }
            }
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