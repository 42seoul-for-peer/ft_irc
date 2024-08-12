NAME = game

SERVER = server

all : $(NAME) $(SERVER)

$(NAME):
	gcc Block.cpp GameLogic.cpp Tetris.cpp myio.cpp -o $(NAME) -lstdc++ -I ./

$(SERVER):
	gcc tetrissrv.cpp -o $(SERVER)

clean :
	rm -rf $(NAME) $(SERVER)

re: clean
	make all
