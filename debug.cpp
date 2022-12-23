// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/debug.hpp"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <Windows.h>

#ifdef _DEBUG
namespace wdul::debug
{
	[[nodiscard]] std::size_t strlen_c(_In_reads_or_z_opt_(Max) char const* const Str, std::size_t const Max) noexcept
	{
		std::size_t i;
		if (!Str) return 0;
		for (i = 0; i != Max && Str[i] != '\0'; ++i);
		return i;
	}

	struct category_name
	{
		category_name() noexcept :
			length(0)
		{
			data[0] = '\0';
		}

		explicit category_name(_In_opt_z_ char const* const Name) noexcept
		{
			length = static_cast<std::uint8_t>(strlen_c(Name, max_category_name_length));
			if (Name)
			{
				std::memcpy(data, Name, length);
			}
			data[length] = '\0';
		}

		// The number of characters in the data array, excluding the terminating null character.
		std::uint8_t length;

		// Null-terminated char sequence of the category name.
		char data[max_category_name_length + 1];
	};

	struct category_data
	{
		category_data() noexcept = default;

		explicit category_data(category_options const& Options) :
			options(Options),
			uses_default_options(false)
		{
		}

		explicit category_data(category_name const& Name) :
			name(Name)
		{
		}

		category_data(category_options const& Options, category_name const& Name) :
			options(Options),
			name(Name),
			uses_default_options(false)
		{
		}

		category_options options;
		category_name name;
		std::uint32_t u32 = 0;
		bool uses_default_options = true;
	};

	class facility_data
	{
	public:
		struct output_behavior
		{
			// Output the message?
			bool filter;

			// Call __debugbreak?
			bool debug_break;
		};

		facility_data(_In_opt_z_ char const* const Name, category_options const& DefaultOptions) :
			mDefaultOptions(DefaultOptions)
		{
			auto const nameLength = strlen_c(Name, max_facility_name_length);
			if (nameLength > max_facility_name_length)
			{
				throw std::length_error("facility name too long");
			}
			if (Name)
			{
				std::memcpy(mName, Name, nameLength);
			}
			mName[nameLength] = '\0';
		}

		void set_default_options(category_options const& Options) noexcept
		{
			mDefaultOptions = Options;

			for (auto& catpair : mCategories)
			{
				auto& catdata = catpair.second;
				if (catdata.uses_default_options)
				{
					catdata.options = mDefaultOptions;
				}
			}
		}

		void set_category_options(category const Cat, category_options const& Options)
		{
			mCategories[Cat].options = Options;
		}

		void erase_category_options(category const Cat)
		{
			auto it = mCategories.find(Cat);
			if (it == mCategories.end())
			{
				return;
			}
			auto& catdata = it->second;
			catdata.uses_default_options = true;
			catdata.options = mDefaultOptions;
		}

		void erase_category_data(category const Cat) noexcept
		{
			mCategories.erase(Cat);
		}

		void set_category_name(category const Cat, _In_opt_z_ char const* const Name)
		{
			auto it = mCategories.find(Cat);
			if (it == mCategories.end())
			{
				mCategories.emplace(Cat, category_data(mDefaultOptions, category_name(Name)));
				return;
			}
			mCategories[Cat].name = category_name(Name);
		}

		[[nodiscard]] category_data const* get_category_data(category const Cat) const
		{
			auto catData = mCategories.find(Cat);
			if (catData == mCategories.end())
			{
				return nullptr;
			}
			return &catData->second;
		}

		[[nodiscard]] char const* get_facility_name() const noexcept
		{
			return mName;
		}

		[[nodiscard]] auto const& get_default_category_options() const noexcept
		{
			return mDefaultOptions;
		}

	private:
		void copy_facility_name(facility_data const& Other) noexcept
		{
			auto const facilityNameLength = strlen_c(Other.mName, max_facility_name_length);
			std::memcpy(mName, Other.mName, facilityNameLength + 1); // + 1 for null terminator
		}

		char mName[max_facility_name_length + 1];
		category_options mDefaultOptions;
		std::unordered_map<category, category_data> mCategories;
	};

	struct message_output
	{
		message_desc desc;
		char const* facility_name;
		char const* category_name;
		std::size_t facility_name_length;
		std::size_t category_name_length;
	};

