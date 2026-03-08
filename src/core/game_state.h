#pragma once

#include <stdint.h>

#include "core/2D/game_state_2d.h"
#include "core/3D/game_state_3d.h"

namespace bacon
{
	namespace GameState
	{
		enum class GameType : uint8_t
		{
			NONE = 0,
			GAME_2D,
			GAME_3D,
		};

		inline GameType game_type = GameType::NONE;
		inline GameState2D* state_2d = nullptr;
		inline GameState3D* state_3d = nullptr;

		GameObject* find_object_by_uuid(UUID uuid);
		GameObject* find_object_by_uuid(std::string uuid);
	}
} // namespace bacon
