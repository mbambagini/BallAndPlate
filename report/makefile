PAPER_NAME := paper
SRCS := $(wildcard *.tex)

all: $(PAPER_NAME).pdf

%.ps: %.tex $(SRCS)
	latex $< && \
	bibtex $* && \
	latex $< && \
	latex $< && \
	dvips -o $*.ps $*.dvi

%.pdf: %.ps
	ps2pdf -sPAPERSIZE=a4 $<

clean:
	rm -f *.aux *.log *.blg *.pdf *.dvi *.ps *.bbl 