	class default_sink_output : public sink_output
	{
	public:
		void receive(message_output const& Msg) override
		{
			std::string str(Msg.facility_name, Msg.facility_name_length);
			str += " ";
			switch (Msg.desc.severity)
			{
			case severity::info:
				str += "info";
				break;
			case severity::warn:
				str += "WARNING";
				break;
			case severity::error:
				str += "ERROR";
				break;
			default:
				break;
			}
			str += ": ";
			str += Msg.desc.string ? Msg.desc.string : "unspecified message";
			str += " (From function '";
			str += Msg.desc.function ? Msg.desc.function : "unknown function";
			str += "', category '";
			str.append(Msg.category_name, Msg.category_name_length);
			str += "').\n";

			OutputDebugStringA(str.data());
		}
	};

	class logger
	{
	public:
		[[nodiscard]] static logger& get()
		{
			static logger inst;
			return inst;
		}

		logger()
		{
			std::unique_ptr<sink_output> defaultSinkOutput(new default_sink_output());
			mDefaultSink = this->register_sink(std::move(defaultSinkOutput));

			mWdulFacility = this->register_facility("WDUL", category_options{ .severity_threshold = severity::info, .break_on_errors = true });

			this->set_category_name(mWdulFacility, categories::assertions, "assertions");
			this->set_category_name(mWdulFacility, categories::close_handle, "close_handle");
			this->set_category_name(mWdulFacility, categories::xaudio2, "xaudio2");
			this->set_category_name(mWdulFacility, categories::strings, "strings");
			this->set_category_name(mWdulFacility, categories::window, "window");
			this->set_category_name(mWdulFacility, categories::mf, "mf");
		}

		[[nodiscard]] facility register_facility(
			_In_opt_z_ char const* const Name,
			category_options const& DefaultOptions)
		{
			auto const newFacility = mLastFacility + 1;

			mFacilities.emplace(static_cast<facility>(newFacility), facility_data(Name, DefaultOptions));

			mLastFacility = newFacility;
			return static_cast<facility>(newFacility);
		}

		void unregister_facility(facility const Facility) noexcept
		{
			mFacilities.erase(Facility);
		}

		void set_default_category_options(facility const Facility, category_options const& Options)
		{
			this->find_facility(Facility).set_default_options(Options);
		}

		void set_category_options(facility const Facility, category const Cat, category_options const& Options)
		{
			this->find_facility(Facility).set_category_options(Cat, Options);
		}

		void erase_category_options(facility const Facility, category const Cat)
		{
			this->find_facility(Facility).erase_category_options(Cat);
		}

		void erase_category_data(facility const Facility, category const Cat)
		{
			this->find_facility(Facility).erase_category_data(Cat);
		}

		void set_category_name(facility const Facility, category const Cat, _In_opt_z_ char const* const Name)
		{
			this->find_facility(Facility).set_category_name(Cat, Name);
		}

		[[nodiscard]] sink register_sink(std::unique_ptr<sink_output>&& Output)
		{
			auto const newSink = mLastSink + 1;

			mSinks.emplace(static_cast<sink>(newSink), std::move(Output));

			mLastSink = newSink;
			return static_cast<sink>(newSink);
		}

		void unregister_sink(sink const Sink) noexcept
		{
			mSinks.erase(Sink);
			if (Sink == mDefaultSink)
			{
				mDefaultSink = sink::unknown;
			}
		}

		void broadcast(message_desc const& Msg)
		{
			auto& fac = this->find_facility(Msg.facility);

			auto const& defaultOptions = fac.get_default_category_options();
			auto const catData = fac.get_category_data(Msg.category);
			
			category_options const& workingOptions = catData ? catData->options : defaultOptions;

			if (workingOptions.severity_threshold <= Msg.severity)
			{
				message_output msgOutput;
				msgOutput.desc = Msg;

				msgOutput.facility_name = fac.get_facility_name();
				msgOutput.facility_name_length = std::strlen(msgOutput.facility_name);
				if (msgOutput.facility_name_length == 0)
				{
					msgOutput.facility_name = "unnamed";
					msgOutput.facility_name_length = 7;
				}

				std::uint8_t catNameLength = 0;
				if (catData)
				{
					catNameLength = catData->name.length;
				}

				if (catNameLength > 0)
				{
					msgOutput.category_name = catData->name.data;
					msgOutput.category_name_length = catData->name.length;
				}
				else
				{
					msgOutput.category_name = "unnamed";
					msgOutput.category_name_length = 7;
				}

				for (auto& sinkIt : mSinks)
				{
					sinkIt.second->receive(msgOutput);
				}

				if (workingOptions.break_on_errors && msgOutput.desc.severity == severity::error)
				{
					__debugbreak();
				}
			}
		}

		[[nodiscard]] auto get_default_sink() noexcept
		{
			return mDefaultSink;
		}

		[[nodiscard]] auto get_wdul_facility() noexcept
		{
			return mWdulFacility;
		}

