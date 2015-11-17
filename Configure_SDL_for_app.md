# Конфигурирование библиотеки SDL для отдельных приложений

# Конфигурирование библиотеки SDL для отдельных приложений #

## Конфигурационный файл ##

Конфигурационный файл ложиться рядом с исполняемым файлом и имеет название: название\_исполнямого\_файла\_SDL.cfg. Например если исполняемый файл назвается quake, то конфигурационный файл должен называться quake\_SDL.cfg.

## Пример конфигурационного файла ##

```
[SYSTEM]
Rotation = 1
ShowLogo = 1
ScreenBPP = 0
ScreenWidth = 320
ScreenHeight = 240

[AUDIO]
AutoCloseAudio = 1
Volume = 4
Synchronization = 1

[MOUSE]
UseCursor = 1
ControlCursor = 1
CursorStep = 20

[SDL]
C=EXTRA
Camera=F2
Numeral=F4
Asterisk=F5
VolumeUp=PAGEUP
VolumeDown=PAGEDOWN

[SDLextra]
Camera=F3
Numeral=F6
Asterisk=F7
```

## Описание параметров ##

[Rotation](Rotation.md) - указывает направление поворота экрана, если оно потребуется

[ShowLogo](ShowLogo.md) - указывает необходимость вывода логотипа SDL при запуске приложения

[AutoCloseAudio](AutoCloseAudio.md) - Автоматическая остановка звука при сворачивании приложения

[Volume](Volume.md) - громкость звука в SDL приложении

[UseCursor](UseCursor.md) - указывает требуется ли использовать псевдосистемный указатель

[ControlCursor](ControlCursor.md) - управлять курсором джойстиком (в противном случае приложение само должно управлять курсором)

[CursorStep](CursorStep.md) - чувствительность курсора

В секции SDL можно переназначить кнопки.
В секции SDLextra описываются значения кнопок после нажатии клавиши "EXTRA".