/**
BSD 2-Clause License

Copyright (c) 2025, Andrey Kudryavtsev (andrewkoudr@hotmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <fstream>
#include <string>
//#include <filesystem>
#include <algorithm>
#include <limits>

#define NOMINMAX
#include <windows.h>

using namespace std;

// If wrong exit
void wrongExit(string message)
{
  cout << message << endl << endl;
  cout << "   >embedder in filename filename_data filepos XORbyte" << endl;
  cout << "- embed data from filename_data into filename where" << endl;
  cout << "in             - embedding operation" << endl;
  cout << "filename       - file to embed data into" << endl;
  cout << "filename_data  - file which contains bytes to be embedded" << endl;
  cout << "filepos        - file position in filename to start embedding data" << endl << endl;
  cout << "XORbyte        - byte to xor all embedded data (decimal 0..255)" << endl << endl;
  cout << "   >embedder out filename filename_data filepos XORbyte" << endl;
  cout << "- extract data from filename into new filename_data file (must not exist)" << endl;
  cout << endl << endl << "  Press ENTER..." << endl;

  getchar();
}

// File exists?
bool FileExists(std::string& path)
{
  char buffer[4096] = { 0 };

  strcpy_s(buffer, path.c_str());
  FILE* file = nullptr;
  fopen_s(&file, buffer, "rb");

  if (file)
  {
    fclose(file);
    return true;
  } else
  {
    return false;
  }
}

// Get file size
size_t FileSize(std::string& path)
{
  struct stat st;

  stat(path.c_str(), &st);
  size_t size = st.st_size;

  return size;
}

// Upcase
std::string UpCase(std::string str)
{
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

// Read chunk of file into memory, read chunk BETWEEN pos1 and pos0
// (pos1 is first byte OUT of chunk!)
char* readFile(std::string& filename, int pos0, int pos1)
{
                              // chunk size
  int len = pos1 - pos0;
                              // nothing to do
  if (len <= 0)
  {
    return nullptr;
  }
                              // file exists?
  if (!FileExists(filename))
  {
    return nullptr;
  }
                              // open file for reading
  ifstream file(filename, std::ifstream::binary | std::ofstream::in);

  if (!file.is_open())
  {
    return nullptr;
  }

  char* data = (char*) malloc(len);
  if (data)
  {
    file.seekg(pos0);
    file.read(data, len);
  }

  file.close();

  return data;
}

/**
  Compiler
  --------
  VS 2019 is the compiler for the whole project.

  Purpose
  -------
  Embed into/extract a file into/from other file with byte XOR.

  Invocation
  ----------
  (1) embedding : embed data into filename

  >embedder in filename filename_data filepos XORbyte

  where
    filename      - file to embed data into
    filename_data - file which contains bytes to be embedded
    filepos       - file position in filename to start embedding data
    XORbyte       - byte to xor all embedded data

  (2) extracting : extract data from filename into new filename_data (must not exist)

  >embedder out filename filename_data filepos XORbyte

  Test
  ----

  Hide data.zip inside container.bmp, extract this data into new file data1.zip 
  and compare data.zip and data1.zip/

C:\AndrewK\MyProjects2\Embedder\x64\Release>Embedder.exe in container.bmp data.zip 500 37

C:\AndrewK\MyProjects2\Embedder\x64\Release>Embedder.exe out container.bmp data1.zip 500 37

C:\AndrewK\MyProjects2\Embedder\x64\Release>fc data.zip data1.zip
Comparing files data.zip and DATA1.ZIP
FC: no differences encountered

*/

int main(int argc, char* argv[])
{
                              // test # command line arguments

  if (argc != 6)
  {
    wrongExit("Wrong number of command line parameters");

    return 1;
  }
                              // get arguments
  string operation = UpCase(argv[1]);
  string filename = argv[2];
  string filename_data = argv[3];
  int pos = std::stoi(argv[4]);
  unsigned char xorbyte = std::stoi(argv[5]);
                              // check arguments (primitive, improvement if needed)
  bool embed = (operation == "IN");
                              // check if file exists
  if (!FileExists(filename))
  {
    wrongExit("Unable to find file " + filename);

    return 1;
  }
                              // input file size
  size_t isize = FileSize(filename);

  if (embed)
  {
                              // check if data file exists
    if (!FileExists(filename_data))
    {
      wrongExit("Unable to find file " + filename_data);
      return 1;
    }
                              // data size
    size_t dsize = FileSize(filename_data);
                              // total data length will be stored at pos
    int pos0 = pos;
    int pos1 = pos + sizeof(size_t) + dsize;

    if (pos1 > isize)
    {
      wrongExit("Data size does not fit within " + filename);

      return 1;
    }
                              // save unchanged "head" in memory
    char* head = readFile(filename, 0, pos0);
                              // save unchanged "tail" in memory
    char* tail = readFile(filename, pos1, isize);
                              // open file for writing (rewrite!)
    ofstream file(filename, std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);

    if (!file.is_open())
    {
      wrongExit("Unable to write to file " + filename);
      return 1;
    }
                              // save head
    if (head)
    {
      file.write(head, pos0);
      free(head);
    }
                              // read data
    char* data = readFile(filename_data, 0, dsize);
                              // write data size
    file.write((char*)& dsize, sizeof(size_t));

    if (data)
    {
                              // xor data
      for (size_t i = 0; i < dsize; i++)
      {
        data[i] = data[i] ^ xorbyte;
      }
                              // write
      file.write(data, dsize);
      free(data);
    }
                              // save tail
    if (tail)
    {
      file.write(tail, isize - pos1);
      free(tail);
    }

    file.close();
                              // extract
  } else
  {
                              // check if data file exists
    if (FileExists(filename_data))
    {
      wrongExit("This file already exists (would be overwritten) " + filename_data);
      return 1;
    }
                               // open input

    ifstream file(filename, std::ifstream::binary | std::ifstream::in);

    if (!file.is_open())
    {
      wrongExit("Unable to read from file " + filename_data);
      return 1;
    }
                              // go to data start
    file.seekg(pos);
                              // get data size
    size_t dsize = 0;

    file.read((char*)&dsize, sizeof(size_t));
                              // close file
    file.close();
                              // read data
    char* data = readFile(filename, pos + sizeof(size_t), pos + sizeof(size_t) + dsize);

    if (data)
    {
                                // open output (rewrite)
      ofstream fdata(filename_data, std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);
      if (!fdata.is_open())
      {
        wrongExit("Unable to create file " + filename_data);
        return 1;
      }
                                // xor data
      for (size_t i = 0; i < dsize; i++)
      {
        data[i] = data[i] ^ xorbyte;
      }

      fdata.write(data, dsize);
      fdata.close();
      free(data);
    }
  }

  return 0;
}