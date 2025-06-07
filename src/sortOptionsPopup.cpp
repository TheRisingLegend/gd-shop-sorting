#include "sortOptionsPopup.hpp"

using namespace geode::prelude;

bool SortOptionsPopup::setup(int* activeSort) {
    this->setTitle("Sort Options");

    m_activeSort = activeSort;

    auto buttons = CCMenu::create();
    buttons->setID("sort-options-buttons");
    m_mainLayer->addChild(buttons);

    buttons->addChild(createSortOptionButton("Default", 0));
    buttons->addChild(createSortOptionButton("Price", 1));
    buttons->addChild(createSortOptionButton("Price", 2));
    buttons->addChild(createSortOptionButton("Type", 3));

    buttons->setLayout(
        ColumnLayout::create()
            ->setGap(5.f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(true));
    buttons->setContentSize({72.3f, 120});
    buttons->setPosition({m_size.width / 2, m_size.height / 2 - 20});

    updateSortOptionButtons();  // For some reason this is needed to ensure the buttons are centered in the layout

    return true;
}

SortOptionsPopup* SortOptionsPopup::create(int* activeSort, std::function<void()> sortShopItemsCallback) {
    auto ret = new SortOptionsPopup();
    if (ret->initAnchored(200.f, 180.f, activeSort)) {  // this init function calls setup()
        ret->m_sortShopItemsCallback = sortShopItemsCallback;
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

CCMenuItemSpriteExtra* SortOptionsPopup::createSortOptionButton(std::string const& text, int tag) {
    auto buttonSprite = CCSprite::createWithSpriteFrameName(*m_activeSort == tag ? "GJ_longBtn02_001.png" : "GJ_longBtn01_001.png");

    auto button = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(SortOptionsPopup::onSortOption));
    button->setTag(tag);
    button->setID(text + "-button-" + std::to_string(tag));
    button->setContentSize({72.3f, 30.5f});

    auto label = CCMenu::create();
    label->setID("button-label");
    label->setPosition({buttonSprite->getContentSize().width / 2, buttonSprite->getContentSize().height / 2 + 2});
    label->setScale(0.4f);
    label->setZOrder(1);
    button->addChild(label);

    auto labelText = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    labelText->setID("button-text");
    labelText->setScale(0.4f);
    label->addChild(labelText);

    auto stonksIcon = CCSprite::createWithSpriteFrameName("GJ_sTrendingIcon_001.png");
    stonksIcon->setID("button-icon");
    if (tag == 1 || tag == 2) {
        label->addChild(stonksIcon);

        if (tag == 2)
            stonksIcon->setFlipY(true);
    }

    label->setLayout(
        RowLayout::create()
            ->setGap(6.f)
            ->setAxisAlignment(AxisAlignment::Center));

    stonksIcon->setScale(1.6f);
    stonksIcon->setPosition({stonksIcon->getPositionX() + 4.f, stonksIcon->getPositionY() - 2.f});

    m_sortButtons.push_back(button);

    return button;
}

void SortOptionsPopup::onSortOption(CCObject* sender) {
    *m_activeSort = static_cast<CCMenuItemSpriteExtra*>(sender)->getTag();

    updateSortOptionButtons();

    if (m_sortShopItemsCallback) {
        m_sortShopItemsCallback();
    }
};

void SortOptionsPopup::updateSortOptionButtons() {
    // load blue sprite for active option
    for (auto& button : m_sortButtons) {
        auto newSprite = CCSprite::createWithSpriteFrameName(button->getTag() == *m_activeSort ? "GJ_longBtn02_001.png" : "GJ_longBtn01_001.png");

        button->setNormalImage(newSprite);
    }
}
