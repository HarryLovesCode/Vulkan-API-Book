#!/bin/bash

pandoc book.md                \
  chap2/chap2.md              \
  chap3/chap3.md              \
  chap4/chap4.md              \
  chap4/chap4-linux.md        \
  chap4/chap4-windows.md      \
  chap5/chap5.md              \
  chap5/chap5-windows.md      \
  chap5/chap5-linux.md        \
  chap6/chap6.md              \
  chap7/chap7.md              \
  chap8/chap8.md              \
  chap9/chap9.md              \
-o VulkanApiBook.epub --epub-stylesheet book.css --toc

pandoc book.md                \
  chap2/chap2.md              \
  chap3/chap3.md              \
  chap4/chap4.md              \
  chap4/chap4-linux.md        \
  chap4/chap4-windows.md      \
  chap5/chap5.md              \
  chap5/chap5-windows.md      \
  chap5/chap5-linux.md        \
  chap6/chap6.md              \
  chap7/chap7.md              \
  chap8/chap8.md              \
  chap9/chap9.md              \
-o VulkanApiBook.pdf --toc --latex-engine=xelatex
