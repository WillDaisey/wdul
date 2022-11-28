// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "window.hpp"
#include "window_message.hpp"
#include "menu.hpp"
#include <memory>

namespace wdul
{
	/// <summary>Event types for an <c>app_window_event</c>.</summary>
	enum class app_window_event_type : std::uint8_t
	{
		/// <summary>Invalid event type.</summary>
		nul,

		/// <summary>The window has been requested to close.</summary>
		close_request = 1,

		/// <summary>The effective DPI for the window was changed.</summary>
		dpi_changed,

		/// <summary>The user selected an item from a menu.</summary>
		menu,

		/// <summary>An accelerator keystroke was translated.</summary>
		accelerator,
	};

	/// <summary>Holds event-specific arguments. This class template is empty, but is specialised for certain event types.</summary>
	template <app_window_event_type>
	struct app_window_event_context
	{};

	/// <summary>Specialises the <c>app_window_event_context</c> class template for the <c>close_request</c> event type.</summary>
	template <>
	struct app_window_event_context<app_window_event_type::close_request>
	{
		/// <summary>
		/// If this field is set to <c>true</c> (the default), the window is destroyed.
		/// If this field is set to <c>false</c>, the window is not destroyed.
		/// </summary>
		bool destroy = true;
	};

	/// <summary>Specialises the <c>app_window_event_context</c> class template for the <c>dpi_changed</c> event type.</summary>
	template <>
	struct app_window_event_context<app_window_event_type::dpi_changed>
	{};

	/// <summary>Specialises the <c>app_window_event_context</c> class template for the <c>menu</c> event type.</summary>
	template<>
	struct app_window_event_context<app_window_event_type::menu>
	{
		/// <summary>The menu identifier.</summary>
		std::uint16_t id;
	};

	/// <summary>Specialises the <c>app_window_event_context</c> class template for the <c>accelerator</c> event type.</summary>
	template<>
	struct app_window_event_context<app_window_event_type::accelerator>
	{
		/// <summary>The accelerator identifier.</summary>
		std::uint16_t id;
	};

	/// <summary>
	/// Arguments for the <c>app_window_handler::on_event</c> function.
	/// Holds the event type and a pointer to arguments specific to the event type.
	/// </summary>
	class app_window_event
	{
	public:
		/// <summary>Constructs an <c>app_window_event</c>.</summary>
		/// <param name="Type">The event type. By default, this is set to <c>app_window_event_type::nul</c>.</param>
		/// <param name="Context">A pointer to arguments associated with the event type. By default, this is set to <c>nullptr</c>.</param>
		explicit app_window_event(app_window_event_type const Type = app_window_event_type::nul, void* const Context = nullptr) noexcept :
			type(Type),
			context(Context)
		{
		}

		/// <summary>Casts the context pointer to a pointer to an <c>app_window_event_context&lt;<typeparamref name="Type"/>&gt;</c>.</summary>
		/// <typeparam name="Type">The event type.</typeparam>
		/// <returns>The casted context pointer, which is a pointer to the structure associated with the event type.</returns>
		/// <remarks>In debug mode, this function asserts <typeparamref name="Type"/> compares equal to the <c>type</c> field.</remarks>
		template <app_window_event_type Type>
		[[nodiscard]] auto as() const noexcept
		{
			WDUL_ASSERT(type == Type);
			return static_cast<app_window_event_context<Type>*>(context);
		}

		/// <summary>The event type.</summary>
		app_window_event_type type;

		/// <summary>Pointer to an <c>app_window_event_context&lt;T&gt;</c> structure, where <c>T</c> is the event type.</summary>
		void* context;
	};

	/// <summary>Mouse button types.</summary>
	enum class mouse_button : std::uint8_t
	{
		/// <summary>The left mouse button.</summary>
		left,

		/// <summary>The right mouse button.</summary>
		right,

		/// <summary>The middle mouse button.</summary>
		middle,

		/// <summary>The first extended mouse button.</summary>
		x1,

		/// <summary>The second extended mouse button.</summary>
		x2,
	};

	/// <summary>
	/// Informs the system on how to handle a key press when a menu is active and the key does not correspond to a mnemonic or accelerator key.
	/// </summary>
	/// <remarks>Used as the return value for the <c>app_window_handler::on_menu_char</c> function.</remarks>
	struct menu_char_result
	{
		/// <summary>
		/// Tells the system how to handle the character and active menu:
		/// <para><c>menu_char_cmd::ignore</c> informs the system it should discard the character the user pressed and create a short beep on the system speaker.</para>
		/// <para><c>menu_char_cmd::close</c> informs the system it should close the active menu.</para>
		/// <para><c>menu_char_cmd::execute</c> informs the system it should execute the item specified by <c>item</c>.</para>
		/// <para><c>menu_char_cmd::select</c> informs the system it should choose the item specified by <c>item</c>.</para>
		/// </summary>
		menu_char_cmd cmd;

		/// <summary>Used when the <c>cmd</c> field is <c>menu_char_cmd::execute</c> or <c>menu_char_cmd::select</c>.</summary>
		std::uint16_t item;
	};

	/// <summary>Handles events for an <c>app_window</c>.</summary>
	struct app_window_handler
	{
		virtual ~app_window_handler() = default;

		/// <summary>
		/// Called when a request to paint a portion of the window has been made.
		/// Note this function is called between the <c>BeginPaint</c> and <c>EndPaint</c> functions.
		/// </summary>
		virtual void on_paint(window_paint_scope& /*Paint*/) {}

