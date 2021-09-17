# Linux File System Tree Simulator

## Requirements (to compile it as a 32-bit executable)
* Linux 32-bit support libraries, you can install them with: `sudo apt install gcc-multilib`

## Get started
* Compile (32-bit): `gcc -m32 linux_file_system_tree_sim.c`
* Run it and enjoy: `./a.out`

### Supported commands
1. `mkdir <pathname>` : create a new directory node in the file system tree;
2. `rmdir <pathname>` : remove an empty directory from the file system tree;
3. `cd [<pathname>]` : change the current working directory;
4. `ls [<pathname>]` : list the directory contents;
5. `pwd` : print the (absolute) path to the current working directory;
6. `create <pathname>` : create a new file node in the file system tree;
7. `rm <pathname>` : remove a file from the file system tree;
8. `save <filename>` : save the current file system tree as a text file (.txt);
9. `reload <filename>` : construct a file system tree from a well formatted text file;
10. `menu` : show the current help menu;
11. `prompt <prompt>` : change the command prompt text;
12. `quit` : save the file system tree as "file_system_tree.txt" and quit.

## References
* Book: "System Programming in Unix/Linux" by K.C. Wang (2018)
* C naming conventions: https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html#stacknames
