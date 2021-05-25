/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	conf.h
 * Module:
 * Project:	lwdfwiz
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2021 Robinson Mittmann. All Rights Reserved.
 *
 * LWDFWiz is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef __CONF_H__
#define __CONF_H__

struct conf_type;

struct conf_var {
	const char *name;
	struct conf_type *type;
	void *p;
};

#define CONF_VOID 0
#define CONF_SECTION 1
#define CONF_INT 2
#define CONF_UINT 3
#define CONF_FLOAT 4
#define CONF_STRING 5
#define CONF_BOOLEAN 6
#define CONF_CHAR 7
#define CONF_HEX8 8
#define CONF_HEX16 9
#define CONF_HEX32 10
#define CONF_HEX64 11
#define CONF_BIN8 12
#define CONF_BIN16 13
#define CONF_BIN32 14
#define CONF_BIN64 15
#define CONF_OCT8 16
#define CONF_OCT16 17
#define CONF_OCT32 18
#define CONF_OCT64 19
#define CONF_IPADDR 20
#define CONF_RGB 21
#define CONF_RGBI 22
#define CONF_CYMK 23
#define CONF_RATIO 24
#define CONF_MAX 25
#define CONF_IPPORT 26

#define CONF_TYPE(TYPE) (&conf_type_tab[TYPE])

#define DEFINE_VOID(NM, PTR) {NM, CONF_TYPE(CONF_VOID), (void *)(PTR)},
#define DEFINE_SECTION(NM, PTR) {NM,  CONF_TYPE(CONF_SECTION), (void *)(PTR)},
#define DEFINE_INT(NM,  PTR) {NM, CONF_TYPE(CONF_INT), (void *)(PTR)},
#define DEFINE_UINT(NM,  PTR) {NM, CONF_TYPE(CONF_UINT), (void *)(PTR)},
#define DEFINE_STRING(NM, PTR) {NM, CONF_TYPE(CONF_STRING), (void *)(PTR)},
#define DEFINE_FLOAT(NM, PTR) {NM, CONF_TYPE(CONF_FLOAT), (void *)(PTR)},
#define DEFINE_BOOLEAN(NM, PTR) {NM, CONF_TYPE(CONF_BOOLEAN), (void *)(PTR)},
#define DEFINE_CHAR(NM, PTR) {NM, CONF_TYPE(CONF_CHAR), (void *)(PTR)},
#define DEFINE_HEX8(NM, PTR) {NM,  CONF_TYPE(CONF_HEX8), (void *)(PTR)},
#define DEFINE_HEX16(NM, PTR) {NM,  CONF_TYPE(CONF_HEX16), (void *)(PTR)},
#define DEFINE_HEX32(NM, PTR) {NM,  CONF_TYPE(CONF_HEX32), (void *)(PTR)},
#define DEFINE_HEX64(NM, PTR) {NM,  CONF_TYPE(CONF_HEX64), (void *)(PTR)},
#define DEFINE_BIN8(NM, PTR) {NM,  CONF_TYPE(CONF_BIN8), (void *)(PTR)},
#define DEFINE_BIN16(NM, PTR) {NM,  CONF_TYPE(CONF_BIN16), (void *)(PTR)},
#define DEFINE_BIN32(NM, PTR) {NM,  CONF_TYPE(CONF_BIN32), (void *)(PTR)},
#define DEFINE_BIN64(NM, PTR) {NM,  CONF_TYPE(CONF_BIN64), (void *)(PTR)},
#define DEFINE_OCT8(NM, PTR) {NM,  CONF_TYPE(CONF_OCT8), (void *)(PTR)},
#define DEFINE_OCT16(NM, PTR) {NM,  CONF_TYPE(CONF_OCT16), (void *)(PTR)},
#define DEFINE_OCT32(NM, PTR) {NM,  CONF_TYPE(CONF_OCT32), (void *)(PTR)},
#define DEFINE_OCT64(NM, PTR) {NM,  CONF_TYPE(CONF_OCT64), (void *)(PTR)},
#define DEFINE_IPADDR(NM, PTR) {NM,  CONF_TYPE(CONF_IPADDR), (void *)(PTR)},
#define DEFINE_IPPORT(NM, PTR) {NM,  CONF_TYPE(CONF_IPADDR), (void *)(PTR)},
#define DEFINE_RGB(NM, PTR) {NM,  CONF_TYPE(CONF_RGB), (void *)(PTR) },
#define DEFINE_RGBI(NM, PTR) {NM,  CONF_TYPE(CONF_RGBI), (void *)(PTR)},
#define DEFINE_CYMK(NM, PTR) {NM,  CONF_TYPE(CONF_CYMK), (void *)(PTR)},
#define DEFINE_RATIO(NM, PTR) {NM,  CONF_TYPE(CONF_RATIO), (void *)(PTR)},

#define SECTION_TYPE	struct conf_var 
#define BEGIN_SECTION(X) SECTION_TYPE X [] = {
#define SECTION_TERMINATOR	{ NULL, NULL, NULL }
#define END_SECTION SECTION_TERMINATOR };

