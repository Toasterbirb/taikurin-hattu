CXXFLAGS=-O2 -flto
BIN=asset_cleaner

all:
	g++ ${CXXFLAGS} -I ./include `Magick++-config --cppflags --cxxflags --ldflags --libs` ./src/main.cpp -o ${BIN}

clean:
	rm ./${BIN}
