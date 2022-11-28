// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "app_window.hpp"
#include "counted_ptr.hpp"
#include "time.hpp"
#include "keyboard.hpp"

namespace wdul
{
	/// <summary>Event types for a <c>game_window_event</c>.</summary>
	enum class game_window_event_type : std::uint8_t
	{
		/// <summary>Invalid event type.</summary>
		nul,

		/// <summary>A request has been made to toggle fullscreen.</summary>
		toggle_fullscreen,

		/// <summary>The DPI has changed for the game window.</summary>
		dpi_changed,
	};

	template <game_window_event_type>
	struct game_window_event_context
	{
	};

	/// <summary>Arguments for the <c>game_window_event_type::toggle_fullscreen</c> event.</summary>
	template <>
	struct game_window_event_context<game_window_event_type::toggle_fullscreen>
	{
		/// <summary>
		/// If the event is handled, set this field to <c>true</c>.
		/// If this field is <c>false</c> (the default), the default behaviour is performed.
		/// </summary>
		bool handled = false;
	};

	/// <summary>Arguments for the <c>game_window_event_type::dpi_changed</c> event.</summary>
	template <>
	struct game_window_event_context<game_window_event_type::dpi_changed>
	{
	};

	/// <summary>Holds the type of event and event-specific arguments for a game window event.</summary>
	class game_window_event
	{
	public:
		explicit game_window_event(game_window_event_type const Type = game_window_event_type::nul, void* const Context = nullptr) noexcept :
			type(Type),
			context(Context)
		{
		}

		/// <summary>Casts the context pointer to a pointer to a <c>game_window_event_context&lt;<typeparamref name="Type"/>&gt;</c>.</summary>
		/// <typeparam name="Type">The event type.</typeparam>
		/// <returns>The casted context pointer, which is a pointer to the structure associated with the event type.</returns>
		/// <remarks>In debug mode, this function asserts <typeparamref name="Type"/> compares equal to the <c>type</c> field.</remarks>
		template <game_window_event_type Type>
		[[nodiscard]] auto as() const noexcept
		{
			WDUL_ASSERT(type == Type);
			return static_cast<game_window_event_context<Type>*>(context);
		}

		/// <summary>The event type.</summary>
		game_window_event_type type;

		/// <summary>Pointer to arguments specific to the event type.</summary>
		void* context;
	};

	class game_window;

	/// <summary>Handles game window events.</summary>
	class game_window_handler : public app_window_handler
	{
	public:
		game_window_handler(game_window_handler const&) = delete;
		game_window_handler(game_window_handler&&) = delete;
		game_window_handler& operator=(game_window_handler const&) = delete;
		game_window_handler& operator=(game_window_handler&&) = delete;

		explicit game_window_handler(game_window& GameWindow);

		virtual void on_game_event(game_window_event&) {}
		virtual void on_game_tick() {}
		virtual void draw_game(float /*Interpolation*/) {}
		virtual void resize_game() {}

		game_window& get_game_window() noexcept
		{
			return mGw;
		}

		game_window const& get_game_window() const noexcept
		{
			return mGw;
		}

	private:
		void on_event(app_window_event&) override;
		void resized(wm_size const&) override;
		menu_char_result on_menu_char(wm_menuchar const&) override;

		game_window& mGw;
	};

	class game_window
	{
		friend game_window_handler;
	public:
		game_window(game_window const&) = delete;
		game_window(game_window&&) = delete;
		game_window& operator=(game_window const&) = delete;
		game_window& operator=(game_window&&) = delete;

		/// <summary>Constructs the game window, which involves registering a window class and showing the window.</summary>
		/// <param name="Hinstance">The <c>hInstance</c> argument from the program entry point.</param>
		/// <param name="ShowCmd">The <c>nCmdShow</c> argument from the program entry point.</param>
		/// <param name="Title">A pointer to a null-terminated UTF-16 string containing the name of the game window.</param>
		/// <remarks>There should only be one game window per program.</remarks>
		game_window(
			_In_ HINSTANCE const Hinstance,
			std::int32_t const ShowCmd,
			_In_z_ wchar_t const* const Title
		);