	private:
		facility_data& find_facility(facility const Facility)
		{
			facility_data* data;
			try
			{
				data = &mFacilities.at(Facility);
			}
			catch (std::out_of_range const&)
			{
				std::string str = "WDUL DEBUG ERROR: Facility #";
				str += std::to_string(to_underlying(Facility));
				str += " is not registered.\n";
				throw std::logic_error(str);
			}
			return *data;
		}

		std::underlying_type_t<facility> mLastFacility = 0;
		std::unordered_map<facility, facility_data> mFacilities;
		std::underlying_type_t<sink> mLastSink = 0;
		std::unordered_map<sink, std::unique_ptr<sink_output>> mSinks;
		sink mDefaultSink = sink::unknown;
		facility mWdulFacility;
	};

	[[nodiscard]] facility register_facility(_In_z_ char const* const Name, category_options const& Options)
	{
		auto& logger = logger::get();
		return logger.register_facility(Name, Options);
	}

	void set_default_category_options(facility const Facility, category_options const& Options)
	{
		auto& logger = logger::get();
		return logger.set_default_category_options(Facility, Options);
	}

	void set_category_options(facility const Facility, category const Category, category_options const& Options)
	{
		auto& logger = logger::get();
		logger.set_category_options(Facility, Category, Options);
	}

	void erase_category_options(facility const Facility, category const Category)
	{
		auto& logger = logger::get();
		logger.erase_category_options(Facility, Category);
	}

	// This function is not currently exposed in debug.hpp.
	//
	// It removes category data for the given category.
	// This means the category name, category options, etc. will be erased.
	void erase_category_data(facility const Facility, category const Category)
	{
		auto& logger = logger::get();
		logger.erase_category_data(Facility, Category);
	}

	void set_category_name(facility const Facility, category const Category, _In_opt_z_ char const* const Name)
	{
		auto& logger = logger::get();
		logger.set_category_name(Facility, Category, Name);
	}

	[[nodiscard]] message_desc const& get_message_desc(message_output const& MsgOutput) noexcept
	{
		return MsgOutput.desc;
	}

	[[nodiscard]] char const* get_message_facility_name(message_output const& MsgOutput) noexcept
	{
		return MsgOutput.facility_name;
	}

	[[nodiscard]] std::size_t get_message_facility_name_length(message_output const& MsgOutput) noexcept
	{
		return MsgOutput.facility_name_length;
	}

	[[nodiscard]] char const* get_message_category_name(message_output const& MsgOutput) noexcept
	{
		return MsgOutput.category_name;
	}

	[[nodiscard]] std::size_t get_message_category_name_length(message_output const& MsgOutput) noexcept
	{
		return MsgOutput.category_name_length;
	}

	[[nodiscard]] sink register_sink(std::unique_ptr<sink_output>&& Output)
	{
		auto& logger = logger::get();
		return logger.register_sink(std::move(Output));
	}

	void unregister_sink(sink const Sink) noexcept
	{
		auto& logger = logger::get();
		return logger.unregister_sink(Sink);
	}

	void output(facility const Facility, category const Category, severity const Severity, _In_opt_z_ char const* const Fn,
		_In_opt_z_ char const* const Msg)
	{
		auto& logger = logger::get();
		logger.broadcast(message_desc{ .facility = Facility, .category = Category, .severity = Severity,
				.function = Fn, .string = Msg });
	}

	[[nodiscard]] sink get_default_sink() noexcept
	{
		auto& logger = logger::get();
		return logger.get_default_sink();
	}

	[[nodiscard]] facility categories::get_facility() noexcept
	{
		auto& logger = logger::get();
		return logger.get_wdul_facility();
	}
}

namespace wdul::impl
{
	[[noreturn]] void assertion_error(
		_In_opt_z_ char const* const Fn,
		_In_opt_z_ char const* const Expr,
		_In_opt_z_ char const* const Msg
	) noexcept
	{

		std::string str = "ASSERTION ERROR: Expression '";
		str += Expr ? Expr : "unspecified";
		str += "' evaluated to false in function '";
		str += Fn ? Fn : "unknown function";
		if (Msg)
		{
			str += "': ";
			str += Msg;
			str += ".\n";
		}
		else
		{
			str += "'.\n";
		}
		auto& logger = debug::logger::get();

		logger.broadcast(
			debug::message_desc{
				.facility = logger.get_wdul_facility(),
				.category = debug::categories::assertions,
				.severity = debug::severity::error,
				.function = Fn,
				.string = str.data()
			}
		);
		__debugbreak();
		std::abort();
	}
}

#endif // _DEBUG
