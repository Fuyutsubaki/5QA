#include "GameView.h"


s3d::String to_str(gamedat::Card::Color c) {
	switch (c) {
	case gamedat::Card::Color::red:
		return L"Red";
	case gamedat::Card::Color::green:
		return L"Green";
	case gamedat::Card::Color::blue:
		return L"Blue";
	case gamedat::Card::Color::orange:
		return L"Orange";
	}
}

void GameView::init() {
	hand_list.clear();
	discard = nullptr;

	s3d::JSONReader conf{ L"dat/gameview.json" };

	auto list = conf[L"hands"];
	for (auto e : list.arrayView()) {

		auto id = e[L"id"].getString();
		auto w = e[L"wh"].arrayView()[0].get<int>();
		auto h = e[L"wh"].arrayView()[1].get<int>();
		auto x = e[L"at"].arrayView()[0].get<int>();
		auto y = e[L"at"].arrayView()[1].get<int>();
		int kodo = e[L"rot"].get<int>();

		hand_list.push_back(gameview_detail::PlayerHand{ id, s3d::Rect{ x - w / 2,y - h / 2,w,h }, kodo / 360. * 2_pi });
	}
	{
		auto rect = conf[L"discard"][L"rect"].arrayView();
		discard = std::make_unique<gameview_detail::Discard>(L"discard", s3d::Rect{ rect[0].get<int>(),rect[1].get<int>(),rect[2].get<int>(),rect[3].get<int>() });
		discard->setCardList(dat->get_card_list());
	}


	// card size
	{
		auto wh = conf[L"hand-cardsize"].arrayView();
		card_size = {wh[0].get<int>(), wh[1].get<int>()};
	}

	//result window
	{
		auto rect = conf[L"result"][L"rect"].arrayView();
		auto ok = conf[L"result"][L"ok"].arrayView();

		s3d::Rect rect_tmp{ rect[0].get<int>(),rect[1].get<int>(),rect[2].get<int>(),rect[3].get<int>() };
		s3d::Rect ok_tmp{ ok[0].get<int>(),ok[1].get<int>(),ok[2].get<int>(),ok[3].get<int>() };
		result_window = std::make_unique<gameview_detail::ResultWindow>(L"result", rect_tmp, ok_tmp);
	}
}

void GameView::apply_msg_impl(gamescene::StartGame const & msg) {
	discard->set_discardable_color_num(msg.discardable_color_num);
}

void GameView::apply_msg_impl(gamescene::EndGame const & msg) {
	for (auto p : msg.player_info_list) {
		result_window->add_player({ p.name,p.point_list,p.point_list.sum() });
	}
	auto text1 =
		util::visit(msg.kimarite)(
		[](gamescene::EndGame::Handless const&msg) {return msg.name + L"の手札がなくなりました"; },
		[](gamescene::EndGame::AllDiscard const&msg) {
		return L"{}色の数字カード{}がすべて捨てられました"_fmt(msg.color_list.size(), msg.color_list.map([](auto c) {return to_str(c); }).join());
	},
		[](gamescene::EndGame::Basy const&msg) {return L"{}がBASYのカードを集めました"_fmt(msg.name); }
	);
	auto text2 = [&]()->s3d::String {
		if (msg.gameover) {
			auto list = msg.player_info_list.map([](auto const&p) {return std::make_pair(p.name, p.point_list.sum()); })
				.sorted_by([](auto const&lhs, auto const&rhs) {return lhs.second < rhs.second; });

			int rank = 42;
			for (int i = 0; i < list.size(); ++i) {
				if (list[i].first == L"player")
					rank = i;
			}
			return  L"\nあなたは{}位でした。 ゲームを終了します"_fmt(rank + 1);
		}
		else
		{
			return L"";
		}
	}();
	
	result_window->add_text(text1 + text2);

	pop_gameresult = true;
}
namespace gameview_detail {

	s3d::Color to_s3dcolor(gamedat::Card::Color c) {
		switch(c){
		case gamedat::Card::Color::red:
			return asset::Palette::red;
		case gamedat::Card::Color::green:
			return asset::Palette::green;
		case gamedat::Card::Color::blue:
			return asset::Palette::blue;
		case gamedat::Card::Color::orange:
			return asset::Palette::orange;
		}
	}


	void PlayerHand::draw_impl() const {
		s3d::Rect{ rect.size }.drawFrame();
		for (auto&e : hands) {
			e.draw();
		}
		if (!enable_draw) {
			localRect().drawFrame(2.0, s3d::Palette::Red);
			s3d::Line{ localRect().tl() ,localRect().br() }.draw(2.0, s3d::Palette::Red);
		}

	}

	void PlayerHand::update() {
		int max_w = rect.w;
		int sum_w = hands.map([](Card const&c) {return c.getRect().w; }).sum();
		if (sum_w < max_w) {
			int tmp = 0;
			for (auto &e : hands) {
				e.setPos({ tmp,0 });
				tmp += e.getRect().w;
			}
		}
		else {
			int back_w = hands.back().getRect().w;
			std::size_t n = hands.size() - 1;
			hands.each_index([&](std::size_t idx, auto&e) {
				int x = static_cast<int>(((double) idx / n) * (max_w - back_w));
				e.setPos({ x,0 });
			});
		}
	}

