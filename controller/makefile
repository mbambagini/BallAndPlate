
###########################
# Executable Names

PRG_LIST :=	trainer \
		ball_and_plate

##########################
# Directories section 

MAIN_DIR := $(shell pwd)
SRC_DIR  := src
INC_DIR  := inc
OBJ_DIR  := obj
PRG_DIR := progs

##########################
# Make Variables

CC := gcc

##########################
# Compiler Option

CFLAGS += -g -I./$(INC_DIR) -Wall
LIBS :=

LIBS_UNIX += -lm -lGL -lglut -lGLU
LIBS_WIN += -lm -lopengl32 -lglut32 -lglu32 

##########################
# Rules params

VPATH	:= $(SRC_DIR) $(PRG_DIR)

OTHER_DEPS := $(shell ls $(INC_DIR)/*.h)
SRC_FILES  := $(shell ls $(SRC_DIR)/*.c;cd $(CURR_DIR);)
OBJ_FILES  := ${SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o}

PRG_FILES 	:= $(shell ls ${PRG_LIST:%=$(PRG_DIR)/%.c} ; cd $(CURR_DIR))
OBJ_PRG_FILES 	:= ${PRG_FILES:$(PRG_DIR)/%.c=$(OBJ_DIR)/%.o}

##########################
# Makefile Rules  

unix:
	@echo "Unix env."
	@make _all_ LIBS='$(LIBS) $(LIBS_UNIX)' CFLAGS='$(CFLAGS)'

cygwin:
	@echo "Cygwin env."
	make _all_ LIBS='$(LIBS) $(LIBS_WIN)' CFLAGS='$(CFLAGS)'

clean: 
	@rm -rf $(OBJ_DIR)
	@rm -f $(PRG_LIST)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@$(CC) -c $< $(CFLAGS) -o $@

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

_all_: $(OBJ_DIR) $(OBJ_FILES) $(OBJ_PRG_FILES)
	@echo "Building executables: " $(PRG_LIST) 
	
	@for i in $(PRG_LIST); do $(CC) -o $$i $(OBJ_DIR)/$$i.o $(OBJ_FILES) $(LIBS) ; done;
	@echo "Done."

################################################
#       Overriding Implict Build Rule          #
################################################

$(OBJ_DIR)/%.o : %.c $(OTHER_DEPS)
	@$(CC) -c $(CFLAGS) $< -o $@

################################################

