/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "util/test/suite.h"

#include <mgba/core/core.h>
#include <mgba/gba/core.h>
#include <mgba/internal/gba/gba.h>
#include <mgba-util/memory.h>
#include <mgba-util/vfs.h>

M_TEST_DEFINE(create) {
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->init(core));
	core->deinit(core);
}

M_TEST_DEFINE(platform) {
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->platform(core) == mPLATFORM_GBA);
	assert_true(core->init(core));
	core->deinit(core);
}

M_TEST_DEFINE(reset) {
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->init(core));
	mCoreInitConfig(core, NULL);
	core->reset(core);
	mCoreConfigDeinit(&core->config);
	core->deinit(core);
}

M_TEST_DEFINE(loadNullROM) {
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->init(core));
	assert_false(core->loadROM(core, NULL));
	mCoreInitConfig(core, NULL);
	core->reset(core);
	mCoreConfigDeinit(&core->config);
	core->deinit(core);
}

M_TEST_DEFINE(loadExtendedROM) {
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->init(core));

	uint8_t* image = anonymousMemoryMap(GBA_SIZE_XGBA_ROM);
	assert_non_null(image);
	memcpy(&image[0xB5], "XGBA256", 7);
	image[GBA_SIZE_ROM0] = 0x42;
	image[GBA_SIZE_ROM0 + 1] = 0x34;
	image[GBA_SIZE_ROM0 + 2] = 0x56;
	image[GBA_SIZE_ROM0 + 3] = 0x78;
	image[GBA_SIZE_ROM0 + 4] = 0x01;
	image[GBA_SIZE_ROM0 + 5] = 0x02;
	image[GBA_SIZE_ROM0 + 6] = 0x03;
	image[GBA_SIZE_ROM0 + 7] = 0x04;
	image[GBA_SIZE_XGBA_ROM / 2] = 0x78;
	image[GBA_SIZE_XGBA_ROM - 4] = 0x10;
	image[GBA_SIZE_XGBA_ROM - 3] = 0x20;
	image[GBA_SIZE_XGBA_ROM - 2] = 0x30;
	image[GBA_SIZE_XGBA_ROM - 1] = 0xA5;

	struct VFile* vf = VFileFromConstMemory(image, GBA_SIZE_XGBA_ROM);
	assert_non_null(vf);
	assert_true(core->loadROM(core, vf));

	struct GBA* gba = core->board;
	assert_int_equal(gba->memory.romSize, GBA_SIZE_ROM0);
	assert_int_equal(gba->memory.romMask, GBA_SIZE_ROM0 - 1);
	assert_int_equal(gba->memory.xgbaRomSize, GBA_SIZE_XGBA_ROM);
	assert_non_null(gba->memory.xgbaRom);
	assert_int_equal(gba->memory.xgbaRom[GBA_SIZE_ROM0], 0x42);
	assert_int_equal(gba->memory.xgbaRom[GBA_SIZE_XGBA_ROM / 2], 0x78);
	assert_int_equal(gba->memory.xgbaRom[GBA_SIZE_XGBA_ROM - 1], 0xA5);

	assert_int_equal(GBALoad8(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_ROM0, NULL), 0x42);
	assert_int_equal(GBALoad16(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_ROM0, NULL), 0x3442);
	assert_int_equal(GBALoad32(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_ROM0, NULL), 0x78563442);
	assert_int_equal(GBALoad16(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_ROM0 + 1, NULL), 0x42000034);
	assert_int_equal(GBALoad32(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_ROM0 + 1, NULL), 0x42785634);
	uint32_t ldmEnd = GBALoadMultiple(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_ROM0, 3, 0, NULL);
	assert_int_equal(gba->cpu->gprs[0], 0x78563442);
	assert_int_equal(gba->cpu->gprs[1], 0x04030201);
	assert_int_equal(ldmEnd, GBA_BASE_XGBA_ROM + GBA_SIZE_ROM0 + 8);
	assert_int_equal(GBALoad8(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_XGBA_ROM / 2, NULL), 0x78);
	assert_int_equal(GBALoad16(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_XGBA_ROM - 2, NULL), 0xA530);
	assert_int_equal(GBALoad32(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_XGBA_ROM - 4, NULL), 0xA5302010);
	assert_int_equal(GBALoad8(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_XGBA_ROM - 1, NULL), 0xA5);

	core->unloadROM(core);
	assert_null(gba->memory.rom);
	assert_null(gba->memory.xgbaRom);
	assert_int_equal(gba->memory.xgbaRomSize, 0);
	core->deinit(core);
	mappedMemoryFree(image, GBA_SIZE_XGBA_ROM);
}

