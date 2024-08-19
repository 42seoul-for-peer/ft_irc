CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -MP -std=c++98

ifdef DEBUG
	CXXFLAGS += -g -fsanitize=address
endif

CLNT = eclnt
SRV = esrv

SRCS_CLNT = $(wildcard clnt/*.cpp)
SRCS_SRV = $(wildcard srv/*.cpp)
OBJS_CLNT = $(SRCS_CLNT:.cpp=.o)
OBJS_SRV = $(SRCS_SRV:.cpp=.o)
DEPS_CLNT = $(SRCS_CLNT:.cpp=.d)
DEPS_SRV = $(SRCS_SRV:.cpp=.d)

all : $(CLNT) $(SRV)

$(CLNT) : $(OBJS_CLNT)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SRV) : $(OBJS_SRV)
	$(CXX) $(CXXFLAGS) -o $@ $^

-include $(DEPS_CLNT) $(DEPS_SRV)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -f $(OBJS_CLNT) $(OBJS_SRV) $(DEPS_CLNT) $(DEPS_SRV)

fclean :
	make clean
	rm -f $(CLNT) $(SRV)

re :
	make fclean
	make all

.PHONY : all clean fclean re