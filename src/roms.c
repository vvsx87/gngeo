#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "roms.h"
#include "emu.h"
#include "memory.h"
//#include "unzip.h"
#ifdef HAVE_LIBZ
#include "zlib.h"
#endif
#include "unzip.h"

#include "video.h"
#include "transpack.h"
#include "conf.h"
#include "resfile.h"
#include "menu.h"
#ifdef GP2X
#include "gp2x.h"
#include "ym2610-940/940shared.h"
#endif

/* Prototype */
void kof98_decrypt_68k(GAME_ROMS *r);
void kof99_decrypt_68k(GAME_ROMS *r);
void garou_decrypt_68k(GAME_ROMS *r);
void garouo_decrypt_68k(GAME_ROMS *r);
void mslug3_decrypt_68k(GAME_ROMS *r);
void kof2000_decrypt_68k(GAME_ROMS *r);
void kof2002_decrypt_68k(GAME_ROMS *r);
void matrim_decrypt_68k(GAME_ROMS *r);
void samsho5_decrypt_68k(GAME_ROMS *r);
void samsh5p_decrypt_68k(GAME_ROMS *r);
void mslug5_decrypt_68k(GAME_ROMS *r);
void kf2k3pcb_decrypt_s1data(GAME_ROMS *r);
void kf2k3pcb_decrypt_68k(GAME_ROMS *r);
void kof2003_decrypt_68k(GAME_ROMS *r);
void kof99_neogeo_gfx_decrypt(GAME_ROMS *r, int extra_xor);
void kof2000_neogeo_gfx_decrypt(GAME_ROMS *r, int extra_xor);
void cmc50_neogeo_gfx_decrypt(GAME_ROMS *r, int extra_xor);
void cmc42_neogeo_gfx_decrypt(GAME_ROMS *r, int extra_xor);
void neogeo_bootleg_cx_decrypt(GAME_ROMS *r);
void neogeo_bootleg_sx_decrypt(GAME_ROMS *r, int extra_xor);
void svcpcb_gfx_decrypt(GAME_ROMS *r);
void svcpcb_s1data_decrypt(GAME_ROMS *r);
void neo_pcm2_swap(GAME_ROMS *r, int value);
void neo_pcm2_snk_1999(GAME_ROMS *r, int value);
void neogeo_cmc50_m1_decrypt(GAME_ROMS *r);

int neogeo_fix_bank_type = 0;

int bankoffset_kof99[64] = { 0x000000, 0x100000, 0x200000, 0x300000, 0x3cc000,
		0x4cc000, 0x3f2000, 0x4f2000, 0x407800, 0x507800, 0x40d000, 0x50d000,
		0x417800, 0x517800, 0x420800, 0x520800, 0x424800, 0x524800, 0x429000,
		0x529000, 0x42e800, 0x52e800, 0x431800, 0x531800, 0x54d000, 0x551000,
		0x567000, 0x592800, 0x588800, 0x581800, 0x599800, 0x594800, 0x598000, /* rest not used? */
};
/* addr,uncramblecode,.... */
Uint8 scramblecode_kof99[7] = { 0xF0, 14, 6, 8, 10, 12, 5, };
int bankoffset_garou[64] = { 0x000000, 0x100000, 0x200000, 0x300000, // 00
		0x280000, 0x380000, 0x2d0000, 0x3d0000, // 04
		0x2f0000, 0x3f0000, 0x400000, 0x500000, // 08
		0x420000, 0x520000, 0x440000, 0x540000, // 12
		0x498000, 0x598000, 0x4a0000, 0x5a0000, // 16
		0x4a8000, 0x5a8000, 0x4b0000, 0x5b0000, // 20
		0x4b8000, 0x5b8000, 0x4c0000, 0x5c0000, // 24
		0x4c8000, 0x5c8000, 0x4d0000, 0x5d0000, // 28
		0x458000, 0x558000, 0x460000, 0x560000, // 32
		0x468000, 0x568000, 0x470000, 0x570000, // 36
		0x478000, 0x578000, 0x480000, 0x580000, // 40
		0x488000, 0x588000, 0x490000, 0x590000, // 44
		0x5d0000, 0x5d8000, 0x5e0000, 0x5e8000, // 48
		0x5f0000, 0x5f8000, 0x600000, /* rest not used? */
};
Uint8 scramblecode_garou[7] = { 0xC0, 5, 9, 7, 6, 14, 12, };
int bankoffset_garouo[64] = { 0x000000, 0x100000, 0x200000, 0x300000, // 00
		0x280000, 0x380000, 0x2d0000, 0x3d0000, // 04
		0x2c8000, 0x3c8000, 0x400000, 0x500000, // 08
		0x420000, 0x520000, 0x440000, 0x540000, // 12
		0x598000, 0x698000, 0x5a0000, 0x6a0000, // 16
		0x5a8000, 0x6a8000, 0x5b0000, 0x6b0000, // 20
		0x5b8000, 0x6b8000, 0x5c0000, 0x6c0000, // 24
		0x5c8000, 0x6c8000, 0x5d0000, 0x6d0000, // 28
		0x458000, 0x558000, 0x460000, 0x560000, // 32
		0x468000, 0x568000, 0x470000, 0x570000, // 36
		0x478000, 0x578000, 0x480000, 0x580000, // 40
		0x488000, 0x588000, 0x490000, 0x590000, // 44
		0x5d8000, 0x6d8000, 0x5e0000, 0x6e0000, // 48
		0x5e8000, 0x6e8000, 0x6e8000, 0x000000, // 52
		0x000000, 0x000000, 0x000000, 0x000000, // 56
		0x000000, 0x000000, 0x000000, 0x000000, // 60
		};
Uint8 scramblecode_garouo[7] = { 0xC0, 4, 8, 14, 2, 11, 13, };

int bankoffset_mslug3[64] = { 0x000000, 0x020000, 0x040000, 0x060000, // 00
		0x070000, 0x090000, 0x0b0000, 0x0d0000, // 04
		0x0e0000, 0x0f0000, 0x120000, 0x130000, // 08
		0x140000, 0x150000, 0x180000, 0x190000, // 12
		0x1a0000, 0x1b0000, 0x1e0000, 0x1f0000, // 16
		0x200000, 0x210000, 0x240000, 0x250000, // 20
		0x260000, 0x270000, 0x2a0000, 0x2b0000, // 24
		0x2c0000, 0x2d0000, 0x300000, 0x310000, // 28
		0x320000, 0x330000, 0x360000, 0x370000, // 32
		0x380000, 0x390000, 0x3c0000, 0x3d0000, // 36
		0x400000, 0x410000, 0x440000, 0x450000, // 40
		0x460000, 0x470000, 0x4a0000, 0x4b0000, // 44
		0x4c0000, /* rest not used? */
};
Uint8 scramblecode_mslug3[7] = { 0xE4, 14, 12, 15, 6, 3, 9, };
int bankoffset_kof2000[64] = { 0x000000, 0x100000, 0x200000, 0x300000, // 00
		0x3f7800, 0x4f7800, 0x3ff800, 0x4ff800, // 04
		0x407800, 0x507800, 0x40f800, 0x50f800, // 08
		0x416800, 0x516800, 0x41d800, 0x51d800, // 12
		0x424000, 0x524000, 0x523800, 0x623800, // 16
		0x526000, 0x626000, 0x528000, 0x628000, // 20
		0x52a000, 0x62a000, 0x52b800, 0x62b800, // 24
		0x52d000, 0x62d000, 0x52e800, 0x62e800, // 28
		0x618000, 0x619000, 0x61a000, 0x61a800, // 32
		};
Uint8 scramblecode_kof2000[7] = { 0xEC, 15, 14, 7, 3, 10, 5, };

#define LOAD_BUF_SIZE (128*1024)
static Uint8* iloadbuf = NULL;

/* Some utility function  */
/* Get the file szFileName in the zip.
 If szFileName begins by 0x, search by crc
 */
