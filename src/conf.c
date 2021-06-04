/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	conf.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "conf.h"

static const char STUB_STRING [] = "NULL";

#define LTRIM(x) { char * _cp; \
                  for (_cp = x; isspace(*_cp) && (*_cp != 0); _cp++);\
                  x = _cp; }

#define RTRIM(x) { char * _cp; \
                  for (_cp = x + (strlen(x) -1); (_cp != x) && isspace(*_cp); _cp--);\
                  _cp++; *_cp = '\0'; }

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define MAX(x, y) ((x) > (y) ? (x) : (y))

struct conf_type conf_type_tab[] = {
	{CONF_VOID, "void", void_get, void_set},
	{CONF_SECTION, "section", void_get, void_set},
	{CONF_INT, "integer", int_get, int_set},
	{CONF_UINT, "unsigned", uint_get, uint_set},
	{CONF_FLOAT, "float", float_get, float_set},
	{CONF_STRING, "string", string_get, string_set},
	{CONF_BOOLEAN, "boolean", bool_get, bool_set},
	{CONF_CHAR, "char", char_get, char_set},
	{CONF_HEX8, "hex_8", hex8_get, hex8_set},
	{CONF_HEX16, "hex_16", hex16_get, hex16_set},
	{CONF_HEX32, "hex_32", hex32_get, hex32_set},
	{CONF_HEX64, "hex_64", hex64_get, hex64_set},
	{CONF_BIN8, "binary_8", bin8_get, bin8_set},
	{CONF_BIN16, "binary_16", bin16_get, bin16_set},
	{CONF_BIN32, "binary_32", bin32_get, bin32_set},
	{CONF_BIN64, "binary_64", bin64_get, bin64_set},
	{CONF_OCT8, "octal_8", oct8_get, oct8_set},
	{CONF_OCT16, "octal_16", oct16_get, oct16_set},
	{CONF_OCT32, "octal_32", oct32_get, oct32_set},
	{CONF_OCT64, "octal_64", oct64_get, oct64_set},
	/* {CONF_IPADDR, "ip_address", ipaddr_get, ipaddr_set}, */
	{CONF_IPADDR, "ip_address", string_get, string_set},
	{CONF_RGB, "rgb", rgb_get, rgb_set},
	{CONF_RGBI, "rgbi", rgbi_get, rgbi_set},
	{CONF_CYMK, "cymk", cymk_get, cymk_set},
	{CONF_RATIO, "ratio", ratio_get, ratio_set},
	/* {CONF_IPPORT, "ip_port", ipport_get, ipport_set} */
	{CONF_IPPORT, "ip_port", string_get, string_set}
};

struct conf_var *var_lookup(struct conf_var *section, const char *name)
{
	struct conf_var *entry;

	if ((name == NULL) || (*name == '\0')) {
		//      fprintf(stderr, "name invalid\n");
		return NULL;
	}

	if ((entry = section) == NULL) {
		//      fprintf(stderr, "section invalid\n");
		return NULL;
	}

	while (entry->name != NULL) {
		if (strcmp(entry->name, name) == 0)
			return entry;
		entry++;
	}

//      fprintf(stderr, "name not found\n");
	return NULL;
}

int var_set(struct conf_var *section, const char *name, const char *value)
{
	struct conf_var *entry;

	entry = var_lookup(section, name);
	if (entry == NULL)
		return -1;

	/* Assuming NULL set as a stub configuration. */
	if (value == NULL)
		value = STUB_STRING;
	
	if (entry->type->t_set(entry, value))
		return 0;

	return -1;
}

int var_get(struct conf_var *section, const char *name, char *value)
{
	struct conf_var *entry;

	if (value == NULL)
		return -1;

	entry = var_lookup(section, name);
	if (entry == NULL)
		return -1;

	if (entry->type->t_get(entry, value))
		return 0;
	else {
		strcpy(value, STUB_STRING);
		return 0;
	}

	return -1;
}

