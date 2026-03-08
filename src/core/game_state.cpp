#include "core/game_state.h"

namespace bacon
{
	GameObject* GameState::find_object_by_uuid(UUID uuid)
	{
		if (game_type == GameType::GAME_2D)
			return GameState::state_2d->scene->find_object_by_uuid(uuid);
		else if (game_type == GameType::GAME_3D)
			// TODO
			// return state_3d->find_object_by_uuid(uuid);
			return nullptr;
		return nullptr;
	}

	GameObject* GameState::find_object_by_uuid(std::string uuid)
	{
		return find_object_by_uuid(UUID(uuid));
	}
}