#ifndef _WIN32
M_TEST_DEFINE(loadExtendedFileROM) {
	char path[] = "/tmp/xgba-rom-XXXXXX";
	int fd = mkstemp(path);
	assert_true(fd >= 0);
	assert_int_equal(ftruncate(fd, GBA_SIZE_XGBA_ROM), 0);
	assert_int_equal(lseek(fd, 0xB5, SEEK_SET), 0xB5);
	assert_int_equal(write(fd, "XGBA256", 7), 7);
	assert_int_equal(lseek(fd, GBA_SIZE_XGBA_ROM - 4, SEEK_SET), GBA_SIZE_XGBA_ROM - 4);
	uint8_t lastWord[4] = { 0x78, 0x56, 0x34, 0x12 };
	assert_int_equal(write(fd, lastWord, sizeof(lastWord)), sizeof(lastWord));
	assert_int_equal(close(fd), 0);

	struct VFile* vf = VFileOpen(path, O_RDONLY);
	assert_non_null(vf);
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->init(core));
	assert_true(core->loadROM(core, vf));
	struct GBA* gba = core->board;
	assert_int_equal(gba->memory.xgbaRomSize, GBA_SIZE_XGBA_ROM);
	assert_int_equal(GBALoad32(gba->cpu, GBA_BASE_XGBA_ROM + GBA_SIZE_XGBA_ROM - 4, NULL), 0x12345678);

	core->unloadROM(core);
	core->deinit(core);
	assert_int_equal(remove(path), 0);
}
#define XGBA_FILE_TEST cmocka_unit_test(loadExtendedFileROM),
#else
#define XGBA_FILE_TEST
#endif

M_TEST_DEFINE(unmarkedOversizedROM) {
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->init(core));

	const size_t size = GBA_SIZE_ROM0 * 2;
	uint8_t* image = anonymousMemoryMap(size);
	assert_non_null(image);
	struct VFile* vf = VFileFromConstMemory(image, size);
	assert_non_null(vf);
	assert_true(core->loadROM(core, vf));

	struct GBA* gba = core->board;
	assert_int_equal(gba->memory.romSize, GBA_SIZE_ROM0);
	assert_null(gba->memory.xgbaRom);
	assert_int_equal(gba->memory.xgbaRomSize, 0);

	core->unloadROM(core);
	core->deinit(core);
	mappedMemoryFree(image, size);
}

M_TEST_DEFINE(rejectExtendedROMOverLimit) {
	struct mCore* core = GBACoreCreate();
	assert_non_null(core);
	assert_true(core->init(core));

	const size_t size = (size_t) GBA_SIZE_XGBA_ROM + 1;
	uint8_t* image = anonymousMemoryMap(size);
	assert_non_null(image);
	memcpy(&image[0xB5], "XGBA256", 7);
	struct VFile* vf = VFileFromConstMemory(image, size);
	assert_non_null(vf);
	assert_false(core->loadROM(core, vf));
	vf->close(vf);

	struct GBA* gba = core->board;
	assert_null(gba->memory.rom);
	assert_null(gba->memory.xgbaRom);
	assert_int_equal(gba->memory.xgbaRomSize, 0);

	core->deinit(core);
	mappedMemoryFree(image, size);
}

M_TEST_SUITE_DEFINE(GBACore,
	cmocka_unit_test(create),
	cmocka_unit_test(platform),
	cmocka_unit_test(reset),
	cmocka_unit_test(loadNullROM),
	cmocka_unit_test(loadExtendedROM),
	XGBA_FILE_TEST
	cmocka_unit_test(unmarkedOversizedROM),
	cmocka_unit_test(rejectExtendedROMOverLimit))
