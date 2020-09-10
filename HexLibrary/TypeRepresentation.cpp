#include "TyperePresentation.h"

RTD::TypeRepresentation::TypeRepresentation(RepresentationCategory category, RepresentationModifier modifier, Type* target)
	:mType(target)
{
	SetCategory(category);
	SetModifier(modifier);
}
