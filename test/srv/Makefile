CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98

ifdef DEBUG
	CXXFLAGS += -g -fsanitize=address
endif

NAME = ircserv

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

-include $(DEPS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -f $(OBJS) $(DEPS)

fclean :
	make clean
	rm -f $(NAME)

re :
	make fclean
	make all

.PHONY : all clean fclean re