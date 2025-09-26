# Goals for 2.0

This document outlines ideas i have for a 2.0 revision that won't be happening any time soon, so consider this as an ideas document and nothing more.

---

## Token key

The purpose of this key is to be able to avoid retyping paths for each origin or target part that is reused

- tokens can be partial paths or full paths
- tokens must use the same @ symbol for dir splitting
- tokens must be defined before used, but can be defined at any stage between any kmf blocks

---

## Print key

The purpose of this key is to be able to print log messages to KalaMove for debugging or information

- print keys only accept a single value - all content after 'print: ' is what is actually printed to KalaMove output
- print keys accept tokens and can print the value of a token, the token must be defined before the print key
- print keys can be placed anywhere and are printed at the parse stage where the print key was placed relative to the kmf file

--- 

## Run key

The purpose of this key is to be able to run external batch/shell scripts when you need to do an action in between a stage

- run keys only accept a single value - the path to the batch/shell script
- run keys dont accept extensions so that the key can remain os-agnostic and run the same way on both windows and linux, instead the parser checks internally if the value leads to a batch or shell script and then runs that
- run keys can be placed anywhere and are ran at the parse stage where the run key was placed relative to the kmf file