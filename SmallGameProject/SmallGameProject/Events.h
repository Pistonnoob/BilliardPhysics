#pragma once
namespace Events
{
	enum EntityEvents {
		ENTITY_CREATED = 0,
		ENTITY_DESTROYED,
		ENTITY_REMOVED
	};
	enum CharacterEvents {
		MOVE,
		HALT,
		MOVE_UP,
		MOVE_LEFT,
		MOVE_DOWN,
		MOVE_RIGHT,
	};
}