		/// <summary>
		/// Runs the game loop, which involves message processing, game updates, and drawing (rendering).
		/// Game updates, or game ticks, occur at a fixed rate defined by the game speed.
		/// <para><c>run_game_loop</c> should only be called once for the game window.</para>
		/// </summary>
		/// <remarks>
		/// This function is blocking and does not return until the game loop has finished.
		/// The game loop finishes when <c>WM_QUIT</c> is detected in the message loop.
		/// <c>WM_QUIT</c> is sent when the window is destroyed.
		/// </remarks>
		std::int32_t run_game_loop();

		/// <summary>Sets the game ticks per second <c>run_game_loop</c> attempts to maintain.</summary>
		/// <param name="TicksPerSec">The target number of game ticks to perform in a second.</param>
		/// <remarks>By default, game windows use 25 game ticks per second.</remarks>
		void set_game_speed(float const TicksPerSec) noexcept;

		/// <summary>Gets the game speed.</summary>
		/// <returns>The target number of game ticks to perform in a second.</returns>
		[[nodiscard]] auto get_game_speed() const noexcept
		{
			return mTargetGameTicksPerSec;
		}

		/// <summary>Sets the game window handler which handles game window events.</summary>
		/// <param name="Handler">Reference to a smart pointer which points to the new game window handler.
		/// The smart pointer must not compare equal to nullptr.
		/// After this function has finished, the referenced smart pointer will compare equal to nullptr.</param>
		void set_handler(std::unique_ptr<game_window_handler>&& Handler)
		{
			mWindow.set_handler(std::move(Handler));
		}

		/// <summary>Constructs and sets the game window handler which handles game window events.</summary>
		/// <typeparam name="T">The game window handler class derived from <c>wdul::game_window_handler</c>.</typeparam>
		/// <typeparam name="...ArgsT">The types of arguments to pass to the constructor of the game window handler.</typeparam>
		/// <param name="...Args">Arguments to pass to the constructor of the game window handler.</param>
		/// <returns>A pointer to the new game window handler.</returns>
		template <class HandlerT, class... ArgsT>
		HandlerT* set_handler(ArgsT&&... Args)
		{
			auto const tmp = new HandlerT(std::forward<ArgsT>(Args)...);
			mWindow.set_handler(std::unique_ptr<game_window_handler>(tmp));	
			return tmp;
		}

		/// <summary>Toggles borderless windowed fullscreen.</summary>
		/// <returns>True if and only if the window is now borderless windowed fullscreen.</returns>
		bool toggle_fullscreen()
		{
			return mWindow.toggle_fullscreen();
		}

		/// <summary>Gets the cached QPC frequency.</summary>
		/// <returns>The frequency of the performance counter, in counts per second.</returns>
		[[nodiscard]] auto get_qpc_freq() const noexcept { return mCountsPerSec; }

		[[nodiscard]] auto const& get_class() const noexcept { return mWindow.get_class(); }

		[[nodiscard]] auto get_hwnd() const noexcept { return mWindow.get_hwnd(); }

		[[nodiscard]] auto get_handler() const noexcept { return static_cast<game_window_handler*>(mWindow.get_handler()); }

		[[nodiscard]] auto get_dpi() const noexcept { return mWindow.get_dpi(); }

		[[nodiscard]] auto const& get_client_size_in_pixels() const noexcept { return mWindow.get_client_size_in_pixels(); }

		[[nodiscard]] auto const& get_client_size_in_dips() const noexcept { return mWindow.get_client_size_in_dips(); }

		[[nodiscard]] auto const& get_mouse_pos_in_pixels() const noexcept { return mWindow.get_mouse_pos_in_pixels(); }

		[[nodiscard]] auto const& get_mouse_pos_in_dips() const noexcept { return mWindow.get_mouse_pos_in_dips(); }

	private:
		void handle_accelerator(app_window_event_context<app_window_event_type::accelerator>& Args);
		void resize(wm_size const& Args);

		enum class AccelIDs : std::uint16_t
		{
			toggle_fullscreen,
		};

		// Instance handle from the program entry point.
		HINSTANCE mHinst;

		// Cache QPC frequency.
		std::int64_t mCountsPerSec = get_performance_counts_per_sec();

		app_window mWindow;
		accelerator_table_handle mAccelTable;
		float mTargetGameTicksPerSec;
		std::int64_t mTargetCountsPerGameTick;
		float mDrawInterpolation;

#ifdef _DEBUG
		bool mGameLoopHasBeenCalled = false;
		bool mGameLoopCurrentlyRunning = false;
#endif
	};
}
