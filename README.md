# ARMelody

## Wymagania

***Debian / Ubuntu / Raspberry***

```
sudo apt install gcc-aarch64-linux-gnu qemu-user-static tmux gdb-multiarch -y
```
***Arch Linux***

```
yay -S aarch64-linux-gnu-gcc quemu-full tmux
```

> Wsparcie dla wielu architektur jest częścią gdb extra (https://gitlab.archlinux.org/archlinux/packaging/packages/gdb/-/commit/baa8b09ae93345ed66c028e6d1479e0ac1cc8311)

## Kompilacja

Do kompilacji użyto MakeFile, a więc komenda to
```
make all
```

Przykładowy output dla kompilacji
```
mkdir -p build/other
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/other/simple_timer_c.o src/other/simple_timer.c
mkdir -p build/other
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/other/graphics_c.o src/other/graphics.c
mkdir -p build/math
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/math/correlation_c.o src/math/correlation.c
mkdir -p build/math
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/math/complex_c.o src/math/complex.c
mkdir -p build/math
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/math/consts_c.o src/math/consts.c
mkdir -p build/math
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/math/fft_c.o src/math/fft.c
mkdir -p build/files
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/files/wav_c.o src/files/wav.c
mkdir -p build/files
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/files/bmp_c.o src/files/bmp.c
mkdir -p build
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/main_c.o src/main.c
mkdir -p build/math
aarch64-linux-gnu-gcc -MMD -Iinclude -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -c -o build/math/fft_S.o src/math/fft.S
aarch64-linux-gnu-gcc -g -O0 -Wall -static -march=armv8.3-a+simd -DIS_ARM_8_3_OR_HIGHER -o build/app.elf build/other/simple_timer_c.o build/other/graphics_c.o build/math/correlation_c.o build/math/complex_c.o build/math/consts_c.o build/math/fft_c.o build/files/wav_c.o build/files/bmp_c.o build/main_c.o build/math/fft_S.o -lm
```

> [!TIP]
> Aby usunąć wszystkie skompilowane pliki .obj można użyć `make clean`

> [!WARNING]
> Jeżeli nasze urządzenie docelowe posiada architekturę starszą niż ARMv8.3-A w pliku MakeFile
> należy usunąć `-DIS_ARM_8_3_OR_HIGHER` 

## Listing

Generowanie listingu z pliku .elf

```
./dump.sh
```

Po wykonaniu skryptu otrzymujemy plik wynikowy o nazwie `dump.txt`

> [!NOTE]
> Projekt musi być już skompilowany

## Uruchominie

***QEMU***

```
./run.sh plik.wav
```

***Urządzenie ARM64***

```
./build/app.elf plik.wav
```
Minimalna wersja ARMv8-A
Zalecana co najmniej ARMv8.3-A


## Debug

Do debugowania użyto gdb, oraz tmux aby stworzyć wygodne środowisko
```
./debug.sh plik.wav
```

## Zestawienie wydajności

***Plik 0.5 mln próbek***
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

***Plik 5 mln próbek***
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

> [!NOTE]
> Podane czasy dotyczą czasu trwania fft (w czasie korelacji fft jest wywoływanie 3 razy)

## Źródła

- https://cp-algorithms.com/algebra/fft.html
- https://developer.arm.com/ NIE POLECAMY (ale nie ma nic innego)
