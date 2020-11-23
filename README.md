# CS537 P3 (537make)

## Author:

|Name|CSlogin|NetID|
|----|-------|-----|
|Hanyuan Wu|hanyuan|hwu384|
|Zhihao Shu|hshu|zshu9|

## Install

* After git clone:
* `bash> cd cs537_p3`
* `bash> make`

## Usage

* ./537make [-f \<makefile\>] [target]
* ./537make [target] [-f \<makefile\>]
* IO redirections in makefile commands are allowed

## Brief Summary

* A mini c-based implement of make, allowing specfying the name of makefile and target.

## Program Features

* Use -f \<makefile\> to specify the name of makefile other than default names. Default names are `makefile` (first priority) and `Makefile`.
* By running without specfying [target], it will make the first target build in makefile
* IO redirections are allowed in command lines within the makefile, using `<` and `>`.
* can check and report cycle (infinite dependent loop) in makefile
