BIN=asset_cleaner

all:
	g++ -I ./include `Magick++-config --cppflags --cxxflags --ldflags --libs` ./src/main.cpp -o ${BIN}

clean:
	rm ./${BIN}
