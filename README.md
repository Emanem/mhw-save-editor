# mhw-save-edior (Linux version)
This is a simple _Monster Hunter: World_ save file editor.

## Summary
Mostly inspired by [AsteriskAmpersand](https://github.com/AsteriskAmpersand/MHW-Save-Editor) and all the folks he's been inspired from and working with him/her, this editor is mostly a research prototype project.

## How to use it
t.b.d.

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
Section describing the file data layout. 

### Basic checksum
If all operation have been executed correctly, the first 4 bytes of the file should be `0x01, 0x00, 0x00, 0x00`.

### Steam Id
Starts at 0x28 and it's an int64_t.

### Save Slots
Each slot (max 3) has a size of 0xF6110 and the slot data starts at 0x003004DC, hence the first slot (0) should start at 0x003004DC, the second at 0x003004DC + 0xF6110 and third should start at 0x003004DC + 0xF6110*2. The content of the slots should be the following:

1. UTF-8 character name (64 bytes)
2. Rank (4 bytes uint32_t)
3. Zenny (4 bytes uint32_t)
4. Research points (4 bytes uint32_t)
5. XP (4 bytes uint32_t)
6. Play time (4 bytes uint32_t)
7. Gender (starting at 0xb0)

### Items/Ammo containers
Each container starts from a given offset for current slot. Using slot starting area as basepoint, one sums up the relative offset and then goes through the items list.
The items list is a `uint32_t`, `uint32_t` pairs, indicating the item code and the quantity.

Each container has a limited max number of items; the min items is 1 and max item is 999 (?).

|Container  |Offset  |Number |
|-----------|--------|------:|
|pouch item|0xa2c79|24|
|pouch ammo|0xa2d39|16|
|box item|0xa2ed9|200|
|box ammo|0xa3519|200|
|box materials|0xa3b59|800|
|box decos|0xa5459|200|   

## Why this editor?
I play on Linux (through SteamPlay) and altough the game runs very well, I have two issues:
* I already have ~1000 hours on PS4 and I don't want to grind again
* On Linux movies can't be played, and given that the game engine _needs_ to be able to play the final movie to let you mark a 'playthrough' as complete, then this can't happen
With this editor one should be able to amend the basics and skip the grind.

Also I like to tinker with computer science related topic, and understanding non-documented format is usually a welcome challange.