#define EXPORT_VOID(VAR) DEFINE_VOID(#VAR, &(VAR))
#define EXPORT_SECTION(VAR) DEFINE_SECTION(#VAR, &(VAR))
#define EXPORT_INT(VAR) DEFINE_INT(#VAR, &(VAR))
#define EXPORT_UINT(VAR) DEFINE_UINT(#VAR, &(VAR))
#define EXPORT_FLOAT(VAR) DEFINE_FLOAT(#VAR, &(VAR))
#define EXPORT_STRING(VAR) DEFINE_STRING(#VAR, &(VAR))
#define EXPORT_BOOLEAN(VAR) DEFINE_BOOLEAN(#VAR, &(VAR))
#define EXPORT_CHAR(VAR) DEFINE_CHAR(#VAR, &(VAR))
#define EXPORT_HEX8(VAR) DEFINE_HEX8(#VAR, &(VAR))
#define EXPORT_HEX16(VAR) DEFINE_HEX16(#VAR, &(VAR))
#define EXPORT_HEX32(VAR) DEFINE_HEX32(#VAR, &(VAR))
#define EXPORT_HEX64(VAR) DEFINE_HEX64(#VAR, &(VAR))
#define EXPORT_BIN8(VAR) DEFINE_BIN8(#VAR, &(VAR))
#define EXPORT_BIN16(VAR) DEFINE_BIN16(#VAR, &(VAR))
#define EXPORT_BIN32(VAR) DEFINE_BIN32(#VAR, &(VAR))
#define EXPORT_BIN64(VAR) DEFINE_BIN64(#VAR, &(VAR))
#define EXPORT_OCT8(VAR) DEFINE_OCT8(#VAR, &(VAR))
#define EXPORT_OCT16(VAR) DEFINE_OCT16(#VAR, &(VAR))
#define EXPORT_OCT32(VAR) DEFINE_OCT32(#VAR, &(VAR))
#define EXPORT_OCT64(VAR) DEFINE_OCT64(#VAR, &(VAR))
#define EXPORT_IPADDR(VAR) DEFINE_IPADDR(#VAR, &(VAR))
#define EXPORT_RGB(VAR) DEFINE_RGB(#VAR, &(VAR))
#define EXPORT_RGBI(VAR) DEFINE_RGBI(#VAR, &(VAR))
#define EXPORT_CYMK(VAR) DEFINE_CYMK(#VAR, &(VAR))
#define EXPORT_RATIO(VAR) DEFINE_RATIO(#VAR, &(VAR))

struct conf_type {
	int t_code;
	char *t_name;
	int (*t_get) (struct conf_var *, char *);
	int (*t_set) (struct conf_var *, const char *);
};

extern struct conf_type conf_type_tab[];

#ifdef __cplusplus
extern "C" {
#endif
	int conf_lookup(char *, const char **, const char **);

	int dump_conf(struct conf_var *);
	int load_conf(const char *, struct conf_var *);
	int save_conf(const char *, struct conf_var *);

	/**
	 * It cannot perform lookups for section names on the root section, due to 
	 * the way it is defined. If done so, the root section address is returned
	 * instead of the (expected) sibling section.
	 */
	struct conf_var * var_lookup(struct conf_var *, const char *);
	int var_set(struct conf_var *, const char *, const char *);
	int var_get(struct conf_var *, const char *, char *);

	int void_get(struct conf_var *, char *);
	int void_set(struct conf_var *, const char *);

	int int_get(struct conf_var *, char *);
	int int_set(struct conf_var *, const char *);

	int uint_get(struct conf_var *, char *);
	int uint_set(struct conf_var *, const char *);

	int float_get(struct conf_var *, char *);
	int float_set(struct conf_var *, const char *);

	int string_get(struct conf_var *, char *);
	int string_set(struct conf_var *, const char *);

	int bool_get(struct conf_var *, char *);
	int bool_set(struct conf_var *, const char *);

	int char_get(struct conf_var *, char *);
	int char_set(struct conf_var *, const char *);

	int hex8_get(struct conf_var *, char *);
	int hex8_set(struct conf_var *, const char *);

	int hex16_get(struct conf_var *, char *);
	int hex16_set(struct conf_var *, const char *);

	int hex32_get(struct conf_var *, char *);
	int hex32_set(struct conf_var *, const char *);

	int hex64_get(struct conf_var *, char *);
	int hex64_set(struct conf_var *, const char *);

	int bin8_get(struct conf_var *, char *);
	int bin8_set(struct conf_var *, const char *);

	int bin16_get(struct conf_var *, char *);
	int bin16_set(struct conf_var *, const char *);

	int bin32_get(struct conf_var *, char *);
	int bin32_set(struct conf_var *, const char *);

	int bin64_get(struct conf_var *, char *);
	int bin64_set(struct conf_var *, const char *);

	int oct8_get(struct conf_var *, char *);
	int oct8_set(struct conf_var *, const char *);

	int oct16_get(struct conf_var *, char *);
	int oct16_set(struct conf_var *, const char *);

	int oct32_get(struct conf_var *, char *);
	int oct32_set(struct conf_var *, const char *);

	int oct64_get(struct conf_var *, char *);
	int oct64_set(struct conf_var *, const char *);

	int ipaddr_get(struct conf_var *, char *);
	int ipaddr_set(struct conf_var *, const char *);

	int ipport_get(struct conf_var *, char *);
	int ipport_set(struct conf_var *, const char *);

	int rgb_get(struct conf_var *, char *);
	int rgb_set(struct conf_var *, const char *);

	int rgbi_get(struct conf_var *, char *);
	int rgbi_set(struct conf_var *, const char *);

	int cymk_get(struct conf_var *, char *);
	int cymk_set(struct conf_var *, const char *);

	int ratio_get(struct conf_var *, char *);
	int ratio_set(struct conf_var *, const char *);
#ifdef  __cplusplus
}
#endif
#endif	/* __CONF_H__ */

