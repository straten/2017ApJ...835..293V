LOCAL_ROOT := ..

DOCS := paper.tex 

all: paper.pdf 

paper.dvi: commutative.eps modes.eps regimes.eps

commutative.eps: commutative_ext.tex
	latex --jobname=commutative commutative_ext.tex
	dvips -o commutative.eps commutative

modes.eps: modes
	./modes modes.eps/ps

regimes.eps: regimes
	./regimes regimes.eps/ps

modes: modes.C
	g++ -o modes modes.C `psrchive --cflags --pgplot-libs --ldflags`

regimes: regimes.C
	g++ -o regimes regimes.C `psrchive --cflags --pgplot-libs --ldflags`

include $(LOCAL_ROOT)/Makefile.tex

publish:
	mkdir -p /tmp/gh-pages
	mv paper.pdf /tmp/gh-pages
	cp README.md /tmp/gh-pages
	git checkout gh-pages
	mv /tmp/gh-pages/README.md index.md
	mv /tmp/gh-pages/paper.pdf 2017ApJ...835..293V_extended.pdf
	git add index.md
	git add 2017ApJ...835..293V_extended.pdf
	git commit -m "published using 'make publish'" .
	git push --set-upstream origin gh-pages
	git checkout main
