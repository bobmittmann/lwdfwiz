# File:		Makefile
# Module: 
# Comment:  
# Copyright(C) 2018 Bob Mittmann. All Rights Reserved.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

PROG = glwdf

#CFILES = glwdf-app.c glwdf-freq.c glwdf-time.c ../src/lwdf-fp64.c \
	plot/plot-figure.c plot/plot-series.c plot/plot-freqresp.c plot/plot-gtk.c 
CFILES = glwdf-application_window.c \
		 glwdf-assistant.c \
		 glwdf-filter_control.c glwdf-filter_view.c \
		 glwdf-frequency_control.c\
		 glwdf-app.c glwdf-about.c resources.c 

UIFILES = ui/toolbars.ui ui/menus.ui ui/glwdf-application_window.ui \
		  ui/glwdf-filter_control.ui ui/glwdf-filter_view.ui \
		  ui/glwdf-frequency_control.ui  \
		  ui/glwdf-assistant_butterworth.ui

SVGFILES = icons/scalable/apps/glwdf-app.svg

#	glwdf-frequency_control.c glwdf-frequency_view.c 
OFILES = $(CFILES:.c=.o)

LIBS = m pthread

INCPATH	= ../include plot  .
LIBPATH =

DBGOPTS = -g 

OPTIONS	= $(DBGOPTS)
#OPTIONS	= $(DBGOPTS) -mno-cygwin

#GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

GTKLIB = `pkg-config --libs gtk4`
GTKCFLAGS = `pkg-config --cflags gtk4` 

SFLAGS	= -Wall
CFLAGS	= -Wall -O1
LDFLAGS	= $(GTKLIB) -export-dynamic

CC	= gcc
LD	= gcc
AS	= gcc
AR	= ar
OBJCOPY	= objcopy
OBJDUMP	= objdump
STRIP = strip
GLIB_COMPILE_RESOURCES = glib-compile-resources

all: Makefile $(PROG) 
#$(PROG).lst

clean:
	@rm -fv $(OFILES) $(PROG).lst $(PROG) $(PROG).exe
	@rm -fv resources.c
	@rm -fv *.png *.plt *.dat

$(PROG): Makefile $(OFILES)
	$(LD) $(OPTIONS) $(LDFLAGS) $(addprefix -L,$(LIBPATH)) -o $@ $(OFILES) $(addprefix -l,$(LIBS))

$(PROG).lst: $(PROG) Makefile
	$(OBJDUMP) -w -D -t -S -r -z $< | sed '/^[0-9,a-f]\{8\} .[ ]*d[f]\?.*$$/d' > $@

glwdf.gresource: glwdf.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies glwdf.gresource.xml)

resources.c: glwdf.gresource.xml glwdf.gresource
	$(GLIB_COMPILE_RESOURCES) $< --target=$@ --generate-source

resources.h: glwdf.gresource.xml glwdf.gresource
	$(GLIB_COMPILE_RESOURCES) $< --target=$@ --generate-header

%.o : %.c Makefile
	$(CC) $(OPTIONS) $(CFLAGS) $(addprefix -I,$(INCPATH)) $(GTKCFLAGS) -o $@ -c $<

