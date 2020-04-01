#ifndef __TOWER_H__
#define __TOWER_H__

#include "gf2d_entity.h"
#include "gf2d_windows.h"
#include "gf2d_element_label.h"
#include "gf2d_element_list.h"

typedef enum
{
	TT_Stinger,
	TT_Slingshot,
	TT_Laser,
	TT_Water,
	TT_Techno,
	TT_Snowglobe,
	TT_Music,
	TT_Power_Bee_Swarm,
	TT_Power_Time_Warp,
	TT_Power_Speed_Totem,
	TT_Power_Cash_Drop
}TowerTypes;

/**
* @brief get enemies in range of a tower
* @param self the tower to check for enemies in range
* @return a list pointer to a list of entites that are in self's shooting range
*/
List* inRange(Entity* self);

/**
* @brief deal instant damage to an entity
* @param self the tower doing the damage
* @param target the target to apply damage to
* @return the child of the damaged target, NULL is no child
*/
Entity* techno_damage(Entity* self, Entity* target);

/**
* @brief set the seek buckets (the buckets the tower can "see" enemies in) for a tower
* @param self the tower to set seek buckets for
*/
void setSeekBuckets(Entity* self); 

/**
* @brief find the closest enemy to a tower
* @param self the tower to find the closest enemy to
* @return a pointer to the nearest enemy entity, NULL if none
*/
Entity* findClosest(Entity* self);

/**
* @brief spawn a new stinger
* @param position where to spawn the tower
* @return the spawned tower
*/
Entity* stinger_spawn(Vector2D position);

/**
* @brief spawn a new slingshot
* @param position where to spawn the tower
* @return the spawned tower
*/
Entity* slingshot_spawn(Vector2D position);

/**
* @brief spawn a new laser tower
* @param position where to spawn the tower
* @return the spawned tower
*/
Entity* laser_spawn(Vector2D position);

/**
* @brief spawn a new water tower
* @param position where to spawn the tower
* @return the spawned tower
*/
Entity* water_spawn(Vector2D position);

/**
* @brief spawn a new techno tower
* @param position where to spawn the tower
* @return the spawned tower
*/
Entity* techno_spawn(Vector2D position);

/**
* @brief spawn a new snow tower
* @param position where to spawn the tower
* @return the spawned tower
* @note named snowglobe due to early production name
*/
Entity* snowglobe_spawn(Vector2D position);

/**
* @brief spawn a new music tower
* @param position where to spawn the tower
* @return the spawned tower
*/
Entity* music_spawn(Vector2D position); 

/**
* @brief spawn a new speed totem
* @param position where to spawn the tower
* @return the spawned tower
*/
Entity* speed_totem_spawn(Vector2D position);

/**
* @brief checks available funds and readies placement for stinger if enough money is available
*/
void stinger_buy();

/**
* @brief checks available funds and readies placement for slingshot if enough money is available
*/
void slingshot_buy();

/**
* @brief checks available funds and readies placement for laser if enough money is available
*/
void laser_buy();

/**
* @brief checks available funds and readies placement for water if enough money is available
*/
void water_buy();

/**
* @brief checks available funds and readies placement for techno if enough money is available
*/
void techno_buy();

/**
* @brief checks available funds and readies placement for snow if enough money is available
*/
void snowglobe_buy();

/**
* @brief checks available funds and readies placement for music if enough money is available
*/
void music_buy();

/**
* @brief checks player inventory and readies placement for swarm if available
*/
void swarm_buy();

/**
* @brief checks player inventory and readies placement for cash drop if available
*/
void cashDrop_buy();

/**
* @brief checks player inventory and readies placement for speed totem if available
*/
void speedTotem_buy();

/**
* @brief checks player inventory and readies placement for time warp if available
*/
void timeWarp_buy();

/**
* @brief spawn the placement entity for a certain type
* @param type the type of tower to ready placement for
* @return the placement entity
*/
Entity* placement_spawn(TowerTypes type);

/**
* @brief stinger think. handles targeting and shooting
* @param self the stinger to do thinking
*/
void stinger_think(Entity* self);

/**
* @brief slingshot think. handles targeting and shooting
* @param self the slingshot to do thinking
*/
void slingshot_think(Entity* self);