#if 0
static int unzLocateFileByCRC(unzFile file,Uint32 crc)
{
	unz_file_pos filepos;
	unz_file_info fileinfo;
	/* remember where we are */
	unzGetFilePos(file,&filepos);
	int err;
	char filename[256];
	//Uint32 crc;

	err = unzGoToFirstFile(file);
	while (err == UNZ_OK) {
		err = unzGetCurrentFileInfo(file,&fileinfo,
				filename,255,
				NULL,0,NULL,0);
		if (err == UNZ_OK) {
			//crc=strtoul(szFileName,NULL,0);
			if (crc==fileinfo.crc)
			return UNZ_OK;
			err = unzGoToNextFile(file);
		}
	}
	unzGoToFilePos(file,&filepos);
	return err;
}
#endif
/* Actuall Code */
/* 
 TODO
 static DRIVER_INIT( fatfury2 )
 {
 DRIVER_INIT_CALL(neogeo);
 fatfury2_install_protection(machine);
 }

 static DRIVER_INIT( mslugx )
 {
 DRIVER_INIT_CALL(neogeo);
 mslugx_install_protection(machine);
 }

 */

int init_mslugx(GAME_ROMS *r) {
	int i;
	Uint8 *RAM = r->cpu_m68k.p;
	for (i = 0; i < r->cpu_m68k.size; i += 2) {
		if ((READ_WORD_ROM(&RAM[i + 0]) == 0x0243)
				&& (READ_WORD_ROM(&RAM[i + 2]) == 0x0001) && /* andi.w  #$1, D3 */
		(READ_WORD_ROM(&RAM[i + 4]) == 0x6600)) { /* bne xxxx */

			WRITE_WORD_ROM(&RAM[i + 4], 0x4e71);
			WRITE_WORD_ROM(&RAM[i + 6], 0x4e71);
		}
	}

	WRITE_WORD_ROM(&RAM[0x3bdc], 0x4e71);
	WRITE_WORD_ROM(&RAM[0x3bde], 0x4e71);
	WRITE_WORD_ROM(&RAM[0x3be0], 0x4e71);
	WRITE_WORD_ROM(&RAM[0x3c0c], 0x4e71);
	WRITE_WORD_ROM(&RAM[0x3c0e], 0x4e71);
	WRITE_WORD_ROM(&RAM[0x3c10], 0x4e71);

	WRITE_WORD_ROM(&RAM[0x3c36], 0x4e71);
	WRITE_WORD_ROM(&RAM[0x3c38], 0x4e71);
	return 0;
}

int init_kof99(GAME_ROMS *r) {
	kof99_decrypt_68k(r);
	neogeo_fix_bank_type = 0;
	kof99_neogeo_gfx_decrypt(r, 0x00);
	memory.bksw_offset = bankoffset_kof99;
	memory.bksw_unscramble = scramblecode_kof99;
	memory.sma_rng_addr = 0xF8FA;
	//kof99_install_protection(machine);
	return 0;
}

int init_kof99n(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0x00);
	return 0;
}

int init_garou(GAME_ROMS *r) {
	garou_decrypt_68k(r);
	kof99_neogeo_gfx_decrypt(r, 0x06);
	neogeo_fix_bank_type = 1;
	memory.bksw_offset = bankoffset_garou;
	memory.bksw_unscramble = scramblecode_garou;
	memory.sma_rng_addr = 0xCCF0;
	//garou_install_protection(machine);
	DEBUG_LOG("I HAS INITIALIZD GAROU\n");
	return 0;
}

int init_garouo(GAME_ROMS *r) {
	garouo_decrypt_68k(r);
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0x06);

	memory.bksw_offset = bankoffset_garouo;
	memory.bksw_unscramble = scramblecode_garouo;
	memory.sma_rng_addr = 0xCCF0;

	//garouo_install_protection(machine);
	return 0;
}
/*
 int init_garoup(GAME_ROMS *r) {
 garou_decrypt_68k(r);
 kof99_neogeo_gfx_decrypt(r, 0x06);

 return 0;
 }
 */
int init_garoubl(GAME_ROMS *r) {
	/* TODO: Bootleg support */
	neogeo_bootleg_sx_decrypt(r, 2);
	neogeo_bootleg_cx_decrypt(r);
	return 0;
}

int init_mslug3(GAME_ROMS *r) {
	mslug3_decrypt_68k(r);
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0xad);

	memory.bksw_offset = bankoffset_mslug3;
	memory.bksw_unscramble = scramblecode_mslug3;
	//memory.sma_rng_addr=0xF8FA;
	memory.sma_rng_addr = 0;

	//mslug3_install_protection(r);
	return 0;
}

int init_mslug3h(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0xad);
	return 0;
}

int init_mslug3b6(GAME_ROMS *r) {
	/* TODO: Bootleg support */
	neogeo_bootleg_sx_decrypt(r, 2);
	cmc42_neogeo_gfx_decrypt(r, 0xad);
	return 0;
}

int init_kof2000(GAME_ROMS *r) {
	kof2000_decrypt_68k(r);
	neogeo_fix_bank_type = 2;
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x00);
	memory.bksw_offset = bankoffset_kof2000;
	memory.bksw_unscramble = scramblecode_kof2000;
	memory.sma_rng_addr = 0xD8DA;
	//kof2000_install_protection(r);
	return 0;

}
int init_kof2000n(GAME_ROMS *r) {
	neogeo_fix_bank_type = 2;
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x00);
	return 0;
}

int init_kof2001(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof2000_neogeo_gfx_decrypt(r, 0x1e);
	neogeo_cmc50_m1_decrypt(r);
	return 0;

}

/* 

 TODO:
 static DRIVER_INIT( cthd2003 )
 {
 decrypt_cthd2003(machine);
 DRIVER_INIT_CALL(neogeo);
 patch_cthd2003(machine);
 }

 static DRIVER_INIT ( ct2k3sp )
 {
 decrypt_ct2k3sp(machine);
 DRIVER_INIT_CALL(neogeo);
 patch_cthd2003(machine);
 }

 static DRIVER_INIT ( ct2k3sa )
 {
 decrypt_ct2k3sa(machine);
 DRIVER_INIT_CALL(neogeo);
 patch_ct2k3sa(machine);
 }

 */

int init_mslug4(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1; /* USA violent content screen is wrong -- not a bug, confirmed on real hardware! */
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x31);

	neo_pcm2_snk_1999(r, 8);
	return 0;

}

int init_ms4plus(GAME_ROMS *r) {
	cmc50_neogeo_gfx_decrypt(r, 0x31);
	neo_pcm2_snk_1999(r, 8);
	neogeo_cmc50_m1_decrypt(r);
	return 0;
}

int init_ganryu(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0x07);
	return 0;
}

int init_s1945p(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0x05);
	return 0;
}

int init_preisle2(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0x9f);
	return 0;
}

int init_bangbead(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0xf8);
	return 0;
}

int init_nitd(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0xff);
	return 0;
}

int init_zupapa(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0xbd);
	return 0;
}

int init_sengoku3(GAME_ROMS *r) {
	neogeo_fix_bank_type = 1;
	kof99_neogeo_gfx_decrypt(r, 0xfe);
	return 0;
}

int init_kof98(GAME_ROMS *r) {
	kof98_decrypt_68k(r);

	//install_kof98_protection(r);
	return 0;
}

int init_rotd(GAME_ROMS *r) {
	neo_pcm2_snk_1999(r, 16);
	neogeo_fix_bank_type = 1;
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x3f);
	return 0;
}

int init_kof2002(GAME_ROMS *r) {
	kof2002_decrypt_68k(r);
	neo_pcm2_swap(r, 0);
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0xec);
	return 0;
}
int init_kof2002b(GAME_ROMS *r) {
	/* TODO: Bootleg */
	kof2002_decrypt_68k(r);
	neo_pcm2_swap(r, 0);
	neogeo_cmc50_m1_decrypt(r);
	//kof2002b_gfx_decrypt(r, r->tiles.p,0x4000000);
	//kof2002b_gfx_decrypt(r, r->game_sfix.p,0x20000);
	return 0;
}

