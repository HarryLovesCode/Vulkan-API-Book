#!/bin/bash

# Generate the .epub version of the book
pandoc book.md                  \
  chap02/chap02.md              \
  chap03/chap03.md              \
  chap04/chap04.md              \
  chap04/chap04-linux.md        \
  chap04/chap04-windows.md      \
  chap05/chap05.md              \
  chap05/chap05-windows.md      \
  chap05/chap05-linux.md        \
  chap06/chap06.md              \
  chap07/chap07.md              \
  chap08/chap08.md              \
  chap09/chap09.md              \
--toc                           \
--epub-stylesheet book.css      \
-o VulkanApiBook.epub 

# Generate the .pdf version of the book
pandoc book.md                  \
  chap02/chap02.md              \
  chap03/chap03.md              \
  chap04/chap04.md              \
  chap04/chap04-linux.md        \
  chap04/chap04-windows.md      \
  chap05/chap05.md              \
  chap05/chap05-windows.md      \
  chap05/chap05-linux.md        \
  chap06/chap06.md              \
  chap07/chap07.md              \
  chap08/chap08.md              \
  chap09/chap09.md              \
--toc                           \
--latex-engine=xelatex          \
--variable mainfont="Ubuntu"    \
--variable fontsize="11pt"      \
-V geometry:margin="1.5cm"      \
-o VulkanApiBook.pdf