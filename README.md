# narcissist
> easy peasy vanity addresses

## Disclaimer
This software is provided without warranty or guarantee. This software eats babies. It may set fire to your computer, blow up your house or generate a weak private key. Use this software at your own peril.

## Dependencies
|Name|Debian Package|
|-|-|
|[Boost](http://www.boost.org)|`libboost-all-dev`|
|[CMake](https://cmake.org)|`cmake`|
|[Crypto++](https://www.cryptopp.com/)|`libcrypto++-dev`|
|[gmp](https://gmplib.org)|`libgmp-dev`|
|[secp256k1](https://github.com/bitcoin-core/secp256k1)|`libsecp256k1-dev`|

## Build Instructions
```bash
# grab the dependencies if you haven't already
sudo apt install git \
                 build-essential \
                 libboost-all-dev \
                 cmake \
                 libcrypto++-dev \
                 libgmp-dev \
                 libsecp256k1-dev

# download the source and build
git clone https://github.com/matthewincognito/narcissist.git
mkdir narcissist/build
cd $_
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# optionally, put it somewhere on your $PATH
sudo install -m 0755 narcissist-cli/narcissist-cli /usr/local/bin/narcissist-cli
```

## Example Scenarios
#### Generating a bech32 address with a prefix
```
narcissist-cli -B -p bc1pppp
Private: 0A094617E4D9E12A0EA354E7EB81FAB5A61AA0019E5B0288D19254CFD2D9A9D4
Address: bc1ppppumvcq7m282lx94ffjnrz53qszqjtcfze82z
    WIF: L68rHUEpdUU1HgUVVEpShqGS7QhJqe8ap8G4D7vFxHEeo7WYYffZ
```

#### Generating a P2PKH address with a prefix
```
$ narcissist-cli -B -p 123 -T p2pkh
Private: 2B48614E3CF6BFD713E84573B3420BC10B2A2B0CE9C17CEF003D1D1ADDD9DF52
Address: 123QHaY2YaVpfBZBsw45qxfUXgwPs22fiE
    WIF: Kxfr56YJ5qa3JJeVuMD3aumkjdSdTbfAiTksHgwqsfWFZzbTA9ot
```

#### Getting an untrusted party to generate a P2PKH address for you
(on your machine)

```
$ narcissist-cli -G
 Public: 02E0DC3ECC36388357205A3BD5D78C3DFCFEEA606D450C0060B91CE922583B998A
Private: 1843FA4866D6C65A8BCB2635F33146E9A99130F5C87034EBCD8FBB4E21EEF67F
```

(on the untrusted party's machine, after you've sent them the *public* key you just generated)

```
$ narcissist-cli -B -T p2pkh \
    -P 02E0DC3ECC36388357205A3BD5D78C3DFCFEEA606D450C0060B91CE922583B998A \
    -p 123
  Tweak: F2524A0AA273801270BAC05384A938F5C6DF7027970E146EF240FD7D7A5EA447
Address: 123YBfoa8EyWQyA68pDTWL2q3uw8WGiQny
```

(back on your machine, after the untrusted party sent you the tweak key)

```
$ narcissist-cli -C \
    -T p2pkh \
    -i 1843FA4866D6C65A8BCB2635F33146E9A99130F5C87034EBCD8FBB4E21EEF67F \
    -P 9BFA74AB3E16522FC4D69F57F661FB89A0AAE302F135D2F0277971D34A4C8D3F
 Public: 0317080825954A4EC067098AF24262ED12245BF6D9B6A9B6919EDA5B37FDBC4E94
Private: 8EF09A007974CE77699A0ED5CEFCECEFDD5A572966E324C809D06BAB26667201
Address: 123w8fcpdpxKMnWyVYbyV7BEQQNCAom9eP
    WIF: L21ZrZ1h7M4iwrfqQFM74zAEv1RnuYXjdEYUxdsnXN5U7ZFmTD98
```

#### Getting an untrusted party to generate a bech32 address for you
Same as above, just don't pass `-T p2pkh`.

#### Importing the private key into Electrum
Create a new Electrum wallet and choose `Import Bitcoin addresses or private keys`,
paste the WIF you got out of `narcissist-cli`.

Add the address to an existing wallet by navigating to `Wallet -> Private keys -> import`
and pasting the WIF into the popup.

## License
Released under the terms of the 3-Clause BSD license. See `LICENSE.txt` for more
information.
