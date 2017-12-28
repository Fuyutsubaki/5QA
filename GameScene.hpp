#pragma once
#include<Siv3D.hpp>
#include <HamFramework.hpp>

#include"Scene.hpp"
#include "GameView.h"
#include "GameSceneMsg.hpp"
#include"GameModel.hpp"
#include "GameModelCPU.hpp"

namespace scene {
	class SceneModel {
		std::shared_ptr<gamedat::GameData> dat;
		GameModel model;
		s3d::Array<std::shared_ptr<GameCPUPlayer>> cpu_player_list;
		bool flag_need_newgame = false;
		bool flag_gameover = false;

	public:
		SceneModel(std::shared_ptr<gamedat::GameData> const&dat) :
			dat(dat),
			model(dat),
			cpu_player_list({
			std::make_shared<GameCPUPlayer>(L"1"),
			std::make_shared<GameCPUPlayer>(L"2"),
			std::make_shared<GameCPUPlayer>(L"3")
		})
		{}
		void apply_msg(s3d::Array<gamescene::CtrlMsg> const&list) {

			model.apply_msg(list);

			bool flag = list.include_if([](auto const&msg) {
				auto click = std::get_if<gamescene::ClickObject>(&msg);
				return click && (click->info == decltype(click->info)({ L"result", L"OK" }));
			});

			if (flag)
			{
				flag_need_newgame = true;
			}
			if (flag && model.gameover()) {
				flag_gameover = true;
			}
		}
		void update();
		s3d::Array<gamescene::GameModelMsg>const& get_msg_list() {
			return model.get_msg_list();
		}
		void setup_update() {
			model.setup_update();
		}
		bool need_newgame()const { return flag_need_newgame; }
		bool gameover()const { return flag_gameover; }
		void start_newgame() {
			model.start_nextgame();
			flag_need_newgame = false;
		}
	};


	class GameScene 
		:public RootScene::Scene
	{
	public:
		GameScene(const InitData& init)
			: IScene(init), 
			dat(gamedat::GameData::make()),
			view(dat),
			model(dat)
		{		}

		void update() override{
			if (model.need_newgame()) {
				view = GameView{ dat };
				model.start_newgame();
			}
			else if (model.gameover()) {
				changeScene(L"Title");
			}
			else
			{
				// input
				view.check_input();

				auto ctrl_msg_list = view.get_ctrl_msg_list();
				auto model_msg_list = model.get_msg_list();
				model.setup_update();
				// model 
				{
					model.apply_msg(ctrl_msg_list);
					model.update();
				}
				// output 
				view.apply_msg(model_msg_list);
				view.update();
			}

		}
		void draw()const override {
			view.draw();
		}
	private:
		std::shared_ptr<gamedat::GameData> dat;
		GameView view;
		SceneModel model;
		s3d::Array<std::shared_ptr<GameCPUPlayer>> cpu_player_list;
	};
}