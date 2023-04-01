#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H

#include "functional"

typedef std::function<int(const char* argument)> TConsoleCallback;

class ConsoleCommand {
  private:
    TConsoleCallback callback;   
    int operation_len;
    char **splitInput(char *input){
      char** arguments;
      return arguments;
    }

  public:
    const char *operation;
    ConsoleCommand(const char* operation, TConsoleCallback callback){
      this->operation = operation;
      this->operation_len = strlen(operation);
      this->callback = callback;
    }

    bool isCandidate(const char *input){
      if(input == nullptr){
        return false;
      }
      return isCandidate(input, strlen(input));
    }

    /// @brief Test if input starts exactly with the operation keywoard (non case sensitive)
    /// @param input char array to examine
    /// @param input_length
    /// @return true if found, false otherwise
    bool isCandidate(const char *input, int input_length){
      if(input == nullptr || operation_len > input_length){
        return false;
      }
      if(strncasecmp(operation, input, operation_len) != 0){
        return false;
      }
      if(input_length > operation_len && input[operation_len] != ' '){
        return false;
      }
      return true;
    }

    int execute(const char *input, int input_length){
      const char *argument = nullptr;
      if(input_length > operation_len +1){
        argument = &input[operation_len+1];
      }
      return this->callback(argument);
    }
};
#endif