/**
* @brief laser think. handles targeting and shooting
* @param self the laser to do thinking
*/
void laser_think(Entity* self);

/**
* @brief water think. handles targeting and shooting
* @param self the water to do thinking
*/
void water_think(Entity* self);

/**
* @brief techno think. handles targeting and shooting
* @param self the techno to do thinking
*/
void techno_think(Entity* self);

/**
* @brief snow think. handles targeting and shooting as well as slowing
* @param self the snow to do thinking
*/
void snowglobe_think(Entity* self);

/**
* @brief music think. handles targeting and shooting as well as buffing
* @param self the music to do thinking
*/
void music_think(Entity* self);

/**
* @brief speed totem think. handles buffing
* @param self the speed totem to do thinking
*/
void speedTotem_think(Entity* self);

/**
* @brief placement think. handles placement collision and detatchment to full tower
* @param self the placement entity to do thinking
*/
void placement_think(Entity* self);
/**
* @brief Turns the placement entity into a full tower based on what type it was spawned as
* @param self the placement entity to turn into a full tower
*/
void placement_detach(Entity* ent);

/**
* @brief set the prices for the tower given UI
* @param window the tower selection UI
*/
void setPrices(Window* window);
/**
* @brief get the price of a certain tower
* @param type the tower type to check the price of
* @return the price of the tower
*/
int getPrice(TowerTypes type);

/**
* @brief get the description of the first upgrade available to the tower
* @param tower the tower to check
* @return the description of the first upgrade available to the tower, "Fully Upgraded!" if error or no upgrades left
*/
char* getUpgradeOneDesc(Entity* tower);
/**
* @brief get the description of the second upgrade available to the tower
* @param tower the tower to check
* @return the description of the second upgrade available to the tower, "Fully Upgraded!" if error or no upgrades left
*/
char* getUpgradeTwoDesc(Entity* tower);

/**
* @brief get the price of the first upgrade available to the tower
* @param tower the tower to check
* @return the price of the first upgrade available to the tower, -1 if error or no upgrades left
*/
int getUpgradeOnePrice(Entity* tower);

/**
* @brief get the price of the second upgrade available to the tower
* @param tower the tower to check
* @return the price of the second upgrade available to the tower, -1 if error or no upgrades left
*/
int getUpgradeTwoPrice(Entity* tower);

/**
* @brief checks cash and buys first upgrade available for selected tower if available
*/
void upgradeOne_buy();

/**
* @brief checks cash and buys second upgrade available for selected tower if available
*/
void upgradeTwo_buy();

/**
* @brief applies the upgrade to the given stinger
* @param tower the tower to apply upgrades to
* @param upgradeNum 0 or 1, specifies to apply the first or second available upgrade
*/
void applyStingerUpgrade(Entity* tower, int upgradeNum);

/**
* @brief applies the upgrade to the given laser
* @param tower the tower to apply upgrades to
* @param upgradeNum 0 or 1, specifies to apply the first or second available upgrade
*/
void applyLaserUpgrade(Entity* tower, int upgradeNum);

/**
* @brief applies the upgrade to the given music
* @param tower the tower to apply upgrades to
* @param upgradeNum 0 or 1, specifies to apply the first or second available upgrade
*/
void applyMusicUpgrade(Entity* tower, int upgradeNum);

/**
* @brief applies the upgrade to the given slingshot
* @param tower the tower to apply upgrades to
* @param upgradeNum 0 or 1, specifies to apply the first or second available upgrade
*/
void applySlingshotUpgrade(Entity* tower, int upgradeNum);

/**
* @brief applies the upgrade to the given snow
* @param tower the tower to apply upgrades to
* @param upgradeNum 0 or 1, specifies to apply the first or second available upgrade
*/
void applySnowUpgrade(Entity* tower, int upgradeNum);

/**
* @brief applies the upgrade to the given techno
* @param tower the tower to apply upgrades to
* @param upgradeNum 0 or 1, specifies to apply the first or second available upgrade
*/
void applyTechnoUpgrade(Entity* tower, int upgradeNum);

/**
* @brief applies the upgrade to the given water
* @param tower the tower to apply upgrades to
* @param upgradeNum 0 or 1, specifies to apply the first or second available upgrade
*/
void applyWaterUpgrade(Entity* tower, int upgradeNum);

#endif