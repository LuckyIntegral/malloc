SO_FLAGS := -shared -fPIC
CFLAGS := -fPIC -Wall -Werror -Wextra

ifeq (,$(HOSTTYPE))
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME := build/libft_malloc_$(HOSTTYPE).so
NAME_SL := libft_malloc.so
LD_PRELOAD_CMD := LD_PRELOAD=$(shell pwd)/libft_malloc.so

# LIBS
OBJDIR := ./build
CFILES := malloc.c free.c utils.c zone.c realloc.c visualizer.c visualizer_utils.c
CFILES := $(addprefix source/, $(CFILES))
OFILES := $(CFILES:%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OFILES)
	@gcc $(SO_FLAGS) -o $(NAME) $(OFILES) # create shared library
	@rm -f $(NAME_SL) # remove old symlink
	@ln -sf $(NAME) $(NAME_SL) # create new symlink
	@echo "Compiled!"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	@gcc $(CFLAGS) -c $< -o $@

clean: 
	@rm -rf $(OBJDIR)
	@echo "Cleaned!"

fclean: clean
	@rm -f $(NAME)
	@rm -f $(NAME_SL)
	@rm -f ./tester
	@echo "FCleaned!"

re: fclean all

test: all
	gcc tests/main.c -L. -lft_malloc
	LD_LIBRARY_PATH=. LD_PRELOAD="./libft_malloc.so" ./a.out
	rm -f ./a.out