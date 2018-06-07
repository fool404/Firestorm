#include "stdafx.h"
#include "Entity.h"
#include "Component.h"

OPEN_NAMESPACE(Elf);

void* Entity::DoInspect(Mirror::Type type)
{
	void* output = nullptr;
	for(const auto& component : m_components)
	{
		output = component->Inspect(type);
		if(output != nullptr)
		{
			return output;
		}
	}
	return IInspectableObject::DoInspect(type);
}

CLOSE_NAMESPACE(Elf);