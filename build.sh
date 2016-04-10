#!/bin/bash

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
-o VulkanApiBook.epub --epub-stylesheet book.css --toc

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
-o VulkanApiBook.pdf --toc --latex-engine=xelatex
