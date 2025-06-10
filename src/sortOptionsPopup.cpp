#include "sortOptionsPopup.hpp"

using namespace geode::prelude;

bool SortOptionsPopup::setup(int* activeSort, bool* pushOwnedToEnd) {
    this->setTitle("Sort Options");

    m_activeSort = activeSort;
    m_pushOwnedToEnd = pushOwnedToEnd;

    // create the checkbox
    m_checkboxMenu = CCMenu::create();
    m_checkboxMenu->setID("push-owned-checkbox-menu");
    m_checkboxMenu->setContentSize({80.f, 35.f});
    m_checkboxMenu->setPosition({(m_size.width - m_checkboxMenu->getContentSize().width) / 2, 8.f});
    m_checkboxMenu->setVisible(*m_activeSort != 0);
    m_mainLayer->addChild(m_checkboxMenu);

    m_pushOwnedToEndCheckbox = CCMenuItemToggler::create(
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        this,
        menu_selector(SortOptionsPopup::onCheckboxToggle));
    m_pushOwnedToEndCheckbox->toggle(!*m_pushOwnedToEnd);
    m_pushOwnedToEndCheckbox->setID("push-owned-checkbox");
    m_pushOwnedToEndCheckbox->setPosition({m_pushOwnedToEndCheckbox->getContentSize().width / 2 - 2.f, m_pushOwnedToEndCheckbox->getContentSize().height / 2});
    m_checkboxMenu->addChild(m_pushOwnedToEndCheckbox);

    auto infoButton = InfoAlertButton::create("Push Owned to End",
                                              "When enabled, items you already own will\nbe shown at the end of the shop.",
                                              0.5f);
    infoButton->setID("push-owned-info-button");
    infoButton->setPosition({-8.f, m_pushOwnedToEndCheckbox->getContentSize().height / 2});
    m_checkboxMenu->addChild(infoButton);

    auto checkboxLabel = CCLabelBMFont::create("Push\nOwned\nto End", "bigFont.fnt");
    checkboxLabel->setID("checkbox-label");
    checkboxLabel->setScale(0.25f);
    checkboxLabel->setPosition({m_pushOwnedToEndCheckbox->getContentSize().width + 20.f, m_pushOwnedToEndCheckbox->getContentSize().height / 2});
    m_checkboxMenu->addChild(checkboxLabel);

    // create the option buttons
    auto buttons = CCMenu::create();
    buttons->setID("sort-options-buttons");
    buttons->setContentSize({80.f, 120});
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
    buttons->setPosition({m_size.width / 2, m_size.height / 2 + 5.f});

    updateSortOptionButtons();  // For some reason this is needed to ensure the buttons are centered in the layout

    return true;
}

SortOptionsPopup* SortOptionsPopup::create(int* activeSort, bool* pushOwnedToEnd, std::function<void()> sortShopItemsCallback) {
    auto ret = new SortOptionsPopup();
    if (ret->initAnchored(200.f, 200.f, activeSort, pushOwnedToEnd)) {  // this init function calls setup()
        ret->m_sortShopItemsCallback = sortShopItemsCallback;
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void SortOptionsPopup::onCheckboxToggle(CCObject* sender) {
    auto checkbox = static_cast<CCMenuItemToggler*>(sender);
    *m_pushOwnedToEnd = checkbox->isOn();

    if (m_sortShopItemsCallback) {
        m_sortShopItemsCallback();
    }
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

    m_checkboxMenu->setVisible(*m_activeSort != 0);  // Hide checkbox for default sort option

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
