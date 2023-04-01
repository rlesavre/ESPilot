#ifndef CONSOLE_H
#define CONSOLE_H

#include "Arduino.h"
#include "BaseModule.h"
#include "Common.h"
#include "BasicArray.hpp"
#include "ConsoleCommand.h"
#include "Logger.h"

#define MAX_INPUT_LINE_LEN 128

class ConsoleModule : public IBaseModule
{
private:
  char input_line[MAX_INPUT_LINE_LEN];
  unsigned int input_pos = 0;
  BasicArray<ConsoleCommand *> commands;

public:
  ConsoleModule() {}
  const char *getName() { return "Console"; }
  IScreenPage *getDisplayPage() { return NULL; }
  void setup()
  {
    auto cmds = &commands;
    commands.push_back(new ConsoleCommand("help", [cmds](const char *args){
      //Serial.println("Available commands:");
      logf("Available commands\n");
      for(int i=0;i<cmds->size();i++){
        logf(" - %s\r\n", cmds->get(i)->operation);
      }
      return 1;
    }));

    auto that = this;
    EspNowAgent::Instance()->registerCallback([that](const uint8_t *data, size_t len){
      that->onInput((const char*)data, len);
    });
  }

  void registerCommand(ConsoleCommand *command){
    commands.push_back(command);
    logf("Registered command: %s\r\n", command->operation);
  }

  void onInput(const char* input_line, int len){
    for(int i=0; i<commands.size(); i++){
      auto command = commands.get(i);
      if(command->isCandidate(input_line, len)){
        int result = command->execute(input_line, len);
        logf(">> %s result: %i\n", command->operation, result);
        break;
      }
    }
  }

  void loop()
  {
    while (Serial.available() > 0)
    {
      const byte inByte = Serial.read();
      switch (inByte)
      {
      case '\n':
        if(input_pos > 0){
          input_line[input_pos] = 0;
          onInput(input_line, input_pos);
          input_pos = 0;
        }
        break;
      case '\r':
        break;
      default:
        if (input_pos < (MAX_INPUT_LINE_LEN - 1))
        {
          input_line[input_pos++] = inByte;
        }
        break;
      }
    }
  }
};
#endif