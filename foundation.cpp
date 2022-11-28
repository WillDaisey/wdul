// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/foundation.hpp"
#include <string>
#include <exception>
#include <stdexcept>
#include <Windows.h>

namespace wdul
{
	namespace impl
	{
#ifdef _DEBUG
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
			OutputDebugStringA(str.data());
			__debugbreak();
			std::abort();
		}

		[[nodiscard]] std::string get_severity_string(severity const Sev)
		{
			switch (Sev)
			{
			case severity::info:
				return "info";
			case severity::warn:
				return "WARNING";
			case severity::error:
				return "ERROR";
			default:
				throw std::invalid_argument("unknown severity");
			}
		}

		void debug_output(
			_In_opt_z_ char const* const Module,
			_In_opt_z_ char const* const Fn,
			_In_opt_z_ char const* const Msg,
			severity const Sev
		) noexcept
		{
			try
			{
				std::string str(Module ? Module : "unknown module");
				str += " ";
				str += get_severity_string(Sev);
				str += ": ";
				str += Msg ? Msg : "unspecified message";
				str += " (From function ";
				str += Fn ? Fn : "unknown function";
				str += ").\n";
				OutputDebugStringA(str.data());

				if (Sev == severity::error)
				{
					__debugbreak();
				}
			}
			catch (std::exception const& Ex)
			{
				OutputDebugStringA("WDUL ERROR: Failed to output debug string: ");
				OutputDebugStringA(Ex.what());
				OutputDebugStringA("\n");
			}
		}
#endif
	}
}
