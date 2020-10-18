# mhw-save-editor (Linux version)
This is a simple _Monster Hunter: World_ save file editor.

## Status
Please note this editor is discontinued since _Monster Hunter World: Iceborne_.

## Summary
Mostly inspired by [AsteriskAmpersand](https://github.com/AsteriskAmpersand/MHW-Save-Editor) and all the folks he's been inspired from and working with him/her, this editor is mostly a research prototype project.

## How to build it
This needs `openssl` installed to be able building it.
On most distributions one just requires to
```sudo apt install libssl-dev```
and it would do the trick. Once done, then just invoke `make` or `make release` and the executable should be compiled.

## How to use it
```
Usage: ./mhw-save-editor [options] ... [savefile]
Executes mhw-save-editor 0.1.0

-b, --list-basic       lists the basic save information for all the slots or
                       the specified slot
-l, --list-all         lists all known information for a given slot (if
                       no slot specified, lists for all slots)
    --items-list (csv) specify a custom csv file with fir row header, containing
                       at least columns "id" (numerical identifier) and
                       "name" (name) of items. by default tries to load the
                       file named 'items.csv'
    --list-items       lists only the items (using custom csv - see '--items-list'
                       option)
    --list-inv         lists only investigations
    --list-decos       lists all the decorations (both in builds and in inventory)
-a, --add (type:item)  adds a single item of the type specified in appropriate container.
                       Note that now onlt decorations (type 'd') and materials (type 'm')
                       are currently supported, and the item specified will be added to
                       specific inventory slot; item needs to be specified using the numeric
                       identifier - example '-a d:741' will a Vitality decoration (see file
                       items.csv for a numeric list of items). If slot is not specified, the
                       given item will be added in slot 0
    --overwrite        overwrites the input file
-d, --dump (file)      dumps the decrypted information in the specified file
    --mask-dump (file) dumps the decrypted information in the specified file, by masking the
                       know layout with its own description. Usefuly for understanding
                       the file format
-s, --slot (n)         specify which slot to select (0, 1 or 2)
    --help             prints this help and exit

```

## _Monster Hunter: World_ save file description
The savefile are usually located under:
```~/.steam/userdata/*/582010/remote```
and is named:
```SAVEDATA1000```

### Binary data
The binary data inside the file shoould be encrypted/decrypted with Blowfish ecb algorithm and using the following key:
```
xieZjoe#P2134-3zmaghgpqoe0z8$3azeq
```

It's worth noting that there is a further _byte-swapping_ inside the file, pre and post encryption, such as the first byte of a quartet needs to be swapped with the last, the second with the third.
This operation is performed before and after the encryption/decription stage.

When opening a save file, one should do
```
swapbytes
decrypt
swapbytes
```

## Data layout
Please see the file [layout_bin.h](https://github.com/Emanem/mhw-save-editor/blob/master/src/layout_bin.h) containing all the known binary layout.

## Why this editor?
I play on Linux (through SteamPlay) and altough the game runs very well, I have two issues:
* I already have ~1000 hours on PS4 and I don't want to grind again
* On Linux movies can't be played, and given that the game engine _needs_ to be able to play the final movie to let you mark a 'playthrough' as complete, then this can't happen
With this editor one should be able to amend the basics and skip the grind.

Also I like to tinker with computer science related topic, and understanding non-documented format is usually a welcome challange.