	s3d::Array<gamescene::CtrlMsg> Card::input_impl() const {
		if (localRect().leftClicked()) {
			return { gamescene::ClickObject::ctor(id(), s3d::Array<s3d::String>{ {L"hands",player,id()} }) };
		}
		else {
			return {};
		}
	}

	void Card::draw_impl() const {
		localRect().draw(to_s3dcolor(dat.color));
		localRect().drawFrame();
		if (player == L"player") {
			util::visit(dat.type)(
				[&](gamedat::Card::Number const&num) {
				asset::Font::me15()(num.num).drawAt(localRect().center(), s3d::Palette::Black);
			},
				[&](gamedat::Card::Alpha const&a) {
				asset::Font::me15()(a.word).drawAt(localRect().center(),s3d::Palette::Black);
			}
			);
		}
	}

	// 全体のカードについての情報が必要

	void Discard::draw_impl() const {
		localRect().drawFrame();
		asset::Font::me15()(L"捨て札").draw(localRect().tl()).drawFrame();
		
		{
			s3d::String text =
				L"---ラウンド終了条件---\n" +
				L"「全ての数字カードが捨てられている色」が{}色以上になる\n"_fmt(discarable_color_num) +
				L"「B」「A」「S」「Y」のカードを手札に集める\n"
				L"誰かの手札がなくなる";
			asset::Font::me12()(text).draw(s3d::Arg::bottomRight(localRect().br())).drawFrame();
		}


		{
			s3d::Array<std::pair<const gamedat::Card::Color, s3d::Array<Dat> >> color_group = 
				util::make_vector(util::group_by(list, [](auto const&t) {return t.card.color;  }));

			auto num_grid = color_group.map([](auto const&a) {
				auto[color, v] = a;

				s3d::Array<s3d::String> tmp;
				tmp.push_back(to_str(color));
				tmp.append(
					util::flatmap(
						v, [&](auto const&t) {
					return util::visit(t.card.type)(
						[&](gamedat::Card::Number const&n)->s3d::Array<s3d::String> {return { L"{}x{}/{}"_fmt(n.num, t.n, t.max) }; },
						[&](gamedat::Card::Alpha const&) ->s3d::Array<s3d::String> {return {}; }
					);
				}));
				return tmp;
			});
			auto alpha_grid = color_group.map([](auto const&a) {
				auto[color, v] = a;

				s3d::Array<s3d::String> tmp;
				tmp.push_back(to_str(color));
				tmp.append(
					util::flatmap(
						v, [&](auto const&t) {
					return util::visit(t.card.type)(
						[&](gamedat::Card::Number const&)->s3d::Array<s3d::String> {return {}; },
						[&](gamedat::Card::Alpha const&a) ->s3d::Array<s3d::String> {return { a.word }; }
					);
				}));
				return tmp;
			});
			s3d::Array<s3d::Array<s3d::String>> tmp;
			tmp.append(num_grid);
			tmp.append(alpha_grid);

			tmp.each_index([](int y, auto const&line) {
				line.each_index([&](int x, auto const&s) {
					asset::Font::me15()(s).draw(60 * x, 40 + 15 * y);
				});
			});

		}
	}

	void Discard::setCardList(s3d::Array<gamedat::Card> const & card_list) {
		list = {};

		auto group = util::group_by(card_list, [](gamedat::Card const&e) {return e.card_type(); });
		for (auto[k, v] : group) {
			list.push_back(Dat{ k , 0,(int) v.size() });
		}
	}

	void Discard::addCard(gamedat::Card const & card) {
		auto it = std::find_if(list.begin(), list.end(), [&](Dat const&c) {return c.card == card.card_type(); });
		if (it != list.end()) {
			it->n += 1;
		}
	}

	void ResultWindow::draw_impl() const {
		localRect().drawShadow({ 3,3 }, 3.5);

		ok_button.drawFrame();
		asset::Font::me15()(L"OK").drawAt(ok_button.center());
		
		s3d::Array<s3d::Array<s3d::String>> dat =
			pointlist.map([](auto const&p) {
			s3d::Array<s3d::String> tmp;
			tmp.push_back(p.name);
			tmp.append(p.point_list.map([](auto i) {return s3d::ToString(i); }));
			tmp.push_back(s3d::ToString(p.point_list.sum()));
			return tmp;
		});

		s3d::Array<s3d::String> head = {L""};
		head.append(step(1, dat[0].size() - 2).map([](int n) {return L"{}戦目"_fmt(n); }));
		head.push_back(L"合計");
		
		int size = dat.size();
		int centerx = localRect().center().x;
		int topy = 40;
		const int gridw = 60;
		const int gridy = 20;
		s3d::Array<s3d::Array<s3d::String>> tmp;
		tmp.push_back(head);
		tmp.append(dat);

		tmp.each_index([&](int x, auto const&line) {
			line.each_index([&](int y, auto const&s) {
				asset::Font::me15()(s).draw((centerx - gridw*size / 2) + gridw * x, topy + gridy * y);
			});
		});

		asset::Font::me15()(text).drawAt(centerx, 300);

	}

	s3d::Array<gamescene::CtrlMsg> ResultWindow::input_impl() const {
		if (ok_button.leftClicked())
			return { gamescene::ClickObject::ctor(L"result", s3d::Array<s3d::String>{L"result", L"OK" }) };
		else
			return {};
	}
}