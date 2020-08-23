# h261-player

Win32 player for h261 videos

to get a h261 video with ffmpeg:
```
ffmpeg.exe -i input output.h261
```
with resizing:
```
ffmpeg.exe -i input -vf "scale=cif:force_original_aspect_ratio=decrease,pad=352:288:(ow-iw)/2:(oh-ih)/2" output.h261
```

Built:
```
cl.exe /Zi /EHsc /Fe: player.exe player.c video.c tables.c user32.lib gdi32.lib
```
