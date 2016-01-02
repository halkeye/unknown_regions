CC      = gcc
PROF    = 
C_FLAGS = -g3 -Wall
L_FLAGS = $(PROF)

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o \
	  boards.o build.o bounty.o ban.o \
	  clans.o comm.o comments.o const.o councils.o\
	  db.o \
	  eng-skills.o \
	  fight.o \
	  gav_info.o grub.o \
	  handler.o hashstr.o homes.o \
	  interp.o ident.o \
	  magic.o makeobjs.o misc.o mud_comm.o mud_prog.o medical.o morespace.o\
	  player.o piloting-skills.o \
	  quest.o \
	  requests.o reset.o restore.o \
	  smalloc.o save.o shops.o skills.o special.o space.o swskills.o slay.o \
	  tables.o track.o \
	  update.o uk-additions.o \
	  vendor.o \
	  wedding.o copyover.o

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c \
	  boards.c build.c bounty.c ban.c \
	  clans.c comm.c comments.c const.c councils.c\
	  db.c \
	  eng-skills.c \
	  fight.c \
	  gav_info.c grub.c \
	  handler.c hashstr.c homes.c \
	  interp.c ident.c \
	  magic.c makeobjs.c misc.c mud_comm.c mud_prog.c medical.c morespace.c \
	  player.c piloting-skills.c \
	  quest.c \
	  requests.c reset.c restore.c \
	  smalloc.c save.c shops.c skills.c special.c space.c swskills.c slay.c \
	  tables.c track.c \
	  update.c uk-additions.c \
	  vendor.c \
	  wedding.c copyover.c


H_FILES = mud.h bet.h slay.h homes.h defines.h restore.h copyover.h

all:
#	co $(H_FILES)
	make unknown
#	rm -f $(H_FILES)

unknown: $(O_FILES)
	rm -f unknown
	$(CC) $(L_FLAGS) -o unknown $(O_FILES) -lm -lcrypt
	chmod g+w unknown
	chmod g+w $(O_FILES)

.c.o: mud.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f $(O_FILES)

