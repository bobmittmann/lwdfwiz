/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	readln.c
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

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define IN_BS       '\x8'
#define IN_TN_BS     0x7F /* TELNET back space */
#define IN_EOL      '\r'
#define IN_CR       '\r'
#define IN_LF       '\n'
#define IN_SKIP     '\3'
#define IN_EOF      '\x1A'
#define IN_ESC      '\033'

#define MK_IN_KEY(CODE)   (0x2000 + (CODE))
#define IN_CTRL        0x4000

#define IN_CURSOR_UP    MK_IN_KEY(0)
#define IN_CURSOR_DOWN  MK_IN_KEY(1)
#define IN_CURSOR_RIGHT MK_IN_KEY(2)
#define IN_CURSOR_LEFT  MK_IN_KEY(3)
#define IN_PAGE_UP      MK_IN_KEY(5)
#define IN_PAGE_DOWN    MK_IN_KEY(6)
#define IN_INSERT       MK_IN_KEY(7)
#define IN_DELETE       MK_IN_KEY(8)
#define IN_HOME         MK_IN_KEY(9)
#define IN_END          MK_IN_KEY(10)

#define IN_CTRL_CURSOR_UP    IN_CURSOR_UP + IN_CTRL 
#define IN_CTRL_CURSOR_DOWN  IN_CURSOR_DOWN + IN_CTRL   
#define IN_CTRL_CURSOR_RIGHT IN_CURSOR_RIGHT + IN_CTRL    
#define IN_CTRL_CURSOR_LEFT  IN_CURSOR_LEFT + IN_CTRL   
#define IN_CTRL_PAGE_UP      IN_PAGE_UP + IN_CTRL   
#define IN_CTRL_PAGE_DOWN    IN_PAGE_DOWN + IN_CTRL   

#define OUT_CURSOR_LEFT     "\x8"
#define OUT_BS              "\x8 \x8"
#define OUT_SKIP            "^C"
#define OUT_BEL             "\7"
#define OUT_EOL             "\r\n"

#define MODE_ESC 1
#define MODE_ESC_VAL1 2
#define MODE_ESC_VAL2 3
#define MODE_ESC_O 4

struct termios orig_term;

void term_restore(void)
{ 
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
}

