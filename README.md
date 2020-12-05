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

## Run Time Warning

* This is the printout message (with real time) from one run. It takes about 3-8 minues to finish one run... Still optimizing. All three algo can complete 12million.addrtrace under 10 min, but bigmix cost much longer...

|Tracefile name|12million.addrtrace|
|---|---|
|AMU|0.956025|
|ARP|0.001124|
|TMR|12000000|
|TPI|5337|
|Running Time (ticks)| 10674561551|
|real|4m26.429s|
|user|4m19.574s|
|sys|0m6.808s|


