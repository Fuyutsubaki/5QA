#pragma once
#include"Scene.hpp"

class TitleScene
	:public scene::RootScene::Scene
{
	s3d::Rect start_button{ 80, 40 };
public:
	TitleScene(const InitData& init)
		: IScene(init)
	{
		start_button.setCenter(400, 400);
	}
	void update()override {
		if (start_button.leftClicked()) {
			changeScene(L"Game");
		}
	}

	void draw()const override {
		asset::Font::me15()(L"BASY").draw();
		start_button.drawFrame();
		asset::Font::me15()(L"‚Í‚¶‚ß‚é").drawAt(start_button.center());
	}
};
