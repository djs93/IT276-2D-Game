#include "player.h"
#include "game.h"
void player_addPerk(PerkNumbers perkNum)
{
	Player* player;
	player = getPlayer();
	player->perks[perkNum] = true;
}

void player_consume(TowerTypes type) {
	Player* player;
	player = getPlayer();
	if (type == TT_Power_Bee_Swarm) {
		player->powerInventory[0] -= 1;
	}
	else if (type == TT_Power_Cash_Drop) {
		player->powerInventory[1] -= 1;
	}
	else if (type == TT_Power_Speed_Totem) {
		player->powerInventory[2] -= 1;
	}
	else if (type == TT_Power_Time_Warp) {
		player->powerInventory[3] -= 1;
	}
}

Player* player_load(char* saveFile) {
	SJson* file;
	SJson* tempJson;
	SJson* tempJsonValue;
	Player* player = gfc_allocate_array(sizeof(Player),1);
	file = sj_load(saveFile);
	if (!file) {
		return player;
	}
	tempJson = sj_object_get_value(file, "powerInv");
	if (tempJson) {
		tempJsonValue = sj_array_get_nth(tempJson, 0);
		if(tempJsonValue){
			sj_get_integer_value(tempJsonValue, &player->powerInventory[0]);
		}

		tempJsonValue = sj_array_get_nth(tempJson, 1);
		if (tempJsonValue) {
			sj_get_integer_value(tempJsonValue, &player->powerInventory[1]);
		}

		tempJsonValue = sj_array_get_nth(tempJson, 2);
		if (tempJsonValue) {
			sj_get_integer_value(tempJsonValue, &player->powerInventory[2]);
		}

		tempJsonValue = sj_array_get_nth(tempJson, 3);
		if (tempJsonValue) {
			sj_get_integer_value(tempJsonValue, &player->powerInventory[3]);
		}
	}

	tempJson = sj_object_get_value(file, "perks");
	if (tempJson) {
		tempJsonValue = sj_array_get_nth(tempJson, 0);
		if (tempJsonValue) {
			sj_get_bool_value(tempJsonValue, &player->perks[0]);
		}

		tempJsonValue = sj_array_get_nth(tempJson, 1);
		if (tempJsonValue) {
			sj_get_bool_value(tempJsonValue, &player->perks[1]);
		}

		tempJsonValue = sj_array_get_nth(tempJson, 2);
		if (tempJsonValue) {
			sj_get_bool_value(tempJsonValue, &player->perks[2]);
		}

		tempJsonValue = sj_array_get_nth(tempJson, 3);
		if (tempJsonValue) {
			sj_get_bool_value(tempJsonValue, &player->perks[3]);
		}

		tempJsonValue = sj_array_get_nth(tempJson, 4);
		if (tempJsonValue) {
			sj_get_bool_value(tempJsonValue, &player->perks[4]);
		}
	}

	tempJson = sj_object_get_value(file, "perkMoney");
	if (tempJson) {
		sj_get_integer_value(tempJson, &player->perkMoney);
	}

	return player;
}

void player_save(char* saveFile) {
	SJson* file;
	SJson* tempJson;
	SJson* tempJsonValue;
	Player* player = getPlayer();
	file = sj_object_new();
	tempJson = sj_array_new();
	sj_array_append(tempJson, sj_new_int(player->powerInventory[0]));
	sj_array_append(tempJson, sj_new_int(player->powerInventory[1]));
	sj_array_append(tempJson, sj_new_int(player->powerInventory[2]));
	sj_array_append(tempJson, sj_new_int(player->powerInventory[3]));
	sj_object_insert(file, "powerInv", tempJson);

	tempJson = sj_array_new();
	sj_array_append(tempJson, sj_new_bool(player->perks[0]));
	sj_array_append(tempJson, sj_new_bool(player->perks[1]));
	sj_array_append(tempJson, sj_new_bool(player->perks[2]));
	sj_array_append(tempJson, sj_new_bool(player->perks[3]));
	sj_array_append(tempJson, sj_new_bool(player->perks[4]));
	sj_object_insert(file, "perks", tempJson);

	tempJson = sj_new_int(player->perkMoney);
	sj_object_insert(file, "perkMoney", tempJson);

	sj_save(file, saveFile);
}

int get_perk_cost(PerkNumbers perkNumber) {
	switch (perkNumber)
	{
	case PN_Pierce:
		return 200;
		break;
	case PN_Speed:
		return 300;
		break;
	case PN_Discount:
		return 400;
		break;
	case PN_Money:
		return 500;
		break;
	case PN_StingerUps:
		return 600;
		break;
	default:
		return -1;
		break;
	}
}

void tryBuyPerk(PerkNumbers perkNumber) {
	int perkCost;
	Player* player;

	perkCost = get_perk_cost(perkNumber);
	player = getPlayer();
	if (get_perk_cost < 0)return;
	if (player->perks[perkNumber] == false && player->perkMoney >= perkCost) {
		//buy and update
		player->perks[perkNumber] = true;
		player->perkMoney -= perkCost;
		updatePerkUI();
		player_save("saves/player.json");
	}
}