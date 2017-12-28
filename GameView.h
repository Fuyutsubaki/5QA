#pragma once

#include<Siv3D.hpp>
#include"Asset.hpp"
#include"GameModel.hpp"
#include "GameSceneMsg.hpp"

namespace gameview_detail {

	class ObjectBase {
	public:
		virtual ~ObjectBase() {}
		void draw() const {
			Transformer2D local{ s3d::Mat3x2::Identity().rotated(rot, rect.size / 2).translated(rect.tl()) };
			draw_impl();
		}

		s3d::Array<gamescene::CtrlMsg> check_input()const {
			Transformer2D local{ s3d::Mat3x2::Identity().rotated(rot, rect.size / 2).translated(rect.tl()),true };
			return input_impl();
		}

		bool is_alive()const { return alive; }
		ObjectBase(s3d::String const&id) :ObjectBase(id, { 0,0,0,0 }) {}
		ObjectBase(s3d::String const&id, s3d::Rect const& rect_) :m_id(id), rect(rect_), rot(0) {}
		ObjectBase(s3d::String const&id, s3d::Rect const& rect_, double rot_) :m_id(id), rect(rect_), rot(rot_) {}

		void setPos(s3d::Point const&pos) {
			rect.setPos(pos);
		}
		s3d::Rect getRect()const {
			return rect;
		}

		s3d::String const id()const {
			return m_id;
		}

	protected:
		s3d::Rect localRect()const {
			return s3d::Rect{ rect.size };
		}
		virtual void draw_impl()const = 0;
		virtual s3d::Array<gamescene::CtrlMsg> input_impl()const { return {}; }
		bool alive = true;
		s3d::Rect rect;
		double rot = 0;
	private:
		s3d::String m_id;
	};

	class Card
		:public ObjectBase
	{
		gamedat::Card dat;
		s3d::String player;

		s3d::Array<gamescene::CtrlMsg> input_impl()const;

		void draw_impl() const override;

	public:
		Card(gamedat::Card const& dat, s3d::String const&player, s3d::Rect const&rect)
			:ObjectBase(dat.id, rect), dat(dat), player(player)
		{}
	};

	// hand
	class PlayerHand
		:public ObjectBase
	{
		bool enable_draw = true;
	public:
		// todo カプセル化
		s3d::Array<Card> hands;

		using ObjectBase::ObjectBase;
		void draw_impl() const override;
		void update();
		void set_enable_draw(bool r) {
			enable_draw = r;
		}
	private:
		s3d::Array<gamescene::CtrlMsg> input_impl()const {
			for (auto it = hands.rbegin(); it != hands.rend(); ++it) {
				auto msg_list = it->check_input();
				if (msg_list) {
					return msg_list;
				}
			}
			return {};
		}
	};

	// 捨て札viewer
	class Discard
		:public ObjectBase
	{
		struct Dat{
			gamedat::Card::Type card;
			int n;
			int max;
		};
		s3d::Array<Dat> list;
		int discarable_color_num = 0;;
	public:
		using ObjectBase::ObjectBase;
		// 全体のカードについての情報が必要
		void draw_impl() const override;
		void setCardList(s3d::Array<gamedat::Card> const&card_list);
		void addCard(gamedat::Card const&card);
		void set_discardable_color_num(int n) {
			discarable_color_num = n;
		}
	};

	class ResultWindow
		:public ObjectBase
	{
		struct PlayerInfo
		{
			s3d::String name;
			s3d::Array<int> point_list;
			int sum_point;
		};
		s3d::Array<PlayerInfo> pointlist;
		s3d::String text;
		s3d::Rect ok_button;
		void draw_impl()const override;
		s3d::Array<gamescene::CtrlMsg> input_impl()const override;

	public:
		ResultWindow(s3d::String const&id, s3d::Rect const&rect, s3d::Rect const& ok)
			:ObjectBase(id, rect), ok_button{ ok }
		{		}
		void add_player(PlayerInfo const&player) {
			pointlist.push_back(player);
		}
		void add_text(s3d::String const&text_) {
			text = text_;
		}
	};
}



class GameView {
public:
	void init();
	void check_input() {
		ctrl_msg_list = util::flatmap(hand_list, [](auto const&hand) {return hand.check_input(); });
		if (pop_gameresult) {
			ctrl_msg_list.append(result_window->check_input());
		}
	}
	s3d::Array<gamescene::CtrlMsg> const& get_ctrl_msg_list()const {
		return ctrl_msg_list;
	}

	void update() {
		for (auto&e : hand_list) {
			e.update();
		}
	}

	void draw()const {
		discard->draw();
		for (auto&e : hand_list) {
			e.draw();
		}
		if (pop_gameresult) {
			result_window->draw();
		}
	}

	void apply_msg(s3d::Array<gamescene::GameModelMsg> const&msg_list) {
		for (auto&msg : msg_list) {
			std::visit([&](auto const&t) {
				apply_msg_impl(t);
			}, msg);
		}
	}

	GameView(std::shared_ptr<gamedat::GameData> const&dat)
		:dat(dat)
	{
		init();
	}
private:
	s3d::Array<gameview_detail::PlayerHand> hand_list;
	std::unique_ptr<gameview_detail::Discard> discard;

	s3d::Size card_size;

	s3d::Array<gamescene::CtrlMsg> ctrl_msg_list;
	std::unique_ptr<gameview_detail::ResultWindow> result_window;
	std::shared_ptr<gamedat::GameData> dat;

	bool pop_gameresult = false;

	void apply_msg_impl(gamescene::AddHand const&msg) {
		util::at_obj(hand_list, msg.player_id).hands.push_back(gameview_detail::Card{ dat->get_card(msg.card_id), msg.player_id,s3d::Rect{  card_size } });
	}

	void apply_msg_impl(gamescene::RemoveHand const&msg) {
		util::at_obj(hand_list, msg.player_id).hands.remove_if([&](auto const&c) {return c.id() == msg.card_id; });
	}

	void apply_msg_impl(gamescene::AddDiscard const&msg) {
		discard->addCard(dat->get_card(msg.card_id));
	}

	void apply_msg_impl(gamescene::StartGame const&msg);
	void apply_msg_impl(gamescene::EndGame const&msg);

	template<class T>
	void apply_msg_impl(T) {	}

	void apply_msg_impl(gamescene::UpdateDrawnPlayerList const&msg) {
		for (auto &p : hand_list) {
			p.set_enable_draw(! msg.playerd_player_id_list.include(p.id()));
		}
	}
};

