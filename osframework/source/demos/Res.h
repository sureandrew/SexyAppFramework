#ifndef __Res_H__
#define __Res_H__

namespace Sexy
{
	class ResourceManager;
	class Image;
	class Font;

	Image* LoadImageById(ResourceManager *theManager, int theId);
	bool ExtractResourcesByName(ResourceManager *theManager, const char *theName);

	// Game Resources
	bool ExtractGameResources(ResourceManager *theMgr);
	extern Image* IMAGE_BG0;
	extern Image* IMAGE_BG1;
	extern Image* IMAGE_BG2;
	extern Image* IMAGE_BUTTON_DOWN;
	extern Image* IMAGE_BUTTON_NORMAL;
	extern Image* IMAGE_BUTTON_OVER;
	extern Image* IMAGE_CHECKBOX;
	extern Image* IMAGE_DIALOG_BOX;
	extern Image* IMAGE_DIALOG_BUTTON;
	extern Image* IMAGE_ROBOTROBOT;
	extern Image* IMAGE_SLIDER_THUMB;
	extern Image* IMAGE_SLIDER_TRACK;
	extern int SOUND_MUTATOR;
	extern int SOUND_TIMER;

	// Hungarr Resources
	bool ExtractHungarrResources(ResourceManager *theMgr);
	extern Image* IMAGE_ATOMIC_EXPLOSION;
	extern Image* IMAGE_BOMB_RADIAL_DEATH;
	extern Image* IMAGE_HUNGARR_BEAM_DOWN;
	extern Image* IMAGE_HUNGARR_BEAM_LEFT;
	extern Image* IMAGE_HUNGARR_BEAM_RIGHT;
	extern Image* IMAGE_HUNGARR_BEAM_UP;
	extern Image* IMAGE_HUNGARR_HORIZ;
	extern Image* IMAGE_HUNGARR_SMALL;
	extern Image* IMAGE_HUNGARR_VERT;
	extern Image* IMAGE_PARTICLE_LIGHTNING;
	extern Image* IMAGE_PLANETS;
	extern Image* IMAGE_SPARK;
	extern int SOUND_BEAM_HIT;
	extern int SOUND_BEAM_MOVING;
	extern int SOUND_BUTTON;
	extern int SOUND_EXPLOSION;
	extern int SOUND_GAME_OVER_CLICK;
	extern int SOUND_GAME_OVER_RESTART;
	extern int SOUND_GAME_OVER_STATS;
	extern int SOUND_GAME_OVER_TEXT;
	extern int SOUND_LEVEL_UP1;
	extern int SOUND_LEVEL_UP2;
	extern int SOUND_LEVEL_UP3;
	extern int SOUND_LEVEL_UP4;
	extern int SOUND_MAGZAP;
	extern int SOUND_PLANET;
	extern int SOUND_PLANET_HIT;
	extern int SOUND_REGION_FILLED;

	// Init Resources
	bool ExtractInitResources(ResourceManager *theMgr);
	extern Font* FONT_DEFAULT;
	extern Font* FONT_HUNGARR;
	extern Image* IMAGE_CUSTOM_DRAGGING;
	extern Image* IMAGE_CUSTOM_HAND;
	extern Image* IMAGE_CUSTOM_POINTER;
	extern Image* IMAGE_CUSTOM_TEXT;
	extern Image* IMAGE_HUNGARR_LOGO;

	// TitleScreen Resources
	bool ExtractTitleScreenResources(ResourceManager *theMgr);
	extern Image* IMAGE_LOADER_BAR;
	extern Image* IMAGE_LOADER_LOADINGTXT;
	extern int SOUND_CONTINUE;

	enum ResourceId
	{
		FONT_DEFAULT_ID,
		FONT_HUNGARR_ID,
		IMAGE_CUSTOM_POINTER_ID,
		IMAGE_CUSTOM_HAND_ID,
		IMAGE_CUSTOM_DRAGGING_ID,
		IMAGE_CUSTOM_TEXT_ID,
		IMAGE_HUNGARR_LOGO_ID,
		IMAGE_LOADER_BAR_ID,
		IMAGE_LOADER_LOADINGTXT_ID,
		SOUND_CONTINUE_ID,
		SOUND_MUTATOR_ID,
		SOUND_TIMER_ID,
		IMAGE_ROBOTROBOT_ID,
		IMAGE_CHECKBOX_ID,
		IMAGE_BG0_ID,
		IMAGE_BG1_ID,
		IMAGE_BG2_ID,
		IMAGE_BUTTON_DOWN_ID,
		IMAGE_BUTTON_OVER_ID,
		IMAGE_BUTTON_NORMAL_ID,
		IMAGE_DIALOG_BOX_ID,
		IMAGE_DIALOG_BUTTON_ID,
		IMAGE_SLIDER_TRACK_ID,
		IMAGE_SLIDER_THUMB_ID,
		IMAGE_HUNGARR_SMALL_ID,
		IMAGE_HUNGARR_BEAM_UP_ID,
		IMAGE_HUNGARR_BEAM_DOWN_ID,
		IMAGE_HUNGARR_BEAM_LEFT_ID,
		IMAGE_HUNGARR_BEAM_RIGHT_ID,
		IMAGE_HUNGARR_HORIZ_ID,
		IMAGE_HUNGARR_VERT_ID,
		IMAGE_ATOMIC_EXPLOSION_ID,
		IMAGE_BOMB_RADIAL_DEATH_ID,
		IMAGE_PLANETS_ID,
		IMAGE_SPARK_ID,
		IMAGE_PARTICLE_LIGHTNING_ID,
		SOUND_MAGZAP_ID,
		SOUND_BUTTON_ID,
		SOUND_PLANET_ID,
		SOUND_LEVEL_UP1_ID,
		SOUND_LEVEL_UP2_ID,
		SOUND_EXPLOSION_ID,
		SOUND_BEAM_HIT_ID,
		SOUND_PLANET_HIT_ID,
		SOUND_BEAM_MOVING_ID,
		SOUND_LEVEL_UP4_ID,
		SOUND_LEVEL_UP3_ID,
		SOUND_GAME_OVER_CLICK_ID,
		SOUND_GAME_OVER_STATS_ID,
		SOUND_GAME_OVER_RESTART_ID,
		SOUND_GAME_OVER_TEXT_ID,
		SOUND_REGION_FILLED_ID,
		RESOURCE_ID_MAX
	};

	Image* GetImageById(int theId);
	Font* GetFontById(int theId);
	int GetSoundById(int theId);

	ResourceId GetIdByImage(Image *theImage);
	ResourceId GetIdByFont(Font *theFont);
	ResourceId GetIdBySound(int theSound);
	const char* GetStringIdById(int theId);
	ResourceId GetIdByStringId(const char *theStringId);

} // namespace Sexy


#endif
