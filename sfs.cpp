#include <eeprom.h>
#include <arduino.h>
#include "sfs.h"

// Constructor
// filename is the name of the new file

sfs::sfs() {
}

sfs::sfs(const char *  fname, uint8_t fsize) {
	strncpy(fileName,fname,6);
  fileSize = fsize;
	GetFileAddress();
}


void sfs::GetFileAddress() {
  for (uint16_t i = 16; i < 1018; i++) {
    //find f start
    if (EEPROM.read(i) == fileName[0] &&
      EEPROM.read(i+1) == fileName[1] &&
      EEPROM.read(i+2) == fileName[2] &&
      EEPROM.read(i+3) == fileName[3] &&
      EEPROM.read(i+4) == fileName[4] &&
      EEPROM.read(i+5) == fileName[5]){
      startAddress = i;
      fileSize = EEPROM.read(i+6);
      break;
    }
  }
}

bool sfs::Exists(){
  if(startAddress>0){
    return true;
  }
  return false;
}

bool sfs::Load() {
  GetFileAddress();
  if(startAddress!=0){
    for (unsigned int t=0; t<sizeof(game_data); t++){
      *((char*)&game_data + t) = EEPROM.read(startAddress + 7 + t);
    }
    return true;
  }
  return false;
}

uint16_t sfs::newfileStartIndex(uint8_t fSize){
  uint16_t empty_bytes=0;
  uint16_t new_start_index = 0;
  uint16_t i = 16;
  do{
    if(EEPROM.read(i)==0 && new_start_index==0){//found an empty byte
      new_start_index=i;
      empty_bytes++;
      i++;
    }else if(EEPROM.read(i)==0 && new_start_index!=0){//found an empty byte
      empty_bytes++;
      if(empty_bytes==fSize){
        return new_start_index;
      }
      i++;
    }else if(EEPROM.read(i)!=0){
      new_start_index=0;
      empty_bytes=0;
      i+=EEPROM.read(i+6)+7;//jump past f found
    }
  }while(i<=1024);
  return 0;
}

bool sfs::Save() {
  GetFileAddress();
  if(startAddress==0){//new f
    startAddress = newfileStartIndex(sizeof(game_data)+7);// find free space to save the f
    if(startAddress!=0){//found space
      //write fname
      for(uint16_t i=0;i<6;i++){// write the f
        EEPROM.write(startAddress+i, *((char*)&fileName + i) );
      }
      //write fsize
      EEPROM.write(startAddress+6, fileSize);
      //write fdata
      for(uint16_t i=0;i<sizeof(game_data);i++){// write the f
        EEPROM.write(startAddress+7+i, *((char*)&game_data + i) );
      }   
      return true;
    }else{
      return false;
    }
  }else{//update existing f
    if(sizeof(game_data) == fileSize){// is new data the same size      
      //write fname
      for(uint16_t i=0;i<6;i++){// write the f
        EEPROM.write(startAddress+i, *((char*)&fileName + i) );
      }
      //write fsize
      EEPROM.write(startAddress+6, fileSize);
      //write fdata
      for(uint16_t i=0;i<sizeof(game_data);i++){// write the f
        EEPROM.write(startAddress+7+i, *((char*)&game_data + i) );
      } 
    }else{
      uint16_t tstartIndex =newfileStartIndex(sizeof(game_data)+7);// find free space to save the f      
      if(tstartIndex!=0){// found enough space
        // delete old f, set all bytes to 0        
        for(uint16_t i=0;i<sizeof(game_data)+7;i++){
          EEPROM.write(startAddress+i, 0);
        } 
        //write fname
        for(uint16_t i=0;i<6;i++){// write the f
          EEPROM.write(tstartIndex+i, *((char*)&fileName + i) );
        }
        //write fsize
        EEPROM.write(tstartIndex+6, fileSize);
        //write fdata
        for(uint16_t i=0;i<sizeof(game_data);i++){// write the f
          EEPROM.write(tstartIndex+7+i, *((char*)&game_data + i) );
        } 
        startAddress=tstartIndex;
        return true;
      }else{
        return false;
      }    
    }
  }
}

bool sfs::Erase(){
  if(startAddress!=0){//if no start index then f not found
    for (uint16_t i=0; i <=7+fileSize; i++) {
      EEPROM.write(startAddress+i, 0);   
    }
    startAddress=0;
    return true;
  }
  return false;
}

void sfs::Clear(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

sfs::~sfs(){
  delete this;
}

