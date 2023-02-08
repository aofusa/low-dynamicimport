Low Dynamic Import
=====


メモリに外部プログラムを読み込んで実行する  


通常のプログラムはコンパイル時にリンクされる  
このプログラムはコンパイル済みバイナリを動的に読み込み実行する  
実行可能な属性を付与したメモリを確保し外部プログラムを確保したメモリ領域に展開し呼び出すことで実現  


コンパイルと実行
-----
```sh
gcc -c -o subroutin_add.o subroutin_add.c
gcc -c -o subroutin_sub.o subroutin_sub.c
gcc -o main.out main.c

./main.out subroutin_add.o
# 3

./main.out subroutin_sub.o
# -1
# main.c は再コンパイルしていないが動作を変更できている
```


外部プログラムのロード
-----
マシンコードをメモリ上に展開し関数として呼び出すようにしている  

以下のような実行属性をつけたメモリにバイナリを展開し呼び出している  
```c
// 実行可能属性をつけたメモリを確保
char *mm = mmap(NULL, map_size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

// メモリにバイナリを展開する
memset(mm, 'program binary', 1);

// 関数の実行
int (*f)(int, int) = (void*)(mm + 0x40);  // 0x40のオフセットはELFのエントリポイントのオフセット
f(1, 2);
```


参考
-----
- [MMAP](https://linuxjm.osdn.jp/html/LDP_man-pages/man2/mmap.2.html)

