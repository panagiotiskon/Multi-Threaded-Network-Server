NAME_SERVER        := server
NAME_CLIENT			:= client 
#------------------------------------------------#
#   INGREDIENTS                                  #
#------------------------------------------------#
# SRCS      source files
# OBJS      object files
#
# CC        compiler
# CFLAGS    compiler flags

SRCS_SERVER        := server2.cpp 
OBJS_SERVER        := server2.o 

SRCS_CLIENT        := client.cpp
OBJS_CLIENT        := client.o 

CC          := g++
CFLAGS      := -Wall -Wextra -Werror

#------------------------------------------------#
#   UTENSILS                                     #
#------------------------------------------------#
# RM        force remove
# MAKEFLAGS make flags

RM          := rm -f
MAKEFLAGS   += --no-print-directory

#------------------------------------------------#
#   RECIPES                                      #
#------------------------------------------------#
# all       default goal
# $(NAME)   linking .o -> binary
# clean     remove .o
# fclean    remove .o + binary
# re        remake default goal

all: $(NAME_SERVER) $(NAME_CLIENT)

$(NAME_SERVER): $(OBJS_SERVER)
	$(CC) $(OBJS_SERVER) -o $(NAME_SERVER)	-lpthread

$(NAME_CLIENT): $(OBJS_CLIENT)
	$(CC) $(OBJS_CLIENT) -o $(NAME_CLIENT)	-lpthread

clean:
	$(RM) $(OBJS_SERVER) $(OBJS_CLIENT)

fclean: clean
	$(RM) $(NAME_SERVER) $(NAME_CLIENT)

re:
	$(MAKE) fclean
	$(MAKE) all

#------------------------------------------------#
#   SPEC                                         #
#------------------------------------------------#

.PHONY: clean fclean re

####################################### END_1 ####