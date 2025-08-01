// SPDX-FileCopyrightText: 2002-2025 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#include "GS/Renderers/HW/GSRendererHW.h"

class GSHwHack
{
public:
	static bool GSC_Manhunt2(GSRendererHW& r, int& skip);
	static bool GSC_SacredBlaze(GSRendererHW& r, int& skip);
	static bool GSC_GuitarHero(GSRendererHW& r, int& skip);
	static bool GSC_SFEX3(GSRendererHW& r, int& skip);
	static bool GSC_DTGames(GSRendererHW& r, int& skip);
	static bool GSC_NamcoGames(GSRendererHW& r, int& skip);
	static bool GSC_SandGrainGames(GSRendererHW& r, int& skip);
	static bool GSC_BurnoutGames(GSRendererHW& r, int& skip);
	static bool GSC_BlackAndBurnoutSky(GSRendererHW& r, int& skip);
	static bool GSC_MidnightClub3(GSRendererHW& r, int& skip);
	static bool GSC_TalesOfLegendia(GSRendererHW& r, int& skip);
	static bool GSC_ZettaiZetsumeiToshi2(GSRendererHW& r, int& skip);
	static bool GSC_UltramanFightingEvolution(GSRendererHW& r, int& skip);
	static bool GSC_TalesofSymphonia(GSRendererHW& r, int& skip);
	static bool GSC_UrbanReign(GSRendererHW& r, int& skip);
	static bool GSC_SteambotChronicles(GSRendererHW& r, int& skip);
	static bool GSC_BlueTongueGames(GSRendererHW& r, int& skip);
	static bool GSC_NFSUndercover(GSRendererHW& r, int& skip);
	static bool GSC_Battlefield2(GSRendererHW& r, int& skip);
	static bool GSC_PolyphonyDigitalGames(GSRendererHW& r, int& skip);
	static bool GSC_MetalGearSolid3(GSRendererHW& r, int& skip);
	static bool GSC_Turok(GSRendererHW& r, int& skip);

	static bool OI_PointListPalette(GSRendererHW& r, GSTexture* rt, GSTexture* ds, GSTextureCache::Source* t);
	static bool OI_DBZBTGames(GSRendererHW& r, GSTexture* rt, GSTexture* ds, GSTextureCache::Source* t);
	static bool OI_RozenMaidenGebetGarden(GSRendererHW& r, GSTexture* rt, GSTexture* ds, GSTextureCache::Source* t);
	static bool OI_SonicUnleashed(GSRendererHW& r, GSTexture* rt, GSTexture* ds, GSTextureCache::Source* t);
	static bool OI_ArTonelico2(GSRendererHW& r, GSTexture* rt, GSTexture* ds, GSTextureCache::Source* t);
	static bool OI_BurnoutGames(GSRendererHW& r, GSTexture* rt, GSTexture* ds, GSTextureCache::Source* t);

	static bool MV_Growlanser(GSRendererHW& r);
	static bool MV_Ico(GSRendererHW& r);

	template <typename F>
	struct Entry
	{
		const char* name;
		F ptr;
	};

	static const Entry<GSRendererHW::GSC_Ptr> s_get_skip_count_functions[];
	static const Entry<GSRendererHW::OI_Ptr> s_before_draw_functions[];
	static const Entry<GSRendererHW::MV_Ptr> s_move_handler_functions[];
};
