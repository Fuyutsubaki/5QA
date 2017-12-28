# include <Siv3D.hpp>
#include "GameModel.hpp"
#include "GameView.h"
#include "Scene.hpp"
#include "GameScene.hpp"
#include "TitleScene.hpp"


void Main()
{
	s3d::Window::Resize({ 800,450 });
	s3d::Window::SetTitle(L"BASY! (babanuki no syuusaku)");
	scene::RootScene scene;
	scene
		.add<TitleScene>(L"Title")
		.add<scene::GameScene>(L"Game")
		;
	
	s3d::Graphics::SetBackground(asset::Palette::base_blue);
	while (System::Update())
	{
		scene.updateScene();
		scene.drawScene();
	}
}