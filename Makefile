.DEFAULT_GOAL = all

NAME = ircserv
# **************************************************************************** #
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -MMD -MP -I incs/
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
MKDIR = mkdir -p
RM += -rf
# **************************************************************************** #
INC_DIR = incs

SRC_DIR = srcs
OBJ_DIR = objs
CMD_DIR = command

serv_srcs = main.cpp\
			Channel.cpp\
			Client.cpp\
			Command.cpp\
			Server.cpp\

cmd_srcs = pass.cpp\
			nick.cpp\
			unknownCommand.cpp\
			user.cpp\
			errorMsg.cpp\
			join.cpp\
			privmsg.cpp\

# INCS = $(addprefix $(INC_DIR)/,$(incs))
SRCS = $(addprefix $(SRC_DIR)/,$(serv_srcs)) $(addprefix $(SRC_DIR)/$(CMD_DIR)/,$(cmd_srcs))
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS_M = $(SRCS:.cpp=.d)
-include $(DEPS_M)
# **************************************************************************** #
.PHONY : all clean fclean re
# **************************************************************************** #
all : $(NAME)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(Q) $(MKDIR) $(OBJ_DIR)
	$(Q) $(MKDIR) $(OBJ_DIR)/$(CMD_DIR)
	$(Q) $(CXX) $(CXXFLAGS) -c $< -o $@ -I$(INC_DIR)

$(NAME) : $(OBJS)
	$(Q) echo "$(GRAY)---------------- start compiling ----------------$(RESET)"
	$(Q) $(CXX) $(CXXFLAGS) -I$(INC_DIR) $(OBJS) -o $(NAME)
	$(Q) echo "\t$(GREEN)make$(RESET) $(BOLD)FT_IRC$(RESET) mandatory OK"

clean :
	$(Q) $(RM) $(wildcard $(OBJ_DIR)/*.o) $(wildcard $(OBJ_DIR)/*.d)
	$(Q) $(RM) $(OBJ_DIR)
	$(Q) echo "\t$(RED)$@$(RESET) $(BOLD)FT_IRC$(RESET) object files OK"

fclean : clean
	$(Q) $(RM) $(NAME)
	$(Q) echo "\t$(RED)$@$(RESET) $(BOLD)FT_IRC$(RESET) OK"

re : fclean
	$(Q) $(MAKE) $(NAME)
# **************************************************************************** #