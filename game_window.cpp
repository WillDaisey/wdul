// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/


#include "include/wdul/game_window.hpp"
#include <cmath>

namespace wdul
{
	game_window_handler::game_window_handler(game_window& GameWindow) :
		mGw(GameWindow)
	{
	}

	void game_window_handler::resized(wm_size const& Args)
	{
		mGw.resize(Args);
	}

	menu_char_result game_window_handler::on_menu_char(wm_menuchar const&)
	{
		return { menu_char_cmd::ignore, 0 };
	}

	void game_window_handler::on_event(app_window_event& Ev)
	{
		switch (Ev.type)
		{
		case app_window_event_type::nul:
			break;
		case app_window_event_type::close_request:
			break;
		case app_window_event_type::dpi_changed:
		{
			game_window_event_context<game_window_event_type::dpi_changed> args;
			game_window_event ev(game_window_event_type::dpi_changed, &args);
			mGw.get_handler()->on_game_event(ev);
			break;
		}
		case app_window_event_type::menu:
			break;
		case app_window_event_type::accelerator:
			mGw.handle_accelerator(*Ev.as<app_window_event_type::accelerator>());
			break;
		default:
			break;
		}
	}

	game_window::game_window(_In_ HINSTANCE const Hinstance,
		std::int32_t const ShowCmd,
		_In_z_ wchar_t const* const Title) :
		mHinst(Hinstance)
	{
		leave_uninitialized(mDrawInterpolation);
		leave_uninitialized(mTargetGameTicksPerSec);
		leave_uninitialized(mTargetCountsPerGameTick);
		set_game_speed(25.f);

		mWindow.register_class(mHinst, L"Game Window");

		mWindow.set_handler<game_window_handler>(*this);
		mWindow.create(ShowCmd, Title);

		ACCEL accels[] = {
			make_accelerator(VK_RETURN, to_underlying(AccelIDs::toggle_fullscreen), accelerator_flags::virtual_key | accelerator_flags::alt)
		};
		mAccelTable = create_accelerator_table(accels);
	}

	std::int32_t game_window::run_game_loop()
	{
#ifdef _DEBUG
		WDUL_ASSERT_MSG(!mGameLoopHasBeenCalled, "run_game_loop has already been called for the game window");
		mGameLoopHasBeenCalled = true;
		mGameLoopCurrentlyRunning = true;
		auto currentlyRunningGuard = finally_always([&]() { mGameLoopCurrentlyRunning = false; });
#endif

		MSG msg;
		auto nextTick = get_performance_counts();
		while (true)
		{
			for (std::uint8_t consecutiveTicks = 0; get_performance_counts() >= nextTick && consecutiveTicks < 20; ++consecutiveTicks)
			{
				while (peek_window_message(&msg))
				{
					if (msg.message == WM_QUIT)
					{
						mWindow.check_procedure();
						return static_cast<std::int32_t>(msg.wParam);
					}
					if (!TranslateAcceleratorW(mWindow.get_hwnd(), mAccelTable.get(), &msg))
					{
						TranslateMessage(&msg);
						DispatchMessageW(&msg);
					}
				}
				get_handler()->on_game_tick();
				nextTick += mTargetCountsPerGameTick;
			}
			mDrawInterpolation = static_cast<float>(get_performance_counts() + mTargetCountsPerGameTick - nextTick) / static_cast<float>(mTargetCountsPerGameTick);
			get_handler()->draw_game(mDrawInterpolation);
			dirty_window(mWindow.get_hwnd());
			UpdateWindow(mWindow.get_hwnd());
		}
	}

	void game_window::set_game_speed(float const TicksPerSec) noexcept
	{
		mTargetGameTicksPerSec = TicksPerSec;
		mTargetCountsPerGameTick = std::llround(static_cast<double>(mCountsPerSec) / static_cast<double>(mTargetGameTicksPerSec));
	}

	void game_window::handle_accelerator(app_window_event_context<app_window_event_type::accelerator>& Args)
	{
		if (Args.id == to_underlying(AccelIDs::toggle_fullscreen))
		{
			toggle_fullscreen();
		}
	}

	void game_window::resize(wm_size const&)
	{
		get_handler()->resize_game();
	}
}
