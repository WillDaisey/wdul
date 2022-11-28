// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"
#include <Windows.h>

namespace wdul
{
	// Standard access rights that correspond to operations common to most types of securable objects.
	// See https://docs.microsoft.com/en-us/windows/win32/secauthz/standard-access-rights.
	enum class standard_access : std::uint32_t
	{
		none = 0x0,

		// Required to delete the object.
		delete_object = DELETE,

		// The right to read information in the security descriptor for the object, not including the information in the SACL (system access control list).
		// To read or write the SACL, you must request the ACCESS_SYSTEM_SECURITY access right.
		read_control = READ_CONTROL,

		// The right to use the object for synchronization. This enables a thread to wait until the object is in the signaled state.
		// Some object types do not support this access right.
		synchronize = SYNCHRONIZE,

		// The right to modify the DACL (discretionary access control list) in the security descriptor for the object.
		write_dac = WRITE_DAC,

		// The right to change the owner in the security descriptor for the object.
		write_owner = WRITE_OWNER,
	};
	WDUL_DECLARE_ENUM_FLAGS(standard_access);

	// Generic access rights.
	// A securable object maps these rights to standard and object-specific access rights.
	// See https://docs.microsoft.com/en-us/windows/win32/secauthz/generic-access-rights.
	enum class generic_access : std::uint32_t
	{
		none = 0x0,

		// All possible access rights.
		all = GENERIC_ALL,

		// Execute access.
		execute = GENERIC_EXECUTE,

		// Read access.
		read = GENERIC_READ,

		// Write access.
		write = GENERIC_WRITE,
	};
	WDUL_DECLARE_ENUM_FLAGS(generic_access);

	template <class SpecificAccessT>
	class access_mask
	{
	public:
		explicit access_mask(std::uint32_t const AccessRights) noexcept :
			mValue(AccessRights)
		{
		}

		access_mask(generic_access const GenericAccessRights) noexcept :
			mValue(to_underlying(GenericAccessRights))
		{
		}

		access_mask(standard_access const StandardAccess, SpecificAccessT const SpecificAccessRights) noexcept :
			mValue(to_underlying(StandardAccess) | to_underlying(SpecificAccessRights))
		{
		}

		[[nodiscard]] auto underlying() const noexcept
		{
			return mValue;
		}

	private:
		std::uint32_t mValue;
	};
}
