bandit3:aBZ0W5EmUfAf7kHTQeOwd8bauFJ2lAiG
bandit4:2EW7BBsr6aMMoJ2HjW067dm8EgX26xNe
bandit5:lrIWWI6bB37kxfiCQZqUdOIYfr6eEeqR
bandit6:P4L4vucdmLnm8I7Vl7jG1ApGSfjYKqJU
bandit7:z7WtoNQU2XfjmMtWA8u5rN4vzqu4v99S
bandit8:TESKZC0XvTetK0S9xNwm25STk5iWrBvP
bandit9:EN632PlfYiZbn3PhVK3XOGSlNInNE00t
bandit10:G7w8LIi6J3kTb8A7j9LgrywtEUlyyp6s
11:6zPeziLdR2RKNdNYFNb6nVCKzphlXHBM
12:JVNBBFSmZwKKOP0XbFXOoW8chDz5yVRv

tr set1 set2 input 

## 12-13
password:wbWdlBxEir4CaE8LaPhauuOo6pwRmrDw

data.txt是压缩文件hexdump之后的结果，需要用xxd将data.txt还原为压缩文件

之后用file获取文件类型,解压即可

## 13-14
password:fGrHPx402xGC7U7rXKDaxiWFTOiF0ENq

there is a private sshkey. we can use it to log in user bandit14 without password.
## 14-15

password:jN2kgmIXJ6fShzhT2avhotn4Zcka6tnt

send password of 14 to local 30000 to get the password of 15.

## 15-16
use `openssl s_client` to send 15 password to get 16 password.
password:JQttfApK4SeyHwDlI9SXGR50qclOAil1

## 16-17
there is only one port speak ssl response between 31000 and 32000 ports.
send this level password to the port and receive a private sshkey to connect to next level.

## 17-18

password:hga5tuuCLF6fFzUpnagiMN8ssu9LFrdg



