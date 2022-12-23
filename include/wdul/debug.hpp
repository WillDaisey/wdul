// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "utility.hpp"
#include <memory>

#ifdef _DEBUG
namespace wdul::debug
{
	/// <summary>Maximum number of characters a facility name can be.</summary>
	inline constexpr std::size_t max_facility_name_length = 15;

	/// <summary>Maximum number of characters a category name can be.</summary>
	inline constexpr std::size_t max_category_name_length = 15;

	/// <summary>Identifies a facility.</summary>
	enum class facility : std::uint32_t
	{
		unknown,
	};

	/// <summary>Identifies a category.</summary>
	using category = std::uint32_t;

	/// <summary>Indicates the severity of a message.</summary>
	enum class severity : std::uint8_t
	{
		/// <summary>An informative message. When set as a severity threshold, all messages will be output.</summary>
		info,

		/// <summary>A warning message. When set as a severity threshold, only warning and error messages will be output.</summary>
		warn,

		/// <summary>An error message. When set as a severity threshold, only error messages will be output.</summary>
		error,
	};

	/// <summary>Describes how to handle debug output.</summary>
	struct category_options
	{
		/// <summary>The severity which must be met for output.</summary>
		severity severity_threshold = severity::warn;

		/// <summary>Whether to call <c>__debugbreak</c> after error messages (true) or not (false).</summary>
		bool break_on_errors = true;
	};

	/// <summary>Identifies a debug output sink.</summary>
	enum class sink : std::uint32_t
	{
		unknown,
	};

	/// <summary>Describes a debug message.</summary>
	struct message_desc
	{
		/// <summary>The facility to which the message belongs.</summary>
		facility facility;

		/// <summary>The category to which the message belongs.</summary>
		category category;

		/// <summary>The severity of the message.</summary>
		severity severity;

		/// <summary>
		/// Null-terminated char sequence specifying the name of the function from which the message originated, or <c>nullptr</c>.
		/// </summary>
		char const* function;

		/// <summary>
		/// Null-terminated char sequence specifying the message to output, or <c>nullptr</c>.
		/// </summary>
		char const* string;
	};

	/// <summary>Describes debug message output.</summary>
	struct message_output;

	/// <summary>An object which receives debug output.</summary>
	class sink_output
	{
	public:
		virtual void receive(message_output const&) = 0;
		virtual ~sink_output() = default;
	};

	/// <summary>Registers a facility.</summary>
	/// <param name="Name">
	/// A pointer to a null-terminated char sequence which specifies the name of the facility. Must not be <c>nullptr</c>.
	/// </param>
	/// <param name="Options">
	/// The default category options. The default category options can be changed with the <c>set_default_category_options</c> function.
	/// </param>
	/// <returns>The newly registered facility.</returns>
	[[nodiscard]] facility register_facility(_In_z_ char const* const Name, category_options const& Options = category_options());

	/// <summary>
	/// Sets the default category options for the given facility.<para/>
	/// By default, categories use the default category options provided by their facility.
	/// </summary>
	/// <param name="Facility">The facility to modify the default category options of.</param>
	/// <param name="Options">The new default category options.</param>
	void set_default_category_options(facility const Facility, category_options const& Options);
	
	/// <summary>
	/// Sets custom category options for a given category.<para/>
	/// Custom category options override the default category options provided by a facility.
	/// </summary>
	/// <param name="Facility">The facility to which the category belongs.</param>
	/// <param name="Category">The category to set the options of.</param>
	/// <param name="Options">The new category options for the given category.</param>
	void set_category_options(facility const Facility, category const Category, category_options const& Options);

	/// <summary>
	/// Removes custom category options from a given category.<para/>
	/// The given category will then inherit its options from the default category options provided by its facility.
	/// </summary>
	/// <param name="Facility">The facility to which the category belongs.</param>
	/// <param name="Category">The category to remove custom category options from.</param>
	void erase_category_options(facility const Facility, category const Category);

	/// <summary>
	/// Sets the name of a category.
	/// </summary>
	/// <param name="Facility">The facility to which the category belongs.</param>
	/// <param name="Category">The category to name.</param>
	/// <param name="Name">Null-terminated char sequence specifying the new name of the category. May be <c>nullptr</c>.</param>
	void set_category_name(facility const Facility, category const Category, _In_opt_z_ char const* const Name);

	// TODO: Document

	[[nodiscard]] message_desc const& get_message_desc(message_output const& MsgOutput) noexcept;

	[[nodiscard]] char const* get_message_facility_name(message_output const& MsgOutput) noexcept;

	[[nodiscard]] std::size_t get_message_facility_name_length(message_output const& MsgOutput) noexcept;

	[[nodiscard]] char const* get_message_category_name(message_output const& MsgOutput) noexcept;

	[[nodiscard]] std::size_t get_message_category_name_length(message_output const& MsgOutput) noexcept;

	[[nodiscard]] sink register_sink(std::unique_ptr<sink_output>&& Output);

	void unregister_sink(sink const Sink) noexcept;

	void output(facility const Facility, category const Category, severity const Severity, _In_opt_z_ char const* const Fn,
		_In_opt_z_ char const* const Msg);

	[[nodiscard]] sink get_default_sink() noexcept;

	namespace categories
	{
		/// <returns>The facility which WDUL uses to output debug messages.</returns>
		[[nodiscard]] facility get_facility() noexcept;

		//inline constexpr category general = 0;
		inline constexpr category assertions = 1;
		inline constexpr category close_handle = 2;
		inline constexpr category xaudio2 = 3;
		inline constexpr category strings = 4;
		inline constexpr category window = 5;
		inline constexpr category mf = 6;
	}
}

namespace wdul::impl
{
	[[noreturn]] void assertion_error(
		_In_opt_z_ char const* const Fn,
		_In_opt_z_ char const* const Expr,
		_In_opt_z_ char const* const Msg
	) noexcept;
}

#endif // _DEBUG

// WDUL_DEBUG_SWITCH expands to DebugCase if _DEBUG is defined, and expands to ReleaseCase if _DEBUG is not defined.
#ifdef _DEBUG
#define WDUL_DEBUG_SWITCH(DebugCase, ReleaseCase) DebugCase
#else
#define WDUL_DEBUG_SWITCH(DebugCase, ReleaseCase) ReleaseCase
#endif

// WDUL_ASSERT_MSG causes the program to terminate with a message if the given expression evaluates to false.
// If _DEBUG is not defined, WDUL_ASSERT_MSG expands to nothing.
// Expr: The expression to evaluate.
// Msg: A pointer to a null-terminated char sequence containing the message, or nullptr.
#define WDUL_ASSERT_MSG(Expr, Msg) WDUL_DEBUG_SWITCH((void)( (!!(Expr)) || (::wdul::impl::assertion_error(__func__, #Expr, Msg), 0) ), )

// Same as calling WDUL_ASSERT_MSG(Expr, nullptr).
#define WDUL_ASSERT(Expr) WDUL_ASSERT_MSG(Expr, nullptr)
