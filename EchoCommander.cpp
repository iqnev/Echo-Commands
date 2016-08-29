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

EchoCommander::EchoCommander(Stream &common, const char cmd_esc_char,
                             const char cmd_separator,
                             const char cmd_line_separator) {
  setup(common, cmd_esc_char, cmd_separator, cmd_line_separator);
}

void EchoCommander::setup(Stream &common, const char cmd_esc_char,
                          const char cmd_separator,
                          const char cmd_line_separator) {
  common = &common;
  bufferSize = BUFFER_MESSAGE_SIZE;
  bufferLastIndex = BUFFER_MESSAGE_SIZE - 1;
  cmdEscapeChar = cmd_esc_char;
  cmdSeparator = cmd_separator;
  cmdFieldChar = cmd_line_separator;

  reset();
  int i = 0;
  for (i = 0; i < MAX_CALLBACKS; i++) {
    commandList[i] = NULL;
  }

  locking_sender = false;
}

void EchoCommander::reset() {
  bufferIndex = 0;
  presentPoint = null;
}

bool EchoCommander::sendMessCommand(byte commandId) {
  if (!startCommand) {
    startCommand = true;
    stopProcessing = true;
    common->print(commandId);
  }
}

void EchoCommander::addNewLineCom(bool newLine) {
  pr_newlines = newLine;
}

void EchoCommander::readSerialData() {
  while (!locking_sender && common->available()) {
    size_t bytesAvailable;
    bytesAvailable = STREAM_BUFFER_SIZE

    if (common->available() < bytesAvailable) {
      bytesAvailable = common->available();
    }

    common->readBytes(streamBuffer, bytesAvailable);

    for (size_t i = 0; i < bytesAvailable; i++) {
      int state = extractMessage(streamBuffer[i]);

      if (state == endOfMessage) {
        dispatcheMessage();
      }
    }
  }
}

void EchoCommander::dispatcheMessage() {

}

int16_t EchoCommander::readNextArg() {

}

bool EchoCommander::nextArg() {
  char * tempPoint = NULL;

  switch (currentMessageState) {
    case extractfOfMessage:
      return false;
      break;
    case endOfMessage:
      tempPoint = commandBuffer;
      currentMessageState = extractOfArguments;
      break;
    default:
      if (lastArgIsReaded) {
        presentPoint = tokenize(tempPoint, cmdFieldChar, &lastPoint);
      }

      if (presentPoint != NULL) {
        lastArgIsReaded = true;
        return true;
      }
      break;
  }
}

char* EchoCommander::tokenize(char *str, const char separator, char **nextPointer) {
  //TODO split_r
}

char* EchoCommander::split_frame(char *str, const char dlm, char **nextPoint) {

  if(str == NULL) {
    str = *nextPoint;
  }

  while(findNext(str, dlm) == 0 && *str) {
    str++;
  }

  //todo


}

int EchoCommander::findNext(char *str, char dlm) {
  int position =0;
  commandLastChar = '\0';
  bool escaped = false;

  while(true) {
    escape = isEscaped(str, cmdEscapeChar, &commandLastChar);
    if(*str == '\0' && !escaped) {
      return position;
    }

    if(*str == cmdFieldChar && !escaped) {
      return position;
    } else {
      str++;
      position++;
    }

    return position;
  }
}

bool EchoCommander::isEscaped(char *currentChar, char escapeChar, char *lastChar) {
  //TODO
}

uint8_t EchoCommander::extractMessage(char currentChar) {
  mState = extractfOfMessage;

  bool clean = isClean(&currentChar, cmdEscapeChar, &commandLastChar);
  if (!clean && (currentChar == cmdSeparator)) {
    commandBuffer[bufferIndex] = 0;

    if (bufferIndex > 0) {
      presentPoint = commandBuffer;
      commandLastChar = '\0';
      mState = endOfMessage;
    }

    reset();
  } else {
    commandBuffer[bufferIndex] = currentChar;
    bufferIndex++;
    if (bufferIndex >= bufferLastIndex) {
      reset();
    }
  }

  return mState;
}

bool EchoCommander::isClean(char *currentChar, const char escapeChar,
                            char *commandLastChar) {
  bool clean = false;

  if (escapeChar == *commandLastChar) {
    clean = true;
  }

  *commandLastChar = *currentChar;

  if (*commandLastChar == escapeChar && cmdEscapeChar) {
    *commandLastChar = '\0';
  }

  return clean;
}

EchoCommander::~EchoCommander() {
  // TODO Auto-generated destructor stub
}

