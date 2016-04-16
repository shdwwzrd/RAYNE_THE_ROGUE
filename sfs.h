#ifndef sfs_h
#define sfs_h

#include "game_data.h"

class sfs  {
public:
  char fileName[7];
  uint8_t fileSize;
  uint16_t startAddress;
 
  void GetFileAddress();
  uint16_t newfileStartIndex(uint8_t fileSize);
  sfs(); 
	sfs(const char* fname,uint8_t fsize);	
  ~sfs();
  bool Exists();
	bool Save();
	bool Load();
	bool Erase();
  void Clear();
  game_data_struct game_data;
};

#endif
