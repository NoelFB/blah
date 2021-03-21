#include <blah/input/binding_registry.h>

using namespace Blah;

Vector<std::weak_ptr<Binding>> BindingRegistry::bindings;
Vector<std::weak_ptr<AxisBinding>> BindingRegistry::axes;
Vector<std::weak_ptr<StickBinding>> BindingRegistry::sticks;

BindingRef BindingRegistry::register_binding()
{
	auto binding = std::make_shared<Binding>();
	bindings.push_back(std::weak_ptr<Binding>(binding));
	return binding;
}

AxisBindingRef BindingRegistry::register_axis()
{
	auto binding = std::make_shared<AxisBinding>();
	axes.push_back(std::weak_ptr<AxisBinding>(binding));
	return binding;
}

StickBindingRef BindingRegistry::register_stick()
{
	auto binding = std::make_shared<StickBinding>();
	sticks.push_back(std::weak_ptr<StickBinding>(binding));
	return binding;
}

void BindingRegistry::update()
{
	for (int i = 0; i < bindings.size(); i++)
	{
		if (bindings[i].use_count() <= 0)
		{
			bindings.erase(i);
			i--;
		}
		else if (auto binding = bindings[i].lock())
		{
			binding->update();
		}
	}

	for (int i = 0; i < axes.size(); i++)
	{
		if (axes[i].use_count() <= 0)
		{
			axes.erase(i);
			i--;
		}
		else if (auto binding = axes[i].lock())
		{
			binding->update();
		}
	}

	for (int i = 0; i < sticks.size(); i++)
	{
		if (sticks[i].use_count() <= 0)
		{
			sticks.erase(i);
			i--;
		}
		else if (auto binding = sticks[i].lock())
		{
			binding->update();
		}
	}
}