int load_conf(const char *path, struct conf_var *root)
{
	FILE *f;
	char buf[1024];
	char name[512];
	char value[512];
	int count;
	char *cp;
	char *ep;
	char *ap;
	int line;
	struct conf_var *section;
	struct conf_var *entry;
	int ignore = 0;

	#ifdef _CONF_DEBUG
	fprintf(stderr,
			"int load_conf(const char *path, struct conf_var *root)\n"
			"path = %p \"%s\",\nroot = %p\n{\n",
			path, path, root);
	#endif

	if (!(f = fopen(path, "r"))) {
		return -1;
	};

	line = 0;
	count = 0;

	/* global section first */
	section = root;
	#ifdef _CONF_DEBUG
	fprintf(stderr,
			"\tif (!(f = fopen(path, \"r\"))) {\n\t\treturn -1;\n\t};\n"
			"\tsection = root;\n\twhile (fgets(buf, 1024, f)) {\n");
	#endif
	
	while (fgets(buf, 1024, f)) {
		line++;
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tline++\t(interaction#: %i)\n", line);
		#endif

		cp = buf;
		
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tcp = buf;\t(buf: %p)\n", buf);
		#endif

		LTRIM(cp);
		RTRIM(cp);
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tLTRIM(cp);\n\t\tRTRIM(cp);\t(cp: %p,", cp);
		fprintf(stderr," *cp: %c)\n", *cp);
		#endif

		if ((*cp == '\0') || (*cp == '#') || (*cp == ';') || (*cp == '\n'))
			continue;

		if (*cp == '[') {

			cp++;
			LTRIM(cp);

			if ((ep = strchr(cp, ']')) != NULL)
				*ep = '\0';
			else {
				fprintf(stderr, "%s:%d: parse error.\n", path, line);
				ignore = 1;
				continue;
			}
			RTRIM(cp);

			/* search for a valid section entry, in the root section */
			entry = var_lookup(root, cp);
			if (entry == NULL) {
				/* section not found */
				fprintf(stderr, "%s:%d: section unknown '%s'\n",
						path, line, cp);
				ignore = 1;
				continue;
			}
			if (entry->type != CONF_TYPE(CONF_SECTION)) {
				fprintf(stderr, "%s:%d: not a section '%s'\n", path, line,
						cp);
				ignore = 1;
			}

			section = (struct conf_var *) entry->p;
			ignore = 0;
			continue;
		}

		if (ignore)
			continue;	/* search the next section */

		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tfor (ap = cp; (*cp != '\\0') && (*cp != '='); cp++);"
				"\t(*cp: %c, conf.c, 255)", *cp);
		#endif
		for (ap = cp; (*cp != '\0') && (*cp != '='); cp++);

		#ifdef _CONF_DEBUG
		fprintf(stderr,"\n\t\tif (*cp == '\\0') {...}\t(cp: %p, *cp: %c)",cp,*cp);
		#endif
		if (*cp == '\0') {
			fprintf(stderr, "%s:%d: parse error.\n", path, line);
			continue;
		}

		cp--;
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\n\t\tcp--\t(cp: %p)\n\t*cp = '\\0';", cp);
		#endif
		*cp = '\0';
		cp++;
		cp++;
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\n\t\tcp++;\n\tcp++;\t(cp: %p)\n", cp);
		#endif


		LTRIM(ap);
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tLTRIM(ap);\t(ap: %p, *ap: %c)\n", ap, *ap);
		#endif
		strcpy(name, ap);
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tstrcpy(name, ap);\t(ap[]: %s\n", ap);
		#endif

		RTRIM(cp);
		strcpy(value, cp);
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tRTRIM(cp);\n\t\tstrcpy(value, cp);\t(cp[]: %s)\n",cp);
		#endif

		entry = var_lookup(section, name);
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tentry = var_lookup(section, name);\n");
		#endif

		if (entry == NULL) {
			fprintf(stderr, "%s:%d: symbol unknown '%s'\n", path, line, name);
			continue;
		}
		#ifdef _CONF_DEBUG
		fprintf(stderr,"(entry: %p, entry->type: %p, entry->type->t_set: %p,"
				" value[]: %s)\n", entry, entry->type, entry->type->t_set, value);
		#endif

		/* set the variable */
		if (entry->type->t_set(entry, value))
			count++;
		else
			fprintf(stderr, "%s:%d: parse error.\n", path, line);
		#ifdef _CONF_DEBUG
		fprintf(stderr,"\t\tif (entry->type->t_set(entry, value))\n");
		#endif
	}
	fclose(f);

	return count;
}

/*
* Recursive writes a section into a stream
*
*/
static int write_section(FILE * f, struct conf_var *section, char *branch)
{
	int count = 0;
	struct conf_var *entry;
	char buf[2048];

	/* define a queue for sections */
	struct conf_var *queue[1024];
	int head = 0;
	int tail = 0;

	if (section == NULL)
		return 0;

	entry = section;
	while (entry->name != NULL) {

		if (entry->type == CONF_TYPE(CONF_SECTION)) {
			queue[tail++] = (struct conf_var *) entry;
		} else {
			if (entry->type->t_get(entry, buf)) {
				fprintf(f, "%s = %s\n", entry->name, buf);
			} else {
				/* Assuming the above failure means configuration stub. */
				fprintf(f, "%s = %s\n", entry->name, STUB_STRING);
			}
			count++;
		}
		entry++;
	}

	for (head = 0; head < tail; head++) {
		entry = queue[head];
		if (head != 0)
			fprintf(f, "\n");
		if ((branch != NULL) && (*branch != '\0'))
			sprintf(buf, "%s/%s", branch, entry->name);
		else
			strcpy(buf, entry->name);
		fprintf(f, "[%s]\n", buf);
		count += write_section(f, (struct conf_var *) entry->p, buf);
	}

	return count;
}

int save_conf(const char *path, struct conf_var *root)
{
	FILE *f;
	int count;

	if (!(f = fopen(path, "w+"))) {
		return -1;
	};

	count = write_section(f, root, NULL);

	fclose(f);

	return count;
}

int dump_conf(struct conf_var *root)
{
	return write_section(stdout, root, NULL);
}

int void_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "%p", var->p);
	return 1;
}

