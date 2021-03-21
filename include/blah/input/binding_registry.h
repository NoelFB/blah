#pragma once
#include <blah/input/binding.h>
#include <blah/containers/vector.h>
#include <memory>

namespace Blah
{
	using BindingRef = std::shared_ptr<Binding>;
	using AxisBindingRef = std::shared_ptr<AxisBinding>;
	using StickBindingRef = std::shared_ptr<StickBinding>;

	// Optional registry to automatically update bindings.
	// You can register different types of bindings here and until they are
	// no longer used, they will be updated without having to explicitely call
	// their update methods
	class BindingRegistry
	{
	public:
		// registers a new binding
		static BindingRef register_binding(const Binding& binding = Binding());

		// registers a new axis binding
		static AxisBindingRef register_axis(const AxisBinding& binding = AxisBinding());

		// registers a new stick binding
		static StickBindingRef register_stick(const StickBinding& binding = StickBinding());

		// updates all the bindings. This is called
		// automatically by the App loop.
		static void update();

	private:
		static Vector<std::weak_ptr<Binding>> bindings;
		static Vector<std::weak_ptr<AxisBinding>> axes;
		static Vector<std::weak_ptr<StickBinding>> sticks;
	};
}