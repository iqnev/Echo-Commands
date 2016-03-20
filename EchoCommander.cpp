/**
 * Copyright (c) 2016 Ivelin Yanev <qnev89@gmail.com>.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 **/

//============================================================================
// Name        : EchoCommander.cpp
// Author      : Ivelin Ynev <qnev89@gmail.com>
// Version     : 1.0
// Since       : 2016
// Description : TODO
//============================================================================
extern "C" {
#include <stdlib.h>
#include <stdarg.h>
}
#include <stdio.h>
#include "EchoCommander.h"

EchoCommander::EchoCommander(Stream &common) {
  setup(common);
}


void EchoCommander::setup(Stream &common) {
  common = &common;
  bufferSize = BUFFER_MESSAGE_SIZE;
  bufferLastIndex = BUFFER_MESSAGE_SIZE - 1;

  reset();
  int i =0;
  for(i = 0; i< MAX_CALLBACKS; i++ ) {
    commandList[i] = NULL;
  }

  locking_sender = false;
}

void EchoCommander::reset() {
  bufferIndex = 0;

}

void EchoCommander::addNewLineCom(bool newLine) {
  pr_newlines = newLine;
}

void EchoCommander::readSerialData() {
    while(!locking_sender && common->available()) {
      size_t bytesAvailable;
      bytesAvailable = STREAM_BUFFER_SIZE

      if(common->available() < bytesAvailable) {
        bytesAvailable = common->available();
      }

      common->readBytes(streamBuffer, bytesAvailable);

      for(size_t i = 0; i < bytesAvailable; i ++ ) {
        int state = extractMessage(streamBuffer[i]);

        if(state == endOfMessage) {
          dispatcheMessage();
        }
      }
    }
}

uint8_t EchoCommander::extractMessage(char currentChar) {
  mState = extractfOfMessage;
  //TODO
}

EchoCommander::~EchoCommander() {
  // TODO Auto-generated destructor stub
}