int init_kf2k2pls(GAME_ROMS *r) {
	kof2002_decrypt_68k(r);
	neo_pcm2_swap(r, 0);
	neogeo_cmc50_m1_decrypt(r);
	cmc50_neogeo_gfx_decrypt(r, 0xec);
	return 0;
}

int init_kf2k2mp(GAME_ROMS *r) {
	/* TODO: Bootleg */
	//kf2k2mp_decrypt(r);
	neo_pcm2_swap(r, 0);
	//neogeo_bootleg_sx_decrypt(r, 2);
	cmc50_neogeo_gfx_decrypt(r, 0xec);
	return 0;
}

int init_kof2km2(GAME_ROMS *r) {
	/* TODO: Bootleg */
	//kof2km2_px_decrypt(r);
	neo_pcm2_swap(r, 0);
	//neogeo_bootleg_sx_decrypt(r, 1);
	cmc50_neogeo_gfx_decrypt(r, 0xec);
	return 0;
}
/* 

 TODO
 static DRIVER_INIT( kof10th )
 {
 decrypt_kof10th(machine);
 DRIVER_INIT_CALL(neogeo);
 install_kof10th_protection(machine);
 }

 static DRIVER_INIT( kf10thep )
 {
 decrypt_kf10thep(machine);
 DRIVER_INIT_CALL(neogeo);
 }

 static DRIVER_INIT( kf2k5uni )
 {
 decrypt_kf2k5uni(machine);
 DRIVER_INIT_CALL(neogeo);
 }

 static DRIVER_INIT( kof2k4se )
 {
 decrypt_kof2k4se_68k(machine);
 DRIVER_INIT_CALL(neogeo);
 }

 static DRIVER_INIT( matrimbl )
 {
 matrim_decrypt_68k(machine);
 neogeo_fixed_layer_bank_type = 2;
 matrimbl_decrypt(machine);
 neogeo_sfix_decrypt(machine);
 DRIVER_INIT_CALL(neogeo);
 }

 */

int init_matrim(GAME_ROMS *r) {
	matrim_decrypt_68k(r);
	neo_pcm2_swap(r, 1);
	neogeo_fix_bank_type = 2;
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x6a);
	return 0;
}

int init_pnyaa(GAME_ROMS *r) {
	neo_pcm2_snk_1999(r, 4);
	neogeo_fix_bank_type = 1;
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x2e);
	return 0;
}

int init_mslug5(GAME_ROMS *r) {
	mslug5_decrypt_68k(r);
	neo_pcm2_swap(r, 2);
	neogeo_fix_bank_type = 1;
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x19);
	//install_pvc_protection(r);
	return 0;
}
/*
 TODO:
 static TIMER_CALLBACK( ms5pcb_bios_timer_callback )
 {
 int harddip3 = input_port_read(machine, "HARDDIP") & 1;
 memory_set_bankptr(machine, NEOGEO_BANK_BIOS, memory_region(machine, "mainbios")+0x20000+harddip3*0x20000);
 }

 */
int init_ms5pcb(GAME_ROMS *r) {

	/* TODO: start a timer that will check the BIOS select DIP every second */
	//timer_set(machine, attotime_zero, NULL, 0, ms5pcb_bios_timer_callback);
	//timer_pulse(machine, ATTOTIME_IN_MSEC(1000), NULL, 0, ms5pcb_bios_timer_callback);

	mslug5_decrypt_68k(r);
	svcpcb_gfx_decrypt(r);
	neogeo_cmc50_m1_decrypt(r);
	kof2000_neogeo_gfx_decrypt(r, 0x19);
	neogeo_fix_bank_type = 2;
	svcpcb_s1data_decrypt(r);
	neo_pcm2_swap(r, 2);

	//install_pvc_protection(r);
	return 0;
}

int init_ms5plus(GAME_ROMS *r) {
	/* TODO: Bootleg */
	cmc50_neogeo_gfx_decrypt(r, 0x19);
	neo_pcm2_swap(r, 2);
	//neogeo_bootleg_sx_decrypt(r, 1);
	neogeo_fix_bank_type = 1;

	//install_ms5plus_protection(r);
	return 0;
}
#if 0
// TODO:
static TIMER_CALLBACK( svcpcb_bios_timer_callback )
{
	int harddip3 = input_port_read(machine, "HARDDIP") & 1;
	memory_set_bankptr(machine, NEOGEO_BANK_BIOS, memory_region(machine, "mainbios")+0x20000+harddip3*0x20000);
}

static DRIVER_INIT( svcpcb )
{
	/* start a timer that will check the BIOS select DIP every second */
	timer_set(machine, attotime_zero, NULL, 0, svcpcb_bios_timer_callback);
	timer_pulse(machine, ATTOTIME_IN_MSEC(1000), NULL, 0, svcpcb_bios_timer_callback);

	svc_px_decrypt(machine);
	svcpcb_gfx_decrypt(machine);
	neogeo_cmc50_m1_decrypt(machine);
	kof2000_neogeo_gfx_decrypt(machine, 0x57);
	svcpcb_s1data_decrypt(machine);
	neo_pcm2_swap(machine, 3);
	neogeo_fixed_layer_bank_type = 2;
	DRIVER_INIT_CALL(neogeo);
	install_pvc_protection(machine);
}

static DRIVER_INIT( svc )
{
	svc_px_decrypt(machine);
	neo_pcm2_swap(machine, 3);
	neogeo_fixed_layer_bank_type = 2;
	neogeo_cmc50_m1_decrypt(machine);
	kof2000_neogeo_gfx_decrypt(machine, 0x57);
	DRIVER_INIT_CALL(neogeo);
	install_pvc_protection(machine);
}

static DRIVER_INIT( svcboot )
{
	svcboot_px_decrypt(machine);
	svcboot_cx_decrypt(machine);
	DRIVER_INIT_CALL(neogeo);
	install_pvc_protection(machine);
}

