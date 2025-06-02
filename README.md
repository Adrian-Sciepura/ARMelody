# ARMelody

## Wymagania
#### debian based
```
sudo apt install gcc-aarch64-linux-gnu qemu-user-static -y
```
#### arch based
```
yay -S aarch64-linux-gnu-gcc quemu-full
```

## Kompilacja
Używamy MakeFile więc po prostu
```
make all
```
Ewentualnie jakby coś poszło nie tak
```
make clean
make all
```

## Dump Assembly
```
./dump.sh
```
otrzymujemy potem plik dump.txt (UWAGA Projekt musi być już skompilowany)

## Uruchominie
W quemu 
```
./run.sh plik.wav
```
Na urządzeniu AArch64
```
./build/app.elf plik.wav
```

## Debug
### Wymagania
#### debian based
```
sudo apt install tmux gdb-multiarch -y
```

#### arch based (Uwaga nie było testowane)
```
yay -S tmux
```
Ponoć wsparcie dla wielu architektur jest częścią gdb extra (https://gitlab.archlinux.org/archlinux/packaging/packages/gdb/-/commit/baa8b09ae93345ed66c028e6d1479e0ac1cc8311)

### Uruchomienie
```
./debug.sh plik.wav
```

## Zestawienie wydajności
Podane czasy dotyczą czasu trwania fft (w czasie korelacji fft jest wywoływanie 3 razy)
```
File successfully opened
-------- WAV FILE DATA --------
- File size: 565808
- Block size: 16
- Audio format: 1
- Number of channels: 1
- Frequency: 48000
- Bytes per second: 96000
- Bytes per block: 2
- Bits per sample: 16
- Data size: 565592
-------------------------------
Content of the file successfully read
Padded size: 524288
Time: 0.242302s
Time: 0.216088s
Time: 0.217714s
Best match found at index 0 with magnitude 562238331027456.000000
Padded size: 524288
Time: 0.041094s
Time: 0.042229s
Time: 0.041020s
Best match found at index 0 with magnitude 562238331027456.000000
```
```
File successfully opened
-------- WAV FILE DATA --------
- File size: 5272162
- Block size: 16
- Audio format: 1
- Number of channels: 1
- Frequency: 44100
- Bytes per second: 88200
- Bytes per block: 2
- Bits per sample: 16
- Data size: 5271976
-------------------------------
Content of the file successfully read
Padded size: 4194304
Time: 2.061366s
Time: 2.060272s
Time: 2.088761s
Best match found at index 0 with magnitude 5207379947814912.000000
Padded size: 4194304
Time: 0.485348s
Time: 0.482174s
Time: 0.488211s
Best match found at index 0 with magnitude 5207379947814912.000000
```
Około 6-krotny wzrost wydajności

## Źródła
https://cp-algorithms.com/algebra/fft.html

https://developer.arm.com/ NIE POLECAMY (ale nie ma nic innego)
