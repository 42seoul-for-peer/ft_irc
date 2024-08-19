.DEFAULT_GOAL = all

NAME = ircserv
# **************************************************************************** #
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -MMD -MP
# **************************************************************************** #
ifdef DEBUG
	CXXFLAGS += -O0 -g3 -fsanitize=address
endif
# **************************************************************************** #
RESET = \033[0m

BOLD = \033[1m

RED = \033[31m
GREEN = \033[32m
GRAY = \033[90m

Q = @
# **************************************************************************** #
SRCS = main.cpp\

OBJS = $(SRCS:.cpp=.o)
-include $(DEPS_M) $(DEPS_B)
# **************************************************************************** #
.PHONY : all clean fclean re
# **************************************************************************** #
all : $(NAME)

%.o : %.c
	$(Q) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(Q) echo "$(GRAY)---------------- start compiling ----------------$(RESET)"
	$(Q) $(MAKE) -C $(LIB_DIR)
	$(Q) $(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)
	$(Q) echo "\t$(GREEN)make$(RESET) $(BOLD)FT_IRC$(RESET) mandatory OK"

clean :
	$(Q) $(RM) $(wildcard *.o) $(wildcard *.d)
	$(Q) echo "\t$(RED)$@$(RESET) $(BOLD)FT_IRC$(RESET) object files OK"

fclean : clean
	$(Q) $(RM) $(NAME)
	$(Q) echo "\t$(RED)$@$(RESET) $(BOLD)FT_IRC$(RESET) OK"

re : fclean
	$(Q) $(MAKE) $(NAME)
# **************************************************************************** #