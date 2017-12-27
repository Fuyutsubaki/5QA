# include <Siv3D.hpp>
#include "GameModel.hpp"
#include "GameView.h"
#include "Scene.hpp"
#include "GameScene.hpp"
#include "TitleScene.hpp"
//#include <HamFramework\PlayingCard.hpp>
void Main()
{
	scene::RootScene scene;
	scene
		.add<TitleScene>(L"Title")
		.add<scene::GameScene>(L"Game")
		;

	s3d::Graphics::SetBackground(s3d::Palette::Blue);
	while (System::Update())
	{
		scene.updateScene();
		scene.drawScene();

	}
}