		/// <summary>Called when the size of the window has changed.</summary>
		virtual void resized(wm_size const&) {}

		/// <summary>Called when the cursor moves.</summary>
		virtual void mouse_moved(wm_mousemove const&) {}

		/// <summary>Called when a mouse button is down whilst the cursor is in the client area of the window.</summary>
		virtual void on_mouse_button_down(mouse_button, generic_mouse_args const&) {}

		/// <summary>Called when a mouse button is released whilst the cursor is in the client area of the window.</summary>
		virtual void on_mouse_button_up(mouse_button, generic_mouse_args const&) {}

		/// <summary>Called when the window has keyboard focus and a key is down whilst the ALT key is not down.</summary>
		virtual void on_key_down(wm_keydown const&) {}

		/// <summary>Called when the window has keyboard focus and a key is released whilst the ALT key is not down.</summary>
		virtual void on_key_up(wm_keyup const&) {}
		virtual bool on_key_down(wm_syskeydown const&) { return false; }
		virtual bool on_key_up(wm_syskeyup const&) { return false; }
		virtual menu_char_result on_menu_char(wm_menuchar const&) { return { menu_char_cmd::ignore, 0 }; }
		virtual void on_event(app_window_event&) {}
	};

	class app_window
	{
	public:
		app_window(app_window const&) = delete;
		app_window& operator=(app_window const&) = delete;
		app_window& operator=(app_window&&) = delete;
		app_window() noexcept = default;
		app_window(app_window&&) noexcept = default;
		~app_window();

		void register_class(_In_ HINSTANCE const InstanceHandle, _In_z_ wchar_t const* const ClassName, _In_opt_ HICON const IconHandle = nullptr, _In_opt_ HICON const IconHandleSm = nullptr);
		void create(std::int32_t const ShowCommand, _In_opt_z_ wchar_t const* const WindowName, menu_bar_handle&& MenuBar = menu_bar_handle());
		void destroy() noexcept;

		/// <summary>Sets the app window handler which handles app window events.</summary>
		/// <param name="Handler">Reference to a smart pointer which points to the new app window handler.
		/// The given smart pointer must not compare equal to nullptr.
		/// After this function has finished, the referenced smart pointer will compare equal to nullptr.</param>
		void set_handler(std::unique_ptr<app_window_handler>&& Handler) noexcept
		{
			WDUL_ASSERT(Handler != nullptr);
			mHandler = std::move(Handler);
		}

		/// <summary>Constructs and sets the app window handler which handles app window events.</summary>
		/// <typeparam name="T">The app window handler class derived from <c>wdul::app_window_handler</c>.</typeparam>
		/// <typeparam name="...ArgsT">The types of arguments to pass to the constructor of the app window handler.</typeparam>
		/// <param name="...Args">Arguments to pass to the constructor of the app window handler.</param>
		/// <returns>A pointer to the new app window handler.</returns>
		template <class HandlerT, class... ArgsT>
		HandlerT* set_handler(ArgsT&&... Args)
		{
			auto const tmp = new HandlerT(std::forward<ArgsT>(Args)...);
			mHandler.reset(tmp);
			return tmp;
		}

		// Propagates any exception caught in the window procedure.
		// This function should be called when WM_QUIT is detected to ensure exceptions are properly propagated.
		void check_procedure() const;

		/// <summary>Toggles borderless windowed fullscreen.</summary>
		/// <returns><c>true</c> if and only if the window entered borderless windowed fullscreen mode.</returns>
		bool toggle_fullscreen();

		/// <summary>Detects if the window is in fullscreen.</summary>
		/// <returns><c>true</c> if and only if the window is in fullscreen.</returns>
		/// <remarks>A window is in fullscreen if its client rectangle fills a display monitor.</remarks>
		bool is_fullscreen() const;

		[[nodiscard]] auto const& get_class() const noexcept
		{
			return mWindowClass;
		}

		[[nodiscard]] auto get_hwnd() const noexcept
		{
			return mHwnd;
		}

		[[nodiscard]] auto get_handler() const noexcept
		{
			return mHandler.get();
		}

		[[nodiscard]] auto get_dpi() const noexcept
		{
			return mDpi;
		}

		[[nodiscard]] auto const& get_client_size_in_pixels() const noexcept
		{
			return mClientSizeInPixels;
		}

		[[nodiscard]] auto const& get_client_size_in_dips() const noexcept
		{
			return mClientSizeInDips;
		}

		[[nodiscard]] auto const& get_mouse_pos_in_pixels() const noexcept
		{
			return mMousePosInPixels;
		}

		[[nodiscard]] auto const& get_mouse_pos_in_dips() const noexcept
		{
			return mMousePosInDips;
		}

	private:
		static LONG_PTR __stdcall window_procedure(HWND const, std::uint32_t const, std::uintptr_t const, LONG_PTR const) noexcept;
		window_class mWindowClass;
		std::exception_ptr mWndProcException;
		HWND mHwnd = nullptr;
		std::unique_ptr<app_window_handler> mHandler;
		float mDpi;
		vec2i mClientSizeInPixels;
		vec2f mClientSizeInDips;
		vec2i mMousePosInPixels;
		vec2f mMousePosInDips;
		WINDOWPLACEMENT mWindowPlacementRestore;
	};
}
