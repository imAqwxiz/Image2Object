#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/utils/file.hpp>


using namespace geode::prelude;

class ImageImportTab : public CCNode {
protected:
    bool init() {
        if (!CCNode::init())
            return false;

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);

        auto plusLabel = CCLabelBMFont::create(
            "+",
            "bigFont.fnt"
        );

        auto button = CCMenuItemSpriteExtra::create(
            plusLabel,
            this,
            menu_selector(ImageImportTab::onImport)
        );

        menu->addChild(button);
        this->addChild(menu);

        menu->setContentSize({40.f, 40.f});
        button->setPosition({20.f, 20.f});

        this->setContentSize({40.f, 40.f});

        return true;
    }

    void onImport(CCObject*) {
        log::info("Opening image picker...");

        async::spawn(
            file::pick(FilePickMode::OpenFile, {}),
            [](Result<std::optional<std::filesystem::path>> result) {

                if (result.isErr()) {
                    log::error("Image picker failed");
                    return;
                }

                auto selected = result.unwrap();

                if (!selected.has_value()) {
                    log::info("Image selection cancelled");
                    return;
                }

                auto path = selected.value();

                if (path.extension() != ".png" &&
                    path.extension() != ".PNG") {
                    Notification::create(
                        "Please select a PNG image",
                        NotificationIcon::Warning
                    )->show();
                    return;
                }

                CCImage image;

                if (!image.initWithImageFile(path.string().c_str())) {
                    log::error("Failed to load image");
                    return;
                }

                log::info(
                    "Loaded image: {}x{}",
                    image.getWidth(),
                    image.getHeight()
                );

                Notification::create(
                    fmt::format(
                        "Loaded {}x{} image",
                        image.getWidth(),
                        image.getHeight()
                    ),
                    NotificationIcon::Success
                )->show();

                // NEXT STAGE:
                // Convert image pixels into GD objects.
            }
        );
    }

public:
    static ImageImportTab* create() {
        auto ret = new ImageImportTab();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(MyEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        alpha::editor_tabs::addTab(
            "fill-this"_spr,
            alpha::editor_tabs::BUILD,

            []() -> CCNode* {
                return ImageImportTab::create();
            },

            []() -> CCSprite* {
                return CCSprite::createWithSpriteFrameName(
                    "edit_addBtn_001.png"
                );
            }
        );

        return true;
    }
};

