#!/bin/bash

# Generate the .epub version of the book
echo "Generating EPUB"
pandoc book.md                  \
  chap02/chap02.md              \
  chap03/chap03.md              \
  chap04/chap04.md              \
  chap04/chap04-linux.md        \
  chap04/chap04-windows.md      \
  chap05/chap05_0.md            \
  chap05/chap05-windows.md      \
  chap05/chap05-linux.md        \
  chap05/chap05_1.md            \
  chap06/chap06.md              \
  chap07/chap07.md              \
  chap08/chap08.md              \
  chap09/chap09.md              \
--toc                           \
--epub-stylesheet book.css      \
-o VulkanApiBook.epub

# Generate the .pdf version of the book
echo "Generating PDF"
pandoc book.md                  \
  chap02/chap02.md              \
  chap03/chap03.md              \
  chap04/chap04.md              \
  chap04/chap04-linux.md        \
  chap04/chap04-windows.md      \
  chap05/chap05_0.md            \
  chap05/chap05-windows.md      \
  chap05/chap05-linux.md        \
  chap05/chap05_1.md            \
  chap06/chap06.md              \
  chap07/chap07.md              \
  chap08/chap08.md              \
  chap09/chap09.md              \
--toc                           \
--latex-engine=xelatex          \
--variable mainfont="Roboto"    \
--variable sansfont="Fira Sans" \
--variable monofont="Fira Mono" \
--variable fontsize="11pt"      \
-V geometry:margin="2.0cm"      \
--chapters                      \
--highlight-style tango         \
-o VulkanApiBook.pdf
