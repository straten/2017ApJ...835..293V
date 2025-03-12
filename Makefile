LOCAL_ROOT := ..

DOCS := paper.tex companion.tex

all: paper.pdf companion.pdf

paper.dvi: commutative.eps modes.eps regimes.eps

commutative.eps: commutative_ext.tex
	latex --jobname=commutative commutative_ext.tex
	dvips -o commutative.eps commutative

modes.eps: modes
	./modes modes.eps/ps

regimes.eps: regimes
	./regimes regimes.eps/ps

modes: modes.C
	g++ -o modes modes.C `psrchive --cflags --ldflags --pgplot-libs`

regimes: regimes.C
	g++ -o regimes regimes.C `psrchive --cflags --ldflags --pgplot-libs`

include $(LOCAL_ROOT)/Makefile.tex

