# Breakout

## Overview / 概要

いわゆるブロック崩しゲームです。   
Simple breakout game on M5Stack.

## Require / 必要なライブラリ
[M5Stack](https://github.com/m5stack/M5Stack)  
[LovyanGFX](https://github.com/lovyan03/LovyanGFX)  
[SdFat](https://github.com/greiman/SdFat)   
[goblib](https://github.com/GOB52/goblib)  
[goblib_m5s](https://github.com/GOB52/goblib_m5s)  

## How to play / 遊び方

|ボタン|入力| 挙動|
|---|---|---|
|Faces 十字左<br>本体Aボタン|押下|パドルを左へ移動|
|Faces 十字右<br>本体Cボタン|押下|パドルを右へ移動|
|Faces B ボタン<br>本体 B ボタン| 押下|パドルの移動を加速(押している間)<br>ゲーム開始時の玉の始動|

|Button|Input| Action|
|---|---|---|
|Faces Left<br> M5Stack A|pressed|The paddle move to left|
|Faces Righ<br>M5Stack C|pressed|The paddle move to right|
|Faces B <br>M5Stack B| pressed|Movement speed increase while pressing<br>Launch the ball|