static DRIVER_INIT( svcplus )
{
	svcplus_px_decrypt(machine);
	svcboot_cx_decrypt(machine);
	neogeo_bootleg_sx_decrypt(machine, 1);
	svcplus_px_hack(machine);
	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( svcplusa )
{
	svcplusa_px_decrypt(machine);
	svcboot_cx_decrypt(machine);
	svcplus_px_hack(machine);
	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( svcsplus )
{
	svcsplus_px_decrypt(machine);
	neogeo_bootleg_sx_decrypt(machine, 2);
	svcboot_cx_decrypt(machine);
	svcsplus_px_hack(machine);
	DRIVER_INIT_CALL(neogeo);
	install_pvc_protection(machine);
}

static DRIVER_INIT( samsho5 )
{
	samsho5_decrypt_68k(machine);
	neo_pcm2_swap(machine, 4);
	neogeo_fixed_layer_bank_type = 1;
	neogeo_cmc50_m1_decrypt(machine);
	kof2000_neogeo_gfx_decrypt(machine, 0x0f);
	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( samsho5b )
{
	samsho5b_px_decrypt(machine);
	samsho5b_vx_decrypt(machine);
	neogeo_bootleg_sx_decrypt(machine, 1);
	neogeo_bootleg_cx_decrypt(machine);
	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( kf2k3pcb )
{
	kf2k3pcb_decrypt_68k(machine);
	kf2k3pcb_gfx_decrypt(machine);
	kof2003biosdecode(machine);
	neogeo_cmc50_m1_decrypt(machine);

	/* extra little swap on the m1 - this must be performed AFTER the m1 decrypt
	 or the m1 checksum (used to generate the key) for decrypting the m1 is
	 incorrect */
	{
		int i;
		UINT8* rom = memory_region(machine, "audiocpu");
		for (i=0;i<0x90000;i++)
		{
			rom[i] = BITSWAP8(rom[i], 5, 6, 1, 4, 3, 0, 7, 2);
		}

	}

	kof2000_neogeo_gfx_decrypt(machine, 0x9d);
	kf2k3pcb_decrypt_s1data(machine);
	neo_pcm2_swap(machine, 5);
	neogeo_fixed_layer_bank_type = 2;
	DRIVER_INIT_CALL(neogeo);
	install_pvc_protection(machine);
	memory_install_read16_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xc00000, 0xc7ffff, 0, 0, (read16_space_func)SMH_BANK(6) ); // 512k bios
}

static DRIVER_INIT( kof2003 )
{
	kof2003_decrypt_68k(machine);
	neo_pcm2_swap(machine, 5);
	neogeo_fixed_layer_bank_type = 2;
	neogeo_cmc50_m1_decrypt(machine);
	kof2000_neogeo_gfx_decrypt(machine, 0x9d);
	DRIVER_INIT_CALL(neogeo);
	install_pvc_protection(machine);
}

static DRIVER_INIT( kof2003h )
{
	kof2003h_decrypt_68k(machine);
	neo_pcm2_swap(machine, 5);
	neogeo_fixed_layer_bank_type = 2;
	neogeo_cmc50_m1_decrypt(machine);
	kof2000_neogeo_gfx_decrypt(machine, 0x9d);
	DRIVER_INIT_CALL(neogeo);
	install_pvc_protection(machine);
}

static DRIVER_INIT( kf2k3bl )
{
	cmc50_neogeo_gfx_decrypt(machine, 0x9d);
	neo_pcm2_swap(machine, 5);
	neogeo_bootleg_sx_decrypt(machine, 1);
	DRIVER_INIT_CALL(neogeo);
	kf2k3bl_install_protection(machine);
}

static DRIVER_INIT( kf2k3pl )
{
	cmc50_neogeo_gfx_decrypt(machine, 0x9d);
	neo_pcm2_swap(machine, 5);
	kf2k3pl_px_decrypt(machine);
	neogeo_bootleg_sx_decrypt(machine, 1);
	DRIVER_INIT_CALL(neogeo);
	kf2k3pl_install_protection(machine);
}

static DRIVER_INIT( kf2k3upl )
{
	cmc50_neogeo_gfx_decrypt(machine, 0x9d);
	neo_pcm2_swap(machine, 5);
	kf2k3upl_px_decrypt(machine);
	neogeo_bootleg_sx_decrypt(machine, 2);
	DRIVER_INIT_CALL(neogeo);
	kf2k3upl_install_protection(machine);
}

static DRIVER_INIT( samsh5sp )
{
	samsh5sp_decrypt_68k(machine);
	neo_pcm2_swap(machine, 6);
	neogeo_fixed_layer_bank_type = 1;
	neogeo_cmc50_m1_decrypt(machine);
	kof2000_neogeo_gfx_decrypt(machine, 0x0d);
	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( jockeygp )
{
	UINT16* extra_ram;

	neogeo_fixed_layer_bank_type = 1;
	neogeo_cmc50_m1_decrypt(machine);
	kof2000_neogeo_gfx_decrypt(machine, 0xac);

	/* install some extra RAM */
	extra_ram = auto_alloc_array(machine, UINT16, 0x2000/2);
	state_save_register_global_pointer(machine, extra_ram, 0x2000 / 2);

	memory_install_readwrite16_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x200000, 0x201fff, 0, 0, (read16_space_func)SMH_BANK(8), (write16_space_func)SMH_BANK(8));
	memory_set_bankptr(machine, NEOGEO_BANK_EXTRA_RAM, extra_ram);

	//  memory_install_read_port_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x280000, 0x280001, 0, 0, "IN5");
	//  memory_install_read_port_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x2c0000, 0x2c0001, 0, 0, "IN6");

	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( vliner )
{
	UINT16* extra_ram;

	/* install some extra RAM */
	extra_ram = auto_alloc_array(machine, UINT16, 0x2000/2);
	state_save_register_global_pointer(machine, extra_ram, 0x2000 / 2);

	memory_install_readwrite16_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x200000, 0x201fff, 0, 0, (read16_space_func)SMH_BANK(8), (write16_space_func)SMH_BANK(8));
	memory_set_bankptr(machine, NEOGEO_BANK_EXTRA_RAM, extra_ram);

	memory_install_read_port_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x280000, 0x280001, 0, 0, "IN5");
	memory_install_read_port_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x2c0000, 0x2c0001, 0, 0, "IN6");

	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( kog )
{
	/* overlay cartridge ROM */
	memory_install_read_port_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x0ffffe, 0x0fffff, 0, 0, "JUMPER");

	kog_px_decrypt(machine);
	neogeo_bootleg_sx_decrypt(machine, 1);
	neogeo_bootleg_cx_decrypt(machine);
	DRIVER_INIT_CALL(neogeo);
}

static DRIVER_INIT( lans2004 )
{
	lans2004_decrypt_68k(machine);
	lans2004_vx_decrypt(machine);
	neogeo_bootleg_sx_decrypt(machine, 1);
	neogeo_bootleg_cx_decrypt(machine);
	DRIVER_INIT_CALL(neogeo);
}

#endif

struct roms_init_func {
	char *name;
	int (*init)(GAME_ROMS *r);
} init_func_table[] = {
  //	{"mslugx",init_mslugx},
  { "kof99", init_kof99 }, 
  { "kof99n", init_kof99n }, 
  { "garou", init_garou },
  { "garouo", init_garouo },
  //	{"garoup",init_garoup},
  { "garoubl", init_garoubl }, 
  { "mslug3", init_mslug3 }, 
  { "mslug3h", init_mslug3h }, 
  { "mslug3n", init_mslug3h }, 
  { "mslug3b6", init_mslug3b6 }, 
  { "kof2000", init_kof2000 }, 
  { "kof2000n", init_kof2000n }, 
  { "kof2001", init_kof2001 }, 
  { "mslug4", init_mslug4 }, 
  { "ms4plus", init_ms4plus }, 
  { "ganryu", init_ganryu }, 
  { "s1945p", init_s1945p }, 
  { "preisle2", init_preisle2 }, 
  { "bangbead", init_bangbead }, 
  { "nitd", init_nitd }, 
  { "zupapa", init_zupapa }, 
  { "sengoku3", init_sengoku3 }, 
  { "kof98", init_kof98 }, 
  { "rotd", init_rotd }, 
  { "kof2002", init_kof2002 }, 
  { "kof2002b",	init_kof2002b }, 
  { "kf2k2pls", init_kf2k2pls }, 
  { "kf2k2mp", init_kf2k2mp }, 
  { "kof2km2", init_kof2km2 }, 
  { "matrim", init_matrim }, 
  { "pnyaa", init_pnyaa }, 
  { "mslug5", init_mslug5 }, 
  { "ms5pcb", init_ms5pcb }, 
  { "ms5plus", init_ms5plus },
  { NULL, NULL } };

static int allocate_region(ROM_REGION *r, Uint32 size, int region) {
  DEBUG_LOG("Allocating 0x%08x byte for Region %d\n", size,region );
	if (size != 0) {
#ifdef GP2X
		switch (region) {
		case REGION_AUDIO_CPU_CARTRIDGE:
			r->p = gp2x_ram_malloc(size, 1);
#               ifdef ENABLE_940T
			shared_data->sm1 = (Uint8*) ((r->p - gp2x_ram2) + 0x1000000);
			printf("Z80 code: %08x\n", (Uint32) shared_data->sm1);
#               endif
			break;
		case REGION_AUDIO_DATA_1:
			r->p = gp2x_ram_malloc(size, 0);
#               ifdef ENABLE_940T
			shared_data->pcmbufa = (Uint8*) (r->p - gp2x_ram);
			printf("SOUND1 code: %08x\n", (Uint32) shared_data->pcmbufa);
			shared_data->pcmbufa_size = size;
#               endif
			break;
		case REGION_AUDIO_DATA_2:
			r->p = gp2x_ram_malloc(size, 0);
#               ifdef ENABLE_940T
			shared_data->pcmbufb = (Uint8*) (r->p - gp2x_ram);
			printf("SOUND2 code: %08x\n", (Uint32) shared_data->pcmbufa);
			shared_data->pcmbufb_size = size;
#               endif
			break;
		default:
			r->p = malloc(size);
			break;

		}
#else
		r->p=malloc(size);
#endif
		if (r->p == 0)
			return 1;
		memset(r->p, 0, size);
	} else
		r->p = NULL;
	r->size = size;
	return 0;
}

static void free_region(ROM_REGION *r) {
	DEBUG_LOG("Free Region %p %p %d\n",r,r->p,r->size);
	if (r->p)
		free(r->p);
	r->size = 0;
	r->p = NULL;
}
/* Bourrin, mais bon... */
#if 0
static int zip_seek_current_file(unzFile *gz,Uint32 offset) {
	Uint8 *buf;
	Uint32 s=4096,c,i;
	buf=malloc(s);
	if (!buf) return -1;
	while (offset) {
		c = offset;
		if (c > s)
		c = s;

		c = unzReadCurrentFile(gz, buf, c);
		if (c == 0) {
			break;
		}
		offset -= c;
	}
	free(buf);
	return 0;

}
static int read_data_i(unzFile *gz,ROM_REGION *r,Uint32 dest,Uint32 size) {
	Uint8 *buf;
	Uint8 *p=r->p+dest;
	Uint32 s=4096,c,i;
	if (r->p==NULL || r->size<(dest&~0x1)+(size*2)) {
		printf("Region not allocated or not big enough %08x %08x\n",r->size,dest+(size*2));
		return -1;
	}
	buf=malloc(s);
	if (!buf) return -1;

	while (size) {
		c = size;
		if (c > s)
		c = s;

		c = unzReadCurrentFile(gz, buf, c);
		if (c == 0) {
			free(buf);
			return 0;
		}
		for (i = 0; i < c; i++) {
			//printf("%d %d\n",i,c);
			*p = buf[i];
			p += 2;
		}
		size -= c;
	}
	free(buf);
	return 0;
}
static int read_data_p(unzFile *gz,ROM_REGION *r,Uint32 dest,Uint32 size) {
	if (r->p==NULL || r->size<dest+size) {
		printf("Region not allocated or not big enough\n");
		return -1;
	}
	unzReadCurrentFile(gz,r->p+dest,size);
	//printf("%08x %08x\n",((Uint32*)(r->p))[0],dest);
	return 0;
}
static int load_region(unzFile *gz,GAME_ROMS *r,int region, Uint32 src, Uint32 dest, Uint32 size, Uint32 crc,char *filename) {
	int rc;
	int badcrc=0;
	/*	if (region==5 || region==0 || region==7) {
	 DEBUG_LOG("Roms contain bios info... TODO\n");
	 //return 0;
	 }
	 */
	if (unzLocateFileByCRC(gz,crc) == UNZ_END_OF_LIST_OF_FILE) {
		DEBUG_LOG("File with crc %08x not found. Trying %s : ",crc,filename);
		if (unzLocateFile(gz,filename,2) == UNZ_END_OF_LIST_OF_FILE) {
			DEBUG_LOG("KO\n");
			DEBUG_LOG("Load file %-17s in region %d: KO\n",filename,region);
			return 1;
		} else {
			DEBUG_LOG("OK\n");
			badcrc=1;
		}
	}
	if (unzOpenCurrentFile(gz) != UNZ_OK) {
		DEBUG_LOG("Load file %-17s in region %d: KO\n",filename,region);
		return 1;
	}
	if (src!=0) {

		if (region==REGION_SPRITES)
		rc=zip_seek_current_file(gz,src/2);
		else
		rc=zip_seek_current_file(gz,src);
		DEBUG_LOG("setoffset: %d %08x %08x %08x\n",rc,src,dest,size);
	}

	switch(region) {
		case REGION_SPRITES: /* Special interleaved loading  */
		read_data_i(gz,&r->tiles,dest,size);
		break;
		case REGION_AUDIO_CPU_CARTRIDGE:
		read_data_p(gz,&r->cpu_z80,dest,size);
		break;
		case REGION_AUDIO_CPU_ENCRYPTED:
		read_data_p(gz,&r->cpu_z80c,dest,size);
		break;
		case REGION_MAIN_CPU_CARTRIDGE:
		read_data_p(gz,&r->cpu_m68k,dest,size);
		break;
		case REGION_FIXED_LAYER_CARTRIDGE:
		read_data_p(gz,&r->game_sfix,dest,size);
		break;
		case REGION_AUDIO_DATA_1:
		read_data_p(gz,&r->adpcma,dest,size);
		break;
		case REGION_AUDIO_DATA_2:
		read_data_p(gz,&r->adpcmb,dest,size);
		break;
		case REGION_MAIN_CPU_BIOS:
		read_data_p(gz,&r->bios_m68k,dest,size);
		break;
		case REGION_AUDIO_CPU_BIOS:
		read_data_p(gz,&r->bios_m68k,dest,size);
		break;
		case REGION_FIXED_LAYER_BIOS:
		read_data_p(gz,&r->bios_sfix,dest,size);
		break;

		default:
		DEBUG_LOG("Unhandled region %d\n",region);
		break;

	}
	DEBUG_LOG("Load file %-17s in region %d: OK %s\n",filename,region,(badcrc?"(Bad CRC)":""));
	unzCloseCurrentFile(gz);
	return 0;
}

#else
static int zip_seek_current_file(ZFILE *gz, Uint32 offset) {
	Uint8 *buf;
	Uint32 s = 4096, c;
	buf = malloc(s);
	if (!buf)
		return -1;
	while (offset) {
		c = offset;
		if (c > s)
			c = s;

		c = gn_unzip_fread(gz, buf, c);
		if (c == 0) {
			break;
		}
		offset -= c;
	}
	free(buf);
	return 0;

}
static int read_data_i(ZFILE *gz, ROM_REGION *r, Uint32 dest, Uint32 size) {
	//Uint8 *buf;
	Uint8 *p = r->p + dest;
	Uint32 s = LOAD_BUF_SIZE, c, i;
	if (r->p == NULL || r->size < (dest & ~0x1) + (size * 2)) {
		printf("Region not allocated or not big enough %08x %08x\n", r->size,
				dest + (size * 2));
		return -1;
	}
	//buf=malloc(s);
	if (!iloadbuf)
		return -1;

	while (size) {
		c = size;
		if (c > s)
			c = s;

		c = gn_unzip_fread(gz, iloadbuf, c);
		if (c == 0) {
			//free(buf);
			return 0;
		}
		for (i = 0; i < c; i++) {
			//printf("%d %d\n",i,c);
			*p = iloadbuf[i];
			p += 2;
		}
		size -= c;
	}
	//free(buf);
	return 0;
}
static int read_data_p(ZFILE *gz, ROM_REGION *r, Uint32 dest, Uint32 size) {
	if (r->p == NULL || r->size < dest + size) {
		printf("Region not allocated or not big enough\n");
		return -1;
	}
	gn_unzip_fread(gz, r->p + dest, size);
	//printf("%08x %08x\n",((Uint32*)(r->p))[0],dest);
	return 0;
}
static int load_region(PKZIP *pz, GAME_ROMS *r, int region, Uint32 src,
		Uint32 dest, Uint32 size, Uint32 crc, char *filename) {
	int rc;
	int badcrc = 0;
	ZFILE *gz;
	/*	if (region==5 || region==0 || region==7) {
	 DEBUG_LOG("Roms contain bios info... TODO\n");
	 //return 0;
	 }
	 */
	gz = gn_unzip_fopen(pz, filename, crc);
	if (gz == NULL) {
		DEBUG_LOG("KO\n");
		DEBUG_LOG("Load file %-17s in region %d: KO\n", filename, region);
		return 1;
	}

	if (src != 0) { /* TODO: Reuse an allready opened zfile */

		if (region == REGION_SPRITES)
			rc = zip_seek_current_file(gz, src / 2);
		else
			rc = zip_seek_current_file(gz, src);
		DEBUG_LOG("setoffset: %d %08x %08x %08x\n", rc, src, dest, size);
	}

	DEBUG_LOG("Trying to load file %-17s in region %d\n", filename, region);

	switch (region) {
	case REGION_SPRITES: /* Special interleaved loading  */
		read_data_i(gz, &r->tiles, dest, size);
		break;
	case REGION_AUDIO_CPU_CARTRIDGE:
		read_data_p(gz, &r->cpu_z80, dest, size);
		break;
	case REGION_AUDIO_CPU_ENCRYPTED:
		read_data_p(gz, &r->cpu_z80c, dest, size);
		break;
	case REGION_MAIN_CPU_CARTRIDGE:
		read_data_p(gz, &r->cpu_m68k, dest, size);
		break;
	case REGION_FIXED_LAYER_CARTRIDGE:
		read_data_p(gz, &r->game_sfix, dest, size);
		break;
	case REGION_AUDIO_DATA_1:
	        read_data_p(gz, &r->adpcma, dest, size);
		break;
	case REGION_AUDIO_DATA_2:
		read_data_p(gz, &r->adpcmb, dest, size);
		break;
	case REGION_MAIN_CPU_BIOS:
		read_data_p(gz, &r->bios_m68k, dest, size);
		break;
	case REGION_AUDIO_CPU_BIOS:
		read_data_p(gz, &r->bios_m68k, dest, size);
		break;
	case REGION_FIXED_LAYER_BIOS:
		read_data_p(gz, &r->bios_sfix, dest, size);
		break;

	default:
		DEBUG_LOG("Unhandled region %d\n", region);
		break;

	}
	DEBUG_LOG("Load file %-17s in region %d: OK %s\n", filename, region,
			(badcrc ? "(Bad CRC)" : ""));
	//unzCloseCurrentFile(gz);
	gn_unzip_fclose(gz);
	return 0;
}

#endif
#if 0
static unzFile *open_rom_zip(char *rom_path,char *name) {
	char *buf;
	int size=strlen(rom_path)+strlen(name)+6;
	unzFile *gz;
	buf=malloc(size);
	snprintf(buf,size,"%s/%s.zip",rom_path,name);
	gz=unzOpen(buf);
	free(buf);
	return gz;
}
#else
static PKZIP *open_rom_zip(char *rom_path, char *name) {
	char *buf;
	int size = strlen(rom_path) + strlen(name) + 6;
	PKZIP *gz;
	buf = malloc(size);
	snprintf(buf, size, "%s/%s.zip", rom_path, name);
	gz = gn_open_zip(buf);
	free(buf);
	return gz;
}
#endif

static int convert_roms_tile(Uint8 *g, int tileno) {
	unsigned char swap[128];
	unsigned int *gfxdata;
	int x, y;
	unsigned int pen, usage = 0;
	gfxdata = (Uint32*) &g[tileno << 7];

	memcpy(swap, gfxdata, 128);

	//filed=1;
	for (y = 0; y < 16; y++) {
		unsigned int dw;

		dw = 0;
		for (x = 0; x < 8; x++) {
			pen = ((swap[64 + (y << 2) + 3] >> x) & 1) << 3;
			pen |= ((swap[64 + (y << 2) + 1] >> x) & 1) << 2;
			pen |= ((swap[64 + (y << 2) + 2] >> x) & 1) << 1;
			pen |= (swap[64 + (y << 2)] >> x) & 1;
			//if (!pen) filed=0;
			dw |= pen << ((7 - x) << 2);
			//memory.pen_usage[tileno]  |= (1 << pen);
			usage |= (1 << pen);
		}
		*(gfxdata++) = dw;

		dw = 0;
		for (x = 0; x < 8; x++) {
			pen = ((swap[(y << 2) + 3] >> x) & 1) << 3;
			pen |= ((swap[(y << 2) + 1] >> x) & 1) << 2;
			pen |= ((swap[(y << 2) + 2] >> x) & 1) << 1;
			pen |= (swap[(y << 2)] >> x) & 1;
			//if (!pen) filed=0;
			dw |= pen << ((7 - x) << 2);
			//memory.pen_usage[tileno]  |= (1 << pen);
			usage |= (1 << pen);
		}
		*(gfxdata++) = dw;
	}

	//if ((usage & ~1) == 0) pen_usage|=(TILE_INVISIBLE<<((tileno&0xF)*2));
	/* TODO transpack support */
	if ((usage & ~1) == 0)
		return (TILE_INVISIBLE << ((tileno & 0xF) * 2));
	else
		return 0;

}

static void convert_all_tile(GAME_ROMS *r) {
	Uint32 i;
	allocate_region(&r->spr_usage, (r->tiles.size >> 11) * sizeof(Uint32), -1);
	memset(r->spr_usage.p, 0, r->spr_usage.size);
	for (i = 0; i < r->tiles.size >> 7; i++) {
		((Uint32*) r->spr_usage.p)[i >> 4] |= convert_roms_tile(r->tiles.p, i);
	}
}

static int init_roms(GAME_ROMS *r) {
	int i = 0;

	while (init_func_table[i].name) {
		if (strcmp(init_func_table[i].name, r->info.name) == 0
				&& init_func_table[i].init != NULL) {
			DEBUG_LOG("Special init func\n");
			return init_func_table[i].init(r);
		}
		i++;
	}
	DEBUG_LOG("Default roms init\n");
	return 0;
}

SDL_bool dr_load_bios(GAME_ROMS *r) {
	FILE *f;
	int i;
	PKZIP *pz;
	ZFILE *z;
	int size;
	char *rpath = CF_STR(cf_get_item_by_name("rompath"));
	char *fpath;
	char *romfile;
	fpath = malloc(strlen(rpath) + strlen("neogeo.zip") + 2);
	sprintf(fpath, "%s/neogeo.zip", rpath);

	pz = gn_open_zip(fpath);
	if (pz == NULL) {
		fprintf(stderr, "Can't open BIOS (%s)\n", fpath);
		free(fpath);
		return SDL_FALSE;
	}

	memory.ng_lo = gn_unzip_file_malloc(pz, "000-lo.lo", 0x0, &size);

	if (!(r->info.flags & HAS_CUSTOM_SFIX_BIOS)) {
	  r->bios_sfix.p = gn_unzip_file_malloc(pz, "sfix.sfx", 0x0,
						&r->bios_sfix.size);
	}
	/* convert bios fix char */
	convert_all_char(memory.rom.bios_sfix.p, 0x20000, memory.fix_board_usage);

	if (!(r->info.flags & HAS_CUSTOM_CPU_BIOS)) {
		if (conf.system == SYS_UNIBIOS) {
			char *unipath = malloc(strlen(rpath) + strlen("uni-bios.rom") + 2);
			sprintf(unipath, "%s/uni-bios.rom", rpath);
			f = fopen(unipath, "rb");
			if (!f) {
				fprintf(stderr, "Can't open Universal BIOS (%s)\n", unipath);
				free(fpath);
				free(unipath);
				return SDL_FALSE;
			}
			r->bios_m68k.p = malloc(0x20000);
			fread(r->bios_m68k.p, 0x20000, 1, f);
			r->bios_m68k.size = 0x20000;
			fclose(f);
			free(unipath);
		} else {
			if (conf.system == SYS_HOME) {
				romfile = "aes-bios.bin";
			} else {
				switch (conf.country) {
				case CTY_JAPAN:
					romfile = "vs-bios.rom";
					break;
				case CTY_USA:
					romfile = "usa_2slt.bin";
					break;
				case CTY_ASIA:
					romfile = "asia-s3.rom";
					break;
				default:
					romfile = "sp-s2.sp1";
					break;
				}
			}
			DEBUG_LOG("Loading %s\n", romfile);
			r->bios_m68k.p = gn_unzip_file_malloc(pz, romfile, 0x0,
					&r->bios_m68k.size);
			if (r->bios_m68k.p == NULL) {
				printf("Couldn't loas bios %s\n", romfile);
				goto error;
			}
		}
	}

	gn_close_zip(pz);
	free(fpath);
	return SDL_TRUE;

	error: gn_close_zip(pz);
	free(fpath);
	return SDL_FALSE;
}

ROM_DEF *dr_check_zip(char *filename) {
	char *game = strdup(basename(filename));
	char *z;
	ROM_DEF *drv;
	printf("Game=%s\n", game);
	if (game == NULL)
		return NULL;
	z = strstr(game, ".zip");
	printf("z=%s\n", game);
	if (z == NULL)
		return NULL;
	z[0] = 0;
	drv = res_load_drv(game);
	return drv;
}

int dr_load_roms(GAME_ROMS *r, char *rom_path, char *name) {
	//unzFile *gz,*gzp=NULL,*rdefz;
	PKZIP *gz, *gzp = NULL;
	ROM_DEF *drv;
	int i;

	memset(r,0,sizeof(GAME_ROMS));

	drv = res_load_drv(name);
	if (!drv) {
		fprintf(stderr, "Can't find rom driver for %s\n", name);
		return SDL_FALSE;
	}

	gz = open_rom_zip(rom_path, name);
	if (gz == NULL) {
		printf("File %s/%s.zip not found\n", rom_path, name);
		return SDL_FALSE;
	}

	/* Open Parent.
	 For now, only one parent is supported, no recursion
	 */
	if (strcasecmp(drv->parent, "0") != 0) {
		gzp = open_rom_zip(rom_path, drv->parent);
	}

	//printf("year %d\n",drv->year);
	//return;

	r->info.name = strdup(drv->name);
	r->info.longname = strdup(drv->longname);
	r->info.year = drv->year;
	r->info.flags = 0;
	allocate_region(&r->cpu_m68k, drv->romsize[REGION_MAIN_CPU_CARTRIDGE],
			REGION_MAIN_CPU_CARTRIDGE);
	if (drv->romsize[REGION_AUDIO_CPU_CARTRIDGE] == 0
			&& drv->romsize[REGION_AUDIO_CPU_ENCRYPTED] != 0) {
		//allocate_region(&r->cpu_z80,drv->romsize[REGION_AUDIO_CPU_ENCRYPTED]);
		//allocate_region(&r->cpu_z80c,drv->romsize[REGION_AUDIO_CPU_ENCRYPTED]);
		allocate_region(&r->cpu_z80c, 0x80000, REGION_AUDIO_CPU_ENCRYPTED);
		allocate_region(&r->cpu_z80, 0x90000, REGION_AUDIO_CPU_CARTRIDGE);
	} else {
		allocate_region(&r->cpu_z80, drv->romsize[REGION_AUDIO_CPU_CARTRIDGE],
				REGION_AUDIO_CPU_CARTRIDGE);
	}
	allocate_region(&r->tiles, drv->romsize[REGION_SPRITES], REGION_SPRITES);
	allocate_region(&r->game_sfix, drv->romsize[REGION_FIXED_LAYER_CARTRIDGE],
			REGION_FIXED_LAYER_CARTRIDGE);
	allocate_region(&r->adpcma, drv->romsize[REGION_AUDIO_DATA_1],
			REGION_AUDIO_DATA_1);
	allocate_region(&r->adpcmb, drv->romsize[REGION_AUDIO_DATA_2],
			REGION_AUDIO_DATA_2);

	/* Allocate bios if necessary */
	DEBUG_LOG("BIOS SIZE %08x %08x %08x\n", drv->romsize[REGION_MAIN_CPU_BIOS],
			drv->romsize[REGION_AUDIO_CPU_BIOS],
			drv->romsize[REGION_FIXED_LAYER_BIOS]);
	if (drv->romsize[REGION_MAIN_CPU_BIOS] != 0) {
		r->info.flags |= HAS_CUSTOM_CPU_BIOS;
		allocate_region(&r->bios_m68k, drv->romsize[REGION_MAIN_CPU_BIOS],
				REGION_MAIN_CPU_BIOS);
	}
	if (drv->romsize[REGION_AUDIO_CPU_BIOS] != 0) {
		r->info.flags |= HAS_CUSTOM_AUDIO_BIOS;
		allocate_region(&r->bios_audio, drv->romsize[REGION_AUDIO_CPU_BIOS],
				REGION_AUDIO_CPU_BIOS);
	}
	if (drv->romsize[REGION_FIXED_LAYER_BIOS] != 0) {
		r->info.flags |= HAS_CUSTOM_SFIX_BIOS;
		allocate_region(&r->bios_sfix, drv->romsize[REGION_FIXED_LAYER_BIOS],
				REGION_FIXED_LAYER_BIOS);
	}

	iloadbuf = malloc(LOAD_BUF_SIZE);

	/* Now, load the roms */
	gn_init_pbar("Loading");
	for (i = 0; i < drv->nb_romfile; i++) {
		gn_update_pbar(i, drv->nb_romfile);
		if (load_region(gz, r, drv->rom[i].region, drv->rom[i].src,
				drv->rom[i].dest, drv->rom[i].size, drv->rom[i].crc,
				drv->rom[i].filename) != 0) {
			/* File not found in the roms, try the parent */
			if (gzp) {
				int region = drv->rom[i].region;
				int pi;
				pi = load_region(gzp, r, drv->rom[i].region, drv->rom[i].src,
						drv->rom[i].dest, drv->rom[i].size, drv->rom[i].crc,
						drv->rom[i].filename);
				DEBUG_LOG("From parent %d\n", pi);
				if (pi && (region != 5 && region != 0 && region != 7)) {
					fprintf(stderr, "ERROR: File %s not found\n",
							drv->rom[i].filename);
					goto error1;
				}
			} else {
				int region = drv->rom[i].region;
				if (region != 5 && region != 0 && region != 7) {
					fprintf(stderr, "ERROR: File %s not found\n",
							drv->rom[i].filename);
					goto error1;
				}

			}
		}

	}
	gn_terminate_pbar();
	/* Close/clean up */
	//unzClose(gz);
	gn_close_zip(gz);

	//if (gzp) unzClose(gzp);
	if (gzp)
		gn_close_zip(gzp);
	free(drv);

	if (r->adpcmb.size == 0) {
		r->adpcmb.p = r->adpcma.p;
		r->adpcmb.size = r->adpcma.size;
#ifdef ENABLE_940T
		shared_data->pcmbufb = (Uint8*) (r->adpcmb.p - gp2x_ram);
		printf("SOUND2 code: %08x\n", (Uint32) shared_data->pcmbufb);
		shared_data->pcmbufb_size = r->adpcmb.size;
#endif
	}

	memory.fix_game_usage = malloc(r->game_sfix.size >> 5);
	memory.pen_usage = malloc((r->tiles.size >> 11) * sizeof(Uint32));
	CHECK_ALLOC(memory.pen_usage);
	memset(memory.pen_usage, 0, (r->tiles.size >> 11) * sizeof(Uint32));
	memory.nb_of_tiles = r->tiles.size >> 7;

	free(iloadbuf);

	/* Init rom and bios */
	init_roms(r);
	convert_all_tile(r);
	return dr_load_bios(r);

	error1:

	//unzClose(gz);
	//if (gzp) unzClose(gzp);
	gn_close_zip(gz);
	if (gzp)
		gn_close_zip(gzp);

	free(drv);
	return SDL_FALSE;
}
/* TODO: */
void set_bankswitchers(int bt) {
	switch (bt) {
	case 0:
		mem68k_fetch_bksw_byte = mem68k_fetch_bk_normal_byte;
		mem68k_fetch_bksw_word = mem68k_fetch_bk_normal_word;
		mem68k_fetch_bksw_long = mem68k_fetch_bk_normal_long;
		mem68k_store_bksw_byte = mem68k_store_bk_normal_byte;
		mem68k_store_bksw_word = mem68k_store_bk_normal_word;
		mem68k_store_bksw_long = mem68k_store_bk_normal_long;
		break;
		/*
		 case BANKSW_KOF2003:
		 mem68k_fetch_bksw_byte=mem68k_fetch_bk_kof2003_byte;
		 mem68k_fetch_bksw_word=mem68k_fetch_bk_kof2003_word;
		 mem68k_fetch_bksw_long=mem68k_fetch_bk_kof2003_long;
		 mem68k_store_bksw_byte=mem68k_store_bk_kof2003_byte;
		 mem68k_store_bksw_word=mem68k_store_bk_kof2003_word;
		 mem68k_store_bksw_long=mem68k_store_bk_kof2003_long;
		 break;
		 case BANKSW_SCRAMBLE:
		 case BANKSW_MAX:
		 break;
		 */
	}
}
SDL_bool dr_load_game(char *name) {
	//GAME_ROMS rom;
	char *rpath = CF_STR(cf_get_item_by_name("rompath"));
	int rc;
	printf("Loading %s/%s.zip\n", rpath, name);
	memory.bksw_handler = 0;
	memory.bksw_unscramble = NULL;
	memory.bksw_offset = NULL;

	rc = dr_load_roms(&memory.rom, rpath, name);
	if (rc == SDL_FALSE) {
		return SDL_FALSE;
	}
	conf.game = memory.rom.info.name;
	/* TODO *///neogeo_fix_bank_type =0;
	/* TODO */
	set_bankswitchers(0);

	memcpy(memory.game_vector, memory.rom.cpu_m68k.p, 0x80);
	memcpy(memory.rom.cpu_m68k.p, memory.rom.bios_m68k.p, 0x80);

	convert_all_char(memory.rom.game_sfix.p, memory.rom.game_sfix.size,
			memory.fix_game_usage);

	/* TODO: Move this somewhere else. */
	init_video();

	return SDL_TRUE;

}

#ifdef HAVE_LIBZ
static int dump_region(FILE *gno, ROM_REGION *rom, Uint8 id, Uint8 type,
		Uint32 block_size) {
	if (rom->p == NULL)
		return FALSE;
	fwrite(&rom->size, sizeof(Uint32), 1, gno);
	fwrite(&id, sizeof(Uint8), 1, gno);
	fwrite(&type, sizeof(Uint8), 1, gno);
	if (type == 0) {
		printf("Dump %d %08x\n", id, rom->size);
		fwrite(rom->p, rom->size, 1, gno);
	} else {
		Uint32 nb_block = rom->size / block_size;
		Uint32 *block_offset;
		Uint32 cur_offset = 0;
		long offset_pos;
		Uint32 i;
		Uint8 *inbuf = rom->p;
		Uint8 *outbuf;
		uLongf outbuf_len;
		uLongf outlen;
		Uint32 cmpsize = 0;
		int rc;
		printf("nb_block=%d\n", nb_block);
		fwrite(&block_size, sizeof(Uint32), 1, gno);
		if ((rom->size & (block_size - 1)) != 0) {
			printf("Waring: Block_size and totsize not compatible %x %x\n",
					rom->size, block_size);
		}
		block_offset = malloc(nb_block * sizeof(Uint32));
		/* Zlib compress output buffer need to be at least the size 
		 of inbuf + 0.1% + 12 byte */
		outbuf_len = compressBound(block_size);
		outbuf = malloc(outbuf_len);
		offset_pos = ftell(gno);
		fseek(gno, nb_block * 4 + 4, SEEK_CUR); /* Skip all the offset table + the total compressed size */

		for (i = 0; i < nb_block; i++) {
			block_offset[i] = cur_offset;
			outlen = outbuf_len;
			rc = compress(outbuf, &outlen, inbuf, block_size);
			printf("%d %ld\n", rc, outlen);
			cur_offset += outlen;
			cmpsize += outlen;
			printf("cmpsize=%d\n", cmpsize);
			inbuf += block_size;
			fwrite(&outlen, sizeof(uLongf), 1, gno);
			fwrite(outbuf, outlen, 1, gno);
		}
		/* Now, write the offset table */
		fseek(gno, offset_pos, SEEK_SET);
		fwrite(block_offset, sizeof(Uint32), nb_block, gno);
		fwrite(&cmpsize, sizeof(Uint32), 1, gno);
		printf("cmpsize=%d\n", cmpsize);
		fseek(gno, 0, SEEK_END);
		offset_pos = ftell(gno);
		printf("currpos=%li\n", offset_pos);
	}
	return TRUE;
}

int dr_save_gno(GAME_ROMS *r, char *filename) {
	FILE *gno;
	char *fid = "gnodmpv1";
	char fname[9];
	Uint8 nb_sec = 0;
	gno = fopen(filename, "wb");
	if (!gno)
		return FALSE;

	if (r->cpu_m68k.p)
		nb_sec++;
	if (r->cpu_z80.p)
		nb_sec++;
	if (r->adpcma.p)
		nb_sec++;
	if (r->adpcmb.p)
		nb_sec++;
	if (r->game_sfix.p)
		nb_sec++;
	if (r->tiles.p)
		nb_sec += 2; /* Sprite + Sprite usage */
	/* TODO: Custom bios dump */

	/* Header information */
	fwrite(fid, 8, 1, gno);
	snprintf(fname, 9, "%-8s", r->info.name);
	fwrite(fname, 8, 1, gno);
	fwrite(&r->info.flags, sizeof(Uint32), 1, gno);
	fwrite(&nb_sec, sizeof(Uint8), 1, gno);

	/* Now each section */
	dump_region(gno, &r->cpu_m68k, REGION_MAIN_CPU_CARTRIDGE, 0, 0);
	dump_region(gno, &r->cpu_z80, REGION_AUDIO_CPU_CARTRIDGE, 0, 0);
	dump_region(gno, &r->adpcma, REGION_AUDIO_DATA_1, 0, 0);
	dump_region(gno, &r->adpcmb, REGION_AUDIO_DATA_2, 0, 0);
	dump_region(gno, &r->game_sfix, REGION_FIXED_LAYER_CARTRIDGE, 0, 0);
	dump_region(gno, &r->tiles, REGION_SPRITES, 1, 4096);
	dump_region(gno, &r->spr_usage, REGION_SPR_USAGE, 0, 0);
	/* TODO: Sprite usage */

	fclose(gno);
	return TRUE;
}
#else
static int dump_region(FILE *gno,ROM_REGION *rom,Uint8 id,Uint8 type,Uint32 block_size) {
	return TRUE;
}
int dr_save_gno(GAME_ROMS *r,char *filename) {
	return TRUE;
}
#endif

void dr_free_roms(GAME_ROMS *r) {
	free_region(&r->cpu_m68k);
	free_region(&r->cpu_z80);
	free_region(&r->cpu_z80c);
	free_region(&r->tiles);
	free_region(&r->game_sfix);
	if (r->adpcmb.p != r->adpcma.p)
		free_region(&r->adpcmb);
	else {
		r->adpcmb.p=NULL;
		r->adpcmb.size=0;
	}

	free_region(&r->adpcma);

	free_region(&r->bios_m68k);
	free_region(&r->bios_sfix);
	free_region(&r->bios_audio);

	free(memory.ng_lo);
	free(memory.pen_usage);
	free(memory.fix_game_usage);

	free(r->info.name);
	free(r->info.longname);
}