void term_raw_mode(void) 
{ 
	struct termios raw;

	tcgetattr(STDIN_FILENO, &orig_term);
	atexit(term_restore);

	raw = orig_term;
	raw.c_lflag &= ~(ECHO | ICANON);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int readln(char * buf, unsigned int max)
{
	char in[4];
	unsigned int pos;
	unsigned int len;
	unsigned int i;
	int mode;
	int val;
	int ctrl;
	int c;
	char * s;
	int fdo = STDOUT_FILENO;

	if (max < 2) {
		fprintf(stderr, "#Error freadln: max=%d!\n", max);
		return -1;
	}

	if (!isatty(STDIN_FILENO)) {
		fprintf(stderr, "#Error freadln: not a tty.\n");
		return -1;
	}

	term_raw_mode(); 
	mode = 0;
	val = 0;
	ctrl = 0;

	/* Reserve space for NULL termination */
	max--;

	s = buf;
	if ((len = strlen(s)) > 0) {
		if (len > max) 
			len = max;
		s[len] = '\0';
		write(fdo, s, len);
	}
	pos = len;

	for (;;) {
//		if ((c = fgetc(f)) == EOF) {
		if (read(STDIN_FILENO, in, 1) <= 0) {
			buf[len] = '\0';
			term_restore();
			return 0;
		}
		c = in[0];	
//		fprintf(stderr, " c=%02x\n", c);

		switch (mode) {
		case MODE_ESC:
			switch (c) {
			case '[':
				mode = MODE_ESC_VAL1;
				val = 0;
				ctrl = 0;
				break;
			case 'O':
				mode = MODE_ESC_O;
				break;
			default:
				mode = 0;
			};
			continue;

		case MODE_ESC_VAL1:
		case MODE_ESC_VAL2:
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				val = val * 10 + c - '0';
				continue;
			case 'A':
				/* cursor up */
				c = IN_CURSOR_UP + ctrl;
				break;
			case 'B':
				/* cursor down */
				c = IN_CURSOR_DOWN + ctrl;
				break;
			case 'C':
				/* cursor right */
				c = IN_CURSOR_RIGHT + ctrl;
				break;
			case 'D':
				/* cursor left */
				c = IN_CURSOR_LEFT + ctrl;
				break;
			case '~':
				switch (val) {
				case 1:
					c = IN_HOME + ctrl;
					break;
				case 2:
					c = IN_INSERT + ctrl;
					break;
				case 3:
					/* delete */
					c = IN_DELETE + ctrl;
					break;
				case 4:
					/* end */
					c = IN_END + ctrl;
					break;
				case 5:
					c = IN_PAGE_UP + ctrl;
					break;
				case 6:
					c = IN_PAGE_DOWN + ctrl;
					break;
				default:
					mode = 0;
					continue;
				}
				break;
			case ';':
				mode = MODE_ESC_VAL2;
				ctrl = IN_CTRL;
				val = 0;
				continue;
			default:
				mode = 0;
				continue;
			};
			mode = 0;
			break;

		case MODE_ESC_O:
			switch (c) {
			case 'F':
				/* end */
				c = IN_END;
				break;
			case 'H':
				/* home */
				c = IN_HOME;
				break;
			default:
				mode = 0;
				continue;
			}
			mode = 0;
			break;
		} 

		switch (c) {
		case IN_ESC:
			mode = MODE_ESC;
			continue;

		case IN_CURSOR_RIGHT:
			if (pos < len)
				write(fdo, &buf[pos++], 1);
			else
				write(fdo, OUT_BEL, sizeof(OUT_BEL) - 1);
			continue;

		case IN_CURSOR_LEFT:
			if (pos > 0) {
				write(fdo, OUT_CURSOR_LEFT, sizeof(OUT_CURSOR_LEFT) - 1);
				pos--;
			} else {
				write(fdo, OUT_BEL, sizeof(OUT_BEL) - 1);
			}
			continue;

		case IN_CTRL_CURSOR_RIGHT:
			while (pos < len) {
				write(fdo, &buf[pos++], 1);
				if ((buf[pos - 1] != ' ') && (buf[pos] == ' '))
					break;
			}
			continue;

		case IN_CTRL_CURSOR_LEFT:
			if (pos > 0) {
				do {
					write(fdo, OUT_CURSOR_LEFT, sizeof(OUT_CURSOR_LEFT) - 1);
					pos--;
					if ((buf[pos - 1] == ' ') && (buf[pos] != ' '))
						break;
				} while (pos > 0);
			}
			continue;

		case IN_PAGE_UP:
			continue;

		case IN_PAGE_DOWN:
			continue;

		case IN_INSERT:
			continue;

		case IN_CURSOR_UP:
		case IN_HOME:
			while (pos > 0) {
				write(fdo, OUT_CURSOR_LEFT, sizeof(OUT_CURSOR_LEFT) - 1);
				pos--;
			} 
			continue;

		case IN_CURSOR_DOWN:
		case IN_END:
			write(fdo, &buf[pos], len - pos);
			len = pos;
			continue;

		case IN_TN_BS:     
		case IN_BS:
			if (pos == 0) {
				write(fdo, OUT_BEL, sizeof(OUT_BEL) - 1);
				continue;
			}
			if (len == pos) {
				pos--;
				len--;
				write(fdo, OUT_BS, sizeof(OUT_BS) - 1);
				continue;
			}

			write(fdo, OUT_CURSOR_LEFT, sizeof(OUT_CURSOR_LEFT) - 1);
			pos--;

			/* FALLTHROUGH */

		case IN_DELETE:

			if (len == pos) {
				continue;
			}
			len--;

			for (i = pos; i < len; i++) {
				buf[i] = buf[i + 1];
			}
			buf[len] = ' ';
			write(fdo, &buf[pos], len - pos + 1);
			for (i = len + 1; i > pos; i--)
				write(fdo, OUT_CURSOR_LEFT, sizeof(OUT_CURSOR_LEFT) - 1);
			continue;

		case IN_CR:
		case IN_LF:
			buf[len] = '\0';
			write(fdo, OUT_EOL, sizeof(OUT_EOL) - 1);
			term_restore();
			return len;

		case IN_SKIP:
			write(fdo, OUT_SKIP, sizeof(OUT_SKIP) - 1);
			term_restore();
			return -1;
		}

		if (len == max) {
			write(fdo, OUT_BEL, sizeof(OUT_BEL) - 1);
			continue;
		}

		if (len == pos) {
			buf[pos] = c;
			write(fdo, &buf[pos], 1);
			pos++;
			len++;
			continue;
		}

		for (i = len; i > pos; i--) {
			buf[i] = buf[i - 1];
		}

		len++;
		buf[pos] = c;
		buf[len] = '\0';

		write(fdo, &buf[pos], len - pos);
		pos++;

		for (i = len; i > pos; i--)
			write(fdo, OUT_CURSOR_LEFT, sizeof(OUT_CURSOR_LEFT) - 1);

	}
}