int void_set(struct conf_var *var, const char *s)
{
	sscanf(s, "%p", &(var->p));
	return 1;
}

int int_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "%i", *(int *) (var->p));
	return 1;
}

int int_set(struct conf_var *var, const char *s)
{
	int *p;

	if (var->p == NULL) {
		var->p = malloc(sizeof(int));
	}

	p = (int *) var->p;

	*p = strtol(s, NULL, 0);

	return 1;
}

int uint_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "%u", *(unsigned int *) (var->p));
	return 1;
}

int uint_set(struct conf_var *var, const char *s)
{
	unsigned int *p;

	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	p = (unsigned int *) var->p;

	*p = strtoul(s, NULL, 0);

	return 1;
}

int float_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "%f", *(double *) (var->p));
	return 1;
}

int float_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(double));
	}
	sscanf(s, "%lf", (double *) (var->p));
	return 1;
}

int string_get(struct conf_var *var, char *s)
{
	char * cp;			/* source */

	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	cp = (char *)(var->p);
	sprintf(s, "\"%s\"", cp);

	return 1;
}

int string_set(struct conf_var *var, const char *s)
{
	char *cp;			/* source */
	int len;
	char quote;
	char *ep;

	if ((!s) || (!var))
		return 0;

	cp = (char *) s;
	LTRIM(cp);
	if ((*cp == '"') || (*cp == '\'')) {
		quote = *cp;
		cp++;
		if ((ep = strchr(cp, quote)) != NULL)
			len = ep - cp;
		else {
#ifdef DEBUG
			fprintf(stderr, "Unterminated string.\n");
#endif
			len = 0;
		}

	} else {
		len = strlen(cp);
	}

	if (var->p == NULL) {
		var->p = malloc(len + 1);
	}

	strncpy((char *) (var->p), cp, len);
	cp = (char *)(var->p);
	cp[len] = '\0';

	return 1;
}

