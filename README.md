# Embedder to hide a file inside a bigger file

Purpose
=======
Embed into/extract a file into/from other file with byte XOR.

Invocation
==========
(1) embedding : embed data into filename

```
>embedder in filename filename_data filepos XORbyte
```

where
  filename      - file to embed data into
  
  filename_data - file which contains bytes to be embedded
  
  filepos       - file position in filename to start embedding data
  
  XORbyte       - byte to xor all embedded data

(2) extracting : extract data from filename into new filename_data (must not exist)

```
>embedder out filename filename_data filepos XORbyte
```

Test
====
Hide data.zip inside container.bmp, extract this data into new file data1.zip 
and compare data.zip and data1.zip :

C:\AndrewK\MyProjects2\Embedder\x64\Release>Embedder.exe in container.bmp data.zip 500 37

C:\AndrewK\MyProjects2\Embedder\x64\Release>Embedder.exe out container.bmp data1.zip 500 37

C:\AndrewK\MyProjects2\Embedder\x64\Release>fc data.zip data1.zip

Comparing files data.zip and DATA1.ZIP

FC: no differences encountered

Compiler
========
VS 2019 is the compiler for this project.


