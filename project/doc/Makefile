all:
	mkdir -p build
	pdflatex -output-directory build main.tex
	pdflatex -output-directory build main.tex
	pdflatex -output-directory build main.tex
	mkdir -p dist
	mv build/main.pdf dist

clean:
	rm -rf build

