# wstatgen

Nástroj na vytváranie štatistík pre generátor využívajúci Markov model, ktorý je implementovaný v nástroji [Wrathion](https://bitbucket.org/gazdikpeter/ibp-wrathion). Vznikol ako súčasť [bakalárskej práce](https://goo.gl/mDPK2M) o využití heuristík pri prelamovaní hesiel pomocou GPU.

## Licencia

Použitie nástroja upravuje MIT licencia, viď LICENSE.txt

## Preklad

Nástroj je možné preložiť s využítím CMake (verzia 2.8 a vyššia).
Pod operačným systémom Windows je potrebný preklad pod MinGW MSYS kvôli spracovaniu parametrov s použitím funkcií z knižnice `getopt.h`

### Postup prekladu:

#### Linux

```
cmake -G"Unix Makefiles"
make
```

#### Windows:

```
cmake -G"MSYS Makefiles"
make
```

Spustiteľný súbor sa nachádza po preklade v zložke `bin/`

## Použitie

### Parametre

```
	-h, --help	          zobrazenie nápovedy
	-f, --file		        slovník, z ktorého sú vytvorené štatistiky
	-o, --output          výstupný súbor pre uloženie štatistík
	-e, --encoding        použitá znaková sada
	-d, --description     popis
	--markov-classic      vytvorenie štatistík pre Markovský model 1. rádu
	--layered-markov	    vytvorenie štatistík pre vrstvový Markovský model
```

#### Príklad použitia

```
./wstatgen -f dictionaries/rockyou.dic -o stats/rockyou.wstat -e us-ascii
           -d "Štatistiky s využitím slovníka Rockyou"
           --markov-classic --layered-markov
```