int bool_get(struct conf_var *var, char *s)
{
	if ((*(bool *) (var->p)) == false)
		sprintf(s, "false");
	else
		sprintf(s, "true");
	return 1;
}

int bool_set(struct conf_var *var, const char *s)
{
	char *cp;			/* source */

	if (var->p == NULL) {
		var->p = malloc(sizeof(int));
	}

	cp = (char *) s;
	LTRIM(cp);

	if (!strncasecmp(cp, "TRUE", 4) ||
		!strncasecmp(cp, "YES", 3) ||
		!strncasecmp(cp, "ON", 2) || !strncasecmp(cp, "1", 1)) {

		*((bool *) var->p) = true;
		return 1;
	}

	if (!strncasecmp(cp, "FALSE", 5) ||
		!strncasecmp(cp, "NO", 2) ||
		!strncasecmp(cp, "OFF", 3) || !strncasecmp(cp, "0", 1)) {

		*((bool *) var->p) = false;
		return 1;
	}

	return 0;
}

int char_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "%c", *(char *) (var->p));
	return 1;
}

int char_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(char));
	}
	sscanf(s, "%c", (char *) (var->p));
	return 1;
}

int hex8_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "0x%02X", *(unsigned char *) (var->p));
	return 1;
}

int hex8_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}
	sscanf(s, "%X", (unsigned int *) (var->p));
	return 1;
}

int hex16_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "0x%04X", *(unsigned int *) (var->p));
	return 1;
}

int hex16_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}
	sscanf(s, "%X", (unsigned int *) (var->p));
	return 1;
}

int hex32_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "0x%08X", *(unsigned int *) (var->p));
	return 1;
}

int hex32_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	sscanf(s, "%X", (unsigned int *) (var->p));
	return 1;
}

int hex64_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}

	sprintf(s, "0x%016llX", *(unsigned long long int *) (var->p));

	return 1;
}

int hex64_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned long long int));
	}

	sscanf(s, "%llX", (unsigned long long int *) (var->p));

	return 1;
}

int bin8_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int bin8_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned char));
	}

	return 0;
}

int bin16_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int bin16_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(short int));
	}

	return 0;
}

int bin32_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int bin32_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned long));
	}
	return 0;
}

int bin64_get(struct conf_var *var, char *s)
{
	s = "";

	return 0;
}

int bin64_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(long long int));
	}

	return 0;
}

int oct8_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int oct8_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned char));
	}

	return 0;
}

int oct16_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int oct16_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(short int));
	}

	return 0;
}

int oct32_get(struct conf_var *var, char *s)
{
	s = "";

	return 0;
}

int oct32_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	return 0;
}

int oct64_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int oct64_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(long long int));
	}

	return 0;
}

int ipaddr_get(struct conf_var *var, char *s)
{
	s = "";

	return 0;
}

int ipaddr_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	return 0;
}

int ipport_get(struct conf_var *var, char *s)
{
	s = "";

	return 0;
}

int ipport_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	return 0;
}

int rgb_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int rgb_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	return 0;
}

int rgbi_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int rgbi_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	return 0;
}

int cymk_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int cymk_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(sizeof(unsigned int));
	}

	return 0;
}

int ratio_get(struct conf_var *var, char *s)
{
	if (var->p == NULL) {
		sprintf(s, "NULL");
		return 0;
	}
	s = "";

	return 0;
}

int ratio_set(struct conf_var *var, const char *s)
{
	if (var->p == NULL) {
		var->p = malloc(2 * sizeof(int));
	}

	return 0;
}
