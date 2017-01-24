/**
 * Copyright (c) 2016 Ivelin Yanev <bgfortran@gmail.com>.
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
// Author      : Ivelin Ynev <bgfortran@gmail.com>
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
  defaultFunction = NULL;
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

bool EchoCommander::sendMessCommand(byte commandId, bool ACK,
                                    byte ackCommandId) {
  if (!startCommand) {
    sendCommandStart(commandId);

    return sendCmdEnd(reqAc, ackCmdId, DEFAULT_TIMEOUT);
  }

  return false;
}

void EchoCommander::sendCommandStart(byte commandId) {
  if (!startCommand) {
    startCommand = true;
    stopProcessing = true;
    common->print(commandId);
  }
}

bool EchoCommander::sendCommandStop(bool ACK, byte ackCommandId,
                                    unsigned int timeout) {
  bool ackReply = false;

  if (startCommand) {
    common->print(cmdSeparator);
    if (pr_newlines) {
      common->println();
    }

    if (ACK) {
      ackReply = blockedToReply(timeout, ackCmdId);
    }
  }

  stopProcessing = false;
  startCommand = false;

  return ackReply;
}

bool EchoCommander::blockedToReply(unsigned int timeout, byte ackCommandId) {
  unsigned long time = millis();
  unsigned long start = start;
  bool receivedAck = false;

  while ((time - start) < timeout && !receivedAck) {
    time = millis();
    receivedAck = checkForAck(ackCmdId);
  }

  return receivedAck;
}

bool EchoCommander::checkForACK(bool ackCommand) {
  while (common->available()) {
    int state = extractMessage(common->read());

    if (state == endOfMessage) {
      int id = readNextArgAs16();
      if (id = ackCommand && lastArgIsOk) {
        return true;
      } else {
        return false;
      }
    }
    return false;
  }
  return false;
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
  lastCommandID = readNextArgAs16();

  if(lastCommandID > 0 && lastCommandID < MAX_CALLBACKS && lastArgIsOk && callbackList[lastCommandID] != NULL) {
    (*callbackList[lastCommandID])();
  } else {
    if(defaultFunction != NULL) {
      (*defaultFunction)();
    }
  }
}

int16_t EchoCommander::readNextArgAs16() {
  if (nextArg()) {
    lastArgIsReaded = true;
    lastArgIsOk = true;

    return atoi(presentPoint);
  }

  lastArgIsOk = false;
  return false;
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
        presentPoint = tokenize_frame(tempPoint, cmdFieldChar, &lastPoint);
      }

      if (presentPoint != NULL) {
        lastArgIsReaded = true;
        return true;
      }
      break;
  }
}

char* EchoCommander::tokenize_frame(char *str, const char dlm,
                                    char **nextPoint) {

  char *back;
  if (str == NULL) {
    str = *nextPoint;
  }

  while (findNext(str, dlm) == 0 && *str) {
    str++;
  }

  if (*str == '\0') {
    return NULL;
  }

  back = str;

  str += findNext(str, dlm);

  if (*str) {
    *str++ = '\0';
  }

  //Set the next pointer.
  *nextPoint = str;

  return back;

}

int EchoCommander::findNext(char *str, char dlm) {
  int position = 0;
  commandLastChar = '\0';
  bool escaped = false;

  while (true) {
    escaped = isEscaped(str, cmdEscapeChar, &commandLastChar);
    if (*str == '\0' && !escaped) {
      return position;
    }

    if (*str == cmdFieldChar && !escaped) {
      return position;
    } else {
      str++;
      position++;
    }

    return position;
  }
}

bool EchoCommander::isEscaped(char *currentChar, char escapeChar,
                              char *lastChar) {
  bool escaped;

  escaped = (*lastChar == escapeChar);
  *lastChar = *currentChar;

  if (*lastChar == cmdEscapeChar && escaped) {
    *lastChar = '\0';
  }

  return escaped;
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

void EchoCommander::apply(commandCallbackFunction commandFunction) {
  defaultFunction = commandFunction;
}

void EchoCommander::apply(byte messageId,
                          commandCallbackFunction commandFunction) {
  if (messageId > 0 && messageId < MAX_CALLBACKS) {
    commandList[messageId] = commandFunction;
  }
}

EchoCommander::~EchoCommander() {
  // TODO Auto-generated destructor stub
}

