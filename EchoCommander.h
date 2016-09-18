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
// Name        : EchoCommander.h
// Author      : Ivelin Ynev <qnev89@gmail.com>
// Version     : 1.0
// Since       : 2016
// Description : TODO
//============================================================================
#ifndef ECHOCOMMANDER_H_
#define ECHOCOMMANDER_H_

#include <inttypes.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
 //TODO
#endif
#include "Stream.h"

extern "C"
{
  // callback functions always follow the signature
  typedef void(*commandCallbackFunction) (void);
}


// Time out on unanswered messages.
#define DEFAULT_TIMEOUT 3000

// The maximum number of possible commands.
#define MAX_CALLBACKS 30

// The length of the commandbuffer.
#define BUFFER_MESSAGE_SIZE 64

// The length of the streambuffer.
#define STREAM_BUFFER_SIZE 512

// Message States
enum {
  endOfMessage,
  extractfOfMessage,
  extractOfArguments,
};

class EchoCommander {
  int number;

 public:

  bool startCommand;
  bool ArgSuccessful;

  bool stopProcessing;

  // Buffer that holds the command data,
  char commandBuffer[MAX_CALLBACKS];

  // Buffer that holds the stream data.
  char streamBuffer[STREAM_BUFFER_SIZE];

  // Command escape character.
  char cmdEscapeChar;

  // The last char of a command.
  char commandLastChar;

  //The sepator end of command.
  char cmdFieldChar;

  // The sepator of a command.
  char cmdSeparator;

  //The last pointer to previous buffer position.
  char *lastPoint;

  // A pointer to current position of a buffer.
  char *presentPoint;

  // If \r\n should be added after send command.
  bool pr_newlines;

  // Locks the input streaming of new commands, during sending a command.
  bool locking_sender;

  //The last argument is readded.
  bool lastArgIsReaded;

  //Indicated if last fetched argument could be read.
  bool lastArgIsOk;

  uint8_t currentMessageState;

  //TODO: delete
  uint8_t bufferSize;

  //The state of a message.
  uint8_t mState;

  //The current index where to write data in buffer.
  uint8_t bufferIndex;

  //The last index of the buffer.
  uint8_t bufferLastIndex;

  //The ID of the last received command.
  uint8_t lastCommandID;

  Stream *common;

  //default function
  commandCallbackFunction defaultFunction;

  commandCallbackFunction commandList[MAX_CALLBACKS];

  //==================================================================
  //
  //    Initialize the **EchoCommander**
  //
  //==================================================================

  /**
   * The deafault constructor with parameters.
   * @param common The current stream from Arduino Stream utility class.
   * @param cmd_esc_char The character, which must be escaped from a command.
   * @param cmd_separator The separator of all commands.
   */
  EchoCommander(Stream & comms, const char cmd_esc_char = '/',
                const char cmd_separator = ';', const char cmd_line_separator = ',');

  /**
   * The method that initialize the EchoCommander class.
   * @param common The current stream from Arduino Stream utility class.
   */
  void setup(Stream & common, const char cmd_esc_char, const char cmd_separator, const char cmd_field_separator);

  /**
   * The reset() method resets the command buffer and message state.
   */
  void reset();

  /**
   * Addes a new line after a sent command.
   */
  void addNewLineCom(bool newLLine = true);

  /**
   * Apply function for commands that are not explicitly attached.
   */
  void apply(commandCallbackFunction commandFunction);

  /**
   * Apply a function to a command ID.
   */
  void apply(byte messageId, commandCallbackFunction commandFunction);
  //==================================================================
  //
  //    Processing
  //
  //==================================================================

  /**
   * Send a command without arguments and without acknowledge.
   */
  bool sendMessCommand(byte commandId);

  /**
   * Send a command with arguments and with acknowledge.
   */
  bool sendMessCommand(byte commandId, bool ACK, byte ackCommandId);

  /**
   * Send START of command.
   */
  void sendCommandStart(byte commandId);

  /**
   * Send ENDT of command.
   */
  bool sendCommandStop(bool ACK = false, byte ackCommandId = 1, unsigned int timeout = DEFAULT_TIMEOUT);

  /**
   * Reads Serial data in EchoCommander from Arduino Searial interface.
   */
  void readSerialData();

  /*
   * Reads the next argument as int 16 bits.
   */
  int16_t readNextArgAs16();

  /*
   * Gets the next argument. If argument is available- return true.
   */
  bool nextArg();

  /**
   * Command extracting from a current message.
   */
  inline uint8_t extractMessage(char currentChar)
      __attribute__((always_inline));

  /**
   * Determines if acknowledge has come in.
   */
  inline bool checkForACK(byte ackCommand) __attribute__((always_inline));

  /*
   * Dispatches attached callbacks.
   */
  inline void dispatcheMessage() __attribute__((always_inline));

  /**
   * Waits for reply from sender or timeout before continuing.
   */
  inline bool blockedToReply(unsigned int timeout = DEFAULT_TIMEOUT, byte ackCommandId = 1) __attribute__((always_inline));
  /**
   * Checks whether a current string is clean.
   */
  bool isClean(char *currentChar, const char clearChar, char *lastChar);


  //####################################################################################
  //        Util methods
  //####################################################################################################################

  /**
   * Split frame in different token by a delimiter.
   */
  char *tokenize_frame(char *str, const char dlm, char **nextPoint);

  /**
   * Find next argument in frame.
   */
  int findNext(char *str, char delim);

  /**
   *Checks if the current character is escaped.
   */
  bool isEscaped(char *currentChar, const char escapeChar, char *lastChar);

  EchoCommander();
  virtual ~EchoCommander();
};

#endif /* ECHOCOMMANDER_H_ */
